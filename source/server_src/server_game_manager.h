//
// Created by Felipe Fialayre on 03/11/2025.
//
#pragma once
#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <map>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include "../common_src/constants.h"
#include "server_types.h"
#include "../common_src/queue.h"
#include "server_client_registry.h"
#include "server_game_context.h"


class GameManager {
public:

    GameManager() = default;
    GameManager(const GameManager&) = delete;
    GameManager& operator=(const GameManager&) = delete;

    std::shared_ptr<gameLoopQueue> CreateJoinGame(ID game_id, SendQPtr sender_queue, ID client_id);
    // baja: remueve si existe
    void EraseQueue(ID id);
    // tamaño actual
    int size() const;

private:
    mutable std::mutex mx;
    ID last_id{0};
    std::unordered_map<ID, GameContext> games;  // server → gamecontext (gameloopqueue y registry)
};



#endif //GAMEMANAGER_H
