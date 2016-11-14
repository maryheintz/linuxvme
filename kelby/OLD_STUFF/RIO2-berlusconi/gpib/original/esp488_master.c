/************************************************************************
 *
 * Enhanced Engineering Software Package for GPIB-1014 Interface
 * Revision C.1
 * Copyright (c) 1993 National Instruments Corporation
 * All rights reserved.
 *
 * The GPIB functions implemented here are a straightforward
 * set of independent functions that can be used to directly
 * manipulate a GPIB interface.  They are at the level of
 * example code provided in the technical reference manual but
 * structured to be a subset of a complete, integrated NI-488
 * package.  Sufficient functionality is implemented so that
 * an application program can control and coordinate data
 * transfers among a suite of instruments on the GPIB.  The
 * primary purpose of these functions is to demonstrate the
 * use of the GPIB interface in an ATE environment so
 * that an engineer or programmer is able to customize the
 * GPIB functions for a dedicated stand-alone or ROM-based
 * system, rather than implement the software from scratch.
 * Whenever an operating system environment is available the
 * recommended approach is to use a specific binary driver or
 * handler package containing the full GPIB capability rather
 * than use this engineering software package.
 *
 * The function calls support synchronous, DMA I/O for
 * a single GPIB-1014 interface board.  All functions
 * return a subset of the standard GPIB status bit vector
 * consisting of just the status bits indicated in ugpib.h.
 * The result of the last call is also available in the global
 * variable, ibsta.  If the function call resulted in an error,
 * the ERR bit is set in ibsta and the error code is contained
 * in the global variable, iberr.  If the call was an I/O
 * operation the actual byte count transferred is available in
 * the global variable, ibcnt.
 *
 * NOTE:
 *      1.  This GPIB interface should always be the System
 *          Controller and, preferably, the Controller-In-Charge.
 *
 *      2.  The ibonl function must be called before any other
 *          call is made in order to initialize the GPIB
 *          interface.
 *
 *      3.  Prior to calling ibrd or ibwrt, the appropriate devices,
 *          including the local GPIB interface, must be addressed
 *          by calling ibcmd with the proper addressing commands.
 *
 *      4.  Secondary addressing is enabled by editing the SAD
 *          parameter (and recompiling) to the desired secondary
 *          address for the GPIB interface.  If enabled, the
 *          interface must be addressed by its primary address,
 *          immediately followed by its secondary address.
 *
 *      5.  Five device level calls are provided with this package.
 *          All of these calls need the primary and secondary address
 *          of the device you want to communicate with. If the device 
 *          does not have a secondary address, pass a zero for the
 *          SAD argument.
 *
 *      6.  The header file esp488.h contains a number of editable
 *          parameters.  These can be changed if the GPIB interface
 *          is to be configured differently than the default.
 *
 ************************************************************************/

#include "esp488.h"		/* local include file	*/

#if DEBUG			/* ------------------------------------ */

#define DBG_ALL      (1 << 0)
#define DBG_ENTRY   ((1 << 1) | DBG_ALL)
#define DBG_EXIT    ((1 << 2) | DBG_ALL)
#define DBG_BRANCH  ((1 << 3) | DBG_ALL)
#define DBG_DATA    ((1 << 4) | DBG_ALL)
#define DBG_INTR    ((1 << 5) | DBG_ALL)

#define IBRD		 0	/* system independent ESP routines...	*/
#define DOREAD		 1
#define IBWRT		 2
#define DOWRT		 3
#define IBCMD		 4
#define IBWAIT		 5
#define IBSTAT		 6
#define IBRPP		 7
#define IBONL		 8
#define IBSIC		 9
#define IBSRE		10
#define IBGTS		11
#define IBCAC		12
#define IBLINES		13
#define CHKNATN		14
#define IBPAD		15
#define IBSAD		16
#define IBTMO		17
#define IBEOT		18
#define IBEOS		19
#define IBRSV		20
#define DVTRG		21
#define DVCLR		22
#define DVRSP		23
#define DVRD		24
#define DVWRT		25

#define RECEIVE_SETUP	26
#define SEND_SETUP	27
#define FNINIT		28

#define STARTTIMER	29	/* system dependent support routines...		*/
#define REMOVETIMER	30
#define WAITFORINT	31
#define OSINIT		32
#define OSRESET		33

static int dbgMask  =   0;	/* class of debug statements allowed		*/
static int fnum     =   0;	/* current function ID number			*/
static int fstk[20] = { 0 };	/* function ID stack (NO OVERFLOW PROTECTION!)	*/
static int fidx     =   0;	/* index into function ID stack			*/

static char *fnames[] = {
	"ibrd",		    "doread",		 "ibwrt",		"dowrt",
	"ibcmd",	    "ibwait",		 "ibstat",		"ibrpp",
	"ibonl",	    "ibsic",		 "ibsre",		"ibgts",
	"ibcac",	    "iblines",		 "chkNATN",		"ibpad",
	"ibsad",	    "ibtmo",		 "ibeot",		"ibeos",
	"ibrsv",	    "dvtrg",		 "dvclr",		"dvrsp",
	"dvrd",		    "dvwrt",		 "receive_setup",	"send_setup",
	"fnInit",

	"startTimer",	    "removeTimer",	 "waitForInt",		"osInit",
	"osReset",
};

#define DBGprint(ms,ar)	{ if (dbgMask & ms) { printf("(%d)", fnum); printf ar; } }
#define DBGin(id)	{ fstk[fidx++] = fnum; fnum = id; DBGprint(DBG_ENTRY, ("%s  ", fnames[id])); }
#define DBGout()	{ DBGprint(DBG_EXIT, ("out  ")); fnum = fstk[--fidx]; }

#define IBLCL			/* IB "local" symbols -- leave global for debugging */

#else				/* ------------------------------------ */
#define DBGprint(ms,ar)	{ }
#define DBGin(i)	{ }
#define DBGout()	{ }

#define IBLCL		static	/* IB "local" symbols */

#endif				/* ------------------------------------ */

#define GPIBin(x)    (ibX->x)	/* Interface board access macros... */
#define GPIBout(x,v) (ibX->x = v)


int       ibXsta    = 0; 	/* status bits returned by last call      */
int       ibXcnt    = 0; 	/* actual byte count of last I/O transfer */
int       ibXerr    = 0; 	/* error code from last function call     */


uint16    ibXbase   = IBBASE;	/* base addr of GPIB interface registers  */
uint8     ibXvec    = IBVEC;	/* interrupt vector number for GPIB       */
uint8     ibXirq    = IBIRQ;	/* interrupt request line for GPIB (1-7)  */
uint8     ibXbrg    = IBBRG;	/* bus request/grant line for GPIB (1-3)  */


IBLCL int pgmstat	= 0;		/* program status vector */
IBLCL int noTimo	= INITTIMO;	/* 0 = I/O or wait operation timed out */

IBLCL int timeidx	= DFLTTIMO;	/* timeout index into timeTable */
IBLCL int pollTimeidx	= T100ms;	/* timeidx for serial and parallel polls */
IBLCL int myPAD		= PAD;		/* current primary address */
IBLCL int mySAD		= SAD;		/* current secondary address */
IBLCL int auxrabits	= AUXRA;	/* static bits for AUXRA (EOS modes) */
IBLCL int ifcDelay	= 200;		/* delay loop counter for IFC pulse */

IBLCL uint8 ibcfg1	= 0;		/* configuration register 1 set-up  */
IBLCL uint8 ibcfg2	= 0;		/* configuration register 2 set-up  */


IBLCL cc_block_t ibccb	= {{0, 0, 1, 0, 0, 2}, 0, 0};
					/* Carry-cycle block */

IBLCL struct ibregs *ibX = ((struct ibregs *) 0);
					/* Local pointer to IB registers */

IBLCL uint32 timeTable[] = {
/*
 *	Since timeTable[0] is never used, store the
 *	current timing factor at this location...
 */
           TMFAC,                      	/*  0: TNONE    */
        TM(TMFAC,10,1000000L),        	/*  1: T10us    */
        TM(TMFAC,30,1000000L),        	/*  2: T30us    */
        TM(TMFAC,100,1000000L),       	/*  3: T100us   */
        TM(TMFAC,300,1000000L),       	/*  4: T300us   */
        TM(TMFAC,1,1000),             	/*  5: T1ms     */
        TM(TMFAC,3,1000),             	/*  6: T3ms     */
        TM(TMFAC,10,1000),            	/*  7: T10ms    */
        TM(TMFAC,30,1000),            	/*  8: T30ms    */
        TM(TMFAC,100,1000),           	/*  9: T100ms   */
        TM(TMFAC,300,1000),           	/* 10: T300ms   */
        TM(TMFAC,1,1),                	/* 11: T1s      */
        TM(TMFAC,3,1),                	/* 12: T3s      */
        TM(TMFAC,10,1),               	/* 13: T10s     */
        TM(TMFAC,30,1),               	/* 14: T30s     */
        TM(TMFAC,100,1),              	/* 15: T100s    */
        TM(TMFAC,300,1),              	/* 16: T300s    */
        TM(TMFAC,1000,1)              	/* 17: T1000s   */
};


/*
 * IBPOKE
 * Undocumented, low-level driver control.  Includes turning
 * on and off different combinations of DEBUG statements.
 */
