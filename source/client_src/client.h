#pragma once
#include <string>

#include "client_receiver.h"
#include "client_sender.h"
#include "client_types.h"
#include "client_window.h"
#include "../common_src/constants.h"
#include "../common_src/queue.h"

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
    Queue<constants::SrvMsg> receiverQueue;
    Queue<constants::CliMsg> senderQueue;
    ClientReceiver receiver;
    ClientSender sender;
    Printer printer;
};
