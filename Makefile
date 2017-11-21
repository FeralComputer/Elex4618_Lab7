# compiler
CC=g++

#compiler options
CFLAGS=-Wall -pedantic -c `pkg-config --cflags opencv` -std=c++11

#linker flags
LDFLAGS=-Wall -pedantic `pkg-config --libs opencv` -std=c++11 -lpigpio -lrt -lpthread

all: hello

hello: 4618_Template.o Base4618.o Control.o Tracker.o Text.o
	$(CC) $(LDFLAGS) 4618_Template.o Base4618.o Control.o Tracker.o Text.o -o main

Camera.o: Camera.cpp
	$(CC) $(CFLAGS) Camera.cpp

Base4618.o: Base4618.cpp
	$(CC) $(CFLAGS) Base4618.cpp
	
Tracker.o: Tracker.cpp
	$(CC) $(CFLAGS) Tracker.cpp
	
4618_Template.o: 4618_Template.cpp
	$(CC) $(CFLAGS) 4618_Template.cpp
	
CText.o: Text.cpp
	$(CC) $(CFLAGS) Text.cpp

clean:
	rm -rf *o hello
