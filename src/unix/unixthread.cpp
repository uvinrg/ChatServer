/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#include "platform.h"
#include "unixthread.h"

//dummy thread function to call the object's thread function
void* readMessageThreadFunc( void* data )
{
    Connection *conn = (Connection*)data;
    conn->internalReadMessageThreadFunc();

    return NULL;
}

//dummy thread function to call the object's thread function
void* sendMessageThreadFunc( void* data )
{
    Connection *conn = (Connection*)data;
    conn->internalSendMessageThreadFunc();

    return NULL;
}

//create a thread
//given a function name
//and the parameter to be given to it
int Thread::create(func_type function_name, void* parameter)
{
    // Start the client thread and pass this instance to it
    int result = pthread_create(&threadid, NULL, function_name, parameter);

    if ( result != 0 )
    {
        return FALSE;
    }

    return TRUE;
}

