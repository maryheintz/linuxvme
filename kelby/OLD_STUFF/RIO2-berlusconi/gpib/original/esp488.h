/************************************************************************
 *
 * Enhanced Engineering Software Package for GPIB-1014 Interface
 * Revision C.1
 * Copyright (c) 1993 National Instruments Corporation
 * All rights reserved.
 *
 * Include file for esp488.c
 *
 ************************************************************************/

#include "ugpib.h"		/* User GPIB include file */


/*----- Editable Parameters --------------------------------------------*/

#define PAD	 0		/* Primary address of GPIB interface	*/
				/*  Range: 00H - 30H			*/
#define SAD	 0		/* Secondary address of GPIB interface	*/
				/*  Range: 60H - 7EH, or 0 if none	*/

#define IBBASE	 0xDFFE2000		/* 16-bit I/O address of GPIB interface	*/
#define IBVEC	 0xF0		/* interrupt vector number		*/
#define IBIRQ	 2		/* interrupt request line (1 - 7)	*/
#define IBBRG	 3		/* bus request/grant line (1 - 3)	*/

#define DFLTTIMO T10s		/* Default timeout for I/O operations	*/
#define HZ	 60		/* System clock ticks per second	*/

#define A32	 0		/* 0 = A24 addressing,   1 = A32/1014D  */
#define GSREXIST 0		/* 0 = no GSR on board,  1 = GSR/1014D  */
#define DEBUG	 0		/* 0 = normal operation, 1 = DEBUG mode */
#define AUTOCC	 0		/* 0 = s/w carry cycle,  1 = h/w CC	*/
#define DMAOP	 0		/* 0 = PIO transfers,    1 = DMA	*/
#define INTROP	 0		/* 0 = polled operation, 1 = interrupt	*/
#define SYSTIMO	 0		/* 0 = no system timeouts (use counter)	*/
				/* 1 = system timeouts supported	*/

/*----- END Editable Parameters ----------------------------------------*/


#define IBMAXIO  0xFFFE			/* Maximum cont. I/O transfer	*/


typedef char		    int8;	/* HW-independent data types...	*/
typedef unsigned char	    uint8;
typedef short		    int16;
typedef unsigned short	    uint16;
typedef long		    int32;
typedef unsigned long	    uint32;

typedef unsigned char	  * faddr_t;	/* far character pointer	*/

/*
 * DMA chaining array definitions for carry cycle function (channel 1).
 */
typedef struct cc_array {
        uint16          cc_ccb_hi;      /* PA of carry-cycle byte       */
        uint16          cc_ccb_lo;      /* PA of carry-cycle byte       */
        uint16          cc_ONE;         /* block transfer count for MTC */
        uint16          cc_n_1addr_hi;  /* PA of last byte in buffer    */
        uint16          cc_n_1addr_lo;  /* PA of last byte in buffer    */
        uint16          cc_TWO;         /* block transfer count for MTC */

} cc_array_t;

typedef struct cc_block {
        cc_array_t      cca;            /* carry-cycle array            */
        uint8           ccbyte;         /* carry-cycle byte for AUXMR   */
        uint8           ccDUMMY;        /* PADDING                      */

} cc_block_t;

/*
 * The following structure dma_chan defines the memory map of a single
 * channel on the Hitachi 68450 (GPIB-1014 only).
 */
