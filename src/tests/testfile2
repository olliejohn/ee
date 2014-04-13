
CC=gcc
CFLAGS=-c -Wall -O3
LDFLAGS=-lncurses -ltinfo
TARGET=ee
OBJECTS=$(shell ls | grep .*\.o)
SOURCES=$(shell ls | grep .*\.c)
BINDIR=/usr/bin
LIBDIR=/usr/lib

all: exec
	@echo -- Make Success --

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
