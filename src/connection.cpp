/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#include "platform.h"
#include "connection.h"

//rename a user
void Connection::renameUser(std::string olduser, std::string newuser)
{
    //enter critical section
    userCriticalSection.enterCriticalSection();

    //get user's socket
    SOCKET sock = user_sock[olduser];

    //replace username for socket
    sock_user[sock] = newuser;
    //delete old username entry
    user_sock.erase(olduser);
    //add new one
    user_sock[newuser] = sock;

    //user cannot be in a room at login, so no changes needed there

    //exit critical section
    userCriticalSection.leaveCriticalSection();
}

//remove user from a room
void Connection::removeFromRoom(std::string user)
{
    //remove user from the room
    userCriticalSection.enterCriticalSection();

    //get room name
    std::string room = user_room[user];
    //delete the room entry
    user_room.erase(user);
    //delete user from the room
    rooms[room].erase(user);
    //delete room if empty
    if (rooms[room].size() == 0)
        rooms.erase(room);

    userCriticalSection.leaveCriticalSection();

    //inform all persons in the room that the user has left
    sendMessageToOthers("* user has left chat: " + user, room);

    //inform the user he has left the room
    sendMessageToUser(user, "* user has left chat: " + user + " (** this is you)");
}

//check if the name is valid
int Connection::isValidName(std::string name)
{
    BOOL valid = TRUE;
    //only alphanumeric letters, _ and . are allowed
    for (int i = 0; i < (int)name.length(); i++)
    {
        //check for non-letters
        if ((name[i] < 48 || (name[i] > 57 && name[i] < 65)  || (name[i] > 90 && name[i] < 97) || name[i] > 122) 
            && name[i] != 46 
            && name[i] != 95)
        {            
            valid = FALSE;
            break;
        }
    }

    return valid;
}

//check if the message is valid
int Connection::isValidMessage(std::string msg)
{
    BOOL valid = TRUE;
    //only printable characters allowed
    for (int i = 0; i < (int)msg.length(); i++)
    {
        //check for non-letters
        if (msg[i] < 32 || msg[i] > 126)
        {            
            valid = FALSE;
            break;
        }
    }

    return valid;
}

//process message based on the message and user state
void Connection::processMessage(std::string user, std::string msg)
{
    //check for quit command
    if (msg.compare("/quit") == 0)
    {
        //quit
        quitServer(user);
        return;
    }

    //check if username must be given
    if (user[0] == ' ' && msg[0] != '/')
    {
        //check if the user name is valid
        if (isValidName(msg))
        {
            std::string newuser = msg;
            //make all letters lowercase
            transform(newuser.begin(), newuser.end(), newuser.begin(), ::tolower);

            //check if username exists
            if (user_sock.find(newuser) != user_sock.end())
            {
                sendMessageToUser(user, 
                    "Sorry, name taken.\r\nLogin Name?");
            }
            else
            {
                //change the user name to his new one
                renameUser(user, newuser);
                sendMessageToUser(newuser, 
                    "Welcome " + newuser + "!");
            }
        }
        else
        {
            //error
            sendMessageToUser(user, "Invalid name. Use only alphanumeric letters, _ and . please!\r\nLogin Name?");
        }

        return;
    }

    //warn if giving commands before setting a username
    if (user[0] == ' ')
    {
        sendMessageToUser(user, "Must register before issuing commands or must /quit\r\nLogin Name?");
        return;
    }

    //rooms
    if (msg.compare("/rooms") == 0)
    {
        listRooms(user);
        return;
    }
    
    //join
    if (msg.substr(0, 6).compare("/join ") == 0)
    {
        std::string room_name = msg.substr(6);

        //join a room or create one if room does not exist
        joinRoom(user, room_name);  
        return;
    }

    //leave
    if (msg.compare("/leave") == 0)
    {
        leaveRoom(user);
        return;
    }

    //whisper
    if (msg.substr(0, 9).compare("/whisper ") == 0)
    {
        //get user name and message
        std::string user_name = msg.substr(9);
        //find start of message
        int poz = user_name.find(" ");        
        //extract message
        std::string message = poz > 0 ? user_name.substr(poz + 1) : "";
        //extract user name
        user_name = user_name.substr(0, poz);

        //try to send the whisper
        whisper(user, user_name, message);     
        return;
    }

    //other "/" command
    if (msg[0] == '/')
    {
        //no other '/' commands permitted
        sendMessageToUser(user, "Invalid command.");
        return;
    }

    //simple message given in the current room
    if (isValidMessage(msg))
    {
        //if room exists, send a message
        if (user_room.find(user) != user_room.end())
            sendMessageToOthers(user + ": " + msg, user_room[user]);
        else
            sendMessageToUser(user, "Must join a room to chat. Try /rooms and /join room_name");
        return;
    }
    else
    {
        sendMessageToUser(user, "Invalid message.\r\n");
    }
}

