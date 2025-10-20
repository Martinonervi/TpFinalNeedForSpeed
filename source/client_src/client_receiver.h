#include "client_types.h"
#include "client_protocol.h"

class ClientReceiver: public Thread{
    public:

    ClientReceiver(Socket& peer_socket);

    protected:

    void run() override;

    private:

    ClientProtocol protocol;
    Printer printer;
};
