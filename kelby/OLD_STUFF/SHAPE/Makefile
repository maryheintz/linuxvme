# Makefile to create BeamROD & others
#

CC = gcc 
ALIB = $(TDAQ_INST_PATH)
ALIBS = -lvme_rcc -lcmem_rcc -lio_rcc -lrcc_error 
CFLAGS = -g -O0 -I$(ALIB)/include 
LDFLAGS = -L$(ALIB)/$(CMTCONFIG)/lib $(ALIBS)

all: shape kjap shaperd cone

shape: shape.o vme.h
	$(CC) -o /home/kelby/BIN/shape shape.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

kjap: kjap.o vme.h
	$(CC) -o /home/kelby/BIN/kjap kjap.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

shaperd: shaperd.o vme.h
	$(CC) -o /home/kelby/BIN/shaperd shaperd.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)

cone: cone.o vme.h
	$(CC) -o /home/kelby/BIN/cone cone.o -L/home/kelby/LIB -lkja -l3in1_13 $(LDFLAGS)











        
	







