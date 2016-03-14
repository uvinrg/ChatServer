/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#include "platform.h"

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
    SOCKET new_socket = 0;
    struct sockaddr_in address;
    int addrlen;

    while(ALWAYS_TRUE)
    {       
        if ((new_socket = accept(hServerSocket, (struct sockaddr *)&address, &addrlen)) >= 0)
        {
            //add socket to list
            sockets.insert(new_socket);
        }
    }


    /*
    //set of socket descriptors
    fd_set readfds;
    int max_sd = 0;
    int activity = 0;

    //add server socket to set
    sockets.insert(hServerSocket);

    //clear the socket set
    FD_ZERO(&readfds);

    //add all the current sockets, just one for now, the server socket
    for (set<SOCKET>::iterator i = sockets.begin(); i != sockets.end(); i++)
    {
        FD_SET(*i, &readfds);
    }

    set<SOCKET>::iterator last = sockets.end();
    last--;

    //get last element, since it must have max size
    max_sd = *last;

    //loop
    while (ALWAYS_TRUE)
    {
        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        //error
        if ((activity < 0) && (errno != EINTR)) 
            return;

        //If something happened on the master socket , then its an incoming connection
        if (FD_ISSET(hServerSocket, &readfds)) 
        {
            if ((new_socket = accept(hServerSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
          
            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
        
            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
            {
                perror("send");
            }
              
            puts("Welcome message sent successfully");
              
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++) 
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);
                     
                    break;
                }
            }
        }
          
        //else its some IO operation on some other socket :)
        for (i = 0; i < max_clients; i++) 
        {
            sd = client_socket[i];
              
            if (FD_ISSET( sd , &readfds)) 
            {
                //Check if it was for closing , and also read the incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                      
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                }
                  
                //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end of the data read
                    buffer[valread] = '\0';
                    send(sd , buffer , strlen(buffer) , 0 );
                }
            }
        }

    }
    */
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

    //set of socket descriptors
    fd_set readfds;

    while (ALWAYS_TRUE)
    {
        //clear the socket set
        FD_ZERO(&readfds);

        //go through each socket and add them
        for (i = sockets.begin(); i != sockets.end(); i++)
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
        for (i = sockets.begin(); i != sockets.end(); i++)
        {
            sock = *i;

            //read message, if we have one
            if (FD_ISSET(sock, &readfds))
            {
                if ((size = recv(sock, buffer, sizeof(buffer), 0)) > 0)
                {
                    buffer[size] = '\0';

                    //use telnet RFC to check for backspace, delete, newline, etc

                    //result = telnet_decode(messages[sock], buffer);

                    //if (result)
                    //    send_to_processing_enqueue_using_crit_section();

                    ////it will either create a new message or add to it
                    //messages[sock] += buffer;

                    //check newline
                    
                }
            }
        }

    }
}

//actual thread function
void Win32Connection::InternalSendMessageThread()
{
    while(ALWAYS_TRUE)
    {
        ;
    }
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
    serverAddr.sin_addr.s_addr = INADDR_ANY;
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

//receive next message from someone
int Win32Connection::receiveNextMessage(string& user, string& message)
{

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


