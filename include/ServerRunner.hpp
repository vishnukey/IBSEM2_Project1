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
