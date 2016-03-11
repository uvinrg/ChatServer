/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_SERVER_H__
#define __CHATSERVER_SERVER_H__

#include "platform.h"

class Server
{
public:
    int try_on(int port_number);
    int start_on(int port_number);

private:

};

#endif //__CHATSERVER_SERVER_H__