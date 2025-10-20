#include <string>

#include "client_protocol.h"
#include "client_types.h"
#include "client_receiver.h"
#include "client_sender.h"

class Client {
public:
    Client(const char* host, const char* service);

    void Main();  // lógica disparadora del cliente
private:
    void close();  // cierra el socket (por si hiciera falta cerrar más cosas)antes de irse
                   // ordenadamente
    void stop();
    void join();

    Socket peer;
    ClientReceiver receiver;
    ClientSender sender;
    Printer printer;
};
