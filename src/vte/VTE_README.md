#VTE

This is the terminal emulator subsystem of Yaw. It borrows heavily from libvterm
by Bryan Christ (GNU GPL v2) which is in turn based on ROTE by Bruno Takahashi
C. de Oliveira (GNU LGPL).

The main changes made to libvterm for Yaw are as follows:
- The original implementation depends on glib2; this version doesn't
- Many control sesquences handlers that were in seperate files have been merged
into vterm_csi.h
- Code formatting
- Due to not needing to be as versatile (this isn't a library, it's an
application) more of the ncurses handling can be built into the subsystem
itself instead of leaving it to be done closer to the root of the source tree.
Most of this is implemented in vte_driver.c, but it is also spread lightly
throughout the API.
