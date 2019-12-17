#ifndef SERVER__H__
#define SERVER__H__

#include "options.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Socket.hpp"

class Server : public Socket{
        Socket client;
public:
        Server (int port);
        Server(int domain, int type, int port);
        Server(int domain, int type, int protocol, int port);

        virtual void close();

        void listen(int backlog);

        void accept();

        virtual void write(char* data, int len);

        virtual void read(char* buf, int len);
};

#endif
