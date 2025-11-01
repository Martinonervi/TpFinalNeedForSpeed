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
    void stop();
    void join();

    Socket peer;
    Queue<SrvMsgPtr> receiverQueue;
    Queue<CliMsgPtr> senderQueue;
    ClientReceiver receiver;
    ClientSender sender;
    Printer printer;
};
