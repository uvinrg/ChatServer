/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_CRTISECTION_H__
#define __CHATSERVER_CRTISECTION_H__

#include "platform.h"

//a critical section is a semaphore with:
//- an initial count of 1
//- we wait() at entering the critical section
//- we increaseCount by 1 at exit
class Critsection
{
public:
    Critsection():
        state(FALSE)
    { 
        semaphore.create(1);
    }

    ~Critsection()
    {
        ;
    }

    void enterCriticalSection()
    {
        //wait on a semaphore with count 1
        //will block on the second wait()
        semaphore.wait();
        //we are now in a critical section
        state = TRUE;
    }

    void leaveCriticalSection()
    {
        //only allow exit of a critical section if a corresponding enter has been called
        if (state)
        {   
            //mark leaving critical section
            state = FALSE;
            //leave critical section
            semaphore.increaseCount(1);    
        }
    }
private:
    Semaphore semaphore; //the semaphore used to implement the critical section
    int state; //TRUE if inside a critical section, FALSE otherwise
};

#endif //__CHATSERVER_CRTISECTION_H__


