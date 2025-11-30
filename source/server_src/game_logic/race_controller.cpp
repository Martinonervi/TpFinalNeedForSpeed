#include "race_controller.h"

#include <chrono>
#include <cmath>

#include <box2d/box2d.h>

#include "../../common_src/cli_msg/cli_cheat_request.h"
#include "../../common_src/srv_msg/client_disconnect.h"
#include "../../common_src/srv_msg/srv_car_hit_msg.h"
#include "../../common_src/srv_msg/srv_current_info.h"
#include "../../common_src/srv_msg/srv_race_finished.h"

using Clock = std::chrono::steady_clock;

RaceController::RaceController(std::shared_ptr<gameLoopQueue> queue,
                               ClientsRegistry& registry,
                               WorldManager& worldManager,
                               std::queue<WorldEvent>& worldEvents,
                               std::unordered_map<ID, Car>& playerCars,
                               std::unordered_map<ID, Checkpoint>& checkpoints,
                               PlayerManager& playerManager,
                               WorldEventHandlers& eventHandlers,
                               const Config& config,
                               float& raceTimeSeconds,
                               bool& raceEnded,
                               uint8_t& totalCars,
                               uint8_t& finishedCarsCount,
                               std::vector<RaceResult>& lastRaceResults,
                               std::unordered_map<ID, Car>& npcCars)
    : queue(std::move(queue))
    , registry(registry)
    , worldManager(worldManager)
    , worldEvents(worldEvents)
    , playerCars(playerCars)
    , checkpoints(checkpoints)
    , playerManager(playerManager)
    , eventHandlers(eventHandlers)
    , config(config)
    , raceTimeSeconds(raceTimeSeconds)
    , raceEnded(raceEnded)
    , totalCars(totalCars)
    , finishedCarsCount(finishedCarsCount)
    , lastRaceResults(lastRaceResults)
    , npcCars(npcCars)
{}

void RaceController::runRace(uint8_t raceIndex,
                             uint8_t totalRaces,
                             const std::function<bool()>& shouldKeepRunning) {
    raceTimeSeconds = 0.f;
    raceEnded       = false;

    auto raceStartTime = Clock::now();
    try {
        ConstantRateLoop loop(config.loops.raceHz);

        while (shouldKeepRunning()) {
            checkPlayersStatus();
            processCmds();

            worldManager.step(config.physics.timeStep,
                              config.physics.subStepCount);

            processWorldEvents();

            if (!raceEnded) {
                auto now = Clock::now();
                std::chrono::duration<float> elapsed = now - raceStartTime;
                raceTimeSeconds = elapsed.count();

                if (raceTimeSeconds >= config.lobby.maxRaceTimeSec) {
                    raceEnded = true;
                }
                if (totalCars <= 0) {
                    raceEnded = true;
                }
            }

            if (raceEnded) break;

            playerManager.broadcastSnapshots();
            sendCurrentInfo(raceIndex, totalRaces);

            loop.sleep_until_next_frame();
        }
    } catch (const std::exception& e) {
        std::cerr << "[RaceController] fatal: " << e.what() << "\n";
    } catch (...) {
        std::cerr << "[RaceController] fatal: unknown\n";
    }

    if (shouldKeepRunning){
        finalizeDNFs();
        sendRaceFinish();
    }

}

void RaceController::sendRaceFinish() {
    auto msg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<RaceFinished>());
    registry.broadcast(msg);
}

std::list<Cmd> RaceController::emptyQueue() {
    std::list<Cmd> cmd_list;
    Cmd cmd_aux;
    try {
        while (queue->try_pop(cmd_aux)) {
            cmd_list.push_back(std::move(cmd_aux));
        }
    } catch (const ClosedQueue&) {
        // queue cerrada: no hay más comandos
    }
    return cmd_list;
}

void RaceController::broadcastNpcCars() {
    for (auto& [id, npc] : npcCars) {
        auto pos = npc.getPosition();
        auto msg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<NewPlayer>(
                id,
                npc.getCarType(),
                pos.x, pos.y,
                npc.getAngleRad()
            )
        );
        registry.broadcast(msg);
    }
}

void RaceController::checkPlayersStatus() {
    std::vector<ID> ids;
    ids.reserve(playerCars.size());

    for (auto& [id, car] : playerCars) {
        ids.push_back(id);
    }

    std::vector<ID> toDisconnect = registry.checkClients(ids);

    for (ID idToDisconnect : toDisconnect) {
        disconnectHandler(idToDisconnect);

        std::cout << "[RaceController] auto con id: "
                  << idToDisconnect << " borrado\n";

        auto msg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<ClientDisconnect>(idToDisconnect));
        registry.broadcast(msg);
    }
}

