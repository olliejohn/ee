#Yaw

Yaw is a command line text editor with the goal of being as easy to jump into as
Nano and as configuarable as Vim.

NOTE: Yaw is still under heavy development - it is distributed in the hope that
it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
Public License for more details. Do NOT use Yaw for editing documents that you
don't have copies of elsewhere - it is not yet stable.

Yaw currently features:
- Opening multiple buffers at once in tabs
- A built in terminal emulator
- Extensibility and configuation with the built in Lisp interpretter

My main focus for the moment is taking what we have so far (about 5800 lines of
C as of 6th May 2014) and making it as stable and, ultimately, useable as
possible. I will, however, continue to add features along the way.


###Building and Installing
Either run 'make' or 'make debug' in the src directory depending on your needs.
This can then be installed on your system by running 'make install'. In order
to build Yaw, you will need GCC, ncursesw and some variant of make. Updating the
ChangeLog will also require Perl and Git whilst building the documentation
files will require man, gzip, info and makeinfo.


###Licensing
Yaw is available under the GNU GPL Version 2. See 'LICENSE' for more details.