//accept a connection once it is confirmed by select
void Connection::acceptConnection()
{
    SOCKET new_socket;

    //blocking until a new user connects to the server
    if ((new_socket = Socket::acceptConnection(hServerSocket)) != (SOCKET)(-1))
    {        
        //enter critical section
        userCriticalSection.enterCriticalSection();

        //check max user count
        if (userCount == MAX_CLIENTS)
        {
            //close new sockets once max user count has been reached
            Socket::closeTheSocket(new_socket);

            //leave critical section
            userCriticalSection.leaveCriticalSection();

            return;
        }

        //create a user for the socket with a space followed by a number 
        //such a name is illegal for user names and can help differentiate
        //users which have not yet chosen a user name
        sock_user[new_socket] = " ";
        sock_user[new_socket] += std::to_string(userCount);
        user_sock[ sock_user[new_socket] ] = new_socket;
        //create an empty partial message for the user
        messages[new_socket] = "";

        //increment number of users
        userCount++;

        //add socket to list
        sockets.insert(new_socket);

        //leave section
        userCriticalSection.leaveCriticalSection();

        //send Welcome message to user
        sendMessageToUser(sock_user[new_socket], 
            "Welcome to the XYZ chat server\r\n"
            "Accepted commands are:\r\n"
            "/rooms\r\n"
            "/join room_name\r\n"
            "/whisper user_name message\r\n"
            "/leave\r\n"
            "/quit\r\n"
            "end of list.\r\n"
            "Login Name?");
    }
}

//actual thread function for reading messages from clients and accepting connections
void Connection::internalReadMessageThreadFunc()
{
    std::set<SOCKET>::iterator i;
    SOCKET sock;
    int activity = 0;
    char buffer[MAX_LINE_SIZE] = {0};
    int size = 0;
    std::string localmsg;
    std::set<SOCKET> localsockets;

    while (ALWAYS_TRUE)
    {
        //make a local copy of the sockets so they are immune to external changes
        userCriticalSection.enterCriticalSection();
        localsockets = sockets;
        userCriticalSection.leaveCriticalSection();

        activity = Socket::doSelect(localsockets);               

        //error
        if ((activity < 0) && (errno != EINTR)) 
            continue;

        //check for accept
        if (Socket::isSocketSet(hServerSocket))
        {
            //accept the new connection
            acceptConnection();
        }

        //go through each socket
        for (i = localsockets.begin(); i != localsockets.end(); i++)
        {
            sock = *i;

            //read message, if we have one
            if (sock != hServerSocket && Socket::isSocketSet(sock))
            {
                //read as many bytes as we can
                if ((size = Socket::receiveOnSocket(sock, buffer, sizeof(buffer))) > 0)
                {
                    //add terminating character to string
                    buffer[size] = '\0';

                    //get the partial message
                    userCriticalSection.enterCriticalSection();
                    localmsg = messages[sock];
                    userCriticalSection.leaveCriticalSection();

                    //use telnet RFC to check for backspace, delete, newline, etc
                    //the new part of the message into the old message
                    telnetDecode(localmsg, buffer, strlen(buffer), sock);     

                    //store the new partial message back
                    userCriticalSection.enterCriticalSection();
                    messages[sock] = localmsg;
                    userCriticalSection.leaveCriticalSection();
                }
                else
                {
                    //if select() gave no error
                    //but we received 0 bytes or we get an error
                    //means the connection closed, so we remove the user
                    //from the server
                    std::string user = sock_user[sock];
                    //check if user is in a room
                    if (user_room.find(user) != user_room.end())
                    {
                        //remove him from the room
                        removeFromRoom(user);
                    }
                    //delete user connection
                    deleteConnection(sock);
                }
            }
        }
    }
}

void Connection::deleteConnection(SOCKET sock)
{
    //critical section
    userCriticalSection.enterCriticalSection();

    //check if the user hasn't been already closed/deleted by another thread
    if (sockets.find(sock) != sockets.end())
    {
        //delete the socket from the list
        sockets.erase(sock);
        //close the socket
        Socket::closeTheSocket(sock);

        //get user
        std::string user = sock_user[sock];
            
        //clean user info
        sock_user.erase(sock);
        user_sock.erase(user);
        messages.erase(sock);
        //decrease active user count
        userCount--;
        if (user_room.find(user) != user_room.end())
        {
            //remove user from his room (only for non-lobby users)
            rooms[ user_room[user] ].erase(user);
            //if room is empty, remove it
            if (rooms[ user_room[user] ].size() == 0)
                rooms.erase(user_room[user]);
            //finally, remove the user and its associated room
            user_room.erase(user);
        }
    }

    userCriticalSection.leaveCriticalSection();
}

