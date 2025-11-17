#include "event_manager.h"

#include "../common_src/srv_msg/client_disconnect.h"
#include "../common_src/srv_msg/srv_checkpoint_hit_msg.h"

EventManager::EventManager( ID& myCarId, ID& nextCheckpoint,
                                std::unordered_map<ID, std::unique_ptr<Car>>& cars,
                                SDL2pp::Renderer& renderer,
                                Queue<CliMsgPtr>& senderQueue,
                                TextureManager& textureManager,
                                std::unordered_map<ID, std::unique_ptr<Checkpoint>>& checkpoints,
                                Hint& hint,
                                bool& running, bool& showMap, bool& quit)
:       myCarId(myCarId),
        nextCheckpoint(nextCheckpoint),
        cars(cars),
        checkpoints(checkpoints),
        renderer(renderer),
        senderQueue(senderQueue),
        tm(textureManager),
        hint(hint),
        running(running),
        showMap(showMap),
        quit(quit)
{}

void EventManager::handleEvents() const {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
            quit = true;
        } else if (event.type == SDL_KEYDOWN && myCarId != -1) {
            auto it = keyToMove.find(event.key.keysym.sym);
            if (it != keyToMove.end()) {
                auto msg = std::make_shared<MoveMsg>(it->second);
                CliMsgPtr clientMsg = msg;
                senderQueue.push(clientMsg);
            } else if (event.key.keysym.sym == SDLK_m) {
                showMap = !showMap;
            }
        }
    }
}


void EventManager::handleServerMessage(const SrvMsgPtr& msg) const {
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
            // Lo vamos a mover
            if (!checkpoints.count(ps.getNextCheckpointId())) {
                checkpoints[ps.getNextCheckpointId()] = std::make_unique<Checkpoint>(renderer, tm,
                    ps.getCheckX()*PIXELS_PER_METER, ps.getCheckY()*PIXELS_PER_METER);
            }
            nextCheckpoint = ps.getNextCheckpointId();
            hint.update(0, 0, cars[myCarId]->getX(), cars[myCarId]->getY());
            //_______

            break;
        }
        case COLLISION: {
            const auto ch = dynamic_cast<const SrvCarHitMsg&>(*msg);
            if (cars.count(ch.getPlayerId())) {
                cars[ch.getPlayerId()]->setHealth(ch.getCarHealth());
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
                checkpoints[nextCheckpoint]->setInactive();
            }
        }
        default:
            break;
    }
}