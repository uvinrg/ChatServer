/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_W32SEMAPHORE_H__
#define __CHATSERVER_W32SEMAPHORE_H__

class Semaphore
{
public:
    Semaphore():
        isinit(FALSE)
    { 
        ;
    }

    ~Semaphore()
    {
        if (isinit)
        {
            sem_close(semaphore);
            sem_unlink(semname.c_str());            
        }
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
    sem_t *semaphore;
    std::string semname;
    int isinit;
};

#endif //__CHATSERVER_W32SEMAPHORE_H__


