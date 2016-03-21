/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_DEFS_H__
#define __CHATSERVER_DEFS_H__

#ifndef UNREFERENCED_PARAMETER
#define UNREFERENCED_PARAMETER(P) (P)
#endif 

#ifndef TRUE
#define TRUE  1
#endif 

#ifndef FALSE
#define FALSE 0
#endif 

#ifndef ALWAYS_TRUE
#   define ALWAYS_TRUE ((void)0, TRUE)
#endif

#ifndef MaxCS
#   define MaxCS(x, y) ((y) > (x) ? (y) : (x))
#endif

//make sure to issue instructions on a single row
#define CHECK_LOG(x) if (log != NULL) x

enum
{
    CS_OK                  =  0,
    CS_INVALID_ARGS        = -1,
    CS_FAIL                = -2
};

#endif //__CHATSERVER_DEFS_H__
