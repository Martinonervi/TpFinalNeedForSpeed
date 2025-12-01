#include "race_controller.h"
#include <chrono>
#include <cmath>
#include <algorithm>
#include <vector>
#include <box2d/box2d.h>
#include "../../common_src/cli_msg/cli_cheat_request.h"
#include "../../common_src/srv_msg/client_disconnect.h"
#include "../../common_src/srv_msg/srv_car_hit_msg.h"
#include "../../common_src/srv_msg/srv_current_info.h"
#include "../../common_src/srv_msg/srv_race_finished.h"
#include "../../common_src/srv_msg/srv_npc_spawn.h"

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

void RaceController::runRace(uint8_t raceIndex, uint8_t totalRaces,
                             const std::function<bool()>& shouldKeepRunning) {
    raceTimeSeconds = 0.f;
    raceEnded       = false;
    //broadcastNpcSpawn();
    auto raceStartTime = Clock::now();

    try {
        ConstantRateLoop loop(config.loops.raceHz);

        while (shouldKeepRunning()) {
            checkPlayersStatus(); //desconecta los que se fueron (no estan en el registry)
            processCmds(); // MOVEMENT CHEATS INIT_PLAYER(devuelve false en este loop)
            worldManager.step(config.physics.timeStep, config.physics.subStepCount);
            processWorldEvents(); // colisiones checkpoints

            if (!raceEnded) {
                updateRaceClockAndCheckEnd(raceStartTime);
            }
            if (raceEnded) break;

            playerManager.broadcastSnapshots(); //posicion de los autos
            //broadcastNpcSnapshots();
            sendCurrentInfo(raceIndex, totalRaces); //info que fran tiene que dibujar
            loop.sleep_until_next_frame();
        }
    } catch (const std::exception& e) {
        std::cerr << "[RaceController] fatal: " << e.what() << "\n";
    }

    if (shouldKeepRunning()){
        finalizeDNFs(); //le pone tiempo final a los destruidos
        bool isLastRace = (raceIndex + 1 == totalRaces);
        if (!isLastRace) {
            sendRaceFinish(); //msj para q cliente ponga logica de compras
        }
    }
}


void RaceController::updateRaceClockAndCheckEnd(const Clock::time_point& raceStartTime) {
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

void RaceController::broadcastNpcSpawn() {
    for (auto& [id, npc] : npcCars) {
        auto pos = npc.getPosition();
        float angle = npc.getAngleRad();

        auto msg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<SrvNpcSpawn>(
                id,
                npc.getCarType(),
                pos.x,
                pos.y,
                angle
            )
        );
        registry.broadcast(msg);
    }
}


void RaceController::broadcastNpcSnapshots() {
    for (auto& [id, car] : npcCars) {
        PlayerState ps = car.snapshotState();
        auto base = std::static_pointer_cast<SrvMsg>(
                std::make_shared<PlayerState>(std::move(ps)));
        registry.broadcast(base);
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
    // 1) Construyo el ranking online completo
    auto ranking = buildLiveRanking();

    // 2) Mapeo carId -> posición en el ranking (1,2,3,...)
    std::unordered_map<ID, uint8_t> livePosById;
    livePosById.reserve(ranking.size());

    uint8_t pos = 1;
    for (const auto& e : ranking) {
        livePosById[e.id] = pos++;
    }

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

        // Puesto online de este jugador
        uint8_t livePos = 0;
        auto itPos = livePosById.find(id);
        if (itPos != livePosById.end()) {
            livePos = itPos->second;
        }

        //std::cout << "[RaceController] Id: " << car.getClientId()
        //<< " con ranking: " << static_cast<int>(livePos) << "\n";

        SrvCurrentInfo ci(
            cp.getId(),
            cp.getX(), cp.getY(),
            angle,
            len,
            config.lobby.maxRaceTimeSec - raceTimeSeconds,
            raceIndex + 1,
            speed,
            totalRaces,
            totalCheckpoints,
            livePos
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

    ID finishId = 0;
    for (const auto& [cpId, cp] : checkpoints) {
        if (cp.getKind() == CheckpointKind::Finish && cpId > finishId) {
            finishId = cpId;
        }
    }
    Checkpoint& cp = checkpoints.at(finishId);

    car.setCheckpoint(finishId - 1); //para q el handler lo valide
    car.setPosition(cp.getX(), cp.getY());

    // fabrico el evento
    WorldEvent ev{};
    ev.type         = WorldEventType::CarHitCheckpoint;
    ev.carId        = id;
    ev.checkpointId = finishId;

    eventHandlers.CarHitCheckpointHandler(ev);
}

void RaceController::finalizeDNFs() {
    for (auto& [id, car] : playerCars) {
        if (!car.isFinished()) {
            lastRaceResults.push_back(RaceResult{
                id,
                raceTimeSeconds + car.getPenalty(),
                0,
                false
            });
        }
    }
}

std::vector<LiveRankEntry> RaceController::buildLiveRanking() {
    std::vector<LiveRankEntry> v;
    v.reserve(playerCars.size());

    for (auto& [id, car] : playerCars) {
        LiveRankEntry e{};
        e.id               = id;
        e.finished         = car.isFinished();
        e.actualCheckpoint = car.getActualCheckpoint();

        if (e.finished) {
            e.finishTime = car.getFinishTimeNoPenalty();
            e.distToNext = 0.f;
        } else {
            ID next = e.actualCheckpoint + 1;
            auto itCp = checkpoints.find(next);
            if (itCp == checkpoints.end()) {
                e.distToNext = 0.f;
            } else {
                const Checkpoint& cp = itCp->second;
                b2BodyId body = car.getBody();
                b2Vec2   pos  = b2Body_GetPosition(body);
                float dx = cp.getX() - pos.x;
                float dy = cp.getY() - pos.y;
                e.distToNext = std::sqrt(dx * dx + dy * dy);
            }
            // Para los que no terminaron, el finishTime no importa,
            // dejo algo grande por las dudas.
            e.finishTime = 1e9f;
        }

        v.push_back(e);
    }
    std::sort(v.begin(), v.end(),
          [this](const LiveRankEntry& a, const LiveRankEntry& b) {
              return liveRankLess(a, b);
          });
    return v;
}

bool RaceController::liveRankLess(const LiveRankEntry& a, const LiveRankEntry& b) {
    // terminados primero
    if (a.finished != b.finished)
        return a.finished > b.finished;

    // ambos terminaron, veo quien llego antes
    if (a.finished && b.finished)
        return a.finishTime < b.finishTime;

    // ninguno termino, veo cehckpoint
    if (a.actualCheckpoint != b.actualCheckpoint)
        return a.actualCheckpoint > b.actualCheckpoint;

    // mismo checkpoint, veo la dist al proximo
    return a.distToNext < b.distToNext;
}

