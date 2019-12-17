#include "../include/ServerRunner.hpp"

ServerRunner::ServerRunner(Shared& share) : shared(share)
{

        shared.server = new Server(shared.PORT);;
}

void ServerRunner::operator()()
{
        const int SUCCESS = Header::MESSAGE | Header::WHOIS | Header::USERNAME | Header::OPEN;

        //initialize server
        shared.server->listen(3);
        shared.server->accept();
        shared.canRead = true;

        Header conn;
        shared.server->read((char*) &conn, sizeof(Header));

        //main shared.server->read loop while connected
        while (conn.conn_type && !(conn.conn_type & Header::CLOSE)){
                if (conn.conn_type & SUCCESS){
                        if (conn.conn_type & Header::MESSAGE){
                                if (shared.interlocutor_name == "" && shared.canWrite){
                                        Header data {Header::OPEN, 0};
                                        shared.client->write((char*) &data, sizeof(Header));
                                }
                                char* buff = new char[conn.data_len + 1];
                                shared.server->read(buff, conn.data_len);
                                std::cout << "["
                                        << shared.interlocutor_name
                                        << "] "
                                        << buff
                                        << std::endl;
                        }
                        if (conn.conn_type & Header::WHOIS){
                                Header data {Header::USERNAME,
                                        (int)shared.username.length()};
                                if (shared.canWrite){
                                        shared.client->write((char*)&data,
                                                                sizeof(Header));
                                        shared.client->write((char*)shared.username.c_str(),
                                                                (int)shared.username.length());
                                }
                        }
                        if (conn.conn_type & Header::USERNAME){
                                char* buff = new char[conn.data_len + 1];
                                shared.server->read(buff, conn.data_len);
                                shared.interlocutor_name = buff;
                        }
                }else{
                        Header data {Header::CLOSE, 0};
                        shared.server->write((char*) &data, sizeof(Header));
                        #ifdef DEBUG
                                std::cout << "bad header received with value: " << conn.conn_type << std::endl;
                        #endif
                        break;
                }

                shared.server->read((char*) &conn, sizeof(Header));
        }

        #ifdef DEBUG
                std::cout << "Stopping server" << std::endl;
        #endif
        shared.server->close();
}
