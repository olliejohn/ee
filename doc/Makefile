
MANSRC=yaw.1
MANDIR=/usr/share/man

all: man

man:
	gzip -k $(MANSRC)

clean:
	rm -rf $(MANSRC).gz

install:
	cp $(MANSRC).gz $(MANDIR)/man1/$(MANSRC).gz

uninstall:
	rm -rf $(MANDIR)/man1/$(MANSRC).gz
