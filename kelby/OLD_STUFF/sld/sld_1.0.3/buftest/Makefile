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
# Configurable items:
# 1. source directory of the linux kernel
# 2. debug support
# 3. use bottom half handling or not
#
LINUXSOURCE = /usr/src/linux
LSD_DEBUG = yes
LSD_BH = yes


VER = $(shell uname -r)
MODDIR = /lib/modules/$(VER)/misc
CONF = $(wildcard /etc/modules.conf)
ifeq ($(strip $(CONF)),)
CONF = /etc/conf.modules
endif

INCLUDEDIRS = -I$(LINUXSOURCE)/include

STD_CFLAGS = -D__KERNEL__ -DMODULE -O -Wall -pipe -m486 $(INCLUDEDIRS)

ifeq ($(LSD_DEBUG),yes)
CFLAGS = $(STD_CFLAGS) -g -DDEBUG=1
else
CFLAGS = $(STD_CFLAGS) -O2 -fomit-frame-pointer
endif

ifeq ($(LSD_BH),yes)
CFLAGS += -D__LSD_USE_BH__=1
endif

CFILES = queue.c bufkern.c bufuser.c testbuf.c
OFILES = $(CFILES:.c=.o)

all:	testbuf


clean:
	rm -f $(OFILES) testbuf

testbuf:	$(OFILES)
	$(CC) -o testbuf $(OFILES)

$(OFILES): buf.h bufintern.h queue.h lsd.h lsdpriv.h amcc.h slink.h

lsd.h:	buf.h

lsdpriv.h:	queue.h amcc.h

bufintern.h:	buf.h lsdpriv.h

hwddef.h:	lsdpriv.h

