#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or 
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
# 

#
# If you want to use a debugger, set this thing to yes.
#
#DEBUG = yes
DEBUG = no


STD_CFLAGS = -O2 -Wall -pipe -m486 $(INCLUDEDIRS) -DSLD_VERSION=\"$(VERSION)\"

ifeq ($(DEBUG),yes)
CFLAGS = $(STD_CFLAGS) -g
else
CFLAGS = $(STD_CFLAGS) -fomit-frame-pointer
endif

CFILES = sldtest.c bufuser.c queue.c
OFILES = $(CFILES:.c=.o)

all:	sldtest

install:
	cp sldtest ${LOCAL}/bin
	cp gen     ${LOCAL}/bin

clean:
	rm -f sldtest $(OFILES)

sldtest:	$(OFILES)
	$(CC) -o sldtest $(OFILES)

# ../Makefile makes sure any version changes are compiled in.
sldtest.o: ../Makefile sldtest.c sld.h

bufuser.o: bufuser.c sldpriv.h sld.h queue.h amcc.h

queue.o: queue.c queue.h

# these files are linked in from the parent directory
bufuser.c queue.c sldpriv.h sld.h queue.h amcc.h :
	ln -s ../$@
