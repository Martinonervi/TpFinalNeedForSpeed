#include "server_gameloop.h"

#include <chrono>
#include <thread>

#include "../common_src/move_Info.h"
#include "../common_src/player.h"
#include "../common_src/player_state.h"
#include "../common_src/send_player.h"
#include "../common_src/init_player.h"
#include "../common_src/new_player.h"

#define TIME_STEP 1.0f / 60.0f //cuánto tiempo avanza el mundo en esa llamada.
#define SUB_STEP_COUNT 4 //por cada timeStep resuelve problemas 4 veces mas rapido (ej: colisiones)

GameLoop::GameLoop(gameLoopQueue& queue, ClientsRegistry& registry):
        worldManager(), queue(queue), registry(registry) {
    //worldManager.loadMapFromYaml("map.yaml"); :)
}

void GameLoop::run() {
    try {
        while (should_keep_running()) {
            processCmds();

            // esto lo tengo q ver con fran, a ver si el me manda cada vez q mantienen apretado
            for (auto& [id, car] : cars) {
                auto it = lastInput.find(id);
                if (it != lastInput.end()) {
                    car.applyControlsToBody(it->second, TIME_STEP);
                } else {
                    MoveMsg mv(0, 0, 0, 0);
                    car.applyControlsToBody(mv, TIME_STEP);
                }
            }

            worldManager.step(TIME_STEP,  SUB_STEP_COUNT);

            broadcastCarSnapshots();

            std::this_thread::sleep_for(std::chrono::milliseconds(TICK_MS));
        }
    } catch (const std::exception& e) {
        std::cerr << "[GameLoop] fatal: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[GameLoop] fatal: unknown\n";
    }
}


void GameLoop::processCmds() {
    std::list<Cmd> to_process = emptyQueue();
    for (Cmd& cmd: to_process) {
        switch (cmd.msg->type()) {
            case (Opcode::Movement): {
                std::cout << "aca en el gameloop MOVEMENT\n";
                movementHandler(cmd);
                break;
            }
            case (Opcode::INIT_PLAYER): {
                std::cout << "aca en el gameloop INIT_PLAYER\n";
                initPlayerHandler(cmd);
                break;
            }
            default: {
                std::cout << "cmd desconocido: " << cmd.msg->type() << "\n";
            }
        }

    }
}



void GameLoop::initPlayerHandler(Cmd& cmd){
    // spawn auto físico
    b2Vec2 spawn = { 2.0f * static_cast<float>(cars.size()), 0.0f };
    EntityId eid = worldManager.createCarBody(spawn, 0.f);
    b2BodyId body = worldManager.getBody(eid);
    //podria guardar el carType y el name (en el Car)
    cars.emplace(cmd.client_id, Car(cmd.client_id, body));
    clientToEntity.emplace(cmd.client_id, eid);


    const InitPlayer ip = dynamic_cast<const InitPlayer&>(*cmd.msg);
    auto base = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SendPlayer>(cmd.client_id, ip.getCarType(), spawn.x, spawn.y, 3));
    registry.sendTo(cmd.client_id, base); //le aviso al cliente q ya tiene su auto


    for (auto [id, car]: cars) {
        auto newPlayer = std::static_pointer_cast<SrvMsg>(
                std::make_shared<NewPlayer>(id, ip.getCarType(), 1, 2, 3));
        if (id == cmd.client_id) continue;

        //le aviso a los demas que hay un nuevo auto en la partida
        registry.broadcast(newPlayer);


    }

}

void GameLoop::movementHandler(Cmd& cmd) {
    auto it = cars.find(cmd.client_id);
    if (it == cars.end()) return;

    const auto& mv = dynamic_cast<const MoveMsg&>(*cmd.msg);

    lastInput[cmd.client_id] = mv;
    //it->second.applyControlsToBody(mv, timeStep);

}

// la voy implementando aunque la logica del msj todavia no esta hecha
void GameLoop::disconnectHandler(ID id) {
    cars.erase(id);

    auto it = clientToEntity.find(id);
    if (it != clientToEntity.end()) {
        worldManager.destroyEntity(it->second);
        clientToEntity.erase(it);
    }
}


void GameLoop::broadcastCarSnapshots() {
    for (auto& [id, car] : cars) {
        PlayerState ps = car.snapshotState();
        auto base = std::static_pointer_cast<SrvMsg>(
                std::make_shared<PlayerState>(std::move(ps)));
        registry.broadcast(base); // todos los jugadores quieren saber tu posicion
    }
}


std::list<Cmd> GameLoop::emptyQueue() {
    std::list<Cmd> cmd_list;
    Cmd cmd_aux;

    while (queue.try_pop(cmd_aux)) {
        cmd_list.push_back(std::move(cmd_aux));
    }

    return cmd_list;
}

void GameLoop::stop() {
    Thread::stop();
    try {
        queue.close();
    } catch (...) {}
}

GameLoop::~GameLoop() {}