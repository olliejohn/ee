# Yaw VM Makefile

CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O3
DEFS=-D_GNU_SOURCE
NDEBUG=-UDEBUG -DNDEBUG
CDEBUG=-g -UNDEBUG -DDEBUG
SOURCES:=$(shell ls | grep \\.c | grep -v runtime)
RT=yawvm
ASM=yawsm
OBJECTS=*.o
BINDIR=/usr/bin

all: assembler runtime build
	@echo -- VM Make Success --

debug: assemblerDebug runtimeDebug buildDebug
	@echo -- VM Make Debug Success --

build:
	$(CC) $(CFLAGS) $(DEFS) $(NDEBUG) $(SOURCES)

buildDebug:
	$(CC) $(CFLAGS) $(DEFS) $(CDEBUG) $(SOURCES)

runtime:
	$(CC) $(CFLAGS) $(DEFS) -DFREESTANDING_RUNTIME $(NDEBUG) $(SOURCES)
	$(CC) $(OBJECTS) $(DEFS) $(NDEBUG) -o $(RT)

runtimeDebug:
	$(CC) $(CFLAGS) $(DEFS) -DFREESTANDING_RUNTIME $(CDEBUG) $(SOURCES)
	$(CC) $(OBJECTS) -o $(RT)

assembler: buildAssembler linkAssembler

assemblerDebug: buildAssemblerDebug linkAssembler

linkAssembler:
	$(CC) $(ASM).o -o $(ASM)

buildAssembler:
	$(CC) $(CFLAGS) $(DEFS) -DFREESTANDING_ASSEMBLER $(NDEBUG) $(ASM).c

buildAssemblerDebug:
	$(CC) $(CFLAGS) $(DEFS) -DFREESTANDING_ASSEMBLER $(CDEBUG) $(ASM).c

install: installAssembler installRuntime

uninstall: uninstallAssembler uninstallRuntime

installRuntime:
	strip $(RT)
	cp $(RT) $(BINDIR)
	chmod 755 $(BINDIR)/$(RT)

uninstallRuntime:
	rm -rf $(BINDIR)/$(RT)

installAssembler:
	strip $(ASM)
	cp $(ASM) $(BINDIR)
	chmod 755 $(BINDIR)/$(ASM)

uninstallAssembler:
	rm -rf $(BINDIR)/$(ASM)

clean:
	rm -rf *.o $(ASM) $(RT)