ibXpoke(mask)
int mask;
{
	int	pkcode;
	int	pkvalue;
	char  * mesgp;

	pkcode  = mask & PK_CODE;
	pkvalue = mask & PK_VALUE;

	switch (pkcode) {
#if DEBUG
	case PK_DEBUG:
		mesgp = "New debug mask for ESP488: ";
		dbgMask = pkvalue;
		break;
#endif
#if USEINTS
	case PK_USEINTS:
		mesgp = "New interrupt setting for ESP488: ";
		if (pkvalue)
			pgmstat &= ~PS_NOINTS;
		else
			pgmstat |= PS_NOINTS;

		break;
#endif
	case PK_IFCDELAY:
		mesgp = "New IFC delay factor for ESP488: ";
		ifcDelay = pkvalue;
		break;
	case PK_POLLTIMO:
		if ((pkvalue < TNONE) || (pkvalue > T1000s))
			mesgp = "INVALID POLLTIMO VALUE: ";
		else {
			mesgp = "New 'poll' timeout index for ESP488: ";
			pollTimeidx = pkvalue;
		}
		break;
#if DMAOP
	case PK_ENABMEM:		/* enable system memory for DMA xfers */
		enab_mem();
		mesgp = "Memory ENABLED\n";
		break;
#endif
	case PK_EOSMODE:
		mesgp = "New 'END with EOS' mode for ESP488: ";
		if (pkvalue)
			pgmstat &= ~PS_NOEOSEND;
		else
			pgmstat |= PS_NOEOSEND;

		break;
	case PK_INFO:
		mesgp = "New poke info setting for ESP488: ";
		if (pkvalue)
			pgmstat &= ~PS_SILENT;
		else
			pgmstat |= PS_SILENT;

		break;
	default:
		mesgp = "INVALID POKE CODE: ";
		pkvalue = pkcode;
		break;
	}
	if (!(pgmstat & PS_SILENT))
		printf("%s%d (0x%x)\n", mesgp, pkvalue, pkvalue);
}


/************************************************************************
 * Standard ESP functions
 ************************************************************************/

/*
 * IBRD
 * Read up to cnt bytes of data from the GPIB into buf.  End
 * on detection of EOI.
 *
 * NOTE:
 *      1.  The interface is placed in the controller standby
 *          state prior to beginning the read.
 *      2.  Prior to calling ibrd, the intended devices as well
 *          as the interface board itself must be addressed by
 *          calling ibcmd.
 *      3.  Be sure to type cast the buffer to (faddr_t) before
 *          calling this function.
 */
ibXrd(buf, cnt)
faddr_t buf;
unsigned int cnt;
{
	unsigned int	requested_cnt;

	DBGin(IBRD);
	if (fnInit(HR_LA) & ERR) {
		ibXcnt = 0;
		DBGout();
		return ibXsta;
	}
	startTimer(timeidx);
	DBGprint(DBG_BRANCH, ("go to standby  "));
	GPIBout(auxmr, AUX_GTS);	/* if CAC, go to standby */
	requested_cnt = cnt;
	while ((cnt > 0) && !(ibXsta & (ERR | TIMO | END))) {
		ibXcnt = 0;
#if USECC
		doread(buf, ((cnt > IBMAXIO) ? IBMAXIO : cnt), 0);
#else
		if (cnt > IBMAXIO)	/* read in a block... */
			doread(buf, IBMAXIO, 1);
		else if (cnt > 1)
			doread(buf, (cnt - 1), 1);
		else
			doread(buf, 1, 0);
#endif
		buf += ibXcnt;
		cnt -= ibXcnt;
	}
	ibXcnt = requested_cnt - cnt;
	removeTimer();

	if ((pgmstat & PS_NOEOSEND) && (ibXsta & END)) {
		if (!(GPIBin(adr1) & HR_EOI))
			ibXsta &= ~END;
	}
	ibstat();
	DBGout();
	return ibXsta;
}


#if DMAOP
/*
 *  DOREAD (DMA)
 *  This function performs a single DMA read operation.
 *  Note that the hand-shake is held off at the end of every read.
 */
IBLCL doread(buf, cnt, no_HLDA)
faddr_t buf;
unsigned int cnt;
int no_HLDA;
{
	uint8 s1, s2;				/* software copies of HW status regs... */
	uint8 ch0csr, ch1csr;
	uint8 ccfunc;				/* Carry-cycle function */

	DBGin(DOREAD);
	DBGprint(DBG_DATA, ("buf=0x%x cnt=%d  ", buf, cnt));

	GPIBout(ch1.csr, D_CLEAR);		/* clear PCLT bit (master int status) */
	s2 = GPIBin(isr2);			/* clear the status registers... */
	s1 = GPIBin(isr1) | GPIBin(isr1);	/* read isr1 twice in case of END delay */
	DBGprint(DBG_DATA, ("isr1=0x%x isr2=0x%x  ", s1, s2));

	if (pgmstat & PS_HELD) {
		DBGprint(DBG_BRANCH, ("finish handshake  "));
		GPIBout(auxmr, auxrabits | HR_HLDA);
		GPIBout(auxmr, AUX_FH);		/* set HLDA in AUXRA to ensure FH works */
		pgmstat &= ~PS_HELD;
	}
	else if ((s1 & HR_DI) && (s1 & HR_END)) {
		DBGprint(DBG_BRANCH, ("one-byte read with END  "));
		GPIBout(auxmr, auxrabits | HR_HLDA);
		pgmstat |= PS_HELD;
		buf[0] = GPIBin(dir);
		ibXsta |= END;
		ibXcnt = 1;
		DBGout();
		return;
	}
	DBGprint(DBG_BRANCH, ("set-up EOS modes  "));
/*
 *	Set EOS modes, holdoff on END, and holdoff on all carry cycle...
 */
#if (!AUTOCC)
	if (no_HLDA) {
		DBGprint(DBG_BRANCH, ("holdoff on END, no carry-cycle  "));
		GPIBout(auxmr, auxrabits | HR_HLDE);
		ccfunc = 0;
	}
	else
#endif
	if (cnt == 1) {
		DBGprint(DBG_BRANCH, ("manual hold, no carry-cycle  "));
		GPIBout(auxmr, auxrabits | HR_HLDA);
		ccfunc = 0;
	}
	else {
		GPIBout(auxmr, auxrabits | HR_HLDE);
		ccfunc = auxrabits | HR_HLDA;
	}
	GPIBout(imr1, HR_ENDIE);		/* set imr1 before imr2 */
	DBGprint(DBG_BRANCH, ("start DMA  "));
	dma_start(buf, cnt, D_DTM, ccfunc);

	DBGprint(DBG_BRANCH, ("begin waiting for DONE  "));
	while (!(GPIBin(ch1.csr) & D_PCLT)) {
		DBGprint(DBG_DATA, ("ch0.csr=0x%x ch1.csr=0x%x  ", GPIBin(ch0.csr), GPIBin(ch1.csr)));
		if (TimedOut())
			goto rddone;
		WaitingFor(0, 0);		/* HR_END already set above */
	}
rddone:
	DBGprint(DBG_BRANCH, ("done  "));
	ch0csr = GPIBin(ch0.csr);
	ch1csr = GPIBin(ch1.csr);
	DBGprint(DBG_DATA, ("ch0csr=0x%x ch1csr=0x%x  ", ch0csr, ch1csr));
	ibXcnt = cnt - dma_stop(buf, cnt, D_DTM, ccfunc);
	if ((s1 = GPIBin(isr1)) & HR_END) {
#if (!AUTOCC)
		if (s1 & HR_DI) {
			DBGprint(DBG_BRANCH, ("extra byte with END  "));
			buf[ibXcnt++] = GPIBin(dir);
		}
#endif
		ibXsta |= END;
	}
	if ((ibXsta & END) || (!no_HLDA && (ibXcnt == cnt)))
		pgmstat |= PS_HELD;
	if ((ch0csr & D_ERR) || (ch1csr & D_ERR)) {
		DBGprint(DBG_BRANCH, ("DMA error  "));
		DBGprint(DBG_DATA, ("ch0.cer=0x%x ch1.cer=0x%x  ", GPIBin(ch0.cer), GPIBin(ch1.cer)));
		ibXsta |= ERR;
		ibXerr = EDMA;
	}
	else if (!noTimo) {
		DBGprint(DBG_BRANCH, ("timeout  "));
		ibXsta |= (ERR | TIMO);
		ibXerr = EABO;
	}
	DBGout();
}
#else						/* DMAOP */
/*
 *  DOREAD (PIO)
 *  This function performs a single Programmed I/O read operation.
 *  Note that the hand-shake is held off at the end of every read.
 */
