/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#include "server.h"

int Server::try_on(int port_number)
{
    UNREFERENCED_PARAMETER(port_number);

    return CS_OK;
}

int Server::start_on(int port_number, FILE* logfile)
{
    UNREFERENCED_PARAMETER(port_number);

    //assign file pointer
    log = logfile;

    return CS_OK;
}
