#include "client_types.h"
#include "client_protocol.h"

class ClientReceiver: public Thread{
    public:

    ClientReceiver(Socket& peer_socket, Queue<std::string>& recv_queue);

    protected:

    void run() override;

    private:

    ClientProtocol protocol;
    Queue<std::string>& recv_queue;
    Printer printer;
};
