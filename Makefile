
EXEC=yaw
SRCDIR=./src

all: exec

exec:
	$(MAKE) -C $(SRCDIR)
	@echo -- Made executable at $(SRCDIR)/$(EXEC) --

debug:
	$(MAKE) debug -C $(SRCDIR)
	@echo -- Made debug executable at $(SRCDIR)/$(EXEC) --

clean:
	$(MAKE) clean -C $(SRCDIR)
	@echo -- Cleaned local source tree --

install:
	$(MAKE) install -C $(SRCDIR)

uninstall:
	$(MAKE) uninstall -C $(SRCDIR)

log:
	@./gitlog-to-changelog > ChangeLog
	@echo -- Updated ChangeLog from Git commit history --
