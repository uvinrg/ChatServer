/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_CONNECTION_H__
#define __CHATSERVER_CONNECTION_H__

class Connection
{
public:
    //start the server
    virtual int start_on(int port_number) = 0;
    //init
    virtual int init(int port_number) = 0;
    //receive next message from someone
    virtual int receiveNextMessage(string& user, string& message) = 0;
    //send a message to all users in a room
    virtual int sendMessageToOthers(string message, string room) = 0;
    //send a message back to the user from the server
    virtual int sendMessageToUser(string user, string message) = 0;
    //join a non-empty room or create one if room does not exist
    //placing the user in that room
    virtual int joinRoom(string user, string room) = 0;    
    //leave a room, deleting it if it now contains no users
    virtual int leaveRoom(string user, string room) = 0;
    //list non-empty rooms
    virtual int listRooms(string user) = 0;
    //whisper from user1 to user2 with the message
    virtual int whisper(string acting_user, string dest_user, string message) = 0;
    //process message based on the message and user state
    virtual void processMessage(string user, string msg) = 0;

};

#endif //__CHATSERVER_CONNECTION_H__