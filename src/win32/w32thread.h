/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_W32THREAD_H__
#define __CHATSERVER_W32THREAD_H__

//dummy thread function to call the object's thread function
DWORD WINAPI readMessageThreadFunc( LPVOID lpData );
//dummy thread function to call the object's thread function
DWORD WINAPI sendMessageThreadFunc( LPVOID lpData );

class Thread
{
public:
    Thread():
        hClientThread(NULL),
        dwThreadId(0)
    { 
        ;
    }

    ~Thread()
    {
        ;
    }

    //create a thread
    //given a function name
    //and the parameter to be given to it
    int create(void* function_name, void* parameter);
private:
    DWORD dwThreadId;
    HANDLE hClientThread;
};

#endif //__CHATSERVER_W32THREAD_H__


