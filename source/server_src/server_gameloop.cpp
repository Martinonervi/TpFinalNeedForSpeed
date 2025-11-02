#include "server_gameloop.h"

#include <chrono>
#include <thread>

#include "../common_src/move_Info.h"
#include "../common_src/player.h"
#include "../common_src/player_state.h"
#include "../common_src/send_player.h"
#include "../common_src/init_player.h"
#include "../common_src/new_player.h"

GameLoop::GameLoop(gameLoopQueue& queue, ClientsRegistry& registry):
        queue(queue), registry(registry) {

    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, 0.0f};   // sin gravedad
    this->world = b2CreateWorld(&worldDef);

}
const float timeStep = 1.0f / 60.0f; //cuánto tiempo avanza el mundo en esa llamada.
const int subStepCount = 4; //por cada timeStep resuelve problemas 4 veces mas rapido o algo asi
//sobre todo para hacer calculos de colisiones

void GameLoop::run() {
    try {
        while (should_keep_running()) {
            processCmds();

            b2World_Step(this->world, timeStep,  subStepCount);

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



void GameLoop::initPlayerHandler(Cmd& cmd){ //testeamos
    //aca tendria q crear el auto
    const InitPlayer ip = dynamic_cast<const InitPlayer&>(*cmd.msg);


    // separo autos 3m en X para que no spawneen superpuestos
    b2Vec2 spawn = { 3.0f * static_cast<float>(cars.size()), 0.0f };
    float angle  = 0.0f;

    // try_emplace construye el Car in-place
    auto [insIt, inserted] =
            cars.try_emplace(cmd.client_id, cmd.client_id, this->world, spawn, angle);
    if (!inserted) {
        return;
    }


    auto base = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SendPlayer>(cmd.client_id, ip.getCarType(), 1, 2, 3));
    registry.sendTo(cmd.client_id, base);



    for (auto [id, car]: cars) {
        auto newPlayer = std::static_pointer_cast<SrvMsg>(
                std::make_shared<NewPlayer>(id, ip.getCarType(), 1, 2, 3));
        registry.broadcast(newPlayer);
        std::cout << "llego al boradcast?\n";
    }

}

void GameLoop::movementHandler(Cmd& cmd) {
    auto it = cars.find(cmd.client_id);
    const auto& mv = dynamic_cast<const MoveMsg&>(*cmd.msg);
    it->second.applyControlsToBody(mv, timeStep);

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

GameLoop::~GameLoop() {
    if (world.index1) {
        b2DestroyWorld(this->world);
        this->world = {0};
    }
}