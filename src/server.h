/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_SERVER_H__
#define __CHATSERVER_SERVER_H__

class Server
{
public:
    int start_on(int port_number, FILE* logfile = NULL);

    Server():
        log(NULL),
        conn(NULL)
    { 
        ;
    }

    ~Server() 
    { 
        if (log != NULL) 
        {
            fclose(log); 
            log = NULL;
        }

        if (conn != NULL)
        {
            delete conn;
            conn = NULL;
        }
    }

private:
    FILE *log;
    Connection *conn;

};

#endif //__CHATSERVER_SERVER_H__