IBLCL doread(buf, cnt, no_HLDA)
faddr_t buf;
unsigned int cnt;
int no_HLDA;					/* unused for PIO */
{
	uint8 s1, s2;				/* software copies of HW status regs */
	uint8 ccfunc;				/* Carry-cycle function */

	DBGin(DOREAD);
	DBGprint(DBG_DATA, ("buf=0x%x cnt=%d  ", buf, cnt));

	GPIBout(ch1.csr, D_CLEAR);		/* clear PCLT bit (master int status) */
	s2 = GPIBin(isr2);			/* clear the status registers... */
	s1 = GPIBin(isr1) | GPIBin(isr1);	/* read isr1 twice in case of END delay */
	DBGprint(DBG_DATA, ("isr1=0x%x isr2=0x%x  ", s1, s2));

	if (pgmstat & PS_HELD) {
		DBGprint(DBG_BRANCH, ("finish handshake  "));
		GPIBout(auxmr, auxrabits | HR_HLDA);
		GPIBout(auxmr, AUX_FH);		/* set HLDA in AUXRA to ensure FH works */
		pgmstat &= ~PS_HELD;
	}
	else if ((s1 & HR_DI) && (s1 & HR_END)) {
		DBGprint(DBG_BRANCH, ("one-byte read with END  "));
		GPIBout(auxmr, auxrabits | HR_HLDA);
		pgmstat |= PS_HELD;
		buf[0] = GPIBin(dir);
		ibXsta |= END;
		ibXcnt = 1;
		DBGout();
		return;
	}
	DBGprint(DBG_BRANCH, ("set-up EOS modes  "));
/*
 *	Set EOS modes, holdoff on END, and holdoff on all carry cycle...
 */
	if (cnt == 1) {
		DBGprint(DBG_BRANCH, ("no carry-cycle  "));
		GPIBout(auxmr, auxrabits | HR_HLDA);
		ccfunc = 0;
	}
	else {
		GPIBout(auxmr, auxrabits | HR_HLDE);
		ccfunc = auxrabits | HR_HLDA;
	}
	DBGprint(DBG_BRANCH, ("begin PIO loop  "));
	cnt--;					/* decrement temporarily */
	while (ibXcnt <= cnt) {
		while (!((s1 |= GPIBin(isr1)) & HR_DI)) {
			if ((s1 & HR_END) || TimedOut())
				goto rddone;
			WaitingFor((HR_DIIE | HR_ENDIE), 0);
		}
		if ((ibXcnt == cnt) && ccfunc)
			GPIBout(auxmr, ccfunc);
		buf[ibXcnt++] = GPIBin(dir);
		s1 &= ~HR_DI;
	}
rddone:
	cnt++;					/* reset to original value */
	DBGprint(DBG_BRANCH, ("done  "));
	GPIBout(imr1, 0);			/* clear DIIE and ENDIE if set */

	if (s1 & HR_END)
		ibXsta |= END;
	if ((ibXsta & END) || (ibXcnt == cnt))
		pgmstat |= PS_HELD;
	if (!noTimo) {
		DBGprint(DBG_BRANCH, ("timeout  "));
		ibXsta |= (ERR | TIMO);
		ibXerr = EABO;
	}
	DBGout();
}
#endif						/* DMAOP */


/*
 * IBWRT
 * Write cnt bytes of data from buf to the GPIB.  The write
 * operation terminates only on I/O complete.  By default,
 * EOI is always sent along with the last byte.
 *
 * NOTE:
 *      1.  Prior to beginning the write, the interface is
 *          placed in the controller standby state.
 *      2.  Prior to calling ibwrt, the intended devices as
 *          well as the interface board itself must be
 *          addressed by calling ibcmd.
 *      3.  Be sure to type cast the buffer to (faddr_t) before
 *          calling this function.
 */
ibXwrt(buf, cnt)
faddr_t buf;
unsigned int cnt;
{
	unsigned int	requested_cnt;

	DBGin(IBWRT);
	if (fnInit(HR_TA) & ERR) {
		ibXcnt = 0;
		DBGout();
		return ibXsta;
	}
	startTimer(timeidx);
	DBGprint(DBG_BRANCH, ("go to standby  "));
	GPIBout(auxmr, AUX_GTS);	/* if CAC, go to standby */
	requested_cnt = cnt;
	while ((cnt > 0) && !(ibXsta & (ERR | TIMO))) {
		ibXcnt = 0;
		if (cnt > IBMAXIO)	/* write out a block... */
			dowrt(buf, IBMAXIO, PS_NOEOI);
#if USECC
		else	dowrt(buf, cnt, (pgmstat & PS_NOEOI));
#else
		else if (cnt > 1)
			dowrt(buf, (cnt - 1), PS_NOEOI);
		else
			dowrt(buf, 1, (pgmstat & PS_NOEOI));
#endif
		buf += ibXcnt;
		cnt -= ibXcnt;
	}
	ibXcnt = requested_cnt - cnt;
	removeTimer();

	ibstat();
	DBGout();
	return ibXsta;
}


#if DMAOP
/*
 *  DOWRT (DMA)
 *  This function performs a single DMA write operation.
 */
IBLCL dowrt(buf, cnt, no_eoi)
faddr_t buf;
unsigned int cnt;
unsigned int no_eoi;
{ 
	uint8 s1, s2;				/* software copies of HW status regs... */
	uint8 ch0csr, ch1csr;
	uint8 ccfunc;				/* Carry-cycle function */

	DBGin(DOWRT);
	DBGprint(DBG_DATA, ("buf=0x%x cnt=%d  ", buf, cnt));

	GPIBout(ch1.csr, D_CLEAR);		/* clear PCLT bit (master int status) */
	s2 = GPIBin(isr2);			/* clear the status registers... */
	s1 = GPIBin(isr1);
	DBGprint(DBG_DATA, ("isr1=0x%x isr2=0x%x  ", s1, s2));

	DBGprint(DBG_BRANCH, ("set-up EOT and EOS  "));
/*
 *	Set EOT and EOS modes...
 */
	if (no_eoi)
		ccfunc = 0;			/* no default EOI w/last byte */
	else if (cnt == 1) {
		ccfunc = 0;			/* can't use carry-cycle on 1-byte write... */
		if ((GPIBin(adsr) & (HR_NATN | HR_SPMS | HR_TA)) != (HR_NATN | HR_TA)) {
			DBGprint(DBG_BRANCH, ("wait for TACS  "));
		/*
		 *	The 7210 must be in TACS (as opposed to TADS or SPAS) for the
		 *	SEOI command to work -- in other words, TA and NATN must both
		 *	be set.  If they are not, wait until they are...
		 *
		 *	NOTE: "Data out" (DO) will be set when we are in TACS.
		 */
			while (!(GPIBin(isr1) & HR_DO)) {
				if (TimedOut()) {
					GPIBout(imr1, 0);
						/* clear DOIE if set */
					goto wrttimo;
				}
				WaitingFor(HR_DOIE, 0);
			}
#if USEINTS
			GPIBout(ch1.csr, D_CLEAR);
#endif						/* clear PCLT bit set because of DO */
		}
		GPIBout(auxmr, AUX_SEOI);	/* write out "send EOI" command now */
	}
	else	ccfunc = AUX_SEOI;		/* cnt > 1: send EOI on carry cycle */

	GPIBout(auxmr, auxrabits);		/* send EOI w/EOS if requested */
	GPIBout(imr1, HR_ERR);			/* set imr1 before imr2 */
	DBGprint(DBG_BRANCH, ("start DMA  "));
	dma_start(buf, cnt, D_MTD, ccfunc);

	DBGprint(DBG_BRANCH, ("begin waiting for DONE  "));
	while (!(GPIBin(ch1.csr) & D_PCLT)) {
		DBGprint(DBG_DATA, ("ch0.csr=0x%x ch1.csr=0x%x  ", GPIBin(ch0.csr), GPIBin(ch1.csr)));
		if (TimedOut())
			goto wrtdone;
		WaitingFor(0, 0);		/* HR_ERR already set above */
	}
wrtdone:
	DBGprint(DBG_BRANCH, ("done  "));
	ch0csr = GPIBin(ch0.csr);
	ch1csr = GPIBin(ch1.csr);
	DBGprint(DBG_DATA, ("ch0csr=0x%x ch1csr=0x%x  ", ch0csr, ch1csr));
	ibXcnt = cnt - dma_stop(buf, cnt, D_MTD, ccfunc);
	if (GPIBin(isr1) & HR_ERR) {
		DBGprint(DBG_BRANCH, ("no listeners  "));
		ibXsta |= ERR;
		ibXerr = ENOL;
	}
	else if ((ch0csr & D_ERR) || (ch1csr & D_ERR)) {
		DBGprint(DBG_BRANCH, ("DMA error  "));
		DBGprint(DBG_DATA, ("ch0.cer=0x%x ch1.cer=0x%x  ", GPIBin(ch0.cer), GPIBin(ch1.cer)));
		ibXsta |= ERR;
		ibXerr = EDMA;
	}
	else if (!noTimo) {
wrttimo:
		DBGprint(DBG_BRANCH, ("timeout  "));
		ibXsta |= (ERR | TIMO);
		ibXerr = EABO;
	}
	DBGout();
}
#else						/* DMAOP */
/*
 *  DOWRT (PIO)
 *  This function performs a single Programmed I/O write operation.
 */
