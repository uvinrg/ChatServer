/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_DEFS_H__
#define __CHATSERVER_DEFS_H__

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (P)
#endif UNREFERENCED_PARAMETER

#ifndef TRUE
#define TRUE  1
#endif //TRUE

#ifndef FALSE
#define FALSE 0
#endif //FALSE

enum
{
    CS_OK                  =  0,
    CS_INVALID_ARGS        = -1
};

#endif __CHATSERVER_DEFS_H__