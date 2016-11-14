# Makefile to create BeamROD & others
#

CC = gcc 
ALIB = $(TDAQ_INST_PATH)
ALIBS = -lvme_rcc -lcmem_rcc -lio_rcc -lrcc_error 
CFLAGS = -g -O0 -I$(ALIB)/include 
LDFLAGS = -L$(ALIB)/$(CMTCONFIG)/lib $(ALIBS)

all: coinc plat8 threshold_set threshold2 arb arb_data fix

coinc: coinc.o vme.h
	$(CC) -o coinc coinc.o $(LDFLAGS)

plat8: plat8.o vme.h
	$(CC) -o plat8 plat8.o -L/home/kelby/LIB -lkja $(LDFLAGS)

threshold_set: threshold_set.o 
	$(CC) -o threshold_set threshold_set.o -L/home/kelby/LIB -lkja $(LDFLAGS)

threshold2: threshold2.o 
	$(CC) -o threshold2 threshold2.o -L/home/kelby/LIB -lkja $(LDFLAGS)

arb: arb.o 
	$(CC) -o arb arb.o -L/home/kelby/LIB -lkja $(LDFLAGS)

arb_data: arb_data.o 
	$(CC) -o arb_data arb_data.o -L/home/kelby/LIB -lkja $(LDFLAGS)

fix: fix.o 
	$(CC) -o fix fix.o $(LDFLAGS)

	







