/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_W32SOCKET_H__
#define __CHATSERVER_W32SOCKET_H__

//static class members
//for working with sockets on
//different platforms
class Socket
{
public:    
    //Initialized socket system
    static int globalInit();
    //Exist socket system
    static int globalShutdown();

    //create listening socket
    static SOCKET createServerSocket(int port_number);
    //accept a new connection on a socket
    //blocking if no incoming connection
    static SOCKET acceptConnection(SOCKET listening_socket);
    //close a socket
    static void closeTheSocket(SOCKET sock);

    //do a select waiting for an event on a set of sockets
    static int doSelect(std::set<SOCKET>& sockets);
    //check if a socket was set by select
    static int isSocketSet(SOCKET& sock);
    //block and wait for receiving data on a socket
    static int receiveOnSocket(SOCKET sock, char* buffer, int buff_size);
    //send data through a socket
    static int sendOnSocket(SOCKET sock, const char* buffer, int buff_size);

private:
    //set of socket descriptors for select
    static fd_set readfds;

};

#endif //__CHATSERVER_W32SOCKET_H__


