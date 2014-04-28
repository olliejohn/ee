# Yaw Makefile

CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O3
LDFLAGS=-lncursesw -ltinfo -ltui -lvterm
TARGET=yaw
OBJECTS=*.o
SOURCES=*.c
BINDIR=/usr/bin
LIBDIR=/usr/lib

all: debugExec
	@echo -- Make Success --

debugExec: debugBuild link

exec: build link

link:
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

debugBuild: buildvte
	$(CC) $(CFLAGS) -g $(SOURCES)

build: buildvte
	$(CC) $(CFLAGS) $(SOURCES)

buildvte:
	$(MAKE) -C vte

clean:
	rm -rf *.o

install: exec
	strip $(TARGET)
	cp $(TARGET) $(BINDIR)
	chmod 755 $(BINDIR)/$(TARGET)
