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
