# Makefile to create BeamROD & others
#

CC = gcc
TDAQ_INST_PATH=/home/tiledaq/tdaq-01-01-00/installed
CMTCONFIG=i686-slc3-gcc323-opt
ALIB = $(TDAQ_INST_PATH)
ALIBS = -lvme_rcc -lcmem_rcc -lio_rcc -lrcc_error -lDFDebug
CFLAGS = -g -O0 -I$(ALIB)/include 
LDFLAGS = -L$(ALIB)/$(CMTCONFIG)/lib $(ALIBS)

all: lin swnew t13 pre find1 find2 kjapulse tog tpre lin_caen

lin: lin.o vme.h
	$(CC) -o /home/kelby/BIN/lin lin.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

swnew: swnew.o vme.h
	$(CC) -o /home/kelby/BIN/swnew swnew.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

t13: t13.o vme.h
	$(CC) -o /home/kelby/BIN/t13 t13.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

pre: pre.o vme.h
	$(CC) -o /home/kelby/BIN/pre pre.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

find1: find1.o vme.h
	$(CC) -o /home/kelby/BIN/find1 find1.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

find2: find2.o vme.h
	$(CC) -o /home/kelby/BIN/find2 find2.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

kjapulse: kjapulse.o vme.h
	$(CC) -o /home/kelby/BIN/kjapulse kjapulse.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

tog: tog.o vme.h
	$(CC) -o /home/kelby/BIN/tog tog.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

tpre: tpre.o vme.h
	$(CC) -o /home/kelby/BIN/tpre tpre.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

lin_caen: lin_caen.o vme.h
	$(CC) -o /home/kelby/BIN/lin_caen lin_caen.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)







        
	







