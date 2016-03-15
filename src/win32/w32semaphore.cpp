/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#include "platform.h"
#include "w32semaphore.h"

//create a semaphore with an initial and a max count
int W32semaphore::Create(int initial_count, int max_count)
{
    //check for existing semaphore
    if (semaphore != NULL)
        CloseHandle(semaphore);

    if (max_count == 0)
        max_count = ((UINT32)1 << 31) - 1;

    semaphore = CreateSemaphore( 
        NULL,           // default security attributes
        initial_count,  // initial count
        max_count,      // maximum count
        NULL);          // unnamed semaphore

    if (semaphore == NULL) 
    {
        return CS_FAIL;
    }

    return CS_OK;
}

//increase its count by the specified amount
int W32semaphore::IncreaseCount(int increase_amount)
{
    if (ReleaseSemaphore( 
            semaphore,        // handle to semaphore
            increase_amount,  // increase count by the amount specified
            NULL)             // not interested in previous count
            == 0)
    {
        return CS_FAIL;
    }

    return CS_OK;
}

//decrease the count by 1,
//if count becomes 0, the function blocks
//until another thread increases count by at least 1
int W32semaphore::Wait()
{
    WaitForSingleObject( semaphore, INFINITE );

    return CS_OK;
}