IBLCL dowrt(buf, cnt, no_eoi)
faddr_t buf;
unsigned int cnt;
unsigned int no_eoi;
{ 
	uint8 s1, s2;				/* software copies of HW status regs */
	uint8 ccfunc;				/* Carry-cycle function */

	DBGin(DOWRT);
	DBGprint(DBG_DATA, ("buf=0x%x cnt=%d  ", buf, cnt));

	GPIBout(ch1.csr, D_CLEAR);		/* clear PCLT bit (master int status) */
	s2 = GPIBin(isr2);			/* clear the status registers... */
	s1 = GPIBin(isr1);
	DBGprint(DBG_DATA, ("isr1=0x%x isr2=0x%x  ", s1, s2));

	DBGprint(DBG_BRANCH, ("set-up EOT and EOS  "));
/*
 *	Set EOT and EOS modes...
 */
	if (no_eoi)
		ccfunc = 0;			/* no default EOI w/last byte */
	else
		ccfunc = AUX_SEOI;		/* send EOI on carry cycle */

	GPIBout(auxmr, auxrabits);		/* send EOI w/EOS if requested */

	if ((GPIBin(adsr) & (HR_NATN | HR_SPMS | HR_TA)) == (HR_NATN | HR_TA))
		s1 = HR_DO;			/* first DO assumed */
	else {
		DBGprint(DBG_BRANCH, ("wait for TACS  "));
	/*
	 *	The 7210 must be in TACS (as opposed to TADS or SPAS) for the
	 *	SEOI command to work -- in other words, TA and NATN must both
	 *	be set.  If they are not, wait until they are...
	 *
	 *	NOTE: "Data out" (DO) will be set when we are in TACS.
	 */
		while (!((s1 = GPIBin(isr1)) & HR_DO)) {
			if (TimedOut()) {
				GPIBout(imr1, 0);
				goto wrttimo;	/* ...clear DOIE if set */
			}
			WaitingFor(HR_DOIE, 0);
		}
#if USEINTS
		GPIBout(ch1.csr, D_CLEAR);	/* clear PCLT bit set because of DO */
#endif
	}
	DBGprint(DBG_BRANCH, ("begin PIO loop  "));
	cnt--;					/* decrement temporarily */
	while ((ibXcnt <= cnt) && !(s1 & HR_ERR)) {
		if ((ibXcnt == cnt) && ccfunc)
			GPIBout(auxmr, ccfunc);
		GPIBout(cdor, buf[ibXcnt++]);
		s1 &= ~HR_DO;
		while (!((s1 |= GPIBin(isr1)) & HR_DO)) {
			if ((s1 & HR_ERR) || TimedOut())
				goto wrtdone;
			WaitingFor((HR_DOIE | HR_ERRIE), 0);
		}
	}
wrtdone:
	cnt++;					/* reset to original value */
	DBGprint(DBG_BRANCH, ("done  "));
	GPIBout(imr1, 0);			/* clear DOIE and ERRIE if set */

	if (s1 & HR_ERR) {
		DBGprint(DBG_BRANCH, ("no listeners  "));
		ibXsta |= ERR;
		ibXerr = ENOL;
	}
	else if (!noTimo) {
wrttimo:
		DBGprint(DBG_BRANCH, ("timeout  "));
		ibXsta |= (ERR | TIMO);
		ibXerr = EABO;
	}
	DBGout();
}
#endif						/* DMAOP */


#if DMAOP
/*
 *  DMA_START
 *  This function configures and starts the 68450 DMA controller.
 */
IBLCL dma_start(buf, cnt, direc_bit, ccfunc)
faddr_t buf;
unsigned int cnt;
uint8 direc_bit;			/* OCR direction bit (MTD or DTM) */
uint8 ccfunc;				/* Carry-cycle function */
{
	uint32		paddr;
	uint32		tmpaddr;
	uint8		ch1ccr_init;
	extern uint32	dma_alloc();

	paddr = dma_alloc(buf, cnt, direc_bit);
#if (A32)
	GPIBout(pgr, ((uint8)(paddr >> 24)));
#endif
	if (direc_bit == D_DTM)		/* GPIB read */
		ibcfg1 |= D_IN;
	else				/* GPIB write */
		ibcfg1 |= D_OUT;

	GPIBout(ch0.csr, D_CLEAR);
#if AUTOCC
	if (ccfunc) {			/* set up channel 1 (carry cycle)... */
		cnt--;
		ibccb.ccbyte = ccfunc;
		tmpaddr = dma_alloc(&ibccb.ccbyte, (sizeof ibccb.ccbyte), D_MTD);
		ibccb.cca.cc_ccb_hi = (tmpaddr >> 16);
		ibccb.cca.cc_ccb_lo = (tmpaddr & 0xFFFF);
		tmpaddr = paddr + cnt;
		ibccb.cca.cc_n_1addr_hi = (tmpaddr >> 16);
		ibccb.cca.cc_n_1addr_lo = (tmpaddr & 0xFFFF);
		GPIBout(ch1.ocr, (direc_bit | D_ACH | D_XRQ));
		tmpaddr = dma_alloc(&ibccb.cca, (sizeof ibccb.cca), D_MTD);
		GPIBout(ch1.barhi, (tmpaddr >> 16));
		GPIBout(ch1.barlo, (tmpaddr & 0xFFFF));
		GPIBout(ch1.btc, 2);
		ibcfg1 |= D_ECC;
		ch1ccr_init = D_SRT;
	}
	else {
		ibcfg1 &= ~D_ECC;
		ch1ccr_init = 0;
	}
#else
	ibcfg1 &= ~D_ECC;
	ch1ccr_init = 0;
#endif
	GPIBout(cfg1, ibcfg1);
					/* set up channel 0 (read/write)... */
	GPIBout(ch0.ocr, (direc_bit | D_XRQ));
	GPIBout(ch0.marhi, (paddr >> 16));
	GPIBout(ch0.marlo, (paddr & 0xFFFF));
	GPIBout(ch0.mtc, cnt);
					/* start DMA (channel 1 always first)... */
	GPIBout(ch1.ccr, ch1ccr_init);
	GPIBout(ch0.ccr, D_SRT);
					/* enable DMA, all other ints. disabled... */
	GPIBout(imr2, ((direc_bit == D_DTM) ? HR_DMAI : HR_DMAO));
}


/*
 *  DMA_STOP
 *  This function halts the 68450 DMA controller.
 */
IBLCL dma_stop(buf, cnt, direc_bit, ccfunc)
faddr_t buf;
unsigned int cnt;
uint8 direc_bit;			/* OCR direction bit (MTD or DTM) */
uint8 ccfunc;				/* Carry-cycle function flag */
{
	int     resid;			/* residual transfer count */
	int     oldmtc, newmtc;		/* current and last MTC values */
	int     num;			/* number of times MTC has been the same */
	int     i;			/* delay counter */
/*
 *	NOTE:  Sending a software abort before a transfer is complete
 *	can cause the VMEbus to lock up.  If the DMAC is stopped when
 *	a bus request is pending, the DMAC will not acknowledge the
 *	subsequent bus grant.  The bus will lock waiting for the DMAC
 *	to accept the bus.
 *
 *	The following delay code reduces the risk of locking up the bus
 *	by stopping the NEC 7210 from requesting another DMA cycle and
 *	waiting for a while so that any current cycle can end.
 */
	GPIBout(imr1, 0);		/* Stop the DMA transfer... */
	GPIBout(imr2, 0);

	num = 0;
	oldmtc = GPIBin(ch0.mtc);	/* get initial MTC value */
	do {
		for (i = 0; i < 0xFF; i++)
			;		/* wait a while between MTC reads */
		newmtc = GPIBin(ch0.mtc);
		if (newmtc == oldmtc)	/* if the MTC value hasn't changed, */
			num++;		/*  then increment num              */
		else {
			num = 0;	/* otherwise, start num over again */
			oldmtc = newmtc;
		}

	} while (num < 3);		/* quit when MTC hasn't */
					/*  changed in 3 reads  */
	GPIBout(ch0.ccr, D_SAB);
	GPIBout(ch1.ccr, D_SAB);
	ibcfg1 &= ~(D_IN | D_ECC);
	GPIBout(cfg1, ibcfg1);

	resid = GPIBin(ch0.mtc) & 0xFFFF;
#if AUTOCC
	if (ccfunc) {
		if (resid || (GPIBin(ch1.mtc) == 2))
			resid++;	/* if carry-cycle incomplete, adjust resid */

		dma_relse(&ibccb.cca, (sizeof ibccb.cca), D_MTD);
		dma_relse(&ibccb.ccbyte, (sizeof ibccb.ccbyte), D_MTD);
	}
#endif
	dma_relse(buf, cnt, direc_bit);
        return resid;
}
#endif					/* DMAOP */


/*
 * IBCMD
 * Write cnt command bytes from buf to the GPIB.  The
 * command operation terminates only on I/O complete.
 *
 * NOTE:
 *      1.  Prior to beginning the command, the interface is
 *          placed in the controller active state.
 *      2.  Before calling ibcmd for the first time, ibsic
 *          must be called to initialize the GPIB and enable
 *          the interface to leave the controller idle state.
 *      3.  Be sure to type cast the buffer to (faddr_t) before
 *          calling this function.
 */
ibXcmd(buf, cnt)
faddr_t buf;
unsigned int cnt;
{
	uint8 s1, s2;

	DBGin(IBCMD);
	ibXcnt = 0;
	if (fnInit(HR_CIC) & ERR) {
		DBGout();
		return ibXsta;
	}
	if (cnt == 0) {
		DBGprint(DBG_BRANCH, ("cnt=0  "));
		ibstat();
		DBGout();
		return ibXsta;
	}
	DBGprint(DBG_DATA, ("buf=0x%x cnt=%d  ", buf, cnt));
	
	startTimer(timeidx);
	if (GPIBin(adsr) & HR_NATN) {	/* if standby, go to CAC */
		DBGprint(DBG_BRANCH, ("take control  "));
		GPIBout(auxmr, AUX_TCA);
	}
	GPIBout(ch1.csr, D_CLEAR);	/* clear PCLT bit (master int status) */
	s2 = GPIBin(isr2);		/* clear the status registers... */
	s1 = GPIBin(isr1);
	DBGprint(DBG_DATA, ("isr1=0x%x isr2=0x%x  ", s1, s2));

	DBGprint(DBG_BRANCH, ("begin PIO loop  "));
	while (ibXcnt < cnt) {		/* NOTE: first CO assumed */
		GPIBout(cdor, buf[ibXcnt++]);
		while (!((s2 = GPIBin(isr2)) & HR_CO)) {
			if (((s2 & HR_ADSC) && !(GPIBin(adsr) & HR_CIC)) || TimedOut())
				goto cmddone;
			WaitingFor(0, (HR_COIE | HR_ACIE));
					/* wait for ADSC (!CIC) in case TCT sent out */
		}
	}
cmddone:
	DBGprint(DBG_BRANCH, ("done  "));
	GPIBout(imr2, 0);		/* clear COIE and ACIE if set */
	if (!noTimo) {
		DBGprint(DBG_BRANCH, ("timeout  "));
		ibXsta |= (ERR | TIMO);
		ibXerr = EABO;
	}
	removeTimer();

	ibstat();
	DBGout();
	return ibXsta;
}


