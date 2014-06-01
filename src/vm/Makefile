# Yaw VM Makefile

CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O3
DEFS=-D_GNU_SOURCE
NDEBUG=-UDEBUG -DNDEBUG
CDEBUG=-g -UNDEBUG -DDEBUG
DEMO=vm_demo
SOURCES:=$(shell ls | grep \\.c | grep -v demo)
ASM=yawsembler
OBJECTS=*.o
BINDIR=/usr/bin

all: demo assembler
	@echo -- VM Make Success --

debug: demoDebug assemblerDebug

demo: buildVM
	$(CC) $(DEMO).c $(OBJECTS) $(DEFS) $(NDEBUG) -o $(DEMO)

demoDebug: debugVM
	$(CC) $(DEMO).c $(OBJECTS) $(DEFS) $(CDEBUG) -o $(DEMO)

buildVM:
	$(CC) $(CFLAGS) $(DEFS) $(NDEBUG) $(SOURCES)

debugVM:
	$(CC) $(CFLAGS) $(DEFS) $(CDEBUG) $(SOURCES)

assembler: buildAssembler linkAssembler

assemblerDebug: buildAssemblerDebug linkAssembler

linkAssembler:
	$(CC) $(ASM).o -o yawsembler

buildAssembler:
	$(CC) $(CFLAGS) $(DEFS) -DFREESTANDING_ASSEMBLER $(NDEBUG) $(ASM).c

buildAssemblerDebug:
	$(CC) $(CFLAGS) $(DEFS) -DFREESTANDING_ASSEMBLER $(CDEBUG) $(ASM).c

installAssembler:
	strip $(ASM)
	cp $(ASM) $(BINDIR)
	chmod 755 $(BINDIR)/$(ASM)

uninstallAssembler:
	rm -rf $(BINDIR)/$(ASM)

clean:
	rm -rf *.o
	rm -rf $(DEMO)
