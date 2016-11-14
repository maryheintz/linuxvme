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
DEBUG = yes
#DEBUG = no


STD_CFLAGS = -O2 -Wall -pipe -m486 $(INCLUDEDIRS) -DSLD_VERSION=\"$(VERSION)\"

ifeq ($(DEBUG),yes)
CFLAGS = $(STD_CFLAGS) -g
else
CFLAGS = $(STD_CFLAGS) -fomit-frame-pointer
endif

CFILES = sldstat.c
OFILES = $(CFILES:.c=.o)

all:	sldstat

install:
	cp sldstat ${LOCAL}/bin

clean:
	rm -f sldstat $(OFILES)

sldtest:	$(OFILES)
	$(CC) -o sldstat $(OFILES)

# ../Makefile makes sure version changes are compiled in.
sldstat.o: ../Makefile sldstat.c sld.h

sld.h :
	ln -s ../$@
