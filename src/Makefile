# Yaw Makefile

CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O3
LDFLAGS=-lncursesw -ltinfo -ltui
TARGET=yaw
OBJECTS=$(shell ls | grep [.*][\.o])
SOURCES:=$(shell ls | grep [.*][\.c])
BINDIR=/usr/bin
LIBDIR=/usr/lib

all: debugExec
	@echo -- Make Success --

debugExec: debugBuild
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

debugBuild:
	$(CC) $(CFLAGS) -g $(SOURCES)

exec: build
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

build:
	$(CC) $(CFLAGS) $(SOURCES)

clean:
	rm -rf *.o

install: exec
	strip $(TARGET)
	cp $(TARGET) $(BINDIR)
	chmod 755 $(BINDIR)/$(TARGET)