typedef struct dma_chan {
	uint8           csr;    /*  +0 (x00) channel status register    */
	uint8           cer;    /*  +1 (x01) channel error register     */
	uint8   f00[2];         /*  +2 (x02) PADDING                    */
	uint8           dcr;    /*  +4 (x04) device control register    */
	uint8           ocr;    /*  +5 (x05) operation control register */
	uint8           scr;    /*  +6 (x06) sequence control register  */
	uint8           ccr;    /*  +7 (x07) channel control register   */
	uint8   f01[2];         /*  +8 (x08) PADDING                    */
        uint16          mtc;    /* +10 (x0A) memory transfer counter    */
	uint16          marhi;  /* +12 (x0C) memory address register    */
	uint16          marlo;  /* +14 (x0E) memory address register    */
	uint8   f02[4];         /* +16 (x10) PADDING                    */
	uint16          darhi;  /* +20 (x14) device address register    */
	uint16          darlo;  /* +22 (x16) device address register    */
	uint8   f03[2];         /* +24 (x18) PADDING                    */
        uint16          btc;    /* +26 (x1A) base transfer counter      */
	uint16          barhi;  /* +28 (x1C) base address register      */
	uint16          barlo;  /* +30 (x1E) base address register      */
	uint8   f04[5];         /* +32 (x20) PADDING                    */
	uint8           niv;    /* +37 (x25) normal interrupt vector    */
	uint8   f05;            /* +38 (x26) PADDING                    */
	uint8           eiv;    /* +39 (x27) error interrupt vector     */
	uint8   f06;            /* +40 (x28) PADDING                    */
	uint8           mfc;    /* +41 (x29) memory function codes      */
	uint8   f07[3];         /* +42 (x2A) PADDING                    */
	uint8           cpr;    /* +45 (x2D) channel priority register  */
	uint8   f08[3];         /* +46 (x2E) PADDING                    */
	uint8           dfc;    /* +49 (x31) device function codes      */
	uint8   f09[7];         /* +50 (x32) PADDING                    */
	uint8           bfc;    /* +57 (x39) base function codes        */
	uint8   f0A[6];         /* +58 (x3A) PADDING                    */

} dma_chan_t;

typedef struct ibregs {         /*  hardware registers                  */
	dma_chan_t      ch0;    /*   +0 (x000) dma channel 0            */
	dma_chan_t      ch1;    /*  +64 (x040) dma channel 1            */
	dma_chan_t      ch2;    /* +128 (x080) dma channel 2            */
	dma_chan_t      ch3;    /* +192 (x0C0) dma channel 3            */
	uint8   f0B;            /* +256 (x100) PADDING                  */
	uint8           cfg1;   /* +257 (x101) configuration register 1 */
	uint8   f0C[3];         /* +258 (x102) PADDING                  */
	uint8           cfg2;   /* +261 (x105) configuration register 2 */
#if (A32)
	uint8   f0Da[3];        /* +262 (x106) PADDING                  */
	uint8           pgr;    /* +265 (x109) page address register    */
	uint8   f0Db[6];        /* +266 (x10A) PADDING                  */
#else
	uint8   f0D[10];        /* +262 (x106) PADDING                  */
#endif
	uint8   f0E,    cdor;   /* +273 (x111) byte out register        */
	uint8   f0F,    imr1;   /* +275 (x113) interrupt mask reg. 1    */
	uint8   f10,    imr2;   /* +277 (x115) interrupt mask reg. 2    */
	uint8   f11,    spmr;   /* +279 (x117) serial poll mode reg.    */
	uint8   f12,    admr;   /* +281 (x119) address mode register    */
	uint8   f13,    auxmr;  /* +283 (x11B) auxiliary mode register  */
	uint8   f14,    adr;    /* +285 (x11D) address register 0/1     */
	uint8   f15,    eosr;   /* +287 (x11F) end of string register   */

} ibregs_t;


#define gcr     ch3.f0A[5]      /* +255 (x0FF) general control register */
#if GSREXIST
#define gsr     cfg1            /* +257 (x101) GPIB status register     */
#endif


/* Read-only register mnemonics corresponding to write-only registers */

#define dir	cdor		/* Data In Register */
#define isr1	imr1		/* Interrupt Status Register 1 */
#define isr2	imr2		/* Interrupt Status Register 2 */
#define spsr	spmr		/* Serial Poll Status Register */
#define adsr	admr		/* Address Status Register */
#define cptr	auxmr		/* Command Pass Through Register */
#define adr0	adr		/* Address 0 Register */
#define adr1	eosr		/* Address 1 Register */ 


