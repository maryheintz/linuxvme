/************************************************************************
 *
 * Enhanced Engineering Software Package for GPIB-1014 Interface
 * System Support Functions Template for "GenericOS" (GOS)
 * Revision C.1
 * Copyright (c) 1992 National Instruments Corporation
 * All rights reserved.
 *
 *  +-------------------------------------------------------------+
 *  | NOTE: This file is included and compiled within esp488.c.   |
 *  |       It is NOT compiled separately.                        |
 *  +-------------------------------------------------------------+
 *
 * This file defines a generic operating system support layer for the
 * ESP-488 library.  The functions in this file must be customized for
 * the actual operating system and host machine that will be used.
 *
 * At the very least, all "GOS" variables and functions must be either
 * replaced by their specific OS counterparts or written by the user.
 * If the target environment does not support direct access from a
 * user application program to the functions in the ESP-488 library,
 * then it will also be necessary for the user to develop a suitable
 * interface layer through which all calls can pass.  For example,
 * on a UNIX-based system the following model could be used.
 *
 *                                          +---------------+
 *          Application program             |   main();     |
 *                                          +---- | --------+
 *          Application interface layer     |     V         |
 *                                          |   ibsic();    |
 *                                          +---- : --------+
 *     USER                                       :
 *    - - - - - - - - - - - - - - - - - - - - - - V - - - - - - -
 *     KERNEL                                   ioctl();
 *                                                :
 *                                          +---- : --------+
 *                                          |     V         |
 *          SYS488 interface layer          |   ibioctl();  |
 *                                          +---- | --------+
 *          ESP-488 library                 |     V         |
 *                                          |   ibsic();    |
 *                                          +---------------+
 *
 ************************************************************************/

/*
 * Include any GOS-specific and/or machine-specific #include files here.
 *
 * The following functions, as mentioned earlier and illustrated later
 * throughout the code, must be provided or replaced by the user of
 * this package:
 *
 *	GOSsemCreate()		- Create a semaphore
 *	GOSsemGive()		- Give or set a semaphore
 *	GOSsemTake()		- Wait on and/or take a semaphore
 *	GOSsemClear()		- Take and/or clear a semaphore
 *	GOStimerCreate()	- Create a timer function
 *	GOStimerStart()		- Start a timer function
 *	GOStimerCancel()	- Cancel a timer function
 *	GOSintrAck()		- Acknowledge an interrupt
 *	GOSgetSysClkRate()	- Get System Clock Rate (ticks/second)
 *	GOSinterruptPrint()	- Print from an interrupt routine
 */


#if SYSTIMO
/*
 * Define and/or declare any timer-related functions and variables here.
 */
IBLCL int	espTimerID = 0;		/* GOS timer ID for ESP */
					/* etc... */
#endif

#if USEINTS
/*
 * Define and/or declare any interrupt-related functions and variables here.
 */
IBLCL int	espSemID   = 0;		/* GOS semaphore ID for ESP */
					/* etc... */
#endif


#if SYSTIMO
/*
 * Timer functions
 */
IBLCL ibtmintr()			/* GOS-called timeout routine */
{
#if DEBUG
	if (dbgMask & DBG_INTR)
		GOSinterruptPrint("TIMER INTERRUPT!\n");
#endif
	noTimo = 0;
#if USEINTS
	if (!(pgmstat & PS_NOINTS)) {
		GPIBout(ch1.ccr, 0);	/* disable interrupts on board */
		GOSsemGive(espSemID);	/* wake up sleeping task */
	}
#endif
}
#endif


IBLCL startTimer(v)			/* Starts the timeout task  */
int v;					/* v = index into timeTable */
{
	DBGin(STARTTIMER);
	noTimo = INITTIMO;
#if USEINTS
	if (!(pgmstat & PS_NOINTS))
		GOSsemClear(espSemID);
#endif
	if (v > 0) {
		DBGprint(DBG_DATA, ("timo=%d  ", timeTable[v]));
#if SYSTIMO
		GOStimerStart(espTimerID, timeTable[v], ibtmintr);
#else
		noTimo = timeTable[v];
#endif
		pgmstat |= PS_TIMINST;
	}
	DBGout();
}


IBLCL removeTimer()			/* Removes the timeout task */
{
	DBGin(REMOVETIMER);
	if (pgmstat & PS_TIMINST) {
#if SYSTIMO
		GOStimerCancel(espTimerID);
#else
		if (noTimo > 0)
			noTimo = INITTIMO;
#endif
		pgmstat &= ~PS_TIMINST;
	}
	DBGout();
}


#if USEINTS
/*
 * GPIB interrupt service routine -- fast and simple
 */
IBLCL ibintr()
{
#if DEBUG
	if (dbgMask & DBG_INTR)
		GOSinterruptPrint("GPIB INTERRUPT!\n");
#endif
	GPIBout(ch1.ccr, 0);		/* disable interrupts on board */
	GOSsemGive(espSemID);		/* wake up sleeping task */
	GOSintrAck(ibirq);		/* acknowledge interrupt request */
}


/*
 * Wait for GPIB or timer interrupt.  Semaphore will be posted by
 * either ibintr or ibtmintr.
 */
IBLCL waitForInt(imr1mask, imr2mask)
int imr1mask;
int imr2mask;
{
	DBGin(WAITFORINT);
	if (GOSsemClear(espSemID) < 0) {
	/*
	 *	If semaphore not already available, enable
	 *	requested interrupts and wait until it is...
	 */
		DBGprint(DBG_DATA, ("imr1mask=0x%x imr2mask=0x%x  ", imr1mask, imr2mask));
		if (imr1mask)
			GPIBout(imr1, imr1mask);
		if (imr2mask)
			GPIBout(imr2, imr2mask);
		GPIBout(ch1.ccr, D_EINT);
					/* enable interrupts on board */
		GOSsemTake(espSemID);
	}
	DBGout();
}
#endif