//actual thread function
void Connection::internalSendMessageThreadFunc()
{
    writemessage writemsg;
    int result;

    while(ALWAYS_TRUE)
    {
        //wait until there are messages to send
        writeMsgListEmpty.wait();

        //enter critical section to get message
        writeCriticalSection.enterCriticalSection();
        writemsg = writeMsgList.front(); writeMsgList.pop();
        writeCriticalSection.leaveCriticalSection();

        //check if user is still connected
        if (user_sock.find(writemsg.user) != user_sock.end())
        {
            //send the message to destination
            SOCKET sock = user_sock[writemsg.user];

            //append newline to message
            writemsg.msg += "\r";
            writemsg.msg += "\n";

            //if user is in lobby, means we are sending the message only to him
            if (writemsg.room[0] == ' ')
            {
                //send the message to the user
                result = Socket::sendOnSocket(sock, writemsg.msg.c_str(), writemsg.msg.length());

                //if error, close the connection
                if (result == -1) 
                {
                    //delete the connection
                    deleteConnection(sock);
                }
            }
        }
    }
}

//take the code from the buffer and transform it into a string
int Connection::telnetDecode(std::string &msg, char* buffer, int size, SOCKET socket)
{
	//parse the received buffer 
	for(int i = 0; i < size; i++)
	{
        //if first character is non-printable and not new-line, skip entire sequence
        if (i == 0 && (buffer[i] < 32 || buffer[i] > 126) && buffer[i] != 13 && buffer[i] != 10 )
            return CS_INVALID_ARGS;
		//if delete or backspace ascii code remove next character
		if( buffer[i] == 127 ||  (unsigned char)(buffer[i]) == 224)
		{            
			continue;
		}
		//if backspace ascii code then remove previous character
		else if ( buffer[i] == 8)
		{
			msg.erase(msg.end() - 1);
		}
        //check for newline
        else if ( buffer[i] == 13 || buffer[i] == 10)
        {
            //skip next char if needed
            if (i + 1 < size && buffer[i + 1] < 32)
                i++;

            //prepare the message for processing
            readmessage readmsg;
            readmsg.msg = msg;

            //get user who sent it
            userCriticalSection.enterCriticalSection();
            readmsg.user = sock_user[socket];
            userCriticalSection.leaveCriticalSection();

            //our message is ready, send it to the processing thread
            readCriticalSection.enterCriticalSection();
            readMsgList.push(readmsg);
            readCriticalSection.leaveCriticalSection();

            //unblock message parsing if needed
            readMsgListEmpty.increaseCount(1);

            //reset message for current client
            msg = "";
        }
        //only add printable characters
		else if (buffer[i] >= 32 && buffer[i] <= 126)
		{
			msg += buffer[i];
		}	
	}
	return CS_OK;
}

//receive next message from someone
int Connection::receiveNextMessage(std::string& user, std::string& message)
{
    readmessage readmsg;

    //wait until there are messages in the queue
    readMsgListEmpty.wait();

    //make sure we acccess the message list one thread at a time
    readCriticalSection.enterCriticalSection();
    if (!readMsgList.empty())
    {
        //read the message structure
        readmsg = readMsgList.front(); readMsgList.pop();
    }
    readCriticalSection.leaveCriticalSection();

    //get user and message
    user = readmsg.user;
    message = readmsg.msg;

    return CS_OK;
}

//send a message to all users in a room
int Connection::sendMessageToOthers(std::string message, std::string room)
{
    //if the room does not exist, no message to send
    if (rooms.find(room) == rooms.end())
        return CS_FAIL;
    
    //send the same message to every user in a room
    std::set<std::string>::iterator it;
    //go through each user in the room
    for (it = rooms[room].begin(); it != rooms[room].end(); it++)
    {
        //send the message
        sendMessageToUser(*it, message);
    }

    return CS_OK;
}

//send a message back to the user from the server
int Connection::sendMessageToUser(std::string user, std::string message)
{
    writemessage writemsg;
    writemsg.user = user;
    writemsg.msg = message;
    writemsg.room = " lobby"; //normal rooms cannot contain spaces

    //enter critical section to send our message
    writeCriticalSection.enterCriticalSection();
    writeMsgList.push(writemsg);
    writeCriticalSection.leaveCriticalSection();

    //flag the message list contains one more item
    writeMsgListEmpty.increaseCount(1);

    return CS_OK;
}

