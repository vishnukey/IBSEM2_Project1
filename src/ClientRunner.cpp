#include "../include/ClientRunner.hpp"

ClientRunner::ClientRunner(Shared& share) : shared(share)
{
        using namespace std;
        cout << "what is your username?" << endl;
        cin >> shared.username;
        cout << "Who are you connecting to?" << endl;
        cin >> shared.host;

        shared.client = new Client(shared.PORT, shared.host.c_str());
}

/*ClientRunner::~ClientRunner()
{
        std::cout << "Stopping client" << std::endl;
        Header data {Header::CLOSE, 0};
        shared.client->write((char*) &data, sizeof(Header));
}*/

void ClientRunner::operator()()
{
        shared.canWrite = true;
        Header data {Header::OPEN, 0};
        shared.client->write((char*) &data, sizeof(Header));
        while(1){
                #ifdef DEBUG
                        std::cout << "starting message loop" << std::endl;
                #endif
                std::string message;
                std::getline(std::cin, message);
                Header messageHeader {Header::MESSAGE, (int)message.length()};
                shared.client->write((char*) &messageHeader, sizeof(Header));
                shared.client->write((char*)message.c_str(), (int)message.length());
        }
        #ifdef DEBUG
                std::cout << "message loop ended" << std::endl;
        #endif
        
        Header close {Header::CLOSE, 0};
        shared.client->write((char*) &close, sizeof(Header));
}
