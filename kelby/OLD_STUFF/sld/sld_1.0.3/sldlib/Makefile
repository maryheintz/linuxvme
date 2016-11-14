#
# If you want to use a debugger, set this thing to yes.
#
#DEBUG = yes
DEBUG = no

LIBRARY       = libsld.a

STD_CFLAGS = -O2 -Wall -pipe -m486 $(INCLUDEDIRS) -DSLD_VERSION=\"$(VERSION)\"

ifeq ($(DEBUG),yes)
CFLAGS = $(STD_CFLAGS) -g
else
CFLAGS = $(STD_CFLAGS) -fomit-frame-pointer
endif

HDRS          = amcc.h    \
		queue.h   \
		sld.h     \
		sldpriv.h

OBJS          = bufuser.o  \
		queue.o


all:		$(LIBRARY)
$(LIBRARY):     $(OBJS) $(MAKEFILE)
		ar cru $(LIBRARY) $(OBJS)
		ranlib $(LIBRARY)
		@echo "done"

install:	$(LIBRARY)
		cp $(HDRS) ${LOCAL}/include
		install -g $(GRP) $(LIBRARY) ${LOCAL}/lib
		ranlib ${LOCAL}/lib/$(LIBRARY)

clean:
		rm -f $(OBJS)

# ../Makefile makes sure any version changes are compiled in.
bufuser.o: bufuser.c sldpriv.h sld.h queue.h amcc.h

queue.o:   queue.c queue.h

