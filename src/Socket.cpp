#include "../include/Socket.hpp"

Socket::Socket(){ }

Socket::Socket(int domain, int type, int port)
{
        sockfd = socket(domain, type, 0);
        portno = port;

        if (sockfd < 0) throw Socket::bad_socket("Socket could not be made");
        #ifdef DEBUG
                printf("Socket made\n");
        #endif
}

Socket::Socket(int domain, int type, int protocol, int port)
{
        sockfd = socket(domain, type, protocol);
        portno = port;

        if (sockfd < 0) throw Socket::bad_socket("Socket could not be made");
}

Socket::Socket(int fd, struct sockaddr_in _addr)
{
        sockfd = fd;
        addr = _addr;
        portno = _addr.sin_port;

        if (fd < 0) throw Socket::bad_socket("No socket to connect to");
        #ifdef DEBUG
                printf("Socket made\n");
        #endif
}


void Socket::read(char* buff, int len)
{
        #ifdef DEBUG
                printf("Starting to read\n");
        #endif
        if (::read(sockfd, buff, len) < 0) throw Socket::bad_socket("Bad read");
        #ifdef DEBUG
                printf("Read data: %s\n", buff);
        #endif
}

void Socket::write(char* data, int len)
{
        #ifdef DEBUG
                printf("Starting to write\n");
        #endif
        if (::write(sockfd, data, len) < 0)
                throw Socket::bad_socket("Bad write");
        #ifdef DEBUG
                printf("Data sent!\n");
        #endif
}

void Socket::close()
{
        ::close(sockfd);
}
