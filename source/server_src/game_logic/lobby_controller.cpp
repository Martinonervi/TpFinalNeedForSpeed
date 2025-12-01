#include "lobby_controller.h"
#include <cmath>
#include <chrono>

using Clock = std::chrono::steady_clock;

LobbyController::LobbyController(std::shared_ptr<gameLoopQueue> queue,
                                 ClientsRegistry& registry,
                                 PlayerManager& playerManager,
                                 std::unordered_map<ID, Car>& playerCars,
                                 const std::vector<UpgradeDef>& upgrades,
                                 const Config& config,
                                 bool& startRequested,
                                 bool& raceStarted,
                                 uint8_t& totalCars)
    : queue(std::move(queue))
    , registry(registry)
    , playerManager(playerManager)
    , playerCars(playerCars)
    , upgrades(upgrades)
    , config(config)
    , startRequested(startRequested)
    , raceStarted(raceStarted)
    , totalCars(totalCars)
{}

void LobbyController::runLobbyLoop(
        int raceIndex,
        const std::vector<RecommendedPoint>& recommendedPath,
        const std::function<bool()>& shouldKeepRunning) {

    ConstantRateLoop loop(config.loops.lobbyHz);

    startRequested = false;
    raceStarted    = false;

    auto start    = Clock::now();
    auto deadline = start + std::chrono::duration<double>(config.lobby.upgradesPhaseSec);

    while (shouldKeepRunning()) {
        processLobbyCmds();   // START_GAME, INIT_PLAYER, UPGRADE_REQUEST

        if (startRequested) break; //la primera vez START_GAME lo pone en true, desp es por tiempo
        if (raceIndex != 0 && Clock::now() >= deadline) break;

        auto now = Clock::now();
        float remaining_sec = std::chrono::duration_cast<std::chrono::duration<float>>(
                                  deadline - now
                              ).count();
        if (remaining_sec < 0.f) remaining_sec = 0.f;
        playerManager.broadcastSnapshots();
        broadcastTimeLeft(remaining_sec, true);
        broadcastRecommendedPath(recommendedPath);
        broadcastUpgradeLogic();

        loop.sleep_until_next_frame();
    }
    runPreRaceCountdown(shouldKeepRunning);
    raceStarted = true;
}

void LobbyController::runPreRaceCountdown(const std::function<bool()>& shouldKeepRunning) {
    ConstantRateLoop loop(config.loops.lobbyHz);

    auto cdStart    = Clock::now();
    auto cdDeadline = cdStart +
        std::chrono::duration<double>(config.lobby.preRaceCountdownSec);

    while (shouldKeepRunning()) {
        auto now = Clock::now();
        float remaining_sec =
            std::chrono::duration_cast<std::chrono::duration<float>>(
                cdDeadline - now
            ).count();

        if (remaining_sec < 0.f) remaining_sec = 0.f;
        broadcastTimeLeft(remaining_sec, false);
        if (remaining_sec <= 0.f) break;
        loop.sleep_until_next_frame();
    }
}

void LobbyController::processLobbyCmds() {
    std::list<Cmd> to_process = emptyQueue();

    for (Cmd& cmd : to_process) {
        if (!registry.contains(cmd.client_id)) {
            continue;
        }

        switch (cmd.msg->type()) {
            case Opcode::START_GAME: {
                startRequested = true;
                auto base = std::static_pointer_cast<SrvMsg>(
                        std::make_shared<StartingGame>());
                registry.broadcast(base);
                break;
            }
            case Opcode::UPGRADE_REQUEST: {
                playerManager.handleRequestUpgrade(cmd);
                break;
            }
            case Opcode::INIT_PLAYER: {
                bool ok = playerManager.initPlayer(cmd);
                if (ok) {
                    totalCars = static_cast<uint8_t>(playerCars.size());
                }
                break;
            }
            default:
                break;
        }
    }
}

void LobbyController::broadcastTimeLeft(float remaining_sec, bool upgradesEnabled) {
    uint8_t timeToSend = static_cast<uint8_t>(std::floor(remaining_sec));

    auto msg = std::static_pointer_cast<SrvMsg>(
            std::make_shared<TimeLeft>(timeToSend, upgradesEnabled));
    registry.broadcast(msg);
}

void LobbyController::broadcastUpgradeLogic() {
    auto ul = std::static_pointer_cast<SrvMsg>(
            std::make_shared<UpgradeLogic>(upgrades));
    registry.broadcast(ul);
}

void LobbyController::broadcastRecommendedPath(
        const std::vector<RecommendedPoint>& recommendedPath) {

    auto rp = std::static_pointer_cast<SrvMsg>(
            std::make_shared<RecommendedPath>(recommendedPath));
    registry.broadcast(rp);
}

std::list<Cmd> LobbyController::emptyQueue() {
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