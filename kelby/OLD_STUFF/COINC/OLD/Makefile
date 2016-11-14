# Makefile to create BeamROD & others
#

CC = gcc 
ALIB = $(TDAQ_INST_PATH)
ALIBS = -lvme_rcc -lcmem_rcc -lio_rcc -lrcc_error 
CFLAGS = -g -O0 -I$(ALIB)/include 
LDFLAGS = -L$(ALIB)/$(CMTCONFIG)/lib $(ALIBS)

all: bkgnd coinc creg coinc_nsw plat8 plat8_nt disp coinc18 threshold_set

bkgnd: bkgnd.o vme.h
	$(CC) -o bkgnd bkgnd.o $(LDFLAGS)

coinc: coinc.o vme.h
	$(CC) -o coinc coinc.o $(LDFLAGS)

creg: creg.o vme.h
	$(CC) -o creg creg.o $(LDFLAGS)
	
coinc_sw: coinc_nsw.o vme.h
	$(CC) -o coinc_nsw coinc_nsw.o $(LDFLAGS)

plat8: plat8.o vme.h
	$(CC) -o plat8 plat8.o -L/home/kelby/LIB -lkja $(LDFLAGS)

plat8_nt: plat8_nt.o vme.h
	$(CC) -o plat8_nt plat8_nt.o -L/home/kelby/LIB -lkja $(LDFLAGS)

disp: disp.o 
	$(CC) -o disp disp.o -L/home/kelby/LIB -lkja $(LDFLAGS)

coinc18: coinc18.o 
	$(CC) -o coinc18 coinc18.o -L/home/kelby/LIB -lkja $(LDFLAGS)

threshold_set: threshold_set.o 
	$(CC) -o threshold_set threshold_set.o -L/home/kelby/LIB -lkja $(LDFLAGS)

	







