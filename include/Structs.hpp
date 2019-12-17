#ifndef STRUCTS__H__
#define STRUCTS__H__

#include "Socket.hpp"
#include "Server.hpp"
#include "Client.hpp"

struct Header{
        enum TYPE
        {
                ERR      = 0 << 0,
                OPEN     = 1 << 1,
                MESSAGE  = 1 << 2,
                WHOIS    = 1 << 3,
                USERNAME = 1 << 4,
                CLOSE    = 1 << 5
        };
        TYPE conn_type;
        int data_len;
};

struct Shared{
        int PORT = 9999;
        std::string username;
        std::string interlocutor_name;
        std::string host;
        Server* server;
        Client* client;
        bool canRead;
        bool canWrite;
};

#endif
