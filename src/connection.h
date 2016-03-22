/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_CONNECTION_H__
#define __CHATSERVER_CONNECTION_H__

struct readmessage
{
    std::string user;
    std::string msg;
};

struct writemessage
{
    std::string user;
    std::string msg;
    std::string room;
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
    int startOn(int port_number);
    //init
    int init(int port_number);
    //receive next message from someone
    int receiveNextMessage(std::string& user, std::string& message);
    //send a message to all users in a room
    int sendMessageToOthers(std::string message, std::string room);
    //send a message back to the user from the server
    int sendMessageToUser(std::string user, std::string message);
    //join a non-empty room or create one if room does not exist
    //placing the user in that room
    int joinRoom(std::string user, std::string room);    
    //leave a room, deleting it if it now contains no users
    int leaveRoom(std::string user);
    //quit
    int quitServer(std::string user);
    //list non-empty rooms
    int listRooms(std::string user);
    //whisper from user1 to user2 with the message
    int whisper(std::string acting_user, std::string dest_user, std::string message);
    //process message based on the message and user state
    void processMessage(std::string user, std::string msg);

    //accept a connection once it is confirmed by select
    void acceptConnection();

    //rename a user after login
    void renameUser(std::string olduser, std::string newuser);

    //delete a connection
    void deleteConnection(SOCKET sock);

    //removes user from a room
    void removeFromRoom(std::string user);

    //check name is formed only from letters, _ and .
    int isValidName(std::string name);
    //check message is formed only from printable characters
    int isValidMessage(std::string name);

    //thread for receiving messages
    void internalReadMessageThreadFunc();
    //thread for sending messages
    void internalSendMessageThreadFunc();

    int telnetDecode(std::string &msg, char* buffer, int size, SOCKET socket);

private:
    //server started indicator
    int started;

    //the server socket
    SOCKET hServerSocket;

    //the list of sockets
    std::set<SOCKET> sockets;

    //the read message thread
    Thread readMessageThread;

    //the send message thread
    Thread sendMessageThread;

    std::map<SOCKET, std::string> sock_user; //std::map to get from socket to user
    std::map<std::string, SOCKET> user_sock; //std::map to get from user to socket
    std::map<std::string, std::string> user_room; //std::map to get from user to its room
    std::map<SOCKET, std::string> messages; //partially received messages from the users	

    //critical section for modifying users
    Critsection userCriticalSection;

    //sync for read messages
    Critsection readCriticalSection;    
    //sync for waiting if message list to be read is empty
    Semaphore readMsgListEmpty;
    //message list to be read
    std::queue<readmessage> readMsgList;

    //sync for write messages
    Critsection writeCriticalSection;    
    //sync for waiting if message list to be written is empty
    Semaphore writeMsgListEmpty;
    //message list to be written
    std::queue<writemessage> writeMsgList;

    //the list of users in each room
    std::map<std::string, std::set<std::string> > rooms;

    //current number of users on the server
    int userCount;
};

#endif //__CHATSERVER_CONNECTION_H__

