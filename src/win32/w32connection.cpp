/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#include "platform.h"
#include "w32connection.h"

//dummy thread function to call the object's thread function
DWORD WINAPI AcceptThread( LPVOID lpData )
{
    Win32Connection *conn = (Win32Connection*)lpData;
    conn->InternalAcceptThread();

    return CS_OK;
}

//dummy thread function to call the object's thread function
DWORD WINAPI ReadMessageThread( LPVOID lpData )
{
    Win32Connection *conn = (Win32Connection*)lpData;
    conn->InternalReadMessageThread();

    return CS_OK;
}

//dummy thread function to call the object's thread function
DWORD WINAPI SendMessageThread( LPVOID lpData )
{
    Win32Connection *conn = (Win32Connection*)lpData;
    conn->InternalSendMessageThread();

    return CS_OK;
}

//actual thread function
void Win32Connection::InternalAcceptThread()
{
    SOCKET new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    while(ALWAYS_TRUE)
    {       
        if ((new_socket = accept(hServerSocket, (struct sockaddr *)&address, &addrlen)) != INVALID_SOCKET)
        {
            //enter critical section
            userCriticalSection.wait();

            //create a user for the socket with a space followed by a number (illegal for user names)
            sock_user[new_socket] = " ";
            sock_user[new_socket] += to_string(user_count);
            user_sock[ sock_user[new_socket] ] = new_socket;
            //create an empty message for the user
            messages[new_socket] = "";

            //increment number of users
            user_count++;

            //add socket to list
            sockets.insert(new_socket);

            //leave section
            userCriticalSection.increaseCount(1);

            //mark the number of sockets added
            socketsListEmpty.increaseCount(1);
        }
    }
}

//actual thread function
void Win32Connection::InternalReadMessageThread()
{
    set<SOCKET>::iterator i;
    SOCKET sock;
    int max_sd = 0;
    int activity = 0;
    char buffer[MAX_LINE_SIZE] = {0};
    int size = 0;
    int result = 0;
    string localmsg;
    set<SOCKET> localsockets;

    //set of socket descriptors
    fd_set readfds;

    while (ALWAYS_TRUE)
    {
        socketsListEmpty.wait();

        //clear the socket set
        FD_ZERO(&readfds);

        //make a local copy of the sockets so they are immune to external changes
        userCriticalSection.wait();
        localsockets = sockets;
        userCriticalSection.increaseCount(1);

        //go through each socket and add them
        for (i = localsockets.begin(); i != localsockets.end(); i++)
        {
            sock = *i;
            FD_SET(sock, &readfds);
        }
        
        //go to last socket
        i--;
        //its value would be maximum due to set being ordered
        max_sd = *i;

        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        //error
        if ((activity < 0) && (errno != EINTR)) 
            return;

        //go through each socket
        for (i = localsockets.begin(); i != localsockets.end(); i++)
        {
            sock = *i;

            //read message, if we have one
            if (FD_ISSET(sock, &readfds))
            {
                if ((size = recv(sock, buffer, sizeof(buffer), 0)) > 0)
                {
                    buffer[size] = '\0';

                    userCriticalSection.wait();
                    localmsg = messages[sock];
                    userCriticalSection.increaseCount(1);

                    //use telnet RFC to check for backspace, delete, newline, etc
                    result = telnet_decode(localmsg, buffer, strlen(buffer), sock);     

                    userCriticalSection.wait();
                    messages[sock] = localmsg;
                    userCriticalSection.increaseCount(1);
                }
            }
        }

        socketsListEmpty.increaseCount(1);
    }
}

//actual thread function
void Win32Connection::InternalSendMessageThread()
{
    writemessage writemsg;
    int result;

    while(ALWAYS_TRUE)
    {
        //wait until there are messages to send
        writeMsgListEmpty.wait();

        //enter critical section to get message
        writeCriticalSection.wait();
        writemsg = writeMsgList.front(); writeMsgList.pop();
        writeCriticalSection.increaseCount(1);

        //send the message to destination
        SOCKET sock = user_sock[writemsg.user];

        //append newline to message
        writemsg.msg += "\r";
        writemsg.msg += "\n";

        //if user is in lobby, means we are sending the message only to him
        if (writemsg.room[0] == ' ')
        {
            result = send( sock, writemsg.msg.c_str(), writemsg.msg.length(), 0 );

            if (result == SOCKET_ERROR) 
            {
                //decrease number of sockets by 1 in the semaphore
                socketsListEmpty.wait();

                //close the socket
                closesocket(sock);

                //remove user from user list
                userCriticalSection.wait();
            
                //clean user info
                sock_user.erase(sock);
                user_sock.erase(writemsg.user);
                messages.erase(sock);
                //decrease active user count
                user_count--;
                //remove user from his room (only for non-lobby users)
                rooms[ user_room[writemsg.user] ].erase(writemsg.user);
                //if room is empty, remove it
                if (rooms[ user_room[writemsg.user] ].size() == 0)
                    rooms.erase(user_room[writemsg.user]);
                //finally, remove the user and its associated room
                user_room.erase(writemsg.user);

                userCriticalSection.increaseCount(1);
            }
        }

    }
}

