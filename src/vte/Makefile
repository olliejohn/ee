# Yaw VTE Makefile

CC=gcc
CFLAGS=-c -Wall -O2 -std=gnu99
DEFS=-D_GNU_SOURCE -D_REENTRANT
CDEBUG=-g -UNDEBUG -DDEBUG
NDEBUG=-DNDEBUG -UDEBUG
SOURCES=*.c

all: build
	@echo -- VTE Make Success --

build:
	$(CC) $(CFLAGS) $(DEFS) $(NDEBUG) $(SOURCES)

debug:
	$(CC) $(CFLAGS) $(DEFS) $(CDEBUG) $(SOURCES)

clean:
	rm -f *.o
