#include "client_types.h"
#include "client_protocol.h"

class ClientSender: public Thread{
    public:

    ClientSender(Socket& peer_sock);

    bool is_listening();

    protected:
    
    void run() override;

    private:

    bool parseLine(const std::string& line, std::string& cmd, std::string& param);

    ClientProtocol protocol;
    bool listening{true};
};
