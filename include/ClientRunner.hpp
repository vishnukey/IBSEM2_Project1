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
