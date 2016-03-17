/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#include "platform.h"
#include "server.h"

int Server::start_on(int port_number, FILE* logfile)
{
    string user1, user2, message, room;    

    //assign file pointer
    log = logfile;

    //create a new connection
    conn = new PLATFORM_CONNECTION(port_number);

    //try to start the server
    if (conn->start() != CS_OK)
        return CS_FAIL;

    //message loop
    while(ALWAYS_TRUE)
    {
        //get next message received by the server connection
        if (conn->receiveNextMessage(user1, message) == CS_OK)
        {
            //process message based on the message and user state
            conn->processMessage(user1, message);
        }
    }

    return CS_OK;
}
