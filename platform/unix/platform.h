/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_PLATFORM_H__
#define __CHATSERVER_PLATFORM_H__

#include "config.h"

#define SOCKET int
typedef int                 BOOL;

#ifdef __APPLE__
//macos allows redefinition of setsize for select just like windows
#   define FD_SETSIZE      ((MAX_CLIENTS) + 1)
#else //__APPLE__
//linux requires some additional hacks to increase size for select
#   include <sys/types.h> 
#   undef __FD_SETSIZE
#   define __FD_SETSIZE ((MAX_CLIENTS) + 1)
#endif //__APPLE__

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

//STL
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>

#include "../../src/defs.h"
#include "../../src/unix/unixsemaphore.h"
#include "../../src/unix/unixthread.h"
#include "../../src/unix/unixsocket.h"
#include "../../src/critsection.h"
#include "../../src/connection.h"

#endif //__CHATSERVER_PLATFORM_H__