//take the code from the buffer and transform it into a string
int Win32Connection::telnet_decode(string &msg, char* buffer, int size, SOCKET socket)
{
	//parse the received buffer 
	for(int i = 0; i < size; i++)
	{
        //if first character is non-printable, skip entire sequence
        if (i == 0 && (buffer[i] < 32 || buffer[i] > 126) && buffer[i] != 13 && buffer[i] != 10 )
            return CS_INVALID_ARGS;
		//if delete or ctr + backspace ascii code remove next character
		if( buffer[i] == 127 ||  buffer[i] == 224)
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

            //prepare the message
            readmessage readmsg;
            readmsg.msg = msg;

            userCriticalSection.wait();
            readmsg.user = sock_user[socket];
            userCriticalSection.increaseCount(1);

            //our message is ready, send it to server
            readCriticalSection.wait();
            readMsgList.push(readmsg);
            readCriticalSection.increaseCount(1);

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
int Win32Connection::receiveNextMessage(string& user, string& message)
{
    readmessage readmsg;

    //wait until there are messages in the queue
    readMsgListEmpty.wait();

    //make sure we acccess the message list one at a time
    readCriticalSection.wait();
    if (!readMsgList.empty())
    {
        readmsg = readMsgList.front(); readMsgList.pop();
    }
    readCriticalSection.increaseCount(1);

    user = readmsg.user;
    message = readmsg.msg;

    return CS_OK;
}

//send a message to all users in a room
int Win32Connection::sendMessageToOthers(string user, string message, string room)
{

    return CS_OK;
}

//send a message back to the user from the server
int Win32Connection::sendMessageToUser(string user, string message)
{
    writemessage writemsg;
    writemsg.user = user;
    writemsg.msg = message;
    writemsg.room = " lobby"; //normal rooms cannot contain spaces

    //enter critical section to send our message
    writeCriticalSection.wait();
    writeMsgList.push(writemsg);
    writeCriticalSection.increaseCount(1);

    //flag the message list contains one more item
    writeMsgListEmpty.increaseCount(1);

    return CS_OK;
}

//join a non-empty room or create one if room does not exist
//placing the user in that room
int Win32Connection::joinRoom(string user, string room)
{

    return CS_OK;
}

//leave a room, deleting it if it now contains no users
int Win32Connection::leaveRoom(string user, string room)
{

    return CS_OK;
}

//list non-empty rooms
int Win32Connection::listRooms(string user)
{

    return CS_OK;
}

//whisper from user1 to user2 with the message
int Win32Connection::whisper(string acting_user, string dest_user, string message)
{

    return CS_OK;
}

//start the server
int Win32Connection::start()
{
    //start winsock2
    WSADATA wsaData;
    WORD wVersion = MAKEWORD( 2, 0 );
    if ( WSAStartup( wVersion, &wsaData ) != 0 )
        return CS_FAIL;    

    //try to create the socket
    hServerSocket = socket(AF_INET, SOCK_STREAM, 0);
    if ( hServerSocket == INVALID_SOCKET )
    {
        // Cleanup the environment initialized by WSAStartup()
        WSACleanup();
        return CS_FAIL;
    }

    // Create the structure for binding the socket to the listening port
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;     
    serverAddr.sin_addr.s_addr = inet_addr( "127.0.0.1" ) ; //INADDR_ANY;
    serverAddr.sin_port = htons((USHORT)port);

    // Bind the Server socket to the address & port
    if ( bind( hServerSocket, ( struct sockaddr * ) &serverAddr, sizeof( serverAddr ) ) == SOCKET_ERROR )
    {
        // Free the socket and cleanup the environment initialized by WSAStartup()
        closesocket(hServerSocket);
        WSACleanup();
        return CS_FAIL;
    }

    // Put the Server socket in listen state so that it can wait for client connections
    if ( listen( hServerSocket, SOMAXCONN ) == SOCKET_ERROR )
    {
        // Free the socket and cleanup the environment initialized by WSAStartup()
        closesocket(hServerSocket);
        WSACleanup() ;
        return CS_FAIL;
    }

    //initial user count
    user_count = 0;
    //create sockets list semaphore
    socketsListEmpty.create(0);

    //create critical section for modifying users
    userCriticalSection.create(1);

    //create critical section
    readCriticalSection.create(1);
    //create message list semaphore
    readMsgListEmpty.create(0);

    //create critical section
    writeCriticalSection.create(1);
    //create message list semaphore
    writeMsgListEmpty.create(0);

    //create separate thread for select()
    HANDLE hClientThread;
    DWORD dwThreadId;

    //start three threads:
    // - one thread for accepting new connections
    // - one thread for reading messages from clients
    // - one thread for sending messages to clients

    // Start the client thread and pass this instance to it
    hClientThread = CreateThread( NULL, 0,
        ( LPTHREAD_START_ROUTINE ) AcceptThread,
        ( LPVOID ) this, 0, &dwThreadId ) ;
    if ( hClientThread != NULL )
    {
        //close handle now so that thread may finish when the worker function ends
        CloseHandle( hClientThread );
    }

    // Start the client thread and pass this instance to it
    hClientThread = CreateThread( NULL, 0,
        ( LPTHREAD_START_ROUTINE ) ReadMessageThread,
        ( LPVOID ) this, 0, &dwThreadId ) ;
    if ( hClientThread != NULL )
    {
        //close handle now so that thread may finish when the worker function ends
        CloseHandle( hClientThread );
    }

    // Start the client thread and pass this instance to it
    hClientThread = CreateThread( NULL, 0,
        ( LPTHREAD_START_ROUTINE ) SendMessageThread,
        ( LPVOID ) this, 0, &dwThreadId ) ;
    if ( hClientThread != NULL )
    {
        //close handle now so that thread may finish when the worker function ends
        CloseHandle( hClientThread );
    }
    
    started = TRUE;

    return CS_OK;
}


