#ifndef SOCKET__H__
#define SOCKET__H__

#include "options.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <errno.h>

#include <exception>
#include <iostream>

class Socket{
protected:
        int sockfd, portno;
        struct sockaddr_in addr;

public:
        Socket();
        Socket(int domain, int type, int port);
        Socket(int domain, int type, int protocol, int port);
        Socket(int fd, struct sockaddr_in addr);

        virtual void close();

        virtual void read(char* buff, int len);

        virtual void write(char* data, int len);

        class bad_socket : public std::exception{
                const char* _msg;

                public:
                        bad_socket(const char* msg) : _msg(msg){
                                perror("Something went wrong!");
                        }

                        virtual const char* what() const throw(){
                                return _msg;
                        }
        };
};

#endif
