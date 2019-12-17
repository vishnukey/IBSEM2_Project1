#ifndef CLIENT__H__
#define CLIENT__H__

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
#ifndef CLIENTRUNNER__H__
#define CLIENTRUNNER__H__

#include "Structs.hpp"

class ClientRunner{
        Shared& shared;

public:
        ClientRunner(Shared& share);

        void operator()();
};

#endif
#ifndef SERVER__H__
#define SERVER__H__

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
#ifndef SERVERRUNNER__H__
#define SERVERRUNNER__H__

#include "Structs.hpp"

class ServerRunner{
        Shared& shared;

public:
        ServerRunner(Shared& share);

        void operator()();
};

#endif
#ifndef SOCKET__H__
#define SOCKET__H__

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
#ifndef STRUCTS__H__
#define STRUCTS__H__

#include "Socket.hpp"
#include "Server.hpp"
#include "Client.hpp"

struct Header{
        enum TYPE
        {
                ERR      = 1 << 0,
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
        const int PORT = 9999;
        std::string username;
        std::string interlocutor_name;
        char* host;
        Server* server;
        Client* client;
};

#endif
#include <thread>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <cstdio>
#include <iostream>

#include <stdlib.h>

#include "../include/Socket.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Structs.hpp"
#include "../include/ServerRunner.hpp"
#include "../include/ClientRunner.hpp"


int main(void)
{
        using namespace std;

        Shared share;
        ServerRunner s(share);
        thread receiver(s);

        ClientRunner c(share);
        thread sender(c);

        receiver.join();
        sender.join();
}
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
#include "../include/ClientRunner.hpp"

ClientRunner::ClientRunner(Shared& share) : shared(share)
{
        using namespace std;
        cout << "what is your username?" << endl;
        cin >> shared.username;
        cout << "Who are you connecting to?" << endl;
        cin >> shared.host;
        Client c(shared.PORT, shared.host);
        shared.client = &c;
}

void ClientRunner::operator()()
{
        Header data {Header::OPEN, 0};
        shared.client->write((char*) &data, sizeof(Header));
}
#include "../include/Server.hpp"

Server::Server(int port) : Socket(AF_INET, SOCK_STREAM, port)
{
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        int err = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

        if (err < 0) throw Socket::bad_socket("Bad bind on socket");

        printf("socket bound\n");
}

Server::Server(int domain, int type, int port) : Socket(domain, type, port)
{
        addr.sin_family = domain;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        int err = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

        if (err < 0) throw Socket::bad_socket("Bad bind on socket");

        printf("socket bound\n");
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
        printf("Starting to listen\n");
        ::listen(sockfd, backlog);
        printf("Listening\n");
}

void Server::accept(){
        printf("starting to accept\n");
        struct sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);
        int clientfd = ::accept(sockfd, (struct sockaddr*) &cli_addr, &clilen);
        client = Socket(clientfd, cli_addr);
        printf("Accepted!\n");
}

void Server::write(char* data, int len)
{
        client.write(data, len);
}

void Server::read(char* buff, int len)
{
        printf("passing read to client\n");
        client.read(buff, len);
}

void Server::close()
{
        client.close();
        Socket::close();
}
#include "../include/ServerRunner.hpp"

ServerRunner::ServerRunner(Shared& share) : shared(share)
{
        Server s(shared.PORT);
        shared.server = &s;
}

void ServerRunner::operator()()
{
        const int SUCCESS = Header::MESSAGE | Header::WHOIS | Header::USERNAME;

        //initialize server
        shared.server->listen(1);
        shared.server->accept();

        Header conn;
        shared.server->read((char*) &conn, sizeof(Header));
        if (conn.conn_type & Header::OPEN){
                Header data {Header::CLOSE, 0};
                shared.server->write((char*) &data, sizeof(Header));
                goto end; //BAD
        }

        //main shared.server->read loop while connected
        while (conn.conn_type && !(conn.conn_type & Header::CLOSE)){
                if (conn.conn_type & SUCCESS){
                        if (conn.conn_type & Header::MESSAGE){
                                char* buff = new char[conn.data_len + 1];
                                shared.server->read(buff, conn.data_len);
                                std::cout << "["
                                        << shared.interlocutor_name
                                        << "] "
                                        << buff
                                        << std::endl;
                        }
                        if (conn.conn_type & Header::WHOIS){
                                Header data {HEADER::USERNAME,
                                        share.username.length};
                                share.client->write((char*)data,
                                                        sizeof(Header));
                                share.client->write(share.username.cstring,
                                                        share.username.length);
                        }
                        if (conn.conn_type & Header::USERNAME){
                                char* buff = new char[conn.data_len + 1];
                                shared.server->read(buff, conn.data_len);
                                shared.interlocutor_name = buff;
                        }
                }else{
                        Header data {Header::CLOSE, 0};
                        shared.server->write((char*) &data, sizeof(Header));
                        goto end; //BAD
                }

                shared.server->read((char*) &conn, sizeof(Header));
        }
end:
        shared.server->close();
}
#include "../include/Socket.hpp"

Socket::Socket(){ }

Socket::Socket(int domain, int type, int port)
{
        sockfd = socket(domain, type, 0);
        portno = port;

        if (sockfd < 0) throw Socket::bad_socket("Socket could not be made");
        printf("Socket made\n");
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
        printf("Socket made\n");
}


void Socket::read(char* buff, int len)
{
        printf("Starting to read\n");
        if (::read(sockfd, buff, len) < 0) throw Socket::bad_socket("Bad read");
        printf("Read data\n");
}

void Socket::write(char* data, int len)
{
        printf("Starting to write\n");
        if (::write(sockfd, data, len) < 0)
                throw Socket::bad_socket("Bad write");
        printf("Data sent!\n");
}

void Socket::close()
{
        ::close(sockfd);
}
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
#ifndef CLIENTRUNNER__H__
#define CLIENTRUNNER__H__

#include "options.h"

#include "Structs.hpp"

class ClientRunner{
        Shared& shared;

public:
        ClientRunner(Shared& share);
        //~ClientRunner();

        void operator()();
};

#endif
#ifndef OPTIONS__H__
#define OPTIONS__H__

//#define DEBUG

#endif
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
#ifndef SERVERRUNNER__H__
#define SERVERRUNNER__H__

#include "options.h"

#include "Structs.hpp"

class ServerRunner{
        Shared& shared;

public:
        ServerRunner(Shared& share);

        void operator()();
};

#endif
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
#include <thread>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <cstdio>
#include <iostream>

#include <stdlib.h>

#include "../include/Socket.hpp"
#include "../include/Server.hpp"
#include "../include/Client.hpp"
#include "../include/Structs.hpp"
#include "../include/ServerRunner.hpp"
#include "../include/ClientRunner.hpp"


int main(void)
{
        using namespace std;

        Shared share;
        ServerRunner s(share);
        thread receiver(s);

        ClientRunner c(share);
        thread sender(c);

        receiver.join();
        sender.join();
        cout << "Ending main thread" << endl;
}
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
