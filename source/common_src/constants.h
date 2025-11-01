// path: common/constants.h
#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "opcodes.h"
#include "client_msg.h"
#include "move_Info.h"
#include "player_state.h"
#include "queue.h"
#include "server_msg.h"
#include "socket.h"
#include "thread.h"


#define NITRO_ON_MSG "A car hit the nitro!"
#define NITRO_OFF_MSG "A car is out of juice."

#define RETURN_SUCCESS  0
#define RETURN_FAILURE  1

using Cars_W_Nitro = std::uint16_t;

using ID = std::uint32_t;

using CliMsgPtr = std::shared_ptr<CliMsg>;
using SrvMsgPtr = std::shared_ptr<SrvMsg>;

//no veo ganancia en que Cmd sea una clase, cumple su funcion perfecta como struct
struct Cmd {
    ID client_id;
    std::shared_ptr<CliMsg> msg;
};


#define SMALL_CAR 32
#define MEDIUM_CAR 40
#define LARGE_CAR 48

enum CarType {
    CAR_GREEN,
    CAR_RED,
    CAR_PORSCHE,
    CAR_LIGHT_BLUE,
    CAR_JEEP,
    CAR_PICKUP,
    CAR_LIMO,
    CAR_COUNT
};

#define FRAMES_PER_CAR 16
#define FRAMES_PER_DIRECTION 8

struct CarInfo {
    int yOffset;
    int width;
    int height;
};