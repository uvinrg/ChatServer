/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_UNIXSEMAPHORE_H__
#define __CHATSERVER_UNIXSEMAPHORE_H__

class Semaphore
{
public:
    Semaphore():
        semaphore(NULL) 
    { 
        ;
    }

    ~Semaphore()
    {
        //TODO CLOSE SEMAPHORE
    }

    //create a semaphore with an initial count
    int create(int initial_count);
    //increase its count by the specified amount
    int increaseCount(int increase_amount);
    //decrease the count by 1,
    //if count becomes 0, the function blocks
    //until another thread increases count by at least 1
    int wait();
private:
    //TODO SEMAPHORE HANDLE
};

#endif //__CHATSERVER_UNIXSEMAPHORE_H__


