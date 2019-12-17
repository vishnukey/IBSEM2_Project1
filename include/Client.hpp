#ifndef CLIENT__H__
#define CLIENT__H__

#include "options.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "Socket.hpp"

class Client : public Socket{
public:
        Client(int port, const char* addr_name);
        Client(int domain, int type, int port, const char* addr_name);
        Client(int domain,
                int type,
                int protocol,
                int port,
                const char* addr_name);
};

#endif
