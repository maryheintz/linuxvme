#include <types.h>
#include <stdio.h>
#include <time.h>

unsigned int BASE;
char *DIR,*CDOR,*ISR1,*IMR1,*ISR2,*IMR2,*SPSR,*SPMR,*ADSR,*ADMR;
char *CPTR,*AUXMR,*ADR0,*ADR,*ADR1,*EOSR,*MFC0,*CSR0,*DCR0;
char *OCR0,*SCR0,*CCR0,*MFC1,*BFC1,*CSR1,*DCR1;
char *OCR1,*SCR1,*CCR1,*CFG1,*CFG2;
long *BAR1,*MAR0,*MAR1;
short *BTC1,*MTC0,*MTC1;

/* ISR1 bits */
#define DI 0x1       /* Data in */
#define DO 022       /* Data out */
#define ERR 0x4      /* Error */
#define ENDRX 0x10   /* END received */

/* ISR2 bits */
#define CO 0x8       /* Command out */

/* ADSR bits */
#define NATN 0x40    /* Not ATN */

/* ADMR bits */
#define MODE1 0x1    /* Address Mode 1 */
#define TRM 0x30     /* GPIB-1014 functions for T/R2 and T/R3 */

/* ADR bits */
#define DT1 0x40     /* Disable Talker */
#define DL1 0x20      /* Disable Listener */

/* AUXMR Hidden Registers */
#define ICR 0x20     /* Internal Counter Register */
#define PPR 0x60     /* Parallel Poll Register */
#define AUXRA 0x80   /* Auxiliary Register A */
#define AUXRB 0xa0   /* Auxiliary Register B */
#define AUXRE 0xc0   /* Auxiliary Register E */

/* AUXMR Commands */
#define  IEPON 0x0   /* Immediate execute power on */
#define FH 0x3       /* Finish (release) handshake */
#define GTS 0x10     /* Go to standby */
#define TCA 0x11     /* Take control asynchronously */
#define TCS 0x12     /* Take control synchronously */
#define TCSE 0x1a    /* Take control synchronously on END */
#define LTN 0x13     /* Listen */
#define LTNC 0x1b    /* Listen continuously */
#define LUN 0x1c     /* Unlisten */
#define SIFC 0x1e    /* Set IFC */
#define CIFC 0x16    /* Clear IFC */
#define SREN 0x1f    /* Set REN */
#define CREN 0x17    /* Clear REN */
#define CRST 0x2     /* Chip Reset */

/* OCR Bits */
#define GTM 0x82     /* Transfer from GPIB to memory */
#define MTG 0x2      /* Transfer from memory to GPIB */
#define ACHN 0x8     /* Array Chaining */

/* SCR Bits */
#define MCU 0x4      /* Memory address counts up */

/* CCR Bits */
#define GO 0x80      /* Start DMA channel */
#define STOP 0x10    /* Stop DMA channel */

/* CSR Bits */
#define CLRS 0xff    /* Clear all status bits */
#define COC 0x80     /* Channel operations complete */
#define CERR 0x10    /* Error in channel operation */
#define ACT 0x8      /* Channel active */
#define PCT 0x2      /* PCL transition occurred */

/* CFG1 Bits */
#define ECC 0x4      /* Enable Cary Cycle feature */
#define IN 0x1       /* Accepting data from GPIB */
#define OUT 0x0      /* Sending data to GPIB */

/* CFG2 Bits */
#define SLMR 0xa     /* Set Local Master Reset bit */
#define CLMR 0x8     /* Clear Local Master Reset bit */

/* GPIB Commands */
#define TCT 0x9      /* Take Control */
#define UNL 0x3f     /* Universal unlisten */
#define UNT 0x5f     /* Universal untalk */

/* IMR2 Bits */
#define DMAI 0x10
#define ENDRX 0x10

/* User Specified Constants */
#define SEL0 0x0     /* Select ADR0 */
#define SEL1 0x80    /* Select ADR1 */
#define MA 0x0       /* GPIB address of GPIB-1014 */
#define SC 0x9       /* System Controller (set to 010 if not Sys. Con.) */
#define ROR 0x2      /* Release on Request feature (set to 0 if not used) */
#define TMODE 0xa0   /* Cycle Steal DMA transfer mode 
                         (set to 340 if Cycle Steal with Hold is desired) */
