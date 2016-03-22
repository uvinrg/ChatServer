#-----------------------------------------
#
#	 Copyright Vlad	Ionut Ursachi 2016
#	 Telnet	based C++ Chat Server
#
#-----------------------------------------

CC=g++
CFLAGS=-c -Wall -std=c++0x 
LDFLAGS=-pthread
INCPATH	 = -I../../platform/unix
INCPATH	+= -I../platform/unix
TDIR=out
TARGET=$(TDIR)/ChatServer

.PHONY:	directories

all: directories $(TARGET) $(TDIR)/main.o $(TDIR)/connection.o $(TDIR)/unixsemaphore.o $(TDIR)/unixsocket.o	$(TDIR)/unixthread.o	

$(TDIR)/main.o:	../../src/main.cpp
	$(CC) $(CFLAGS)	$(INCPATH) $< -o $@

$(TDIR)/connection.o: ../../src/connection.cpp
	$(CC) $(CFLAGS)	$(INCPATH) $< -o $@

$(TDIR)/unixsemaphore.o: ../../src/unix/unixsemaphore.cpp
	$(CC) $(CFLAGS)	$(INCPATH) $< -o $@

$(TDIR)/unixsocket.o: ../../src/unix/unixsocket.cpp
	$(CC) $(CFLAGS)	$(INCPATH) $< -o $@

$(TDIR)/unixthread.o: ../../src/unix/unixthread.cpp
	$(CC) $(CFLAGS)	$(INCPATH) $< -o $@

$(TARGET): $(TDIR)/main.o $(TDIR)/connection.o $(TDIR)/unixsemaphore.o $(TDIR)/unixsocket.o	$(TDIR)/unixthread.o	
	$(CC) $(LDFLAGS) $(TDIR)/main.o $(TDIR)/connection.o $(TDIR)/unixsemaphore.o $(TDIR)/unixsocket.o	$(TDIR)/unixthread.o -o $@

directories: $(TDIR)
$(TDIR):
	mkdir -p $(TDIR)

clean:
	rm -f $(TDIR)/*.o
	rm -f $(TDIR)/ChatServer
	rmdir $(TDIR)




