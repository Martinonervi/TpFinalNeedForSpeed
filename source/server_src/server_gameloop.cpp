#include "server_gameloop.h"

#include <chrono>
#include <thread>

#include "../common_src/move_Info.h"
#include "../common_src/player.h"
#include "../common_src/player_state.h"
#include "../common_src/send_player.h"
#include "../common_src/init_player.h"
#include "../common_src/new_player.h"
#include <unordered_set>
#include "../common_src/constant_rate_loop.h"

#define TIME_STEP 1.0f / 60.0f //cuánto tiempo avanza el mundo en esa llamada.
#define SUB_STEP_COUNT 4 //por cada timeStep resuelve problemas 4 veces mas rapido (ej: colisiones)

GameLoop::GameLoop(std::shared_ptr<gameLoopQueue> queue, std::shared_ptr<ClientsRegistry> registry):
        worldEvents(), worldManager(worldEvents),queue(std::move(queue)), registry(std::move(registry)) {
    loadMapFromYaml("../server_src/map.yaml");
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
        auto cp = std::make_unique<Checkpoint>(
                worldManager,
                cpCfg.id,
                cpCfg.kind,
                cpCfg.x1, cpCfg.y1,
                cpCfg.x2, cpCfg.y2
        );
        checkpoints.push_back(std::move(cp));
    }
}




void GameLoop::run() {
    try {
        ConstantRateLoop loop(60.0);

        while (should_keep_running()) {
            processCmds();
            worldManager.step(TIME_STEP, SUB_STEP_COUNT);
            broadcastCarSnapshots();

            loop.sleep_until_next_frame();
        }

    } catch (const std::exception& e) {
        std::cerr << "[GameLoop] fatal: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[GameLoop] fatal: unknown\n";
    }
}




/*
void GameLoop::run() {
    try {
        while (should_keep_running()) {
            processCmds();

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

*/

