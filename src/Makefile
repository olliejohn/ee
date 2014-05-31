# Yaw Makefile

CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O3
LDFLAGS=-lncursesw -ltinfo -lutil
DEFS=-D_GNU_SOURCE
NDEBUG=-DNDEBUG -UDEBUG
CDEBUG=-g -UNDEBUG -DDEBUG
TARGET=yaw
OBJECTS=*.o vm/*.o lisp/*.o tui/*.o vte/*.o
SOURCES=*.c
BINDIR=/usr/bin
LIBDIR=/usr/lib

all: exec
	@echo -- Yaw Make Success --

d: debug
debug: buildVMDebug buildLispDebug buildTuiDebug buildVteDebug buildDebug link

exec: buildVM buildLisp buildTui buildVte build link

link:
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

build:
	$(CC) $(CFLAGS) $(DEFS) $(NDEBUG) $(SOURCES)

buildVM:
	$(MAKE) -C vm

buildLisp:
	$(MAKE) -C lisp

buildTui:
	$(MAKE) -C tui

buildVte:
	$(MAKE) -C vte

buildDebug:
	$(CC) $(CFLAGS) $(DEFS) $(CDEBUG) $(SOURCES)

buildVMDebug:
	$(MAKE) -C debug vm

buildLispDebug:
	$(MAKE) debug -C lisp

buildTuiDebug:
	$(MAKE) debug -C tui

buildVteDebug:
	$(MAKE) debug -C vte

clean:
	rm -rf *.o
	rm $(TARGET)
	$(MAKE) clean -C vte
	$(MAKE) clean -C tui
	$(MAKE) clean -C lisp
	$(MAKE) clean -C vm
	@echo -- Clean Success --

install:
	strip $(TARGET)
	cp $(TARGET) $(BINDIR)
	chmod 755 $(BINDIR)/$(TARGET)

uninstall:
	rm $(BINDIR)/$(TARGET)
