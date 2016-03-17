/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_W32SEMAPHORE_H__
#define __CHATSERVER_W32SEMAPHORE_H__

class W32semaphore
{
public:
    W32semaphore():
        semaphore(NULL) 
    { 
        ;
    }

    ~W32semaphore()
    {
        if (semaphore != NULL)
            CloseHandle(semaphore);
    }

    //create a semaphore with an initial and a max count
    //a critical section is a semaphore with:
    //- an initial and max_count of 1
    //- we wait() at entering the critical section
    //- we increaseCount by 1 at exit
    int create(int initial_count, int max_count = 0);
    //increase its count by the specified amount
    int increaseCount(int increase_amount);
    //decrease the count by 1,
    //if count becomes 0, the function blocks
    //until another thread increases count by at least 1
    int wait();
private:
    HANDLE semaphore;
};

#endif //__CHATSERVER_W32SEMAPHORE_H__


