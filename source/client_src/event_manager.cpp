#include "event_manager.h"

#include "../common_src/cli_msg/cli_start_game.h"
#include "../common_src/srv_msg/srv_starting_game.h"
#include "../common_src/srv_msg/srv_time_left.h"
#include "../common_src/srv_msg/srv_npc_spawn.h"

EventManager::EventManager( ID& myCarId, ID& nextCheckpoint,
                                uint8_t& totalCheckpoints, ID& checkpointNumber,
                                std::unordered_map<ID, std::unique_ptr<Car>>& cars,
                                std::unordered_map<ID, std::unique_ptr<Car>>& npcs,
                                SDL2pp::Renderer& renderer,
                                Queue<CliMsgPtr>& senderQueue,
                                SdlDrawer& drawer,
                                TextureManager& textureManager,
                                std::unordered_map<ID, std::unique_ptr<Checkpoint>>& checkpoints,
                                Hint& hint, UpgradeScreen& ups, Button& startBtn,
                                bool& showScreen,
                                bool& running, bool& quit,
                                int& raceTime, uint8_t& totalRaces, uint8_t& raceNumber,
                                std::unique_ptr<PlayerStats>& playerStats,
                                std::vector<RecommendedPoint>& pathArray,
                                std::vector<UpgradeDef>& upgradesArray,
                                bool& srvDisconnect, StartScreen& startScreen, int& countdown)
:       myCarId(myCarId),
        nextCheckpoint(nextCheckpoint),
        totalCheckpoints(totalCheckpoints),
        checkpointNumber(checkpointNumber),
        cars(cars),
        npcs(npcs),
        checkpoints(checkpoints),
        renderer(renderer),
        senderQueue(senderQueue),
        drawer(drawer),
        tm(textureManager),
        hint(hint),
        ups(ups),
        startBtn(startBtn),
        showScreen(showScreen),
        running(running),
        quit(quit),
        playerStats(playerStats),
        raceTime(raceTime),
        totalRaces(totalRaces),
        raceNumber(raceNumber),
        pathArray(pathArray),
        upgradesArray(upgradesArray),
        srvDisconnect(srvDisconnect),
        startScreen(startScreen),
        countdown(countdown)
{}