/*--------------------------------------------------------------*/
/* 7210 bits:                POSITION              7210 reg     */
/*--------------------------------------------------------------*/
#define HR_DI           (unsigned char)(1<<0)	/* ISR1         */
#define HR_DO           (unsigned char)(1<<1)	/*  ,           */
#define HR_ERR          (unsigned char)(1<<2)	/*  ,           */
#define HR_DEC          (unsigned char)(1<<3)	/*  ,           */
#define HR_END          (unsigned char)(1<<4)	/*  ,           */
#define HR_DET          (unsigned char)(1<<5)	/*  ,           */
#define HR_APT          (unsigned char)(1<<6)	/*  ,           */
#define HR_CPT          (unsigned char)(1<<7)	/*  ,           */
#define HR_DIIE         (unsigned char)(1<<0)	/* IMR1         */
#define HR_DOIE         (unsigned char)(1<<1)	/*  ,           */
#define HR_ERRIE        (unsigned char)(1<<2)	/*  ,           */
#define HR_DECIE        (unsigned char)(1<<3)	/*  ,           */
#define HR_ENDIE        (unsigned char)(1<<4)	/*  ,           */
#define HR_DETIE        (unsigned char)(1<<5)	/*  ,           */
#define HR_APTIE        (unsigned char)(1<<6)	/*  ,           */
#define HR_CPTIE        (unsigned char)(1<<7)	/*  ,           */
#define HR_ADSC         (unsigned char)(1<<0)	/* ISR2         */
#define HR_REMC         (unsigned char)(1<<1)	/*  ,           */
#define HR_LOKC         (unsigned char)(1<<2)	/*  ,           */
#define HR_CO           (unsigned char)(1<<3)	/*  ,           */
#define HR_REM          (unsigned char)(1<<4)	/*  ,           */
#define HR_LOK          (unsigned char)(1<<5)	/*  ,           */
#define HR_SRQI         (unsigned char)(1<<6)	/*  ,           */
#define HR_INT          (unsigned char)(1<<7)	/*  ,           */
#define HR_ACIE         (unsigned char)(1<<0)	/* IMR2         */
#define HR_REMIE        (unsigned char)(1<<1)	/*  ,           */
#define HR_LOKIE        (unsigned char)(1<<2)	/*  ,           */
#define HR_COIE         (unsigned char)(1<<3)	/*  ,           */
#define HR_DMAI         (unsigned char)(1<<4)	/*  ,           */
#define HR_DMAO         (unsigned char)(1<<5)	/*  ,           */
#define HR_SRQIE        (unsigned char)(1<<6)	/*  ,           */
#define HR_PEND         (unsigned char)(1<<6)	/* SPSR         */
#define HR_RSV          (unsigned char)(1<<6)	/* SPMR         */
#define HR_MJMN         (unsigned char)(1<<0)	/* ADSR         */
#define HR_TA           (unsigned char)(1<<1)	/*  ,           */
#define HR_LA           (unsigned char)(1<<2)	/*  ,           */
#define HR_TPAS         (unsigned char)(1<<3)	/*  ,           */
#define HR_LPAS         (unsigned char)(1<<4)	/*  ,           */
#define HR_SPMS         (unsigned char)(1<<5)	/*  ,           */
#define HR_NATN         (unsigned char)(1<<6)	/*  ,           */
#define HR_CIC          (unsigned char)(1<<7)	/*  ,           */
#define HR_ADM0         (unsigned char)(1<<0)	/* ADMR         */
#define HR_ADM1         (unsigned char)(1<<1)	/*  ,           */
#define HR_TRM0         (unsigned char)(1<<4)	/*  ,           */
#define HR_TRM1         (unsigned char)(1<<5)	/*  ,           */
#define HR_LON          (unsigned char)(1<<6)	/*  ,           */
#define HR_TON          (unsigned char)(1<<7)	/*  ,           */
#define HR_DL           (unsigned char)(1<<5)	/* ADR          */
#define HR_DT           (unsigned char)(1<<6)	/*  ,           */
#define HR_ARS          (unsigned char)(1<<7)	/*  ,           */
#define HR_EOI          (unsigned char)(1<<7)	/* ADR1         */
#define HR_HLDA         (unsigned char)(1<<0)	/* auxra        */
#define HR_HLDE         (unsigned char)(1<<1)	/*  ,           */
#define HR_REOS         (unsigned char)(1<<2)	/*  ,           */
#define HR_XEOS         (unsigned char)(1<<3)	/*  ,           */
#define HR_BIN          (unsigned char)(1<<4)	/*  ,           */
#define HR_CPTE         (unsigned char)(1<<0)	/* auxrb        */
#define HR_SPEOI        (unsigned char)(1<<1)	/*  ,           */
#define HR_TRI          (unsigned char)(1<<2)	/*  ,           */
#define HR_INV          (unsigned char)(1<<3)	/*  ,           */
#define HR_ISS          (unsigned char)(1<<4)	/*  ,           */
#define HR_PPS          (unsigned char)(1<<3)	/* ppr          */
#define HR_PPU          (unsigned char)(1<<4)	/*  ,           */

