CFLAGS=-c -Wall -O2 -std=gnu99 -fPIC
LDFLAGS=-shared -lncursesw -ltinfo -lutil
DEFS=-D_REENTRANT

makefile: all

all: libvterm

debug: libvterm

libvterm:
	gcc $(CFLAGS) $(DEFS) *.c $(LDFLAGS)
	gcc -o libvterm.so *.o $(LDFLAGS)

clean:
	rm -f *.o
	rm -f *.so