void EventManager::handleEvents(AudioManager& audio) const {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            quit = false;
        } else if (event.type == SDL_KEYDOWN && myCarId != -1) {
            auto itMove = keyToMove.find(event.key.keysym.sym);
            auto itCheat = keyToCheat.find(event.key.keysym.sym);
            if (itMove != keyToMove.end()) {
                auto msg = std::make_shared<MoveMsg>(itMove->second);
                senderQueue.push(msg);
            } else if (itCheat != keyToCheat.end() && !showScreen && countdown == NOT_ACCESSIBLE) {
                auto msg = std::make_shared<CheatRequest>(itCheat->second);
                senderQueue.push(msg);
            } else if (event.key.keysym.sym == SDLK_UP) {
                audio.raiseVolume();
            } else if (event.key.keysym.sym == SDLK_DOWN) {
                audio.lowerVolume();
            }
        }


        if (event.type == SDL_MOUSEMOTION) {
            if (showScreen) {
                ups.handleMouseMotion(event.motion.x, event.motion.y);
                if (startScreen.isStart())
                    startBtn.handleHover(event.motion.x, event.motion.y);
            }
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (showScreen) {
                auto [bought, upType] = ups.handleMouseClick();
                if (bought) {
                    RequestUpgrade reqUp(upType);
                    senderQueue.push(std::make_shared<RequestUpgrade>(reqUp));
                }

                if (startBtn.getHover() && startScreen.isStart()) {
                    StartGame strGame;
                    senderQueue.push(std::make_shared<StartGame>(strGame));
                    showScreen = false;
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
            cars[myCarId] = std::make_unique<Car>(renderer, tm, sp.getX(),
                                                  sp.getY(), sp.getCarType(), sp.getAngleRad());

            break;
        }
        case NEW_PLAYER: {
            const auto snc = dynamic_cast<const NewPlayer&>(*msg);
            auto it = cars.find(snc.getPlayerId());
            if (it == cars.end()) {
                cars[snc.getPlayerId()] = std::make_unique<Car>(renderer, tm, snc.getX(),
                                                                snc.getY(), snc.getCarType(), snc.getAngleRad());
            }

            break;
        }
        case Movement: {
            const auto ps = dynamic_cast<const PlayerState&>(*msg);
            countdown = NOT_ACCESSIBLE;

            if (cars.count(ps.getPlayerId())) {
                cars[ps.getPlayerId()]->update(
                    ps.getX()*PIXELS_PER_METER,
                    ps.getY()*PIXELS_PER_METER,
                    ps.getAngleRad());
            } else if (npcs.count(ps.getPlayerId())) {
                npcs[ps.getPlayerId()]->update(
                    ps.getX()*PIXELS_PER_METER,
                    ps.getY()*PIXELS_PER_METER,
                    ps.getAngleRad());
            }
            break;
        }
        case COLLISION: {
            const auto ch = dynamic_cast<const SrvCarHitMsg&>(*msg);
            if (cars.count(ch.getPlayerId())) {
                const float healthDiff = cars[ch.getPlayerId()]->getHealth() - ch.getCarHealth();
                const float maxHealthDiff = ch.getTotalHealth() - cars[ch.getPlayerId()]->getMaxHealth();
                cars[ch.getPlayerId()]->setMaxHealth(ch.getTotalHealth());
                cars[ch.getPlayerId()]->setHealth(ch.getCarHealth());

                if (cars[ch.getPlayerId()]->getInCamera()) {
                    if (ch.getCarHealth() == 0) {
                        audio.stopSound("explosion");
                        audio.playSound("explosion");
                    } else if (healthDiff > 1 || maxHealthDiff > 0 && ch.getPlayerId() == myCarId) {
                        audio.stopSound("crash");
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
            if ( check_hit.getCheckpointId() == totalCheckpoints ) {
                cars[check_hit.getPlayerId()]->setState(DESTROYED);
            }

            if ( myCarId == check_hit.getPlayerId() ) {
                audio.stopSound("checkpoint");
                audio.playSound("checkpoint");
                checkpoints[nextCheckpoint]->setInactive();
                checkpointNumber = check_hit.getCheckpointId();
            }
            break;
        }
        case CURRENT_INFO: {
            const auto current = dynamic_cast<const SrvCurrentInfo&>(*msg);
            if (current.getRaceNumber() != lastRaceNumber) {
                checkpoints.clear();
                checkpointNumber = 0;
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


            raceTime = static_cast<int>(current.getRaceTimeSeconds());
            raceNumber = current.getRaceNumber();
            totalRaces = current.getTotalRaces();
            totalCheckpoints = current.getTotalCheckpoints();

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
                const auto upgrade = sendUpgrade.getUpgrade();
                audio.stopSound("purchase");
                audio.playSound("purchase");
                ups.changeState(upgrade);
                cars[myCarId]->addUpgrade(upgrade);
            }
            break;
        }
        case UPGRADE_LOGIC: {
            const auto upgrades = dynamic_cast<const UpgradeLogic&>(*msg);
            upgradesArray = upgrades.getUpgrades();
            ups.createButtons(upgradesArray);
            break;
        }
        case SRV_DISCONNECTION: {
            srvDisconnect = true;
            running = false;
            break;
        }
        case STARTING_GAME: {
            std::cout << "STARTING_GAME" << std::endl;
            std::cout << countdown << std::endl;
            showScreen = false;
            startScreen.changeIsStart();
            ups.clearButtons();
            break;
        }
        case TIME: {
            const auto timeLeft = dynamic_cast<const TimeLeft&>(*msg);
            if (!startScreen.isStart()) showScreen = timeLeft.getTimeLeft() > 0 && timeLeft.getUpgradesEnabled();
            if (timeLeft.getUpgradesEnabled()) {
                startScreen.setTimeLeft(timeLeft.getTimeLeft());
            } else {
                audio.playSound("countdown");
                countdown = static_cast<int>(timeLeft.getTimeLeft()) + 1;
            }
            break;
        }
        case NPC_SPAWN: {
            const auto npcSpawn = dynamic_cast<const SrvNpcSpawn&>(*msg);
            auto it = npcs.find(npcSpawn.getId());
            if (it == npcs.end()) {
                npcs[npcSpawn.getId()] = std::make_unique<Car>(renderer, tm, npcSpawn.getX(),
                                                                npcSpawn.getY(), npcSpawn.getCarType(),
                                                                npcSpawn.getAngleRad());
            }
            std::cout << npcSpawn.getId() << std::endl;
            std::cout << npcSpawn.getX() << std::endl;
            std::cout << npcSpawn.getY() << std::endl;

            break;
        }
        default:
            break;
    }
}