######################## Your choice #########################################
DEST          = $(LOCAL)/bin
PROGRAM       = sldtcp
CFLAGS        = -g -Wall -I${LOCAL}/include -I${ANT_EXP}/inc
LDFLAGS       =
LIBS          = $(LOCAL)/lib/libsld.a   $(LOCAL)/lib/libsup.a


######################## Depence     #########################################
EXTHDRS	      =
HDRS	      =
OBJS          = sldtcp.o
SRCS          = sldtcp.c

######################## Fixed       #########################################
CC            = g++
LINKER        = $(CC)
MAKEFILE      = Makefile
RINSTALL      = install

all:		$(PROGRAM)
$(PROGRAM):     $(OBJS) $(LIBS) $(MAKEFILE)
		$(LINKER) $(LDFLAGS) $(OBJS) $(LIBS) -o $(PROGRAM)
		@echo "done"

clean:;         rm -f $(OBJS) $(PROGRAM) core a.out ,*
depend:;        mkmf -f $(MAKEFILE) PROGRAM=$(PROGRAM) DEST=$(DEST)
install:        $(PROGRAM)  Rinstall
Rinstall:;      cp  $(PROGRAM)  $(DEST)
deinstall:;     make RINSTALL=deinstall $(MFLAGS) Rinstall

$(DEST)/$(PROGRAM): $(SRCS) $(LIBS) $(HDRS) $(EXTHDRS)
		@make -f $(MAKEFILE) DEST=$(DEST) install
