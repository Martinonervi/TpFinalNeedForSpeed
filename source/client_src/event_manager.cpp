#include "event_manager.h"

#include "../common_src/cli_msg/cli_start_game.h"

EventManager::EventManager( ID& myCarId, ID& nextCheckpoint,
                                std::unordered_map<ID, std::unique_ptr<Car>>& cars,
                                SDL2pp::Renderer& renderer,
                                Queue<CliMsgPtr>& senderQueue,
                                SdlDrawer& drawer,
                                TextureManager& textureManager,
                                std::unordered_map<ID, std::unique_ptr<Checkpoint>>& checkpoints,
                                Hint& hint, UpgradeScreen& ups, Button& startBtn,
                                bool& showStart,
                                bool& running, bool& showMap, bool& quit,
                                float& raceTime, uint8_t& totalRaces, uint8_t& raceNumber,
                                std::unique_ptr<PlayerStats>& playerStats,
                                std::vector<RecommendedPoint>& pathArray,
                                Upgrade& upgrade,
                                std::vector<UpgradeDef>& upgradesArray)
:       myCarId(myCarId),
        nextCheckpoint(nextCheckpoint),
        cars(cars),
        checkpoints(checkpoints),
        renderer(renderer),
        senderQueue(senderQueue),
        drawer(drawer),
        tm(textureManager),
        hint(hint),
        ups(ups),
        startBtn(startBtn),
        showStart(showStart),
        running(running),
        showMap(showMap),
        quit(quit),
        playerStats(playerStats),
        raceTime(raceTime),
        totalRaces(totalRaces),
        raceNumber(raceNumber),
        pathArray(pathArray),
        upgrade(upgrade),
        upgradesArray(upgradesArray)
{}

void EventManager::handleEvents(AudioManager& audio) const {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            quit = false;
        } else if (event.type == SDL_KEYDOWN && myCarId != -1) {
            auto it = keyToMove.find(event.key.keysym.sym);
            if (it != keyToMove.end()) {
                auto msg = std::make_shared<MoveMsg>(it->second);
                senderQueue.push(msg);
            } else if (event.key.keysym.sym == SDLK_m) {
                showMap = !showMap;
            }
        }

        if (event.type == SDL_MOUSEMOTION) {
            if (showStart) {
                ups.handleMouseMotion(event.motion.x, event.motion.y);
                startBtn.handleHover(event.motion.x, event.motion.y);
            }
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (showStart) {
                auto [bought, upType] = ups.handleMouseClick();
                if (bought) {
                    RequestUpgrade reqUp(upType);
                    senderQueue.push(std::make_shared<RequestUpgrade>(reqUp));
                }

                if (startBtn.getHover()) {
                    StartGame strGame;
                    senderQueue.push(std::make_shared<StartGame>(strGame));
                    showStart = false;
                    audio.playMusic("game_music", -1);
                }
            }
        }
    }
}



