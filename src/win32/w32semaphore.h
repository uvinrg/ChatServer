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
    int Create(int initial_count, int max_count = 0);
    //increase its count by the specified amount
    int IncreaseCount(int increase_amount);
    //decrease the count by 1,
    //if count becomes 0, the function blocks
    //until another thread increases count by at least 1
    int Wait();
private:
    HANDLE semaphore;
};

#endif //__CHATSERVER_W32SEMAPHORE_H__