void GameLoop::processWorldEvents() {
    // para no procesar 50 veces el mismo auto pegado al edificio en este frame
    std::unordered_set<ID> alreadyHitBuildingThisFrame;
    // para no procesar dos veces el mismo choque de autos A-B y B-A
    std::unordered_set<ID> alreadyHitCarPairThisFrame;

    while (!worldEvents.empty()) {
        WorldEvent ev = worldEvents.front();
        worldEvents.pop();

        switch (ev.type) {

            case WorldEventType::CarHitCheckpoint: {
                auto it = cars.find(ev.carId);
                if (it == cars.end()) break;
                auto actualCheckpoint = it -> second.getActualCheckpoint();
                if (actualCheckpoint  + 1 == ev.checkpointId) { //no se salteo ninguno
                    it->second.setCheckpoint(actualCheckpoint);
                    //le aviso a la interfaz calculo
                }
                break;
            }

            case WorldEventType::CarHitBuilding: {
                if (alreadyHitBuildingThisFrame.count(ev.carId)) {
                    break;
                }
                alreadyHitBuildingThisFrame.insert(ev.carId);

                auto it = cars.find(ev.carId);
                if (it == cars.end()) break;
                Car& car = it->second;
                b2BodyId body = car.getBody();

                b2Vec2 vel = b2Body_GetLinearVelocity(body); //vel auto

                // velocidad en la dirección del choque
                // producto interno
                // n unitario
                float impactSpeed = std::fabs(vel.x * ev.nx + vel.y * ev.ny);

                // si es muy lento pase pase
                const float MIN_IMPACT = 1.5f;
                if (impactSpeed < MIN_IMPACT) {
                    break;
                }

                // opcional: ver si fue frontal
                b2Rot rot = b2Body_GetRotation(body);
                b2Vec2 fwd = b2RotateVector(rot, {0.f, 1.f});
                // fwd es unitario
                float frontal = fwd.x * ev.nx + fwd.y * ev.ny;
                //fwd*n = ||fwd||*||n||*cos(ø)
                //como ambos son unitarios
                //fwd*n = cos(ø)


                // daño base
                float damage = impactSpeed * 0.5f;
                if (frontal > 0.7f) { // casi de frente
                    damage *= 2.0f;
                }

                car.applyDamage(damage);
                if (car.isCarDestroy()) {
                    //le aviso a la interfaz
                }

                // frenar un poco empujando contra la normal
                b2Vec2 newVel = {
                        vel.x - ev.nx * impactSpeed * 0.5f,
                        vel.y - ev.ny * impactSpeed * 0.5f
                };
                b2Body_SetLinearVelocity(body, newVel);

                break;
            }

            case WorldEventType::CarHitCar: {
                // normalizamos la pareja (a,b) para que a < b y así no duplicamos
                int a = ev.carId;
                int b = ev.otherCarId;
                if (a > b) std::swap(a, b);

                //chequear esto
                ID key = (static_cast<long long>(a) << 32) | static_cast<unsigned int>(b);
                if (alreadyHitCarPairThisFrame.count(key)) {
                    break;
                }
                alreadyHitCarPairThisFrame.insert(key);

                auto itA = cars.find(ev.carId);
                auto itB = cars.find(ev.otherCarId);
                if (itA == cars.end() || itB == cars.end()) break;

                Car& carA = itA->second;
                Car& carB = itB->second;

                b2BodyId bodyA = carA.getBody();
                b2BodyId bodyB = carB.getBody();

                b2Vec2 vA = b2Body_GetLinearVelocity(bodyA);
                b2Vec2 vB = b2Body_GetLinearVelocity(bodyB);


                float aAlongN = vA.x * ev.nx + vA.y * ev.ny;
                float bAlongN = vB.x * ev.nx + vB.y * ev.ny;

                float aImpact = std::fabs(aAlongN);
                float bImpact = std::fabs(bAlongN);

                const float MIN_IMPACT = 1.0f;
                if (aImpact < MIN_IMPACT && bImpact < MIN_IMPACT) {
                    break;
                }

                // daño cruzado: cada uno sufre por la velocidad del otro
                float damageA = bImpact * 0.4f;
                float damageB = aImpact * 0.4f;


                carA.applyDamage(damageA);
                carB.applyDamage(damageB);

                if (carA.isCarDestroy()) {
                    //le aviso a la interfaz
                }
                if (carB.isCarDestroy()) {
                    //le aviso a la interfaz
                }

                // frenar un poco
                b2Vec2 newVA = {
                        vA.x - ev.nx * aAlongN * 0.4f,
                        vA.y - ev.ny * aAlongN * 0.4f
                };
                b2Vec2 newVB = {
                        vB.x + ev.nx * bAlongN * 0.4f,
                        vB.y + ev.ny * bAlongN * 0.4f
                };

                b2Body_SetLinearVelocity(bodyA, newVA);
                b2Body_SetLinearVelocity(bodyB, newVB);

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
    const InitPlayer ip = dynamic_cast<const InitPlayer&>(*cmd.msg);

    b2Vec2 spawn = { 4.0f, 4.0f };
    cars.emplace(cmd.client_id, Car(this->worldManager, cmd.client_id, spawn, 0, ip.getCarType()));


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

    const auto& mv = dynamic_cast<const MoveMsg&>(*cmd.msg);

    it->second.applyControlsToBody(mv, TIME_STEP);

}

// la voy implementando aunque la logica del msj todavia no esta hecha
void GameLoop::disconnectHandler(ID id) {
    auto it = cars.find(id);
    worldManager.destroyEntity(it->second.getPhysicsId());
    cars.erase(id);
}

void GameLoop::broadcastCarSnapshots() {
    for (auto& [id, car] : cars) {
        PlayerState ps = car.snapshotState();
        auto base = std::static_pointer_cast<SrvMsg>(
                std::make_shared<PlayerState>(std::move(ps)));
        registry->broadcast(base); // todos los jugadores quieren saber tu posicion
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