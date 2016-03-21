/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_CONNECTION_H__
#define __CHATSERVER_CONNECTION_H__

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

class Connection
{
public:
    Connection():        
        started(FALSE)
    { 
        ;
    }

    ~Connection()
    {
        if (started)
        {
            started = FALSE;
            Socket::globalShutdown();
        }
    }

    //start the server
    int start_on(int port_number);
    //init
    int init(int port_number);
    //receive next message from someone
    int receiveNextMessage(string& user, string& message);
    //send a message to all users in a room
    int sendMessageToOthers(string message, string room);
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
    //process message based on the message and user state
    void processMessage(string user, string msg);

    //accept a connection once it is confirmed by select
    void acceptConnection();

    //rename a user after login
    void renameUser(string olduser, string newuser);

    //delete a connection
    void deleteConnection(SOCKET sock);

    //removes user from a room
    void removeFromRoom(string user);

    //check name is formed only from letters, _ and .
    int isValidName(string name);
    //check message is formed only from printable characters
    int isValidMessage(string name);

    //thread for receiving messages
    void InternalReadMessageThreadFunc();
    //thread for sending messages
    void InternalSendMessageThreadFunc();

    int telnet_decode(string &msg, char* buffer, int size, SOCKET socket);

private:
    //server started indicator
    int started;

    //the server socket
    SOCKET hServerSocket;

    //the list of sockets
    set<SOCKET> sockets;

    //the read message thread
    Thread readMessageThread;

    //the send message thread
    Thread sendMessageThread;

    map<SOCKET, string> sock_user; //map to get from socket to user
    map<string, SOCKET> user_sock; //map to get from user to socket
    map<string, string> user_room; //map to get from user to its room
    map<SOCKET, string> messages; //partially received messages from the users	

    //critical section for modifying users
    Critsection userCriticalSection;

    //sync for read messages
    Critsection readCriticalSection;    
    //sync for waiting if message list to be read is empty
    Semaphore readMsgListEmpty;
    //message list to be read
    queue<readmessage> readMsgList;

    //sync for write messages
    Critsection writeCriticalSection;    
    //sync for waiting if message list to be written is empty
    Semaphore writeMsgListEmpty;
    //message list to be written
    queue<writemessage> writeMsgList;

    //the list of users in each room
    map<string, set<string> > rooms;

    //current number of users on the server
    int user_count;
};

#endif //__CHATSERVER_CONNECTION_H__

