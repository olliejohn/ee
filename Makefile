
EXEC=yaw
SRCDIR=./src
DOCDIR=./doc

all: exec

exec: docs
	$(MAKE) -C $(SRCDIR)
	@echo -- Made executable at $(SRCDIR)/$(EXEC) --

debug: docs
	$(MAKE) debug -C $(SRCDIR)
	@echo -- Made debug executable at $(SRCDIR)/$(EXEC) --

docs:
	$(MAKE) -C $(DOCDIR)

clean:
	$(MAKE) clean -C $(DOCDIR)
	$(MAKE) clean -C $(SRCDIR)
	@echo -- Cleaned local source tree --

install:
	$(MAKE) install -C $(SRCDIR)
	$(MAKE) install -C $(DOCDIR)
	@echo -- Install Success --

uninstall:
	$(MAKE) uninstall -C $(SRCDIR)
	$(MAKE) uninstall -C $(DOCDIR)
	@echo -- Uninstall Success --

log:
	@./gitlog-to-changelog > ChangeLog
	@echo -- Updated ChangeLog from Git commit history --
