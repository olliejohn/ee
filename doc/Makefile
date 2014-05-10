
MANSRC=yaw.1
MANDIR=/usr/share/man

INFOSRC=yaw.texi
INFOOUT=yaw.info
INFODIR=/usr/share/info

all: man info

man:
	gzip -f -k $(MANSRC)

info:
	makeinfo $(INFOSRC) -o $(INFOOUT)
	gzip -f -k $(INFOOUT)

clean:
	rm -rf $(MANSRC).gz
	rm -rf $(INFOOUT)
	rm -rf $(INFOOUT).gz

install:
	cp $(MANSRC).gz $(MANDIR)/man1/$(MANSRC).gz
	cp $(INFOOUT).gz $(INFODIR)/$(INFOOUT).gz
	install-info --info-file=$(INFOOUT) --info-dir=$(INFODIR)

uninstall:
	rm -rf $(MANDIR)/man1/$(MANSRC).gz