/*
 * IBWAIT
 * Check or wait for a GPIB event to occur.  The mask argument
 * is a bit vector corresponding to the status bit vector.  It
 * has a bit set for each condition which can terminate the wait
 * (see esp488.h for list of valid bits).  If the mask is 0 then
 * no condition is waited for and the current status is simply
 * returned.
 */
ibXwait(mask)
unsigned int mask;
{
	uint8 s, imr2mask;

	DBGin(IBWAIT);
	if (fnInit((mask & SRQI) ? HR_CIC : 0) & ERR) {
		DBGout();
		return ibXsta;
	}
	if (mask == 0) {
		DBGprint(DBG_BRANCH, ("mask=0  "));
		ibstat();
		DBGout();
		return ibXsta;
	}
	else if (mask & ~WAITBITS) {
		DBGprint(DBG_BRANCH, ("bad mask  "));
		ibXsta |= ERR;
		ibXerr = EARG;
		ibstat();
		DBGout();
		return ibXsta;
	}
	startTimer(timeidx);
	mask |= TIMO;
#if USEINTS
	if (!(pgmstat & PS_NOINTS)) {
		DBGprint(DBG_BRANCH, ("use ints  "));
		s = GPIBin(isr2);           /* clear isr2 by reading */
		GPIBout(ch1.csr, D_CLEAR);  /* clear PCLT bit (master int status) */
		imr2mask = 0;
		if (mask & SRQI)
			imr2mask |= HR_SRQIE;
		if (mask & (CIC | TACS | LACS))
			imr2mask |= HR_ACIE;
		DBGprint(DBG_DATA, ("imr2mask=0x%x  ", imr2mask));
		while (!(ibstat() & mask) && NotTimedOut()) {
			ibXsta = CMPL;
			waitForInt(0, imr2mask);
			GPIBout(imr2, 0);   /* clear imr2 IE bits */
			s = GPIBin(isr2);   /* clear isr2 status bits */
			DBGprint(DBG_DATA, ("isr2=0x%x  ", s));
		}
	}
	else
#endif
	while (!(ibstat() & mask) && NotTimedOut())
		ibXsta = CMPL;

	if (!noTimo)
		ibXsta |= TIMO;
	removeTimer();
	DBGout();
	return ibXsta;
}


IBLCL ibstat()			/* update the GPIB status information */
{
	register int brdstat;

	DBGin(IBSTAT);
	ibXsta |= (GPIBin(ch0.csr) & D_PCLH) ? 0 : SRQI;
	brdstat = GPIBin(adsr);
	ibXsta |= (brdstat & HR_CIC)  ? CIC  : 0;
	ibXsta |= (brdstat & HR_NATN) ? 0    : ATN;
	ibXsta |= (brdstat & HR_TA)   ? TACS : 0;
	ibXsta |= (brdstat & HR_LA)   ? LACS : 0;
	DBGprint(DBG_DATA, ("ibsta=0x%x  ", ibXsta));
	DBGout();
	return ibXsta;
}


/*
 * IBRPP
 * Conduct a parallel poll and return the byte in buf.
 *
 * NOTE:
 *      1.  Prior to conducting the poll the interface is placed
 *          in the controller active state.
 */
ibXrpp(buf)
faddr_t buf;
{
	DBGin(IBRPP);
	if (fnInit(HR_CIC) & ERR) {
		DBGout();
		return ibXsta;
	}
	startTimer(pollTimeidx);
	if (GPIBin(adsr) & HR_NATN) {       /* if standby, go to CAC */
		DBGprint(DBG_BRANCH, ("take control  "));
		GPIBout(auxmr, AUX_TCA);
		while (!(GPIBin(isr2) & HR_CO) && NotTimedOut())
			;                   /* make sure everyone is ready */
	}
	GPIBout(auxmr, AUX_EPP);            /* execute parallel poll */
	DBGprint(DBG_BRANCH, ("wait for CO  "));
	while (!(GPIBin(isr2) & HR_CO) && NotTimedOut())
		;                           /* wait for poll to complete */
	if (!noTimo) {
		ibXsta |= ERR;              /* something went wrong */
		ibXerr = EBUS;
	}
	*buf = GPIBin(cptr);                /* store the response byte */
	DBGprint(DBG_DATA, ("pp=0x%x  ", *buf));
	removeTimer();
	ibstat();
	DBGout();
	return ibXsta;
}


/*
 * IBONL
 * Initialize the interface hardware.  If v is non-zero then
 * the GPIB chip is enabled online.  If v is zero then it is
 * left disabled and offline.
 *
 * NOTE:
 *      1.  Ibonl must be called before any other function.
 */
ibXonl(v)
int v;
{
	uint8		s;
	int		i;
	extern uint32	ibreg_addr();

	DBGin(IBONL);
	ibX = (struct ibregs *) ibreg_addr(ibXbase);
/*
 *	NOTE ON CFG2: Always set sysfail bit to 1 (LED green) when bringing
 *	online (see below). LMR must be cleared before any other bits can be set.
 */
	GPIBout(cfg2, D_LMR | D_SFL);               /* local master reset (LMR) */
	for (i = 0; i < 10000; i++);                /* delay at least 10 msecs. */
	ibcfg2 = 0;
	GPIBout(cfg2, ibcfg2);                      /* clear LMR */
	ibcfg1 = ((ibXirq << 5) | (ibXbrg << 3) | D_DBM);
	GPIBout(cfg1, ibcfg1);                      /* use selected IRQ and BRG lines */
                                                    /* and disarm bus monitor (ROR)   */

	s = GPIBin(cptr);                           /* clear registers by reading */
	s = GPIBin(isr1);
	s = GPIBin(isr2);

	pgmstat &= PS_STATIC;                       /* initialize program status vector */
	noTimo = INITTIMO;                          /* initialize timeout flag */
	timeidx = DFLTTIMO;                         /* initialize configuration variables... */
	myPAD = PAD;
	mySAD = SAD;
	auxrabits = AUXRA;

	GPIBout(imr1, 0);                           /* disable all interrupts */
	GPIBout(imr2, 0);
	GPIBout(spmr, 0);
	GPIBout(adr,(PAD & LOMASK));                /* set GPIB address; MTA=PAD|100, MLA=PAD|040 */
#if (SAD)
	GPIBout(adr, HR_ARS | (SAD & LOMASK));      /* enable secondary addressing */
	GPIBout(admr, HR_TRM1 | HR_TRM0 | HR_ADM1);
#else
	GPIBout(adr, HR_ARS | HR_DT | HR_DL);       /* disable secondary addressing */
	GPIBout(admr, HR_TRM1 | HR_TRM0 | HR_ADM0);
#endif
	GPIBout(eosr, 0);
	GPIBout(auxmr, ICR | 8);                    /* set internal counter register N= 8 */
	GPIBout(auxmr, PPR | HR_PPU);               /* parallel poll unconfigure */
	GPIBout(auxmr, auxrabits);
	GPIBout(auxmr, AUXRB | 0);                  /* set INT pin to active high */
	GPIBout(auxmr, AUXRB | HR_TRI);
	GPIBout(auxmr, AUXRE | 0);
                                                    /* HD68450-8 initializations... */
	GPIBout(ch0.niv, ibXvec);
	GPIBout(ch0.eiv, ibXvec);
	GPIBout(ch1.niv, ibXvec);
	GPIBout(ch1.eiv, ibXvec);
	GPIBout(ch0.cpr, 3);
	GPIBout(ch1.cpr, 3);
	GPIBout(ch0.dcr, (D_CS | D_IACK));
	GPIBout(ch1.dcr, (D_CS | D_IACK | D_IPCL)); /* All ints routed thru ch1 PCL */

	GPIBout(ch0.scr, D_MCU);
	GPIBout(ch1.scr, 0);
#if (A32)                                           /* 32-bit addressing supported  */
        ibcfg2 |= D_32B;
        GPIBout(cfg2, ibcfg2);
/*
 *      Enable A32 Supervisory Data Access (Address Modifier 0D):
 *
 *      AM(5) = HW Jumper W3  = 0
 *      AM(4) = FCR bit M1    = 0
 *      AM(3) = HW Jumper W4  = 1
 *      AM(2) = FCR bit M2    = 1
 *      AM(1) = FCR bit M0    = 0
 *      AM(0) = HW Jumper W5  = AM(1)'
 *
 *      NOTE: AM(5) and AM(4) will be pulled low to 0 regardless
 *      of the settings of W3 and M1 if "32-bit addressing" is
 *      explicitly jumpered on the interface board (W6).
 */
        GPIBout(ch0.mfc, D_SUP);                    /* M2 = 1, M1 = 0, M0 = 0 */
        GPIBout(ch0.bfc, D_SUP);
        GPIBout(ch1.mfc, D_SUP);
        GPIBout(ch1.bfc, D_SUP);
#else                                               /* 24-bit addressing supported  */
/*
 *      Enable A24 Supervisory Data Access (Address Modifier 3D):
 *
 *      AM(5) = HW Jumper W3  = 1
 *      AM(4) = FCR bit M1    = 1
 *      AM(3) = HW Jumper W4  = 1
 *      AM(2) = FCR bit M2    = 1
 *      AM(1) = FCR bit M0    = 0
 *      AM(0) = HW Jumper W5  = AM(1)'
 */
        GPIBout(ch0.mfc, (D_SUP | D_S24));          /* M2 = 1, M1 = 1, M0 = 0 */
        GPIBout(ch0.bfc, (D_SUP | D_S24));
        GPIBout(ch1.mfc, (D_SUP | D_S24));
        GPIBout(ch1.bfc, (D_SUP | D_S24));
#endif
	GPIBout(ch0.ccr, D_SAB);
	GPIBout(ch1.ccr, D_SAB);
	GPIBout(ch0.csr, D_CLEAR);
	GPIBout(ch1.csr, D_CLEAR);

	ibXsta = CMPL;
	ibXcnt = 0;
	if (v) {
		ibcfg2 |= D_SFL;                    /* set SYSFAIL in cfg2 (LED green) */
		GPIBout(cfg2, ibcfg2);
		GPIBout(auxmr, AUX_PON);            /* release pon state to bring online */
		osInit();                           /* initialize system support functions */
		pgmstat |= PS_ONLINE;
		ibstat();
	}
	else {                                      /* OFFLINE: leave SYSFAIL red */
		if (pgmstat & PS_SYSRDY)
			osReset();                  /* reset system interface */
		pgmstat &= ~PS_ONLINE;
	}
	DBGout();
	return ibXsta;
}


