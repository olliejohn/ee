# Yaw Makefile

CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O3
LDFLAGS=-lncursesw -ltinfo -lutil
DEFS=-DNDEBUG
CDEBUG=-g -UNDEBUG -DDEBUG
TARGET=yaw
OBJECTS=*.o vte/*.o tui/*.o
SOURCES=*.c
BINDIR=/usr/bin
LIBDIR=/usr/lib

all: exec
	@echo -- Yaw Make Success --

debug: buildTuiDebug buildVteDebug buildDebug link



exec: buildTui buildVte build link

link:
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

build:
	$(CC) $(CFLAGS) $(DEFS) $(SOURCES)

buildVte:
	$(MAKE) -C vte

buildTui:
	$(MAKE) -C tui

buildDebug:
	$(CC) $(CFLAGS) $(DEFS) $(CDEBUG) $(SOURCES)

buildVteDebug:
	$(MAKE) debug -C vte

buildTuiDebug:
	$(MAKE) debug -C tui

clean:
	rm -rf *.o
	rm $(TARGET)
	$(MAKE) clean -C vte
	$(MAKE) clean -C tui
	@echo -- Clean Success --

install:
	strip $(TARGET)
	cp $(TARGET) $(BINDIR)
	chmod 755 $(BINDIR)/$(TARGET)

uninstall:
	rm $(BINDIR)/$(TARGET)
