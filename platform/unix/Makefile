#-----------------------------------------
#
#    Copyright Vlad Ionut Ursachi 2016
#    Telnet based C++ Chat Server
#
#-----------------------------------------

CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=main.cpp hello.cpp factorial.cpp
OBJECTS=$(SOURCES:.cpp=.o)
TARGET=ChatServer

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
    $(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
    $(CC) $(CFLAGS) $< -o $@

clean:
    rm *.o TARGET



