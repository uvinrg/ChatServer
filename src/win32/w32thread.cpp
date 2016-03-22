/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#include "platform.h"
#include "w32thread.h"

//dummy thread function to call the object's thread function
DWORD WINAPI readMessageThreadFunc( LPVOID lpData )
{
    Connection *conn = (Connection*)lpData;
    conn->internalReadMessageThreadFunc();

    return CS_OK;
}

//dummy thread function to call the object's thread function
DWORD WINAPI sendMessageThreadFunc( LPVOID lpData )
{
    Connection *conn = (Connection*)lpData;
    conn->internalSendMessageThreadFunc();

    return CS_OK;
}

//create a thread
//given a function name
//and the parameter to be given to it
int Thread::create(void* function_name, void* parameter)
{
    // Start the client thread and pass this instance to it
    hClientThread = CreateThread( NULL, 0,
        ( LPTHREAD_START_ROUTINE ) function_name,
        ( LPVOID ) parameter, 0, &dwThreadId ) ;
    if ( hClientThread != NULL )
    {
        //close handle now so that thread may finish when the worker function ends
        CloseHandle( hClientThread );
    }

    return TRUE;
}

