#include "../include/Client.hpp"

#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

Client::Client(int port, const char* addr_name) : Socket(AF_INET,
                                                        SOCK_STREAM,
                                                        port)
{
        struct hostent* server = gethostbyname(addr_name);
        bcopy((char*)server->h_addr,
                (char*)&addr.sin_addr.s_addr,
                server->h_length);
        addr.sin_port = htons(port);
        addr.sin_family = AF_INET;

        if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
                throw Socket::bad_socket("Bad connect with socket");
}

Client::Client(int domain,
                int type,
                int port,
                const char* addr_name) : Socket(domain, type, port)
{
        struct hostent* server = gethostbyname(addr_name);
        bcopy((char*)server->h_addr,
                (char*)&addr.sin_addr.s_addr,
                server->h_length);
        addr.sin_port = htons(port);
        addr.sin_family = domain;

        if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
                throw Socket::bad_socket("Bad connect with socket");
}

Client::Client(int domain,
                int type,
                int protocol,
                int port,
                const char* addr_name) : Socket(domain, type, protocol, port)
{
        struct hostent* server = gethostbyname(addr_name);
        bcopy((char*)server->h_addr,
                (char*)&addr.sin_addr.s_addr,
                server->h_length);

        this->addr.sin_port = htons(port);

        if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
                throw Socket::bad_socket("Bad connect with socket");
}