#define HR_LCM (unsigned char)(HR_HLDE|HR_HLDA)	/* auxra listen continuous */

#define BR_EOI		(unsigned char)(1<<7)	/* gsr          */
#define BR_ATN		(unsigned char)(1<<6)	/*  ,           */
#define BR_SRQ		(unsigned char)(1<<5)	/*  ,           */
#define BR_REN		(unsigned char)(1<<4)	/*  ,           */
#define BR_IFC		(unsigned char)(1<<3)	/*  ,           */
#define BR_NRFD		(unsigned char)(1<<2)	/*  ,           */
#define BR_NDAC		(unsigned char)(1<<1)	/*  ,           */
#define BR_DAV		(unsigned char)(1<<0)	/*  ,           */


#define ICR		(unsigned char)0040	/* AUXMR control masks for hidden regs */
#define PPR		(unsigned char)0140
#define AUXRA		(unsigned char)0200
#define AUXRB		(unsigned char)0240
#define AUXRE		(unsigned char)0300
#define LOMASK		0x1F			/* mask to specify lower 5 bits */

/* 7210 Auxiliary Commands */
#define AUX_PON         (unsigned char)000	/* Immediate Execute pon                  */
#define AUX_CPPF        (unsigned char)001	/* Clear Parallel Poll Flag               */
#define AUX_CR          (unsigned char)002	/* Chip Reset                             */
#define AUX_FH          (unsigned char)003	/* Finish Handshake                       */
#define AUX_TRIG        (unsigned char)004	/* Trigger                                */
#define AUX_RTL         (unsigned char)005	/* Return to local                        */
#define AUX_SEOI        (unsigned char)006	/* Send EOI                               */
#define AUX_NVAL        (unsigned char)007	/* Non-Valid Secondary Command or Address */
#define AUX_SPPF        (unsigned char)011	/* Set Parallel Poll Flag                 */
#define AUX_VAL         (unsigned char)017	/* Valid Secondary Command or Address     */
#define AUX_TCA         (unsigned char)021	/* Take Control Asynchronously            */
#define AUX_GTS         (unsigned char)020	/* Go To Standby                          */
#define AUX_TCS         (unsigned char)022	/* Take Control Synchronously             */
#define AUX_LTN         (unsigned char)023	/* Listen                                 */
#define AUX_DSC         (unsigned char)024	/* Disable System Control                 */
#define AUX_CIFC        (unsigned char)026	/* Clear IFC                              */
#define AUX_CREN        (unsigned char)027	/* Clear REN                              */
#define AUX_TCSE        (unsigned char)032	/* Take Control Synchronously on End      */
#define AUX_LTNC        (unsigned char)033	/* Listen in Continuous Mode              */
#define AUX_LUN         (unsigned char)034	/* Local Unlisten                         */
#define AUX_EPP         (unsigned char)035	/* Execute Parallel Poll                  */
#define AUX_SIFC        (unsigned char)036	/* Set IFC                                */
#define AUX_SREN        (unsigned char)037	/* Set REN                                */


