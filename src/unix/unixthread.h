/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_W32THREAD_H__
#define __CHATSERVER_W32THREAD_H__

typedef void* (*func_type)(void*);

//dummy thread function to call the object's thread function
void* readMessageThreadFunc( void* data );
//dummy thread function to call the object's thread function
void* sendMessageThreadFunc( void* data );

class Thread
{
public:
    Thread()
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
    int create(func_type function_name, void* parameter);
private:    
    pthread_t threadid;
};

#endif //__CHATSERVER_W32THREAD_H__