/*
 * IBSIC
 * Send IFC for at least 100 microseconds.
 *
 * NOTE:
 *      1.  Ibsic must be called prior to the first call to
 *          ibcmd in order to initialize the bus and enable the
 *          interface to leave the controller idle state.
 */
ibXsic() {
	int i;

	DBGin(IBSIC);
	if (fnInit(0) & ERR) {
		DBGout();
		return ibXsta;
	}
	pgmstat |= PS_SAC;
	ibcfg2 |= D_SC;
	GPIBout(cfg2, ibcfg2);                      /* set system controller */
	GPIBout(auxmr, AUX_SIFC);                   /* assert IFC */
	for(i = 0; i < ifcDelay; i++);              /* busy wait >= ~100us */
	GPIBout(auxmr, AUX_CIFC);                   /* clear IFC */
	ibstat();
	DBGout();
	return ibXsta;
}


/*
 * IBSRE
 * Send REN true if v is non-zero or false if v is zero.
 */
ibXsre(v)
int v;
{
	DBGin(IBSRE);
	if (fnInit(0) & ERR) {
		DBGout();
		return ibXsta;
	}
	pgmstat |= PS_SAC;
	ibcfg2 |= D_SC;
	GPIBout(cfg2, ibcfg2);                      /* set system controller */
	GPIBout(auxmr, (v ? AUX_SREN : AUX_CREN));  /* set or clear REN */
	ibstat();
	DBGout();
	return ibXsta;
}


/*
 * IBGTS
 * Go to the controller standby state from the controller
 * active state, i.e., turn ATN off.
 */
ibXgts()
{
	DBGin(IBGTS);
	if (fnInit(HR_CIC) & ERR) {
		DBGout();
		return ibXsta;
	}
	GPIBout(auxmr, AUX_GTS);                    /* go to standby */
	ibstat();
	DBGout();
	return ibXsta;
}


/*
 * IBCAC
 * Return to the controller active state from the
 * controller standby state, i.e., turn ATN on.  Note
 * that in order to enter the controller active state
 * from the controller idle state, ibsic must be called.
 * If v is non-zero, take control synchronously, if
 * possible.  Otherwise, take control asynchronously.
 */
ibXcac(v)
int v;
{
	DBGin(IBCAC);
	if (fnInit(HR_CIC) & ERR) {
		DBGout();
		return ibXsta;
	}
	if (v && (pgmstat & PS_HELD) && (GPIBin(adsr) & HR_LA)) {
		DBGprint(DBG_BRANCH, ("sync  "));
		GPIBout(auxmr, AUX_TCS);            /* assert ATN synchronously */
	}
	else {
		DBGprint(DBG_BRANCH, ("async  "));
		GPIBout(auxmr, AUX_TCA);            /* assert ATN asynchronously */
	}
	ibstat();
	DBGout();
	return ibXsta;
}


/************************************************************************
 * Supplemental ESP functions derived from NI-488
 ************************************************************************/

#if GSREXIST				/* currently GPIB-1014D only */
/*
 * IBLINES
 * Poll the GPIB control lines and return their status in buf.
 *
 *      LSB (bits 0-7)  -  VALID lines mask (lines that can be monitored).
 * Next LSB (bits 8-15) - STATUS lines mask (lines that are currently set).
 *
 */
ibXlines(buf)
int *buf;
{
	int	bus;			/* bus control line status (GSR) */
	int	brdstat;		/* GPIB interface status  (ADSR) */
	int	state;			/* current bus state vector      */

	DBGin(IBLINES);
	if (fnInit(0) & ERR) {
		*buf = 0;
		DBGout();
		return ibXsta;
	}
	bus = GPIBin(gsr);
	brdstat = GPIBin(adsr);
	DBGprint(DBG_DATA, ("gsr=0x%x adsr=0x%x  ", bus, brdstat));
/*
 *	The table below illustrates the relationship between "Lines that
 *	can be monitored" and the following IEEE-488 states (internal to
 *	the GPIB interface):
 *
 *	- System Active Controller            SAC
 *	- Controller-In-Charge                CIC   (Active or Standby)
 *
 *	- Acceptor Ready State                ACRS  (NRFD false, NDAC true)
 *	- Acceptor Wait for New Cycle State   AWNS  (NRFD true, NDAC false)
 *	- Talker Active State                 TACS
 *	- Controller Active State             CACS
 *
 *	+=====+=====+===================================================+
 *	| SAC | CIC | Lines that can be monitored...                    |
 *	+=====+=====+===================================================+
 *	|     |     |    5           8                  1      2     5  |
 *	|     |     | EOI   ATN   SRQ   REN   IFC   NRFD   NDAC   DAV   |
 *	+-----+-----+---------------------------------------------------+
 *	|     |     |    5           8     7     7      1      2     5  |
 *	| yes |     | EOI   ATN   SRQ   REN   IFC   NRFD   NDAC   DAV   |
 *	+-----+-----+---------------------------------------------------+
 *	|     |     |    6     7                        3      4     6  |
 *	|     | yes | EOI   ATN   SRQ   REN   IFC   NRFD   NDAC   DAV   |
 *	+-----+-----+---------------------------------------------------+
 *	|     |     |    6     7           7     7      3      4     6  |
 *	| yes | yes | EOI   ATN   SRQ   REN   IFC   NRFD   NDAC   DAV   |
 *	+=====+=====+===================================================+
 *
 *	NOTES:
 *	  (1) ONLY IF (TACS + (LACS & ~ACRS) + (ATN_IS_ASSERTED & ~ACRS))
 *	  (2) ONLY IF (TACS + (LACS & ~AWNS) + (ATN_IS_ASSERTED & ~AWNS))
 *	  (3) ONLY IF (TACS + (LACS & ~ACRS) + (CACS))
 *	  (4) ONLY IF (TACS + (LACS & ~AWNS) + (CACS))
 *	  (5) ONLY IF (WE_ARE_ASSERTING_THIS_LINE + ~(TACS))
 *	  (6) ONLY IF (WE_ARE_ASSERTING_THIS_LINE + ~(TACS + CACS))
 *	  (7) ONLY IF (WE_ARE_ASSERTING_THIS_LINE)
 *	  (8) ONLY IF (WE_ARE_ASSERTING_THIS_LINE)
 *
 *	SPECIAL NOTE:
 *	  Since there is no way to accurately determine the specific
 *	  state of the acceptor handshake on the GPIB-1014D, the follow-
 *	  ing approximations must be used:
 *						~ACRS =  DAV
 *						~AWNS = ~DAV
 *
 *
 *	The algorithm used below follows the guidelines in the above table
 *	with the exception of notes (5), (6), and (7).  These notes, which
 *	are only relevant for cases in which a control line is being driven
 *	by a source other than the GPIB interface and in violation of the
 *	IEEE-488 spec, are ignored because IBLINES always assumes a "legal"
 *	bus.  Within that constraint, this algorithm (which we shall call
 *	the "rose-colored glasses" algorithm) should yield accurate results
 *	most of the time.
 *
 *	To begin, the following five lines on a legal bus should always be
 *	valid...
 */
	state = (BUS_EOI | BUS_ATN | BUS_REN | BUS_IFC | BUS_DAV) >> 8;

	if (brdstat & HR_CIC) {
		DBGprint(DBG_BRANCH, ("CIC  "));
		state |= BUS_SRQ >> 8;
		if (brdstat & HR_NATN)
			state |= chkNATN(bus, brdstat);
		else {
			DBGprint(DBG_BRANCH, ("CACS  "));
			state |= (BUS_NDAC | BUS_NRFD) >> 8;
		}
	}
	else {
		DBGprint(DBG_BRANCH, ("not CIC  "));
		if (bus & BR_SRQ)	/* if it's on, we must be asserting it */
			state |= BUS_SRQ >> 8;
		if (brdstat & HR_NATN)
			state |= chkNATN(bus, brdstat);
		else {
			DBGprint(DBG_BRANCH, ("ATN  "));
		/*
		 *	if ( DAV) then monitor NRFD;
		 *	if (~DAV) then monitor NDAC;
		 */
			if (bus & BR_DAV)
				state |= BUS_NRFD >> 8;
			else
				state |= BUS_NDAC >> 8;
		}
	}
/*
 *	Now, for all lines, if a bit is set in the GSR ("bus"), assume
 *	the line is valid and set the corresponding "state" bit...
 */
	if (bus & BR_EOI)
		state |= BUS_EOI;
	if (bus & BR_ATN)
		state |= BUS_ATN;
	if (bus & BR_SRQ)		/* an "invalid" line reads as zero */
		state |= BUS_SRQ;
	if (bus & BR_REN)
		state |= BUS_REN;
	if (bus & BR_IFC)
		state |= BUS_IFC;
	if (bus & BR_DAV)
		state |= BUS_DAV;
	if (bus & BR_NRFD)
		state |= BUS_NRFD;
	if (bus & BR_NDAC)
		state |= BUS_NDAC;

	DBGprint(DBG_DATA, ("state=0x%x  ", state));
	*buf = state;

	ibstat();
	DBGout();
	return ibXsta;
}