void RaceController::processCmds() {
    std::list<Cmd> to_process = emptyQueue();

    for (Cmd& cmd : to_process) {
        if (!registry.contains(cmd.client_id)) {
            continue;
        }

        switch (cmd.msg->type()) {
            case Opcode::Movement: {
                playerManager.handleMovement(cmd, config.physics.timeStep);
                break;
            }
            case Opcode::INIT_PLAYER: {
                // por si alguien se intenta unir tarde, se maneja en PlayerManager
                playerManager.initPlayer(cmd);
                break;
            }
            case Opcode::REQUEST_CHEAT: {
                if (!config.cheats.enabled) break;

                const auto& cr = dynamic_cast<const CheatRequest&>(*cmd.msg);
                Cheat cheat = cr.getCheat();

                switch (cheat) {
                    case Cheat::HEALTH_CHEAT:
                    case Cheat::FREE_SPEED_CHEAT:
                    case Cheat::NEXT_CHECKPOINT_CHEAT:
                        playerManager.cheatHandler(cmd);
                        break;

                    case Cheat::WIN_RACE_CHEAT:
                        if (!config.cheats.allowWinRaceCheat) break;
                        forcePlayerWin(cmd.client_id);
                        break;

                    case Cheat::LOST_RACE_CHEAT:
                        if (!config.cheats.allowLostRaceCheat) break;
                        forcePlayerLose(cmd.client_id);
                        break;

                    default:
                        std::cout << "[RaceController] cheat desconocido: "
                                  << (int)cheat << "\n";
                        break;
                }
                break;
            }
            default: {
                std::cout << "[RaceController] comando desconocido: "
                          << static_cast<int>(cmd.msg->type()) << "\n";
                break;
            }
        }
    }
}

void RaceController::processWorldEvents() {
    std::unordered_set<ID>      alreadyHitBuildingThisFrame;
    std::unordered_set<uint64_t> alreadyHitCarPairThisFrame;

    while (!worldEvents.empty()) {
        WorldEvent ev = worldEvents.front();
        worldEvents.pop();

        switch (ev.type) {
            case WorldEventType::CarHitCheckpoint:
                eventHandlers.CarHitCheckpointHandler(ev);
                break;
            case WorldEventType::CarHitBuilding:
                eventHandlers.CarHitBuildingHandler(ev, alreadyHitBuildingThisFrame);
                break;
            case WorldEventType::CarHitCar:
                eventHandlers.CarHitCarHandler(ev, alreadyHitCarPairThisFrame);
                break;
            default:
                break;
        }
    }
}

void RaceController::sendCurrentInfo(uint8_t raceIndex,
                                     uint8_t totalRaces) {
    for (auto& [id, car] : playerCars) {
        ID actual = car.getActualCheckpoint();
        ID next   = actual + 1;

        auto itCp = checkpoints.find(next);
        if (itCp == checkpoints.end()) continue; // ya terminó
        const Checkpoint& cp = itCp->second;

        b2BodyId body = car.getBody();
        b2Vec2 pos    = b2Body_GetPosition(body);

        float vx = cp.getX() - pos.x;
        float vy = cp.getY() - pos.y;

        float len   = std::sqrt(vx * vx + vy * vy);
        float angle = std::atan2(vy, vx);

        b2Vec2 vecVel = b2Body_GetLinearVelocity(body);
        float  speed  = std::sqrt(vecVel.x * vecVel.x + vecVel.y * vecVel.y);

        uint8_t totalCheckpoints = static_cast<uint8_t>(checkpoints.size());

        SrvCurrentInfo ci(
            cp.getId(),
            cp.getX(), cp.getY(),
            angle,
            len,
            config.lobby.maxRaceTimeSec - raceTimeSeconds,
            raceIndex + 1,
            speed,
            totalRaces,
            totalCheckpoints
        );

        auto base = std::static_pointer_cast<SrvMsg>(
                std::make_shared<SrvCurrentInfo>(std::move(ci)));
        registry.sendTo(id, base);
    }
}

void RaceController::disconnectHandler(ID id) {
    auto it = playerCars.find(id);
    if (it == playerCars.end()) return;

    if (!it->second.isFinished()) {
        if (totalCars > 0) {
            totalCars--;
        }
        if (totalCars > 0 && finishedCarsCount >= totalCars) {
            raceEnded = true;
        }
        if (totalCars == 0) {
            raceEnded = true;
        }
    }
    playerManager.disconnectPlayer(id);
}

void RaceController::forcePlayerLose(ID id) {
    auto itCar = playerCars.find(id);
    if (itCar == playerCars.end()) return;

    Car& car = itCar->second;
    if (car.isFinished()) return;

    eventHandlers.setKillCar(car);

    auto msg = std::static_pointer_cast<SrvMsg>(
        std::make_shared<SrvCarHitMsg>(id,
                                       car.getHealth(),
                                       car.getTotalHealth()));
    registry.broadcast(msg);
    // al final de la carrera, finalizeDNFs() lo va a marcar como DNF.
}

void RaceController::forcePlayerWin(ID id) {
    auto itCar = playerCars.find(id);
    if (itCar == playerCars.end()) return;

    Car& car = itCar->second;
    if (car.isFinished()) return;

    car.setCheckpoint(checkpoints.size());
    eventHandlers.CarFinishRace(car);
}

void RaceController::finalizeDNFs() {
    for (auto& [id, car] : playerCars) {
        if (!car.isFinished()) {
            lastRaceResults.push_back(RaceResult{
                id,
                raceTimeSeconds,
                0,
                false
            });
        }
    }
}
