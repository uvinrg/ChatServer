/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_PLATFORM_H__
#define __CHATSERVER_PLATFORM_H__

#include "config.h"

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//STL
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>

using namespace std;

#define FD_SETSIZE      MAX_CLIENTS
#include <winsock2.h>

#include "../../src/defs.h"
#include "../../src/connection.h"
#include "../../src/server.h"

class Win32Connection : public Connection
{
public:
    Win32Connection(int listen_port):
        Connection(listen_port),
        started(FALSE)
    { 
        ;
    }

    ~Win32Connection()
    {
        if (started)
        {
            started = FALSE;
            WSACleanup();
        }
    }

    //start the server
    int start();
    //receive next message from someone
    int receiveNextMessage(string& user, string& message);
    //send a message to all users in a room
    int sendMessageToOthers(string user, string message, string room);
    //send a message back to the user from the server
    int sendMessageToUser(string user, string message);
    //join a non-empty room or create one if room does not exist
    //placing the user in that room
    int joinRoom(string user, string room);    
    //leave a room, deleting it if it now contains no users
    int leaveRoom(string user, string room);
    //list non-empty rooms
    int listRooms(string user);
    //whisper from user1 to user2 with the message
    int whisper(string acting_user, string dest_user, string message);

    //thread for accepting new connections
    void InternalAcceptThread();
    //thread for receiving messages
    void InternalReadMessageThread();
    //thread for sending messages
    void InternalSendMessageThread();

    int telnet_decode(string &msg, char* buffer, int size, SOCKET socket);

private:
    int started;

    SOCKET hServerSocket;

    set<SOCKET> sockets;
    map<SOCKET, string> names;
    map<SOCKET, string> messages; //partially received messages from the users	

    queue<string> msgList;
    queue<string> usrList;
};

#define PLATFORM_CONNECTION Win32Connection

#endif //__CHATSERVER_PLATFORM_H__