# ChatServer
Telnet based C++ Chat Server

Features:
- server supports all the commands from the specification plus /whisper
- when connected to the server with telnet as a client, the server will inform all of its commands.
- joining a non-existent room will create it and place the user inside it.
- in each ChatServer/platform/* folder a config.h file is provided where the developer can change the default port number, the maximum number of concurrent clients and the maximum line size of a chat message.
- the server automatically detects when users login or drop out and performs the required actions, removing them from chat rooms and informing the other users.

Build instructions:

Unix and MacOS:
- change directory to ChatServer/platform/unix
- type "make" to build
- type "make clean" to clear
- change directory to ChatServer/platform/unix/out
- start the server with "./ChatServer". It will start on port 9399
- optionally you can start it with "./ChatServer -port port_number"
- connect with telnet to the server ip and port to start chatting

Windows:
- change directory to ChatServer/platform/windows
- open project with ChatServer.sln (Visual Studio 2012)
- build solution and run it
- similarly, the command-line can include "-port port_number" to start on a different port
- connect with telnet to the server ip and port to start chatting

