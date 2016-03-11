/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#include "platform.h"

int check_number(char* str, int& out)
{
    
}

int main(int argc, char* argv[])
{
    int i, j;
    int port_number = DEFAULT_PORT_NUMBER;

    printf("Usage: ChatServer [-port port_number] [-log log_file_name]\n");

    //check number of parameters
    if (argc != 1 && argc != 3 && argc != 5)
    {
        printf("Invalid number of parameters\n");
        return CS_INVALID_ARGS;
    }

    for (i = 1; i < argc - 1; i++)
    {
        //check arguments
        if (strcmp(argv[i], "-port") == 0)
        {
            ////check the argument is a number
            //for (j = 0; 
        }
    }


    Server chatserver;

    if (chatserver.try_on(port_number) != 0)
    {
        printf("Starting on %d port number\n", port_number);
        chatserver.start_on(port_number);
    }
    else
    {
        printf("Port number %d in use!\n");
    }

    return CS_OK;
}
