/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#include "platform.h"
#include "unixsemaphore.h"

//create a semaphore with an initial and a max count
int Semaphore::create(int initial_count)
{  
    //create semaphore
    if (sem_init(&semaphore, 0, initial_count) < 0)
    {
        isinit = FALSE;
        return CS_FAIL;
    }
    else
    {
        isinit = TRUE;
        return CS_OK;
    }
}

//increase its count by the specified amount
int Semaphore::increaseCount(int increase_amount)
{
    int i;

    if (!isinit)
        return CS_FAIL;
        
    for (i = 0; i < increase_amount; i++)
    {
        if (sem_post(&semaphore) < 0)
            return CS_FAIL;
    }

    return CS_OK;
}

//decrease the count by 1,
//if count becomes 0, the function blocks
//until another thread increases count by at least 1
int Semaphore::wait()
{
    if (!isinit)
        return CS_FAIL;

    sem_wait(&semaphore);

    return CS_OK;
}
