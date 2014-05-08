#Yaw TUI Makefile

CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O3
LDFLAGS=-lncursesw -ltinfo
DEFS=-D_GNU_SOURCE
NDEBUG=-UDEBUG -DNDEBUG
CDEBUG=-g -UNDEBUG -DDEBUG
DEMO=tui_demo
SOURCES=tui.c

all: demo
	@echo -- TUI Make Success --

demo: build
	$(CC) $(DEMO).c tui.o $(LDFLAGS) -o $(DEMO)

build:
	$(CC) $(CFLAGS) $(DEFS) $(NDEBUG) $(SOURCES)

debug:
	$(CC) $(CFLAGS) $(DEFS) $(CDEBUG) $(SOURCES)

clean:
	rm -rf *.o
	rm -rf $(DEMO)
