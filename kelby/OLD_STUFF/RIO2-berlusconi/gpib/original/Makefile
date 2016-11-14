#
# Enhanced Engineering Software Package for GPIB-1014 Interface
# Project MAKE File (for use on Unix Systems)
# Revision C.0
# Copyright (c) 1992 National Instruments Corporation
# All rights reserved.
#
# This file can be used to generate multiple ESP-488 modules, each
# with a unique set of 'ib' and 'dv' entry points, for use with multiple
# GPIB-1014 interface boards.  All module builds use the source files
# 'esp488_master.c' and 'sys488_master.c', as shown below.
#
# The macro SYS_INCL must be defined to point to the location(s) of any
# system include files referenced in sys488_master.c ...
#
SYS_INCL = -I/usr/include


esp488.o: ugpib.h esp488.h esp488_master.c sys488_master.c
	mv esp488.c esp488.cbak
	sed -e 's/ibX/ib/g' -e 's/dvX/dv/g' esp488_master.c > esp488.c
	mv sys488.c sys488.cbak
	sed -e 's/ibX/ib/g' -e 's/dvX/dv/g' sys488_master.c > sys488.c
	cc -c $(SYS_INCL) -O esp488.c -o esp488.o


esp488_1.o: ugpib.h esp488.h esp488_master.c sys488_master.c
	mv esp488.c esp488.cbak
	sed -e 's/ibX/ib1/g' -e 's/dvX/dv1/g' esp488_master.c > esp488.c
	mv sys488.c sys488.cbak
	sed -e 's/ibX/ib1/g' -e 's/dvX/dv1/g' sys488_master.c > sys488.c
	cc -c $(SYS_INCL) -O esp488.c -o esp488_1.o


esp488_2.o: ugpib.h esp488.h esp488_master.c sys488_master.c
	mv esp488.c esp488.cbak
	sed -e 's/ibX/ib2/g' -e 's/dvX/dv2/g' esp488_master.c > esp488.c
	mv sys488.c sys488.cbak
	sed -e 's/ibX/ib2/g' -e 's/dvX/dv2/g' sys488_master.c > sys488.c
	cc -c $(SYS_INCL) -O esp488.c -o esp488_2.o


esp488_3.o: ugpib.h esp488.h esp488_master.c sys488_master.c
	mv esp488.c esp488.cbak
	sed -e 's/ibX/ib3/g' -e 's/dvX/dv3/g' esp488_master.c > esp488.c
	mv sys488.c sys488.cbak
	sed -e 's/ibX/ib3/g' -e 's/dvX/dv3/g' sys488_master.c > sys488.c
	cc -c $(SYS_INCL) -O esp488.c -o esp488_3.o


esp488_4.o: ugpib.h esp488.h esp488_master.c sys488_master.c
	mv esp488.c esp488.cbak
	sed -e 's/ibX/ib4/g' -e 's/dvX/dv4/g' esp488_master.c > esp488.c
	mv sys488.c sys488.cbak
	sed -e 's/ibX/ib4/g' -e 's/dvX/dv4/g' sys488_master.c > sys488.c
	cc -c $(SYS_INCL) -O esp488.c -o esp488_4.o

