#------------------------------------------------------------------------
# Variable PLATFORM has to be initialize to a PPC platform
# Can be : RIOII RTPC MVME160X MVME1300
PLATFORM = RIOII
#------------------------------------------------------------------------
#include $(PRO)/rules/makerules

# Options for the compiler
CFLAGS = -c -g -ansi -D$(PLATFORM) -I$(PRO_INC) $(CES_INC)

# Additional options for the S-LINK library
# Can be :
# DEBUG, REFRAINED_REQUEST, LINK_4BITS, LIBS5933
# POINTER_CHECK, PARAMETER_CHECK, LDOWN_CHECK
# FIFO_CHECK, FIFO_WAIT (by default)

CFLAGS_SLINK = -DPOINTER_CHECK -DPARAMETER_CHECK -DLDOWN_CHECK -DLIBS5933

# Options for the linker
LIBS    = -liomintf -liomutils -liomdaq -liomsched -lemulator -lrctl \
          -lcomlib -lmsg -lerr -lsysind -lgpih -lfifo -lbma -lvme -lSLINK -lpci -ls5933 -liomuio -llynx

LFLAGS  = $(LFLAGS_ATD) -lSLINK -lmenu -lpci -ls5933 -liomuio -llynx
LLFLAGS = -g $(LFLAGS_ATD) $(LIBS)

# List of object files which are real targets
OBJECTS = libSLINK.o libSLINK_O.o

# List of library files in order to define default targets
LIBRARIES_lynxos = libSLINK.a libmenu.a

# List of binary files in order to define default targets
BINARIES_lynxos = testSLINK testSLINK_O Source Destination Destination_SLIDAS Destination_statis  Destination_statis_SLIDAS Test_slink_rob_in Test_slink_rob_in_SLIDAS

#------------------------------------------------------------------------
INST_INC = SLINK.h menu.h term_control.h
INST_BIN = $(BINARIES_$(OS_TYPE))
INST_LIB = $(LIBRARIES_$(OS_TYPE))

#------------------------------------------------------------------------
#Precises default targets
all:: $(INST_LIB) $(INST_BIN)

term_control.o : term_control.c term_control.h
libmenu.a : menu.o term_control.o
	$(MAKELIB)

#How to build libSLINK.a
libSLINK.o : libSLINK.c SLINK.h
	$(CC) $(CFLAGS) $(CFLAGS_SLINK)  $< -o $@
libSLINK.a : libSLINK.o
	$(MAKELIB)

libSLINK_O.o : libSLINK.c SLINK.h
	$(CC) -O3 $(CFLAGS) $(CFLAGS_SLINK) $< -o $@

testSLINK.o : testSLINK.c SLINK.h menu.h
testSLINK_O : testSLINK.o libSLINK_O.o libmenu.a
	$(CC) $< -o $@ $(LFLAGS)

Source.o : Source.c SLINK.h
Destination.o : Destination.c SLINK.h
Destination_statis.o : Destination_statis.c  SLINK.h
Destination_SLIDAS.o : Destination.c SLINK.h
	$(CC) $(CFLAGS) -DSLIDAS $< -o $@
Destination_statis_SLIDAS.o : Destination_statis.c SLINK.h
	$(CC) $(CFLAGS) -DSLIDAS $< -o $@

Destination : Destination.o libSLINK.a
	$(CC) $< -o $@ $(MY_LIB)libSLINK.a $(LFLAGS)
Destination_statis : Destination_statis.o libSLINK.a
	$(CC) $< -o $@ $(MY_LIB)libSLINK.a $(LFLAGS)
Destination_SLIDAS : Destination_SLIDAS.o libSLINK.a
	$(CC) $< -o $@ $(MY_LIB)libSLINK.a $(LFLAGS)
Destination_statis_SLIDAS : Destination_statis_SLIDAS.o libSLINK.a
	$(CC) $< -o $@ $(MY_LIB)libSLINK.a $(LFLAGS)

Test_slink_rob_in : Test_slink_rob_in.o 
	$(CC) $< -o $@ $(MY_LIB)libSLINK.a $(LLFLAGS) 
Test_slink_rob_in.o : Test_slink_rob_in.c
	$(CC) $(CFLAGS) -c $< -o $@
Test_slink_rob_in_DEBUG : Test_slink_rob_in_DEBUG.o
	$(CC) $< -o $@ $(MY_LIB)libSLINK.a $(LLFLAGS)
Test_slink_rob_in_DEBUG.o : Test_slink_rob_in.c
	$(CC) $(CFLAGS) -DDEBUG -DDEBUG_INPUT -DDEBUG_OUTPUT  -c $< -o $@

Test_slink_rob_in_SLIDAS : Test_slink_rob_in_SLIDAS.o
	$(CC) $< -o $@ $(MY_LIB)libSLINK.a $(LLFLAGS)
Test_slink_rob_in_SLIDAS.o : Test_slink_rob_in.c
	$(CC) $(CFLAGS) -DSLIDAS  -c $< -o $@
Test_slink_rob_in_SLIDAS_DEBUG : Test_slink_rob_in_SLIDAS_DEBUG.o
	$(CC) $< -o $@ $(MY_LIB)libSLINK.a $(LLFLAGS)
Test_slink_rob_in_SLIDAS_DEBUG.o : Test_slink_rob_in.c
	$(CC) $(CFLAGS) -DSLIDAS -DDEBUG -DDEBUG_INPUT  -c $< -o $@


rob_in_SLIDAS_EXTSTART : rob_in_SLIDAS_EXTSTART.o
	$(CC) $< -o $@ $(MY_LIB)libSLINK.a $(LLFLAGS)
rob_in_SLIDAS_EXTSTART.o : Test_slink_rob_in.c
	$(CC) $(CFLAGS) -DSLIDAS -DEXT_START  -c $< -o $@
rob_in_SLIDAS_EXTSTART_DEBUG : rob_in_SLIDAS_EXTSTART_DEBUG.o
	$(CC) $< -o $@ $(MY_LIB)libSLINK.a $(LLFLAGS)
rob_in_SLIDAS_EXTSTART_DEBUG.o : Test_slink_rob_in.c
	$(CC) $(CFLAGS) -DSLIDAS -DEXT_START -DDEBUG -DDEBUG_INPUT  -c $< -o $@

