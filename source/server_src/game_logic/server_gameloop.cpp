#include "server_gameloop.h"

#include <chrono>
#include <thread>

#include "../../common_src/cli_msg/init_player.h"
#include "../../common_src/cli_msg/move_Info.h"
#include "../../common_src/constant_rate_loop.h"
#include "../../common_src/srv_msg/client_disconnect.h"
#include "../../common_src/srv_msg/new_player.h"
#include "../../common_src/srv_msg/player.h"
#include "../../common_src/srv_msg/player_state.h"
#include "../../common_src/srv_msg/send_player.h"
#include "../../common_src/srv_msg/srv_car_hit_msg.h"
#include "../../common_src/srv_msg/srv_checkpoint_hit_msg.h"

#define TIME_STEP 1.0f / 60.0f //cuánto tiempo avanza el mundo en esa llamada.
#define SUB_STEP_COUNT 4 //por cada timeStep resuelve problemas 4 veces mas rapido (ej: colisiones)
#define FILE_YAML_PATH "../server_src/world/map.yaml"

GameLoop::GameLoop(std::shared_ptr<gameLoopQueue> queue, std::shared_ptr<ClientsRegistry> registry):
worldEvents(), worldManager(worldEvents),queue(std::move(queue)),
registry(std::move(registry)), eventHandlers(cars, checkpoints, *this->registry,
            raceTimeSeconds, finishedCarsCount, totalCars, raceEnded)  {
    loadMapFromYaml(FILE_YAML_PATH);
}



void GameLoop::loadMapFromYaml(const std::string& path) {
    MapParser parser;
    MapData data = parser.load(path);

    for (const auto& b : data.buildings) {
        auto building = std::make_unique<Building>(
                worldManager,
                b.x, b.y,
                b.w, b.h,
                b.angle
        );
        buildings.push_back(std::move(building));
    }

    for (const auto& cpCfg : data.checkpoints) {
        checkpoints.emplace(
                cpCfg.id,
                Checkpoint(
                        worldManager,
                        cpCfg.id,
                        cpCfg.kind,
                        cpCfg.x, cpCfg.y,
                        cpCfg.w, cpCfg.h, cpCfg.angle
                        )
        );
    }
}


using Clock = std::chrono::steady_clock;
void GameLoop::waitingForPlayers() {
    ConstantRateLoop loop(5.0);
    const int MAX_PLAYERS = 8;
    const double LOBBY_TIMEOUT_SEC = 5.0;

    const auto deadline = Clock::now() + std::chrono::duration<double>(LOBBY_TIMEOUT_SEC);
    while (true) {
        if (registry->size() >= MAX_PLAYERS) break;
        if (Clock::now() >= deadline) break;
        loop.sleep_until_next_frame();
    }
    this->raceStarted = true;
}

void GameLoop::run() {
    waitingForPlayers();
    raceStartTime = Clock::now();
    try {
        ConstantRateLoop loop(60.0);

        while (should_keep_running()) {
            checkPlayersStatus();
            processCmds();
            worldManager.step(TIME_STEP, SUB_STEP_COUNT);

            if (!raceEnded) {
                auto now = Clock::now();
                std::chrono::duration<float> elapsed = now - raceStartTime;
                raceTimeSeconds = elapsed.count();
            }

            broadcastCarSnapshots();
            processWorldEvents();
            loop.sleep_until_next_frame();
        }

    } catch (const std::exception& e) {
        std::cerr << "[GameLoop] fatal: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[GameLoop] fatal: unknown\n";
    }
}

void GameLoop::checkPlayersStatus() {
    std::vector<ID> ids;
    for (auto& car: cars) {
        ids.push_back(car.first);
    }
    std::vector<ID> toDisconnect = registry->checkClients(ids);
    for (ID idToDisconnect : toDisconnect) {
        disconnectHandler(idToDisconnect);
        std::cout << "[GameLoop] auto con id: " << idToDisconnect
        << " borrado" << "\n";
        auto msg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<ClientDisconnect>(idToDisconnect));
        registry->broadcast(msg);
    }
}


bool GameLoop::isRaceStarted() const {
    return this->raceStarted;
}

bool GameLoop::isConnected(ID id) const {
    return registry->contains(id);
}

