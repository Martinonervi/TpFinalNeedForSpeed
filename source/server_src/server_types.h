#pragma once
#include <cstdint>
#include <memory>
#include "../common_src/constants.h"
#include "../common_src/printer.h"
#include "../common_src/queue.h"

#define NITRO_TICKS 12
#define TICK_MS 250

using SendQ = Queue<SrvMsgPtr>;
using SendQPtr = std::shared_ptr<SendQ>;
using gameLoopQueue = Queue<Cmd>;