#define BRG 0x18     /* Bus Request BR3* selected, other options are:
                           (BR2*=020,BR1=010,BR0=000) */
#define ADMC 0x2     /* Address Modifiers set to 24 bit, user access of
                            data area */
			    
char cmdbuf[100];    /* command buffer for interface messages */
int cmdct = 0;       /* number of commands to be sent */
char datbuf[20000];    /* data buffer for device dependent messages */
int count = 0;       /* current number of commands transfered */
int datct = 0;       /* number of data bytes to be sent */
int sre = 0;         /* REN flag (zero to not set REN, non-zero to set REN) */
char cic;            /* Controller In Charge flag (non-zero if CIC) */
char HLDA = 0x81;      /* holdoff on all command AUXMR */
char SEOI = 0x6;       /* Send END command to AUXMR */
/* Read-only register mnemonics corresponding to write-only registers */
time_t usec = 1;
char b8;

main()
 { int i;
   printf("calling setregs\n");
   setregs();
   printf("calling init\n");
   init();
   printf("calling clearicf\n");
   clearicf();			     				    
   sre = 1;
   printf("calling renset\n");
   renset();
   printf("calling read\n");
   read();
   printf("datbuf=%c%c%c%c%c%c%c%c%c%c\n",
      datbuf[0],datbuf[1],datbuf[2],datbuf[3],datbuf[4],
      datbuf[5],datbuf[6],datbuf[7],datbuf[8],datbuf[9]);
    printf("exit count=%d  cmdct=%d\n",count,cmdct);   
}

/* ------------------------- setup registers ------------------------ */
setregs()
  { BASE = 0xdffe2000;
    (char *)DIR = (char *) (BASE | 0x111);
    (char *)CDOR = (char *) (BASE | 0x111);
    (char *)ISR1 = (char *) (BASE | 0x113);
    (char *)IMR1 = (char *) (BASE | 0x113);
    (char *)ISR2 = (char *) (BASE | 0x115);
    (char *)IMR2 = (char *) (BASE | 0x115);
    (char *)SPSR = (char *) (BASE | 0x117);
    (char *)SPMR = (char *) (BASE | 0x117);
    (char *)ADSR = (char *) (BASE | 0x119);
    (char *)ADMR = (char *) (BASE | 0x119);
    (char *)CPTR = (char *) (BASE | 0x11b);
    (char *)AUXMR = (char *) (BASE | 0x11b);
    (char *)ADR0 = (char *) (BASE | 0x11d);
    (char *)ADR = (char *) (BASE | 0x11d);
    (char *)ADR1 = (char *) (BASE | 0x11f);
    (char *)EOSR = (char *) (BASE | 0x11f);
    (short *)MTC0 = (short *) (BASE | 0x00a);
    (long *)MAR0 = (long *) (BASE | 0x00c);
    (char *)MFC0 = (char *) (BASE | 0x029);
    (char *)CSR0 = (char *) (BASE | 0x000);
    (char *)DCR0 = (char *) (BASE | 0x004);
    (char *)OCR0 = (char *) (BASE | 0x005);
    (char *)SCR0 = (char *) (BASE | 0x006);
    (char *)CCR0 = (char *) (BASE | 0x007);
    (short *)MTC1 = (short *) (BASE | 0x04a);
    (long *)MAR1 = (long *) (BASE | 0x04c);
    (char *)MFC1 = (char *) (BASE | 0x069);
    (short *)BTC1 = (short *) (BASE | 0x05a);
    (long *)BAR1 = (long *) (BASE | 0x05c);
    (char *)BFC1 = (char *) (BASE | 0x079);
    (char *)CSR1 = (char *) (BASE | 0x040);
    (char *)DCR1 = (char *) (BASE | 0x044);
    (char *)OCR1 = (char *) (BASE | 0x045);
    (char *)SCR1 = (char *) (BASE | 0x046);
    (char *)CCR1 = (char *) (BASE | 0x047);
    (char *)CFG1 = (char *) (BASE | 0x101);
    (char *)CFG2 = (char *) (BASE | 0x105);
  }