/* 68450 DMAC register definitions */

/* Device Control Register (dcr) bits */
#define D_CS     0x80            /* cycle steal mode */
#define D_CSM    0x80            /* cycle steal mode, with bus Monitor*/
#define D_CSH    0xC0            /* cycle steal with hold */
#define D_CSHM   0xC0            /* cycle steal with hold, with bus Monitor */
#define D_IACK   0x20            /* device with DMAACK, implicitly addressed */
#define D_P16    0x08            /* 16 bit device port size */
#define D_IPCL   0x01            /* PCL set to status input with interrupt */

/* Operation Control Register (ocr) bits */
#define D_MTD    0x00            /* direction is from memory to device */
#define D_DTM    0x80            /* direction is from device to memory */
#define D_TW     0x10            /* transfer size is word */
#define D_TL     0x20            /* transfer size is long word */
#define D_ACH    0x08            /* array chaining */
#define D_LACH   0x0C            /* linked array chaining */
#define D_ARQ    0x03            /* auto request first transfer, then external */
#define D_XRQ    0x02            /* external request mode */
#define D_ARM    0x01            /* auto request at maximum rate */


/* Sequence Control Register (scr) bits */
#define D_MCD    0x08            /* memory address counts down */
#define D_MCU    0x04            /* memory address counts up */
#define D_DCD    0x02            /* device address counts down */
#define D_DCU    0x01            /* device address counts up */

/* Channel Control Register (ccr) bits */
#define D_SRT    0x80            /* start channel operation */
#define D_CON    0x40            /* continue */
#define D_HLT    0x20            /* halt channel operation */
#define D_SAB    0x10            /* software abort */
#define D_EINT   0x08            /* enable channel interrupts */

/* Channel Status Register (csr) bits */
#define D_CLEAR  0xFF            /* clear all bits */
#define D_COC    0x80            /* channel operation complete */
#define D_BTC    0x40            /* block transfer complete */
#define D_NDT    0x20            /* normal device termination */
#define D_ERR    0x10            /* error as coded in cer */
#define D_ACT    0x08            /* channel active */
#define D_PCLT   0x02            /* PCL transition occurred */
#define D_PCLH   0x01            /* PCL line is high */

/* Channel Error Register (cer) bits */
#define D_ECF    0x01            /* configuration error */
#define D_ETM    0x02            /* operation timing error */
#define D_EMA    0x05            /* memory address error */
#define D_EDA    0x06            /* device address error */
#define D_EBA    0x07            /* base address error */
#define D_EBUS   0x08            /* bus error */
#define D_ECT    0x0C            /* transfer count error */
#define D_EEAB   0x10            /* external abort */
#define D_ESAB   0x11            /* software abort */

/* Channel Priority Register (cpr) bits */
#define D_PR1    0x01            /* priority 1 */
#define D_PR2    0x02            /* priority 2 */
#define D_PR3    0x03            /* priority 3 */

/* Function Code Register (fcr) bits */
#define D_SUP    0x04            /* supervisor access */
#define D_S24    0x02            /* standard 24 bit addressing */
#define D_PSA    0x01            /* program space access */

/* Configuration Register 1 (cfg1) bits */
#define D_OUT    0               /* direction memory to GPIB */
#define D_IN    (1<<0)           /* direction GPIB to memory */
#define D_DBM   (1<<1)           /* disarm Bus Monitor mode */
#define D_ECC   (1<<2)           /* arm automatic carry cycle feature */
#define D_BRG0  (0<<3)           /* select bus request/grant line 0 */
#define D_BRG1  (1<<3)           /* select bus request/grant line 1 */
#define D_BRG2  (2<<3)           /* select bus request/grant line 2 */
#define D_BRG3  (3<<3)           /* select bus request/grant line 3 */
#define D_NINT  0x00             /* no interrupts used */
#define D_IRQ1  0x20             /* select interrupt request line 1 */
#define D_IRQ2  0x40             /* select interrupt request line 2 */
#define D_IRQ3  0x60             /* select interrupt request line 3 */
#define D_IRQ4  0x80             /* select interrupt request line 4 */
#define D_IRQ5  0xA0             /* select interrupt request line 5 */
#define D_IRQ6  0xC0             /* select interrupt request line 6 */
#define D_IRQ7  0xE0             /* select interrupt request line 7 */