IBLCL chkNATN(bus, brdstat)		/* called when ATN unasserted */
int bus;
int brdstat;
{
	int	state;

	DBGin(CHKNATN);
	if (brdstat & HR_TA) {
		DBGprint(DBG_BRANCH, ("TACS  "));
		state = (BUS_NRFD | BUS_NDAC) >> 8;
	}
	else {
		state = 0;
		if (brdstat & HR_LA) {
			DBGprint(DBG_BRANCH, ("LACS  "));
		/*
		 *	if ( DAV) then monitor NRFD;
		 *	if (~DAV) then monitor NDAC;
		 */
			if (bus & BR_DAV)
				state |= BUS_NRFD >> 8;
			else
				state |= BUS_NDAC >> 8;
		}
	}
	DBGout();
	return state;
}

#else					/* GSREXIST */
/*
 * IBLINES
 * Poll the GPIB control lines and return their status in buf.
 *
 *      LSB (bits 0-7)  -  VALID lines mask (lines that can be monitored).
 * Next LSB (bits 8-15) - STATUS lines mask (lines that are currently set).
 *
 */
ibXlines(buf)
int *buf;
{
	int	brdstat;		/* GPIB interface status    (ADSR) */
	int	ch0csr, ch2csr;		/* control status registers (CSRs) */
	int	state;			/* current bus state vector        */

	DBGin(IBLINES);
	if (fnInit(0) & ERR) {
		*buf = 0;
		DBGout();
		return ibXsta;
	}
	brdstat = GPIBin(adsr);
	ch0csr = GPIBin(ch0.csr);
	ch2csr = GPIBin(ch2.csr);
	DBGprint(DBG_DATA, ("adsr=0x%x  ", brdstat));
	DBGprint(DBG_DATA, ("ch0csr=0x%x ch2csr=0x%x  ", ch0csr, ch2csr));

/*
 *	The table below illustrates the relationship between "Lines that
 *	can be monitored" and the following IEEE-488 states (internal to
 *	the GPIB interface):
 *
 *	- System Active Controller            SAC
 *	- Controller-In-Charge                CIC   (Active or Standby)
 *
 *	+=====+=====+===================================================+
 *	| SAC | CIC | Lines that can be monitored...                    |
 *	+=====+=====+===================================================+
 *	|     |     |                2                                  |
 *	|     |     |       ATN   SRQ   REN                             |
 *	+-----+-----+---------------------------------------------------+
 *	|     |     |                2     1                            |
 *	| yes |     |       ATN   SRQ   REN                             |
 *	+-----+-----+---------------------------------------------------+
 *	|     |     |          1                                        |
 *	|     | yes |       ATN   SRQ   REN                             |
 *	+-----+-----+---------------------------------------------------+
 *	|     |     |          1           1                            |
 *	| yes | yes |       ATN   SRQ   REN                             |
 *	+=====+=====+===================================================+
 *
 *	NOTES:
 *	  (1) ONLY IF (WE_ARE_ASSERTING_THIS_LINE)
 *	  (2) ONLY IF (WE_ARE_ASSERTING_THIS_LINE)
 *
 *
 *	The algorithm used below follows the guidelines in the above table
 *	with the exception of note (1).  This note, which is only relevant
 *	for cases in which a control line is being driven by a source other
 *	than the GPIB interface and in violation of the	IEEE-488 spec, is
 *	ignored because IBLINES always assumes a "legal" bus.  Within that
 *	constraint, this algorithm should yield accurate results most of
 *	the time.
 */
	state = (BUS_ATN | BUS_REN) >> 8;
	if (brdstat & HR_CIC) {
		DBGprint(DBG_BRANCH, ("CIC  "));
		state |= BUS_SRQ >> 8;
	}
	else {
		DBGprint(DBG_BRANCH, ("not CIC  "));
		if (!(ch0csr & D_PCLH))	/* if SRQ is on, we must be asserting it */
			state |= BUS_SRQ >> 8;
	}
	if (!(brdstat & HR_NATN))
		state |= BUS_ATN;
	if (!(ch0csr & D_PCLH))
		state |= BUS_SRQ;	/* SRQ is connected to ch0 PCL (active low) */
	if (!(ch2csr & D_PCLH))
		state |= BUS_REN;	/* REN is connected to ch2 PCL (active low) */

	DBGprint(DBG_DATA, ("state=0x%x  ", state));
	*buf = state;

	ibstat();
	DBGout();
	return ibXsta;
}
#endif					/* GSREXIST */

/*
 * IBPAD
 * change the GPIB address of the interface board.  The address
 * must be 0 through 30.  ibonl resets the address to PAD.
 */
ibXpad(v)
int v;
{
	DBGin(IBPAD);
	if (fnInit(0) & ERR) {
		DBGout();
		return ibXsta;
	}
	if ((v < 0) || (v > 30)) {
		ibXsta |= ERR;
		ibXerr = EARG;
	}
	else {
		myPAD = v;
		DBGprint(DBG_DATA, ("pad=0x%x  ", myPAD));
		GPIBout(adr, (myPAD & LOMASK));
	}
	ibstat();
	DBGout();
	return ibXsta;
}


/*
 * IBSAD
 * change the secondary GPIB address of the interface board.
 * The address must be 0x60 through 0x7E.  ibonl resets the
 * address to SAD.
 */
ibXsad(v)
int v;
{
	DBGin(IBSAD);
	if (fnInit(0) & ERR) {
		DBGout();
		return ibXsta;
	}
	if (v && ((v < 0x60) || (v > 0x7F))) {
		ibXsta |= ERR;
		ibXerr = EARG;
	}
	else {
		if (v == 0x7F)
			v = 0;		/* v == 0x7F also disables */
		if (mySAD = v) {
			DBGprint(DBG_BRANCH, ("enabled  "));
			DBGprint(DBG_DATA, ("sad=0x%x  ", mySAD));
			GPIBout(adr, HR_ARS | (mySAD & LOMASK));
			GPIBout(admr, HR_TRM1 | HR_TRM0 | HR_ADM1);
		}
		else {
			DBGprint(DBG_BRANCH, ("disabled  "));
			GPIBout(adr, HR_ARS | HR_DT | HR_DL);
			GPIBout(admr, HR_TRM1 | HR_TRM0 | HR_ADM0);
		}
	}
	ibstat();
	DBGout();
	return ibXsta;
}


/*
 * IBTMO
 * Set the timeout value for I/O operations to v.  Timeout
 * intervals can range from 10 usec to 1000 sec.  The value
 * of v specifies an index into the array timeTable.
 * If v == 0 then timeouts are disabled.
 */
ibXtmo(v)
int v;
{
	DBGin(IBTMO);
	if (fnInit(0) & ERR) {
		DBGout();
		return ibXsta;
	}
	if ((v < TNONE) || (v > T1000s)) {
		ibXsta |= ERR;
		ibXerr = EARG;
	}
	else {
		DBGprint(DBG_DATA, ("oldtmo=%d newtmo=%d  ", timeidx, v));
		timeidx = v;
	}
	ibstat();
	DBGout();
	return ibXsta;
}


/*
 * IBEOT
 * Set the end-of-transmission mode for I/O operations to v.
 * If v == 1 then send EOI with the last byte of each write.
 * If v == 0 then disable the sending of EOI.
 */
ibXeot(v)
int v;
{
	DBGin(IBEOT);
	if (fnInit(0) & ERR) {
		DBGout();
		return ibXsta;
	}
	if (v) {
		DBGprint(DBG_BRANCH, ("enable EOI  "));
		pgmstat &= ~PS_NOEOI;
	}
	else {
		DBGprint(DBG_BRANCH, ("disable EOI  "));
		pgmstat |= PS_NOEOI;
	}
	ibstat();
	DBGout();
	return ibXsta;
}


/*
 * IBEOS
 * Set the end-of-string modes for I/O operations to v.
 * Refer to ugpib.h and esp488.h for relevant EOS mask bits.
 */
ibXeos(v)
int v;
{
	int ebyte, emodes;

	DBGin(IBEOS);
	if (fnInit(0) & ERR) {
		DBGout();
		return ibXsta;
	}
	ebyte = v & 0xFF;
	emodes = (v >> 8) & 0xFF;
	if (emodes & ~EOSM) {
		DBGprint(DBG_BRANCH, ("bad EOS modes  "));
		ibXsta |= ERR;
		ibXerr = EARG;
	}
	else {
		auxrabits = AUXRA | emodes;
		DBGprint(DBG_DATA, ("byte=0x%x modes=0x%x  ", ebyte, emodes));
		GPIBout(eosr, ebyte);
		GPIBout(auxmr, auxrabits);
	}
	ibstat();
	DBGout();
	return ibXsta;
}