/* -------------------- initialize ------------------------ */
init()
 { *CFG2 = SLMR;   /* set local master reset bit */
   usec = 1000;
   usleep(usec);
   *CFG2 = CLMR;     /* clear local master reset bit */
   usleep(usec);
   *AUXMR = CRST;    /* Chip reset */
   *IMR1 = 0;        /* disable TLC interrupts */
   *IMR2 = 0;
   b8 = *ISR1;       /* Clear status bits by reading registers */
   b8 = *ISR2;
   *ADMR = (MODE1 | TRM);  /* set address mode, Talker/Listener inactive
                                    and proper T/R signal mode */
   *ADR = (MA | SEL0);  /* Set GPIB address (mode1 primary only),
                              with Talker/Listener enabled */
   *ADR = (DT1 | DL1 | SEL1);  /* Disable secondary address recognition */
   *AUXMR = (ICR | 8);   /* set clock divider for * Mhz, low speed */
   *MFC0 = ADMC;        /* set DMA address modifier codes */
   *MFC1 = ADMC;
   *BFC1 = ADMC;
   *CFG2 = SC;          /* by default be system controller */
   *AUXMR = IEPON;      /* execute pon to bring TLC online */
  }

/* --------------------- INTERFACE CLEAR - IFC ----------------- */
clearicf()			     				    
  {*AUXMR = SIFC;
   usec = 200;
   usleep(usec);
   *AUXMR = CIFC;
  }

/* ---------------------- control ren -------------------------- */
renset()
  { if(sre == 1)
      { *AUXMR = SREN;
        sre = 0;
	return(0); }
    if(sre == 0)
      { *AUXMR = CREN;
        sre = 1;
	return(0); }
  }
rcv()
  { int ipass;
    *DCR0 = TMODE;   /* Set DMA transfer mode */
    *DCR1 = TMODE;  

    *OCR0 = GTM;     /* Set control registers */
    *SCR0 = MCU;

    *CSR0 = 0Xff;    /* Clear status registers */
    *CSR1 = 0xff;
    
    *MAR0 = (long)&datbuf[0];
    
/* transfer with no carry cycle */
    *CFG2 = (BRG | IN);   /* no carry cycle */
    *AUXMR = AUXRA;   /* clear any HLDE or HLDA in effect */
    *MTC0 = 20000;
    *CCR0 = GO;       /* start channel 0 */
    *IMR2 = DMAI;     /* Enable DMAs from the DIR */
    *AUXMR = FH;      /* Release any handshake holdoff in progress */
printf("before rcv3\n");
ipass=0;
rcv3:    /* loop waiting for ENDRX or PCT (DMA done) */
    if(ipass<20) ipass++;
    b8 = *ISR1;
    if(ipass<20) printf("ISR1 = %x\n",(int)b8);
    if((ENDRX & b8) != 0) goto rcv4;
    b8 = *CSR1;
    if(ipass<20) printf("CSR1 = %x\n",(int)b8);
    if((PCT & b8) == 0) goto rcv3;
printf("before rcv4 b8=%x\n",(int)b8);
rcv4:
    *IMR2 = 0;  /* disable DMAs and stop DMA channels */
    *CCR1 = STOP;
    *CCR0 = STOP;
  }
read()
  { cmdbuf[0] = UNT;
    cmdbuf[1] = UNL;
    cmdbuf[2] = 0x20;   /* controller listen */
    cmdbuf[3] = 0x41;   /* scope talker */ 
    cmdct = 4;
    cmd();
    *AUXMR = LTN;
    *AUXMR = GTS;
    printf("calling rcv\n");    
    rcv();
    printf("returned from rcv\n");
    *AUXMR = TCS;
read1:
    b8 = *ADSR;
    if((b8 & NATN) != 0) goto read1;
    cmdct = cmdct-1;
    cmd();
    *AUXMR = LUN;
  }
cmd()
  { *AUXMR = TCA;
    count = 0;
    printf("cmd entry count=%d  cmdct=%d\n",count,cmdct);   
csend1:
    b8 = *ISR2;
    if((b8 & CO) == 0) goto csend1;
    if(count >= cmdct) goto exit;
    *CDOR = cmdbuf[count];
    count++;
    b8 = *ISR1;
    if((b8 & ERR) != 0) goto csend2;
    goto csend1;
csend2:
     printf(" error in cmd\n");
     return(0);
exit:
    printf("cmd exit count=%d  cmdct=%d\n",count,cmdct);   
  }