/*
 * GenericOS initialization functions
 */
IBLCL osInit()
{
	int	s;

	DBGin(OSINIT);
#if SYSTIMO
	if (!espTimerID)
		espTimerID = GOStimerCreate();
	DBGprint(DBG_DATA, ("espTimerID=0x%x  ",  espTimerID));
	s = GOSgetSysClkRate();
	DBGprint(DBG_DATA, ("ClkRate=%d  ", s));
	if (s != timeTable[0]) {
		DBGprint(DBG_BRANCH, ("adjusting timeTable  ", s));
		timeTable[ 0] = s;			/* (New TMFAC)  */
		timeTable[ 1] = TM(s,10,1000000L);	/*  1: T10us    */
		timeTable[ 2] = TM(s,30,1000000L);	/*  2: T30us    */
		timeTable[ 3] = TM(s,100,1000000L);	/*  3: T100us   */
		timeTable[ 4] = TM(s,300,1000000L);	/*  4: T300us   */
		timeTable[ 5] = TM(s,1,1000);		/*  5: T1ms     */
		timeTable[ 6] = TM(s,3,1000);		/*  6: T3ms     */
		timeTable[ 7] = TM(s,10,1000);		/*  7: T10ms    */
		timeTable[ 8] = TM(s,30,1000);		/*  8: T30ms    */
		timeTable[ 9] = TM(s,100,1000);		/*  9: T100ms   */
		timeTable[10] = TM(s,300,1000);		/* 10: T300ms   */
		timeTable[11] = TM(s,1,1);		/* 11: T1s      */
		timeTable[12] = TM(s,3,1);		/* 12: T3s      */
		timeTable[13] = TM(s,10,1);		/* 13: T10s     */
		timeTable[14] = TM(s,30,1);		/* 14: T30s     */
		timeTable[15] = TM(s,100,1);		/* 15: T100s    */
		timeTable[16] = TM(s,300,1);		/* 16: T300s    */
		timeTable[17] = TM(s,1000,1);		/* 17: T1000s   */
	}
#endif
#if USEINTS
	if (!espSemID)
		espSemID = GOSsemCreate();
	DBGprint(DBG_DATA, ("espSemID=0x%x  ", espSemID));
/*
 *	Perform GOS-specific interrupt initializations:
 *	- Install interrupt service routine 'ibintr' for GPIB interrupts
 *	  on vector 'ibvec'.
 *	- Enable the GPIB interrupt request line 'ibirq'.
 *	- etc...
 */
#endif
	pgmstat |= PS_SYSRDY;
	DBGout();
}


IBLCL osReset()
{
	DBGin(OSRESET);
#if USEINTS
/*
 *	Perform GOS interrupt-related cleanup operations:
 *	- Disable GPIB interrupt request line.
 *	- etc...
 */
#endif
	pgmstat &= ~PS_SYSRDY;
	DBGout();
}


/*
 * Convert A16 VME bus address to local GOS virtual address.
 */
IBLCL uint32 ibreg_addr(vme_addr)
faddr_t vme_addr;
{
	faddr_t	local_addr;		/* local address of GPIB board */

/*
 *	Perform any necessary conversion from the specified VME address
 *	to a suitable local virtual address.  In the simplest case,
 *	'vme_addr' maps unchanged to 'local_addr'...
 */
	local_addr = (faddr_t)((unsigned int)vme_addr & 0xffff);
	return ((uint32) local_addr);
}


#if DMAOP
/*
 * Allocate any system DMA resources, as necessary, and
 * convert local virtual address to VME physical address
 * suitable for use by the GPIB-1014 DMA controller.
 */
IBLCL uint32 dma_alloc(buf, cnt, direc_bit)
faddr_t buf;			/* virtual buffer address */
unsigned int cnt;		/* size of buffer in bytes */
uint8 direc_bit;		/* OCR direction bit (MTD or DTM) */
{
	faddr_t	phys_addr;	/* physical address of I/O buffer */

#if A32
/*
 *	Compute an address for use with A32 extended supervisory
 *	data access (Address Modifier 0D).  In the simplest case,
 *	'buf' maps unchanged to 'phys_addr'...
 */
	phys_addr = buf;
#else
/*
 *	Compute an address for use with A24 standard supervisory
 *	data access (Address Modifier 3D).  In the simplest case,
 *	'buf' maps unchanged to 'phys_addr'...
 */
	phys_addr = buf;
#endif
	DBGprint(DBG_DATA, ("physAddr=0x%x  ", phys_addr));
	return ((uint32) phys_addr);
}


/*
 * Release any previously allocated system DMA resources,
 * clear the memory cache, etc.
 */
IBLCL dma_relse(buf, cnt, direc_bit)
faddr_t buf;			/* virtual buffer address */
unsigned int cnt;		/* size of buffer in bytes */
uint8 direc_bit;		/* OCR direction bit (MTD or DTM) */
{
/*
 *	if (direc_bit == D_DTM)
 *		Clear_memory_cache(buf, cnt);
 */
}


/*
 * Enable memory on the host CPU for access by the GPIB-1014
 * DMA controller.  Depending on the host system configuration,
 * this function may never be required.
 */
IBLCL enab_mem()
{
	/* NULL */
}
#endif				/* DMAOP */