void EventManager::handleServerMessage(const SrvMsgPtr& msg, AudioManager& audio) {
    switch (msg->type()) {
        case INIT_PLAYER: {
            const auto sp = dynamic_cast<const SendPlayer&>(*msg);
            myCarId = sp.getPlayerId();
            std::cout << "Bienvenido Player:" << myCarId << std::endl;
            cars[myCarId] = std::make_unique<Car>(renderer, tm, sp.getX(),
                                                  sp.getY(), sp.getCarType(), sp.getAngleRad());

            break;
        }
        case NEW_PLAYER: {
            const auto snc = dynamic_cast<const NewPlayer&>(*msg);
            auto it = cars.find(snc.getPlayerId());
            if (it == cars.end()) {
                std::cout << "Se Unio Player:" << snc.getPlayerId() << std::endl;
                cars[snc.getPlayerId()] = std::make_unique<Car>(renderer, tm, snc.getX(),
                                                                snc.getY(), snc.getCarType(), snc.getAngleRad());
            }

            break;
        }
        case Movement: {
            const auto ps = dynamic_cast<const PlayerState&>(*msg);

            if (cars.count(ps.getPlayerId())) {
                cars[ps.getPlayerId()]->update(
                    ps.getX()*PIXELS_PER_METER,
                    ps.getY()*PIXELS_PER_METER,
                    ps.getAngleRad());
            }
            break;
        }
        case COLLISION: {
            const auto ch = dynamic_cast<const SrvCarHitMsg&>(*msg);
            if (cars.count(ch.getPlayerId())) {
                cars[ch.getPlayerId()]->setHealth(ch.getCarHealth());
                if (ch.getPlayerId()==myCarId) {
                    if (ch.getCarHealth() == 0) {
                        audio.playSound("explosion");
                    } else {
                        audio.playSound("crash");
                    }
                }
            }
            break;
        }

        case CLIENT_DISCONNECT: {
            const auto disconnect = dynamic_cast<const ClientDisconnect&>(*msg);
            cars.erase(disconnect.getPlayerId());
            if (disconnect.getPlayerId() == myCarId) {
                myCarId = -1;
                running = false;
            }
            break;
        }
        case CHECKPOINT_HIT: {
            const auto check_hit = dynamic_cast<const SrvCheckpointHitMsg&>(*msg);
            if ( nextCheckpoint == check_hit.getCheckpointId() && myCarId == check_hit.getPlayerId()) {
                audio.playSound("checkpoint");
                checkpoints[nextCheckpoint]->setInactive();
            }
            break;
        }
        case CURRENT_INFO: {
            const auto current = dynamic_cast<const SrvCurrentInfo&>(*msg);
            if (current.getRaceNumber() != lastRaceNumber) {
                checkpoints.clear();
            }

            lastRaceNumber = current.getRaceNumber();
            if (!checkpoints.count(current.getNextCheckpointId())) {
                checkpoints[current.getNextCheckpointId()] = std::make_unique<Checkpoint>(renderer, drawer, tm,
                    current.getCheckX()*PIXELS_PER_METER, current.getCheckY()*PIXELS_PER_METER);
            }
            nextCheckpoint = current.getNextCheckpointId();
            if (myCarId != -1) {
                auto itCar = cars.find(myCarId);
                if (itCar != cars.end() && itCar->second) {
                    hint.update(current.getAngleHint(),
                                current.getDistanceToCheckpoint()*PIXELS_PER_METER,
                                itCar->second->getX(),
                                itCar->second->getY());
                    itCar->second->setSpeed(current.getSpeed());
                }
            }


            raceTime = current.getRaceTimeSeconds();
            raceNumber = current.getRaceNumber();
            totalRaces = current.getTotalRaces();

            break;
        }
        case STATS: {
            running = false;
            quit = true;
            const auto* raw = dynamic_cast<const PlayerStats*>(msg.get());
            playerStats = std::make_unique<PlayerStats>(*raw);
            break;
        }
        case RECOMMENDED_PATH: {
            const auto recommendedPath = dynamic_cast<const RecommendedPath&>(*msg);
            pathArray = recommendedPath.getPath();
            break;
        }
        case UPGRADE_SEND: {
            const auto sendUpgrade = dynamic_cast<const SendUpgrade&>(*msg);
            if (sendUpgrade.couldBuy()) {
                upgrade = sendUpgrade.getUpgrade();
                audio.stopSound("purchase");
                audio.playSound("purchase");
            }
            break;
        }
        case UPGRADE_LOGIC: {
            const auto upgrades = dynamic_cast<const UpgradeLogic&>(*msg);
            upgradesArray = upgrades.getUpgrades();
            ups.createButtons(upgradesArray);
            break;
        }
        case SRV_DISCONNECTION:{
            std::cout << "Server Disconnection" << std::endl;
            running = false;
            break;
        }
        default:
            break;
    }
}