void GameLoop::processWorldEvents() {
    std::unordered_set<ID> alreadyHitBuildingThisFrame;
    std::unordered_set<uint64_t> alreadyHitCarPairThisFrame;

    while (!worldEvents.empty()) {
        WorldEvent ev = worldEvents.front();
        worldEvents.pop();

        switch (ev.type) {
            case WorldEventType::CarHitCheckpoint: {
                eventHandlers.CarHitCheckpointHandler(ev);
                break;
            }
            case WorldEventType::CarHitBuilding: {
                eventHandlers.CarHitBuildingHandler(ev, alreadyHitBuildingThisFrame);
                break;
            }
            case WorldEventType::CarHitCar: {
                eventHandlers.CarHitCarHandler(ev, alreadyHitCarPairThisFrame);
                break;
            }
            default:
                break;
        }
    }
}


void GameLoop::processCmds() {
    std::list<Cmd> to_process = emptyQueue();
    for (Cmd& cmd: to_process) {
        if (!isConnected(cmd.client_id)) {
            continue;
        }

        switch (cmd.msg->type()) {
            case (Opcode::Movement): {
                movementHandler(cmd);
                break;
            }
            case (Opcode::INIT_PLAYER): {
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
    const InitPlayer ip = dynamic_cast<const InitPlayer&>(*cmd.msg);

    b2Vec2 spawn = { 7.0f, 15.0f };
    cars.emplace(cmd.client_id, Car(this->worldManager, cmd.client_id, spawn, 0, ip.getCarType()));
    totalCars++;

    auto base = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SendPlayer>(cmd.client_id, ip.getCarType(), spawn.x, spawn.y, 3));
    registry->sendTo(cmd.client_id, base); //le aviso al cliente q ya tiene su auto

    // le aviso al nuevo cliente donde estan los otros autos
    for (auto [id, car]: cars) {
        auto newPlayer = std::static_pointer_cast<SrvMsg>(
                std::make_shared<NewPlayer>(id, car.getCarType(), 1, 2, 3));
        // deberian ser pos reales
        if (id == cmd.client_id) continue;

        //le aviso a los demas que hay un nuevo auto en la partida
        registry->sendTo(cmd.client_id, newPlayer);

    }
    // les aviso a todos del auto del nuevo cliente
    for (auto& [otherId, _] : cars) {
        if (otherId == cmd.client_id) continue;
        auto npForOld = std::static_pointer_cast<SrvMsg>(
                std::make_shared<NewPlayer>(cmd.client_id, ip.getCarType(), spawn.x, spawn.y, 0.f)
        );
        registry->sendTo(otherId, npForOld);
    }

}

void GameLoop::movementHandler(Cmd& cmd) {
    auto it = cars.find(cmd.client_id);
    if (it == cars.end()) return;
    if (it->second.isCarDestroy()) return;

    const auto& mv = dynamic_cast<const MoveMsg&>(*cmd.msg);

    it->second.applyControlsToBody(mv, TIME_STEP);

}

// la voy implementando aunque la logica del msj todavia no esta hecha
void GameLoop::disconnectHandler(ID id) {
    auto it = cars.find(id);
    if (it == cars.end()) return;
    worldManager.destroyEntity(it->second.getPhysicsId());
    cars.erase(id);
}

void GameLoop::broadcastCarSnapshots() {
    for (auto& [id, car] : cars) {
        PlayerState ps = car.snapshotState();

        ID actual = car.getActualCheckpoint();
        ID next   = actual + 1;

        float dirX = 0.f, dirY = 0.f;

        auto itCp = checkpoints.find(next);
        if (itCp == checkpoints.end()) return; //termino
        const Checkpoint& cp = itCp->second;

        b2BodyId body = car.getBody();
        b2Vec2 pos = b2Body_GetPosition(body);

        float vx = cp.getX() - pos.x;
        float vy = cp.getY() - pos.y;

        float len = std::sqrt(vx*vx + vy*vy);
        if (len > 0.0001f) { //normalizo
            dirX = vx / len;
            dirY = vy / len;
        }

        ps.setCheckpointInfo(next, cp.getX(), cp.getY(), dirX, dirY);

        auto base = std::static_pointer_cast<SrvMsg>(
                std::make_shared<PlayerState>(std::move(ps)));
        registry->broadcast(base);
    }
}

std::list<Cmd> GameLoop::emptyQueue() {
    std::list<Cmd> cmd_list;
    Cmd cmd_aux;

    while (queue->try_pop(cmd_aux)) {
        cmd_list.push_back(std::move(cmd_aux));
    }
    return cmd_list;
}

void GameLoop::stop() {
    Thread::stop();
    try {
        queue->close();
    } catch (...) {}
}

GameLoop::~GameLoop() {}