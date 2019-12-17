#include "../include/Server.hpp"

Server::Server(int port) : Socket(AF_INET, SOCK_STREAM, port)
{
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        int err = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

        if (err < 0) throw Socket::bad_socket("Bad bind on socket");
        #ifdef DEBUG
                printf("socket bound\n");
        #endif
}

Server::Server(int domain, int type, int port) : Socket(domain, type, port)
{
        addr.sin_family = domain;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        int err = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

        if (err < 0) throw Socket::bad_socket("Bad bind on socket");

        #ifdef DEBUG
                printf("socket bound\n");
        #endif
}

Server::Server(int domain, int type, int protocol, int port)
                                                : Socket(domain,
                                                        type,
                                                        protocol,
                                                        port)
{
        addr.sin_family = domain;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
                throw Socket::bad_socket("Bad bind on socket");
}

void Server::listen(int backlog){
        #ifdef DEBUG
                printf("Starting to listen\n");
        #endif
        ::listen(sockfd, backlog);
        #ifdef BEBUG
                printf("Listening\n");
        #endif
}

void Server::accept(){
        #ifdef DEBUG
                printf("starting to accept\n");
        #endif
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        #ifdef DEBUG
                printf("%d", sockfd);
        #endif
        int clientfd = ::accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
        #ifdef DEBUG
                printf("%d", clientfd);
        #endif
        client = Socket(clientfd, cli_addr);
        #ifdef DEBUG
                printf("Accepted!\n");
        #endif
}

void Server::write(char* data, int len)
{
        client.write(data, len);
}

void Server::read(char* buff, int len)
{
        #ifdef DEBUG
                printf("passing read to client\n");
        #endif
        client.read(buff, len);
}

void Server::close()
{
        client.close();
        Socket::close();
}
