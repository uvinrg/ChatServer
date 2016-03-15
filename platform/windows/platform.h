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

using namespace std;

#define FD_SETSIZE      MAX_CLIENTS
#include <winsock2.h>

#include "../../src/defs.h"
#include "../../src/connection.h"
#include "../../src/server.h"
#include "../../src/win32/w32connection.h"

#define PLATFORM_CONNECTION Win32Connection

#endif //__CHATSERVER_PLATFORM_H__