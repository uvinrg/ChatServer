/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_CONFIG_H__
#define __CHATSERVER_CONFIG_H__

static const int DEFAULT_PORT_NUMBER = 9399;
static const int MAX_CLIENTS = 65000; //may not work for numbers higher than 65535
static const int MAX_LINE_SIZE = 1024; //maximum number of characters in a line, including '\0' terminator

#endif //__CHATSERVER_CONFIG_H__