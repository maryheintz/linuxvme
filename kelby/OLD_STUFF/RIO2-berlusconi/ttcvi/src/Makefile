# Makefile for L1 trigger daq software
# Cancel some implicit rules to let Make run faster.
%.o : %.p
%.o : %.f
%.o : %.F
%.o : %.r
%.c : %.l
%.c : %.y

# Set up compile and link flags.
# The -mposix switch is obsolete in Lynx 2.4.0: for the moment stick with
# the backwards compatible -mposix4d9. Later migrate to the new standard...
#

OSVER = $(shell uname -r)

CC =  gcc 
# for compilation with debug, use "make DBG=1"
ifdef DBG
CC = gcc -g
endif

CXX =  g++
# Set output files to have the names of the targets

# For optional very strict C standards, use "more WF=1"
WARNFULL = -Wshadow -Wstrict-prototypes -Wredundant-decls
WARN =
ifdef WF
WARN = $(WARNFULL)
endif
WARN = $(WARNFULL)
ifeq ($(OSVER),2.3.0)
  # Assume we wont have earlier versions than 2.3.0!?
  CFLAGS =  -g   -pedantic -Wall $(WARN) -c -D_POSIX_SOURCE $(INC) -o $@
  LDFLAGS =  -g -mposix -Xlinker -Map -Xlinker $(MAP)$(@F).map -o $@
else
  # Assume this is Lynx version 2.4.0 or later
  CFLAGS = -g -O2 -mposix4d9 -pedantic -Wall $(WARN) -c -D_POSIX_SOURCE $(INC) -o $@
  LDFLAGS = -g -mposix4d9 -o $@
endif

# Set machine dependent symbol so we compile the correct version of
# the map_vme procedure. At the moment we handle two processors:
# the Motorola MVME167 and the CES RIO. They contain different CPUs
# so for the moment we can just use "uname" to distinguish them.


  VMEPROC = cesrio


#
# Location of various components of the DAQ system
# ------------------------------------------------
#
# DAQROOT - gives the top level of the whole tree (system dependent)
#
# BIN     - is where the installed executable images go (make install)
# DEV     - is where the executable images are first created for testing
# MAP     - Link map files go here
# OBJ     - Location for .o files created during compilation
#
# From 16-Jul-1997, the daq software has numbered versions. These live in
# subdirectories v01, v02 etc from /users/atlast1. These each have their
# own src, obj, map and dev subdirectories where each version is built.
# The "make install" target installs from the vNN/dev directory into the
# top level /users/atlast1/bin directory. The top level dev directory should
# be set (by hand) with a softlink to the latest vNN/dev directory.
#
# SOURCE  - Root of source files, contains Makefile and sub-directories:
#    All of the sub-directories contain source code:
#       BCID - Interface and include files for BCID cards
#	DAQANA - Code directly used in daqana analysis process
#	DAQCTL - Code directly used in daqctl control program
#	DAQPROD - Code directly used in daqprod producer process
#	DAQREC - Code directly used in daqrec recorder process
#	FADC - Interface and include files for FADC cards
#	INITBM - Code directly used to initialise buffer manager
#	PRESENTER - Code directly used in the histogram presenter package
#	VIC - Interface and include files for VIC cards
#       OTHERS - Any other source files which don't fit in specific directories
#       INCLUDE - the .h files
# Where code is used by more than one component, an arbitary choice about where
# it should go has been made.
#
# DIAGROOT - gives the top level of the diagnostic software tree
# DIAGVER  - is the version of the diagnostic software to use
#

TILEROOT = /home/user/pilcher
TTCROOT = /home/user/pilcher/ttcvi

BIN	= $(TILEROOT)/bin
DEV	= ../dev/
ETC	= ../etc/
MAP	= ../map/
OBJ	= ../obj/

SOURCE	= .

all:  versions directories \
      $(DEV)ttcvi_ctl
      
#      @echo
#      @echo The Stockholm TTCvi run control system binaries are 
#      @echo up-to-date in $(DEV).
#      @echo Use "make install" to update the production directory $(BIN)
#      @echo
	


ttcvi_ctl: $(DEV)ttcvi_ctl

# Display versions of tools in use, to make sure we are using the correct ones
versions:
	@$(CC) -v
	@echo LynxOS version $(OSVER)
	@echo Building TTCvi run control at `date`...
	@echo
	
# Ensure all the directories we need actually exist
directories: $(BIN) $(DEV) $(ETC) $(MAP) $(OBJ)
$(BIN):
	mkdir $(BIN)
$(DEV):
	mkdir $(DEV)
	mkdir $(DEV)/etc
$(ETC):
	ln -s $(DAQETC) $(ETC)
$(MAP):
	mkdir $(MAP)
$(OBJ):
	mkdir $(OBJ)

# Install binaries in production bin directory
install: directories \
	$(BIN)ttcvi_ctl \
	@echo "Production version is now up-to-date in $(BIN)"

$(BIN)%: $(DEV)%
	cp -p $< $@

# Create tar file of src directory tree
export:
	rm -f ../src.tar
	(cd ..; tar cvf src.tar src/)

# Create new version of the TTCvi run controle software
# Copy the src tree and also *.las from dev/etc

# Lists of objects to build main targets

CTLOBJS  = 	$(OBJ)map_vme_cesrio.o \
		$(OBJ)ttcvi_interface.o \
		$(OBJ)ttcvi_rc.o \
		$(OBJ)screen.o

$(DEV)ttcvi_ctl : $(CTLOBJS)
	$(CC) $(LDFLAGS) $(CTLOBJS) 
	

$(OBJ)map_vme_cesrio.o : $(SOURCE)/map_vme_cesrio.c
	$(CC) $(CFLAGS) $(SOURCE)/map_vme_cesrio.c

$(OBJ)ttcvi_interface.o : $(SOURCE)/ttcvi_interface.c $(SOURCE)/ttcvi.h
	$(CC) $(CFLAGS) $(SOURCE)/ttcvi_interface.c 

$(OBJ)ttcvi_rc.o : $(SOURCE)/ttcvi_rc.c $(SOURCE)/ttcvi.h
	$(CC) $(CFLAGS) $(SOURCE)/ttcvi_rc.c

$(OBJ)screen.o : $(SOURCE)/screen.c 
	$(CC) $(CFLAGS) $(SOURCE)/screen.c
	
$(OBJ)libttcvi.o : $(SOURCE)/libttcvi.c $(SOURCE)/ttcvi.h $(SOURCE)/std_func.h
	$(CC) $(CFLAGS) $(SOURCE)/libttcvi.c

# Additional dependencies

.PHONY: clean
clean:	
	-rm $(DEV)ttcvi_rc
	-rm -v $(OBJ)*.o
	-rm -v $(MAP)*

# Dependencies of header files

TTCVI_H = $(source)/ttcvi.h
