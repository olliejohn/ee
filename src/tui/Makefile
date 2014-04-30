CC=gcc
CFLAGS=-c -Wall -std=gnu99 -O3
LDFLAGS=-lncursesw -ltinfo
DEMO=tui_demo
SOURCES=tui.c

all: demo
	@echo -- TUI Make Success --

demo: build
	$(CC) $(DEMO).c tui.o $(LDFLAGS) -o $(DEMO)

build:
	$(CC) $(CFLAGS) $(SOURCES)

clean:
	rm -rf *.o
	rm -rf $(DEMO)
