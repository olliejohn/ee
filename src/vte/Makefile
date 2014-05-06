# Yaw VTE Makefile

CC=gcc
CFLAGS=-c -Wall -O2 -std=gnu99
DEFS=-D_REENTRANT -DNDEBUG
CDEBUG=-g -UNDEBUG -DDEBUG
SOURCES=*.c

all: build
	@echo -- VTE Make Success --

build:
	$(CC) $(CFLAGS) $(DEFS) $(SOURCES)

debug:
	$(CC) $(CFLAGS) $(DEFS) $(CDEBUG) $(SOURCES)

clean:
	rm -f *.o
