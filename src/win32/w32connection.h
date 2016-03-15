/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_W32CONNECTION_H__
#define __CHATSERVER_W32CONNECTION_H__

struct readmessage
{
    string user;
    string msg;
};

struct writemessage
{
    string user;
    string msg;
    string room;
};

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
    //server started indicator
    int started;

    //the server socket
    SOCKET hServerSocket;

    //the list of sockets
    set<SOCKET> sockets;

    map<SOCKET, string> sock_user; //map to get from socket to user
    map<string, SOCKET> user_sock; //map to get from user to socket
    map<string, string> user_room; //map to get from user to its room
    map<SOCKET, string> messages; //partially received messages from the users	

    //critical section for modifying users
    W32semaphore userCriticalSection;

    //sync for read messages
    W32semaphore readCriticalSection;    
    //sync for waiting if message list to be read is empty
    W32semaphore readMsgListEmpty;
    //message list to be read
    queue<readmessage> readMsgList;

    //sync for write messages
    W32semaphore writeCriticalSection;    
    //sync for waiting if message list to be written is empty
    W32semaphore writeMsgListEmpty;
    //message list to be written
    queue<writemessage> writeMsgList;

    //the list of users in each room
    map<string, set<string> > rooms;

    //current number of users on the server
    int user_count;
};

#endif //__CHATSERVER_W32CONNECTION_H__

