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
# the overall version number
#
REL = 1
MAJOR = 0
MINOR = 3
# EXTRA = a
export VERSION := $(REL).$(MAJOR).$(MINOR)$(EXTRA)


#
# Configurable items:
# 1. source directory of the linux kernel
# 2. debug support (debugger, symbols and such)
# 3. use bottom half handling or not
#
# LINUXSOURCE = /usr/src/linux
LINUXSOURCE = /data/piep/usr_src/linux
SLD_DEBUG = yes
SLD_BH = yes


VER = $(shell uname -r)
MODDIR = /lib/modules/$(VER)/misc
CONF = $(wildcard /etc/modules.conf)
ifeq ($(strip $(CONF)),)
CONF = /etc/conf.modules
endif

INCLUDEDIRS = -I$(LINUXSOURCE)/include

STD_CFLAGS = -D__KERNEL__ -DCPU=686 -DMODULE -DEXPORT_SYMTAB -O2 -Wall \
		-pipe -m486 -DSLD_VERSION=\"$(VERSION)\" $(INCLUDEDIRS)

ifeq ($(SLD_DEBUG),yes)
CFLAGS = $(STD_CFLAGS) -g
else
CFLAGS = $(STD_CFLAGS) -fomit-frame-pointer
endif

ifeq ($(SLD_BH),yes)
CFLAGS += -D__SLD_USE_BH__=1
endif

CFILES = queue.c bufkern.c bufuser.c ddf.c hwd.c
OFILES = $(CFILES:.c=.o)

all:	sld.o
	$(MAKE) -C sldtest
	$(MAKE) -C sldstat
	@echo Created version $(VERSION)

dist:
	cd ..; \
		rm sld_$(VERSION).tar.gz ; \
		tar --create --file=sld_$(VERSION).tar \
			sld/README sld/TODO sld/COPYING; \
		find sld -type f -name *.[ch] -exec \
			tar --append --file=sld_$(VERSION).tar {} \; ; \
		find sld -type f -name Makefile -exec \
			tar --append --file=sld_$(VERSION).tar {} \; ; \
		gzip sld_$(VERSION).tar

compile: sld.o

install:
	-/sbin/rmmod sld
	cp -f sld.o $(MODDIR)
	/sbin/depmod -ae
	/sbin/modprobe sld

sld.o: $(OFILES)
	$(LD) -m elf_i386 -r $^ -o $@

clean:
	rm -f sld.o $(OFILES)
	$(MAKE) -C sldtest clean
	$(MAKE) -C sldstat clean

bufkern.o: bufkern.c sld.h queue.h amcc.h bufintern.h

bufuser.o: bufuser.c sldpriv.h sld.h queue.h amcc.h

ddf.o: Makefile ddf.c sld.h hwddef.h sldpriv.h queue.h amcc.h bufintern.h

hwd.o: hwd.c sld.h sldpriv.h queue.h amcc.h hwddef.h slink.h

queue.o: queue.c queue.h