/*
 * IBRSV
 * Request service from the CIC and/or set the serial poll
 * status byte.
 */
ibXrsv(v)
int v;
{
	DBGin(IBRSV);
	if (fnInit(0) & ERR) {
		DBGout();
		return ibXsta;
	}
	DBGprint(DBG_DATA, ("stb=0x%x  ", v));
	GPIBout(spmr, 0);		/* clear current serial poll status */
	GPIBout(spmr, v);		/* set new status to v */
	ibstat();
	DBGout();
	return ibXsta;
}


/************************************************************************
 * Enhanced ESP device-level functions (488.2 compatible)
 ************************************************************************/

/*
 * DVTRG
 * Trigger the device with primary address pad and secondary 
 * address sad.  If the device has no secondary address, pass a
 * zero in for this argument.  This function sends the TAD of the
 * GPIB interface, UNL, the LAD of the device, and GET.
 */ 
dvXtrg(padsad)
int padsad;
{
	char cmdString[2];

	DBGin(DVTRG);
	if (fnInit(HR_CIC) & ERR) {
		DBGout();
		return ibXsta;
	}
	if (!(send_setup(padsad) & ERR)) {
		cmdString[0] = GET;
		ibXcmd((faddr_t)cmdString, 1);
	}
	DBGout();
	return ibXsta;
}	


/*
 * DVCLR
 * Clear the device with primary address pad and secondary 
 * address sad.  If the device has no secondary address, pass a
 * zero in for this argument.  This function sends the TAD of the
 * GPIB interface, UNL, the LAD of the device, and SDC.
 */ 
dvXclr(padsad)
int padsad;
{
	char cmdString[2];

	DBGin(DVCLR);
	if (fnInit(HR_CIC) & ERR) {
		DBGout();
		return ibXsta;
	}
	if (!(send_setup(padsad) & ERR)) {
		cmdString[0] = SDC;
		ibXcmd((faddr_t)cmdString, 1);
	}
	DBGout();
	return ibXsta;
}	


/*
 * DVRSP
 * This function performs a serial poll of the device with primary 
 * address pad and secondary address sad. If the device has no
 * secondary adddress, pass a zero in for this argument.  At the
 * end of a successful serial poll the response is returned in spb.
 * SPD and UNT are sent at the completion of the poll.
 */
dvXrsp(padsad, spb)
int padsad;
char *spb;
{
	char spdString[3];
	uint8 isreg1;
	int sp_noTimo;
	
	DBGin(DVRSP);
	if (fnInit(HR_CIC) & ERR) {
		DBGout();
		return ibXsta;
	}
	if (!(receive_setup(padsad, 1) & ERR)) {
        	spdString[0] = SPD;	/* disable serial poll bytes */
        	spdString[1] = UNT;
		GPIBout(auxmr, auxrabits | HR_HLDA);
		if (pgmstat & PS_HELD) {
			DBGprint(DBG_BRANCH, ("finish handshake  "));
			GPIBout(auxmr, AUX_FH);
			pgmstat &= ~PS_HELD;
		}
		GPIBout(auxmr, AUX_GTS);
	
		startTimer(pollTimeidx);
		DBGprint(DBG_BRANCH, ("wait for spoll byte  "));
		while (!((isreg1 = GPIBin(isr1)) & HR_DI) && NotTimedOut())
			;		/* wait for byte to come in */
		GPIBout(auxmr, AUX_TCS);
		if (isreg1 & HR_DI) {
			DBGprint(DBG_BRANCH, ("received  "));
			*spb = GPIBin(dir);
			DBGprint(DBG_DATA, ("spb=0x%x  ", *spb));
			pgmstat |= PS_HELD;
		}
		else {
			DBGprint(DBG_BRANCH, ("NOT received  "));
			*spb = 0;
		}
		while ((GPIBin(adsr) & HR_NATN) && NotTimedOut())
			;		/* wait for synchronous take control */
		sp_noTimo = noTimo;
		removeTimer();
		if ((ibXcmd((faddr_t)spdString, 2) & ERR) || !sp_noTimo) {
			ibXsta |= ERR;	/* something went wrong */
			ibXerr = EBUS;
		}
		else	ibXcnt = 1;	/* SUCCESS: spoll byte received */
	}
	DBGout();
	return ibXsta;
}


/*
 * DVRD
 * Read cnt bytes from the device with primary address pad and
 * secondary address sad.  If the device has no secondary address,
 * pass a zero in for this argument.  The device is adressed to
 * talk and the GPIB interface is addressed to listen.  ibrd is
 * then called to read cnt bytes from the device and place them
 * in buf.
 */
dvXrd(padsad, buf, cnt)
int padsad;
faddr_t buf;
unsigned int cnt;
{
	DBGin(DVRD);
	ibXcnt = 0;
	if (fnInit(HR_CIC) & ERR) {
		DBGout();
		return ibXsta;
	}
	if (!(receive_setup(padsad, 0) & ERR))
		ibXrd(buf, cnt);

	DBGout();
	return ibXsta;
}


/*
 * DVWRT
 * Write cnt bytes to the device with primary address pad and
 * secondary address sad.  If the device has no secondary address,
 * pass a zero in for this argument.  The device is adressed to
 * listen and the GPIB interface is addressed to talk.  ibwrt is
 * then called to write cnt bytes to the device from buf.
 */
dvXwrt(padsad, buf, cnt)
int padsad;
faddr_t buf;
unsigned int cnt;
{
	DBGin(DVWRT);
	ibXcnt = 0;
	if (fnInit(HR_CIC) & ERR) {
		DBGout();
		return ibXsta;
	}
	if (!(send_setup(padsad) & ERR))
		ibXwrt(buf, cnt);

	DBGout();
	return ibXsta;
}


/*
 * 488.2 Controller sequences
 */
IBLCL receive_setup(padsad, spoll)
int padsad;
int spoll;				/* TRUE if this is for a serial poll */
{
	uint8 pad, sad;
	char cmdString[8];
	unsigned int i = 0;

	DBGin(RECEIVE_SETUP);
	pad = padsad;
	sad = padsad >> 8;
	DBGprint(DBG_DATA, ("pad=0x%x, sad=0x%x  ", pad, sad));
	if ((pad > 0x1E) || (sad && ((sad < 0x60) || (sad > 0x7E)))) {
		DBGprint(DBG_BRANCH, ("bad addr  "));
		ibXsta |= ERR;
		ibXerr = EARG;
		ibstat();
		DBGout();
		return ibXsta;
	}
	cmdString[i++] = UNL;
	cmdString[i++] = myPAD | LAD;	/* controller's listen address */
	if (mySAD)
		cmdString[i++] = mySAD;
	if (spoll) {
		DBGprint(DBG_BRANCH, ("SPE  "));
		cmdString[i++] = SPE;
	}
	cmdString[i++] = pad | TAD;
	if (sad)
		cmdString[i++] = sad;
	if ((ibXcmd(cmdString, i) & ERR) && (ibXerr == EABO))
		ibXerr = EBUS;
	DBGout();
	return ibXsta;
}


IBLCL send_setup(padsad)
int padsad;
{
	uint8 pad, sad;
	char cmdString[8];
	unsigned i = 0;

	DBGin(SEND_SETUP);
	pad = padsad;
	sad = padsad >> 8;
	DBGprint(DBG_DATA, ("pad=0x%x, sad=0x%x  ", pad, sad));
	if ((pad > 0x1E) || (sad && ((sad < 0x60) || (sad > 0x7E)))) {
		DBGprint(DBG_BRANCH, ("bad addr  "));
		ibXsta |= ERR;
		ibXerr = EARG;
		ibstat();
		DBGout();
		return ibXsta;
	}
	cmdString[i++] = myPAD | TAD;	/* controller's talk address */
	if (mySAD)
		cmdString[i++] = mySAD;
	cmdString[i++] = UNL;
	cmdString[i++] = pad | LAD;
	if (sad)
		cmdString[i++] = sad;
	if ((ibXcmd(cmdString, i) & ERR) && (ibXerr == EABO))
		ibXerr = EBUS;
	DBGout();
	return ibXsta;
}


/*
 * Function initialization -- called on entry to any ESP function
 */
IBLCL fnInit(reqd_adsbit)
int reqd_adsbit;
{
	int brdstat;

	DBGin(FNINIT);
	ibXsta = CMPL;
	noTimo = INITTIMO;
	if (!(pgmstat & PS_ONLINE)) {
		DBGprint(DBG_BRANCH, ("ERR:offline  "));
		ibXsta |= ERR;
		ibXerr = ENEB;
	}
	else if (reqd_adsbit) {
		brdstat = GPIBin(adsr);
		if (brdstat & HR_CIC) {
			if (!(brdstat & reqd_adsbit)) {
				DBGprint(DBG_BRANCH, ("ERR:not addressed  "));
				ibXsta |= ERR;
				ibXerr = EADR;
				ibstat();
			}
		}
		else if (reqd_adsbit & HR_CIC) {
			DBGprint(DBG_BRANCH, ("ERR:not CIC  "));
			ibXsta |= ERR;
			ibXerr = ECIC;
			ibstat();
		}
	}
	DBGout();
	return ibXsta;
}


/************************************************************************
 * Enhanced ESP system support functions
 ************************************************************************/


#include "sys488.c"		/* OS-specific function include file	*/


/* (END OF FILE) ------------------------------------------------------ */

