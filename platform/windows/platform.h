/*--------------------------------------

    Copyright Vlad Ionut Ursachi 2016
    Telnet based C++ Chat Server

----------------------------------------*/

#ifndef __CHATSERVER_PLATFORM_H__
#define __CHATSERVER_PLATFORM_H__

#include "config.h"

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

//STL
#include <string>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>

//our FD_SET structure must accomodate all clients plus the server listen socket
#define FD_SETSIZE      (MAX_CLIENTS + 1)
#include <winsock2.h>

#include "../../src/defs.h"
#include "../../src/win32/w32semaphore.h"
#include "../../src/win32/w32thread.h"
#include "../../src/win32/w32socket.h"
#include "../../src/critsection.h"
#include "../../src/connection.h"

#endif //__CHATSERVER_PLATFORM_H__

