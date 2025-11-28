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
    bool lobbyState(); // lógica disparadora de QT (lobby), devuelve true
                       // si el cliente nunca entró en partida
    void postGame(PlayerStats& stats);  // lógica disparadora de QT (estadísticas)
    void stop();
    void join();

    Socket peer;
    Queue<SrvMsgPtr> receiverQueue;
    Queue<CliMsgPtr> senderQueue;
    ClientProtocol protocol;
    ClientReceiver receiver;
    ClientSender sender;
    bool in_game{false};
};