//join a non-empty room or create one if room does not exist
//placing the user in that room
int Connection::joinRoom(std::string user, std::string room_name)
{
    //check if the room name is valid
    if (isValidName(room_name))
    {
        //make room lowercase
        transform(room_name.begin(), room_name.end(), room_name.begin(), ::tolower);

        //remove user from other rooms if he's there
        if (user_room.find(user) != user_room.end())
        {
            removeFromRoom(user);
        }

        //inform people about the new user
        sendMessageToOthers("* new user joined chat: " + user, room_name);

        std::string message = "entering room: " + room_name + "\r\n";

        //add user to room
        user_room[user] = room_name;
        rooms[room_name].insert(user);

        //list users in the room
        std::set<std::string>::iterator it;
        //for each user in the new room
        for (it = rooms[room_name].begin(); it != rooms[room_name].end(); it++)
        {
            //list the user
            if ((*it).compare(user) != 0)
            {
                message += "* " + (*it) + "\r\n";
            }
            //special case when the user is himself
            else
            {
                message += "* " + (*it)  + " (** this is you)\r\n";
            }
        }
        //end of message
        message += "end of list.";
        //send it to user
        sendMessageToUser(user, message);
    }
    else
    {
        //error
        sendMessageToUser(user, "Invalid room name. Use only alphanumeric letters, _ and . please!");
    }

    return CS_OK;
}

//leave a room, deleting it if it now contains no users
int Connection::leaveRoom(std::string user)
{
    //check if user is in a room
    if (user_room.find(user) != user_room.end())
    {
        //remove him from the room
        removeFromRoom(user);
    }
    else
    {
        //error
        sendMessageToUser(user, "In lobby already. Use /quit for exiting.");            
    }

    return CS_OK;
}

//quit
int Connection::quitServer(std::string user)
{
    //check if user is in a room
    if (user_room.find(user) != user_room.end())
    {
        //if true, remove him from it
        removeFromRoom(user);
    }
    //remove user
    sendMessageToUser(user, "BYE");
    //delete connection if user still exists
    if (user_sock.find(user) != user_sock.end())
        deleteConnection(user_sock[user]);

    return CS_OK;
}

//list non-empty rooms
int Connection::listRooms(std::string user)
{
    int usercnt;
    std::string message = "Active rooms are:\r\n";
    std::map<std::string, std::set<std::string> >::iterator it;
    //go through each room in the list
    for (it = rooms.begin(); it != rooms.end(); it++)
    {
        //get its user count
        usercnt = it->second.size();
        //write the room's description in the message
        message += "* " + it->first + " (" + std::to_string(usercnt) + ")\r\n";
    }
    message += "end of list.";
    //send the message to the user
    sendMessageToUser(user, message);

    return CS_OK;
}

//whisper from user1 to user2 with the message
int Connection::whisper(std::string acting_user, std::string dest_user, std::string message)
{
    //check for valid user name
    if (isValidName(dest_user) && message.compare("") != 0)
    {
        //make name lowercase
        transform(dest_user.begin(), dest_user.end(), dest_user.begin(), ::tolower);

        //try to find name
        if (user_sock.find(dest_user) == user_sock.end())
        {
            sendMessageToUser(acting_user, "No such user.");
            return CS_FAIL;
        }

        //check for valid message
        if (!isValidMessage(message))
        {
            sendMessageToUser(acting_user, "Invalid message.");
            return CS_FAIL;
        }

        //if all checks are ok, send the whisper
        sendMessageToUser(dest_user, "Whisper from " + acting_user + ": " + message);
    }
    else
        sendMessageToUser(acting_user, "Invalid format.");

    return CS_OK;
}

//start the server
int Connection::init(int port_number)
{
    //start winsock2
    if (Socket::globalInit() == CS_FAIL)
        return CS_FAIL;

    //try to create the socket
    if ((hServerSocket = Socket::createServerSocket(port_number)) == (SOCKET)(-1))
    {
        return CS_FAIL;
    }

    //initial user count
    userCount = 0;

    //add server socket to the sockets set
    sockets.insert(hServerSocket);

    //create message list semaphore
    readMsgListEmpty.create(0);

    //create message list semaphore
    writeMsgListEmpty.create(0);

    //start two threads:    
    // - one thread for reading messages from clients and accepting new connections
    // - one thread for sending messages to clients

    // Start the client thread and pass this instance to it
    readMessageThread.create(readMessageThreadFunc, (void*)this);

    // Start the client thread and pass this instance to it
    sendMessageThread.create(sendMessageThreadFunc, (void*)this);
    
    //mark server as started
    started = TRUE;

    return CS_OK;
}

int Connection::startOn(int port_number)
{
    std::string user, message;    

    //try to start the server
    if (init(port_number) != CS_OK)
        return CS_FAIL;

    //message loop
    while(ALWAYS_TRUE)
    {
        //get next message received by the server connection
        if (receiveNextMessage(user, message) == CS_OK)
        {
            //process message based on the message and user state
            processMessage(user, message);
        }
    }

    return CS_OK;
}




