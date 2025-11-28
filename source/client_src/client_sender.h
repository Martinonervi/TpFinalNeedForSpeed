#pragma once
#include "../common_src/cli_msg/init_player.h"
#include "../common_src/cli_msg/move_Info.h"
#include "../common_src/constants.h"

#include "client_protocol.h"
#include "client_types.h"

class ClientSender: public Thread {
public:
    explicit ClientSender(ClientProtocol& protocol, Queue<CliMsgPtr>& senderQueue);
    bool is_listening() const;
    void run() override;

private:
    ClientProtocol& protocol;
    Queue<CliMsgPtr>& senderQueue;
};
