#pragma once
#include <string>
#include "client_receiver.h"
#include "client_sender.h"
#include "client_types.h"
#include "../common_src/constants.h"

class Client {
public:
    Client(const char* host, const char* service);

    void run();  // lógica disparadora del cliente
private:
    void close();  // cierra el socket (por si hiciera falta cerrar más cosas)antes de irse
                   // ordenadamente
    void lobbyState();
    void sendRequest(int game_id);
    void recvGame();
    void stop();
    void join();

    Socket peer;
    Queue<SrvMsgPtr> receiverQueue;
    Queue<CliMsgPtr> senderQueue;
    ClientProtocol protocol;
    ClientReceiver receiver;
    ClientSender sender;
    Printer printer;
    bool in_game{false};
};
