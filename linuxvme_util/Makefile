###############################################################################
#
# Filename: 	Makefile
# 
# Description:	Makefile for Linux VME Utility Program.
#
# $Revision: 1.1.1.1 $
#
# $Date: 2005/05/24 14:34:03 $
#
# $Source: z:\\cvsroot/Linuxvme2/linuxvme_util/Makefile,v $
#
# Copyright 2000 - 2005 Concurrent Technologies.
#
###############################################################################

# Defaults
RELEASE = TRUE
POWER_PC = FALSE

TARGET = linuxvme
OBJS = $(TARGET)

ifeq ($(POWER_PC),TRUE)
INCLUDE = /opt/eldk/ppc_74xx/usr/include
LIBPATH = /opt/eldk/ppc_74xx/usr/lib
CC 	= ppc_74xx-gcc
LD	= ppc_74xx-ld
else
INCLUDE = /usr/include
LIBPATH = /usr/lib
CC 	= gcc
LD	= ld
endif

LIBS    = -lncurses -lpthread -lcctvme
CFLAGS  = -Wall -D_REENTRANT

ifeq ($(RELEASE),FALSE)
# options for development
SRC = main.c thread.c debug.c
CFLAGS  += -g -DDEBUG
else
# options for release
SRC = main.c thread.c
endif

all: .depend $(TARGET)
ifeq ($(RELEASE),FALSE)
	@echo ++WARNING++  Development Build
endif

depend .depend dep:
	$(CC) $(CFLAGS) -M *.c > $@

$(TARGET): $(SRC:.c=.o)
	$(CC) $(CFLAGS) -I$(INCLUDE) $^ -o $@ -L . -L $(LIBPATH) $(LIBS)

clean:
	rm -f $(SRC:.c=.o) $(TARGET) *~ core .depend
