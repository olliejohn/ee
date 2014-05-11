# Yaw Lisp Makefile

CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O3
DEFS=-D_GNU_SOURCE
NDEBUG=-UDEBUG -DNDEBUG
CDEBUG=-g -UNDEBUG -DDEBUG
DEMO=lisp_demo
SOURCES=*.c
OBJECTS=*.o

all: demo
	@echo -- Lisp Make Success --

demo: build
	$(CC) $(OBJECTS) -o $(DEMO)

demoDebug: debug
	$(CC) $(OBJECTS) -o $(DEMO)

build:
	$(CC) $(CFLAGS) $(DEFS) $(NDEBUG) $(SOURCES)

debug:
	$(CC) $(CFLAGS) $(DEFS) $(CDEBUG) $(SOURCES)

clean:
	rm -rf *.o
	rm -rf $(DEMO)
