# Yaw Makefile

CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O3
LDFLAGS=-lncursesw -ltinfo -ltui -lvterm
TARGET=yaw
OBJECTS=$(shell ls | grep [.*][\.o])
SOURCES:=$(shell ls | grep [.*][\.c])
BINDIR=/usr/bin
LIBDIR=/usr/lib

all: debugExec
	@echo -- Make Success --

debugExec: debugBuild link

exec: build link

link:
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

debugBuild:
	$(CC) $(CFLAGS) -g $(SOURCES)

build:
	$(CC) $(CFLAGS) $(SOURCES)

clean:
	rm -rf *.o

install: exec
	strip $(TARGET)
	cp $(TARGET) $(BINDIR)
	chmod 755 $(BINDIR)/$(TARGET)