/* Configuration Register 2 (cfg2) bits */
#define D_SC    (1<<0)          /* set system controller (SC) bit */
#define D_LMR   (1<<1)          /* set local master reset bit */
#define D_SPAC  (1<<2)          /* set supervisor only access to board */
#define D_SFL   (1<<3)         	/* clear SYSFAIL line */
#define D_32B   (1<<4)          /* enable 32-bit addressing (1014D only) */


/* PROGRAM STATUS codes (see int pgmstat) */

#define PS_ONLINE	(1 << 0)		/* GPIB interface is online */
#define PS_SYSRDY	(1 << 1)		/* System support functions are initialized */
#define PS_TIMINST	(1 << 2)		/* Watch dog timer is installed and running */
#define PS_HELD		(1 << 3)		/* Handshake holdoff in effect */
#define PS_NOINTS	(1 << 4)		/* Do NOT use interrupts for I/O and waits */
#define PS_NOEOI	(1 << 5)		/* Do NOT send EOI at the end of writes */
#define PS_SAC		(1 << 6)		/* GPIB interface is System Controller */
#define PS_NOEOSEND	(1 << 7)		/* Do NOT set END in ibsta with EOS */
#define PS_SILENT	(1 << 8)		/* Do NOT print any messages from ibpoke */

#define PS_STATIC	(PS_SYSRDY | PS_NOINTS | PS_NOEOSEND | PS_SILENT)


/* IBPOKE codes for undocumented, low-level control */

#define PK_CODE		0x7FFF0000		/* CODE portion of mask */
#define PK_VALUE	0x0000FFFF		/* VALUE portion of mask */

#define PK_DEBUG	0x00000000		/* Turn on debugging if VALUE non-zero */
#define PK_USEINTS	0x00010000		/* Turn on interrupts if VALUE non-zero */
#define PK_IFCDELAY	0x00020000		/* Adjust length of IFC delay */
#define PK_POLLTIMO	0x00030000		/* Adjust POLL timeout index */
#define PK_ENABMEM	0x00040000		/* Enable system memory for DMA xfers */
#define PK_EOSMODE	0x00050000		/* Turn off 'END with EOS' mode if VALUE is zero */
#define PK_INFO		0x00100000		/* Turn off poke messages if VALUE is zero */


/* DMA/carry-cycle support macros */

#define USECC		(AUTOCC || !DMAOP)	/* "CC" always works for PIO */


/* Interrupt support macros */

#define USEINTS		(INTROP && SYSTIMO)	/* interrupts require SYSTIMO */

#if USEINTS
#define WaitingFor(i,j)	{if (!(pgmstat & PS_NOINTS)) waitForInt(i,j);}
#else
#define WaitingFor(i,j)
#endif


/* Timeout support macros */

#if SYSTIMO
#define TimedOut()	(!noTimo)		/* for testing within loops... */
#define NotTimedOut()	(noTimo)

#define TMFAC		HZ			/* timing factor for TM() macro */

#else
#define TimedOut()	((noTimo > 0) && !(--noTimo))
#define NotTimedOut()	((noTimo < 0) || ((noTimo > 0) && --noTimo))

#define TMFAC		(HZ * 1000)		/* timing factor for TM() macro */
#endif						/* -- approximate and system dependent! */

#define INITTIMO	(-1)			/* initial timeout setting */
#define TM(f,n,d)	((((f)*(n))/(d))+1)	/* clock ticks or counts per timo value */

