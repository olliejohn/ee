# Yaw Makefile

CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O3 -Itui
LDFLAGS=-lncursesw -ltinfo -lutil
TARGET=yaw
OBJECTS=*.o vte/*.o tui/*.o
SOURCES=*.c
BINDIR=/usr/bin
LIBDIR=/usr/lib

all: debugExec
	@echo -- Make Success --

debugExec: debugBuild link

exec: build link

link:
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)

debugBuild: buildtui buildvte
	$(CC) $(CFLAGS) -g $(SOURCES)

build: buildtui buildvte
	$(CC) $(CFLAGS) $(SOURCES)

buildvte:
	$(MAKE) -C vte

buildtui:
	$(MAKE) -C tui

clean:
	rm -rf *.o
	$(MAKE) clean -C vte
	$(MAKE) clean -C tui
	@echo -- Clean Success --

install: exec
	strip $(TARGET)
	cp $(TARGET) $(BINDIR)
	chmod 755 $(BINDIR)/$(TARGET)

uninstall:
	rm $(BINDIR)/$(TARGET)
