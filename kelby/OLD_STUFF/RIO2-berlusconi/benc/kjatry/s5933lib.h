#ifndef _s5933lib_h_
#define _s5933lib_h_
/*---------------------------------------------------------------------------*/
/* s5933lib.h -- AMCC S5933 PCI Controller library                           */
/*                                                                           */
/* AMCC S5933 PCI Operation Registers definitions                            */
/*                                                                           */
/* D. Francis                                                                */
/* P. Kapinos  6/27/96                                                       */
/*                                                                           */
/* Note: Byte ordering takes into account that CPU is big endian & PCI is    */
/*       little endian.                                                      */
/*---------------------------------------------------------------------------*/
#include "atdaq_types.h"

typedef union {       /* Mailbox Register */
  byte        bytes[4];
  dword       all;
} s5933_mbox; 

typedef union {    /* Mailbox Empty/Full Status Register (MBEF) */
  struct {            /* status bits mapping to bytes within each mailbox */ 
    byte  m2b3  : 1;     /* mailbox 2 byte 3 */
    byte  m2b2  : 1;     /* mailbox 2 byte 2 */
    byte  m2b1  : 1;     /* mailbox 2 byte 1 */
    byte  m2b0  : 1;     /* mailbox 2 byte 0 */
    byte  m1b3  : 1;     /* mailbox 1 byte 3 */
    byte  m1b2  : 1;     /* mailbox 1 byte 2 */
    byte  m1b1  : 1;     /* mailbox 1 byte 1 */
    byte  m1b0  : 1;     /* mailbox 1 byte 0 */

    byte  m4b3  : 1;     /* mailbox 4 byte 3 */
    byte  m4b2  : 1;     /* mailbox 4 byte 2 */
    byte  m4b1  : 1;     /* mailbox 4 byte 1 */
    byte  m4b0  : 1;     /* mailbox 4 byte 0 */
    byte  m3b3  : 1;     /* mailbox 3 byte 3 */
    byte  m3b2  : 1;     /* mailbox 3 byte 2 */
    byte  m3b1  : 1;     /* mailbox 3 byte 1 */
    byte  m3b0  : 1;     /* mailbox 3 byte 0 */
  } bit;
  word all; 
} s5933_mbef_status; 

typedef struct {                /* Mailbox Empty/Full Status Register (MBEF) */
  const s5933_mbef_status  out;    /* Outgoing Mailbox Status (RO) */
  const s5933_mbef_status  in;     /* Incoming Mailbox Status (RO) */
} s5933_mbef; 

typedef struct {       /* Interrupt Control/Status Register (INTCSR) */
                          /* Actual Interrupt */
  const byte inta : 1;       /* Interrupt Asserted (RO) */
  byte            : 1;       
  byte     tabort : 1;       /* Target Abort (R/WC) */
  byte     mabort : 1;       /* Master Abort (R/WC) */
  byte     rtc    : 1;       /* Read Transfer Complete (R/WC) */
  byte     wtc    : 1;       /* Write Transfer Complete (R/WC) */
  byte     in     : 1;       /* Incoming Mailbox Interrupt (R/WC) */
  byte     out    : 1;       /* Outcoming Mailbox Interrupt (R/WC) */
} s5933_intstatus;

typedef struct {
  unsigned char     ouf    : 1;
  unsigned char     inf    : 1;
  unsigned char     faca   : 2;
  unsigned char     facp   : 2;
  unsigned char     ec     : 2;
} s5933_intendian;

typedef struct {       /* Interrupt Control/Status Register (INTCSR) */
                          /* Interrupt Source, Enable & Selection */
    byte             : 3;    
    byte     outien  : 1;    /* Outgoing Mailbox Goes Empty Interrupt Enable */
    byte     outmbox : 2;    /* Outgoing Mailbox# */
    byte     outbyte : 2;    /* Outgoing Mailbox Byte# */

    byte     rtcien  : 1;    /* Interrupt on Read Transfer Complete */
    byte     wtcien  : 1;    /* Interrupt on Write Transfer Complete */
    byte             : 1;
    byte     inien   : 1;    /* Incoming Mailbox Becomes Full Interrupt Enable */
    byte     inmbox  : 2;    /* Incoming Mailbox# */
    byte     inbyte  : 2;    /* Incoming Mailbox Byte# */
} s5933_intselect;

typedef union {            /* Interrupt Control/Status Register (INTCSR) */
  struct {
    s5933_intselect select; 
    s5933_intstatus status;
    s5933_intendian endian;
  } bit; 
  dword all;
} s5933_intcsr;

typedef union {            /* Bus Master Control/Status Register (MCSR) */
  struct {                    /*----------------- Status -----------------*/
    const byte  a2ptc  : 1;   /* Add-on to PCI Transfer Count Equals Zero */
    const byte  p2atc  : 1;   /* PCI to Add-on Transfer Count Equals Zero */
    const byte  a2pfe  : 1;   /* Add-on to PCI FIFO Empty                 */
    const byte  a2pf4  : 1;   /* Add-on to PCI 4+ words                   */
    const byte  a2pff  : 1;   /* Add-on to PCI FIFO Full                  */
    const byte  p2afe  : 1;   /* PCI to Add-on FIFO Empty                 */
    const byte  p2af4  : 1;   /* PCI to Add-on 4+ space                   */
    const byte  p2aff  : 1;   /* PCI to Add-on FIFO Full                  */

                              /*----------------- Control ----------------*/
    byte          : 1;        /* Reserved. Always zero                    */
    byte  rte     : 1;        /* Read Transfer Enable                     */
    byte  rms     : 1;        /* Read FIFO management scheme              */
    byte  rvw     : 1;        /* Read versus Write priority               */
    byte          : 1;        /* Reserved. Always zero                    */
    byte  wte     : 1;        /* Write Transfer Enable                    */
    byte  wms     : 1;        /* Write FIFO management scheme             */
    byte  wvr     : 1;        /* Write versus Read priority               */

    byte  nvop;               /* Non-volatile memory address/data port    */
    
    byte  nvctl   : 3;        /* nvRAM Access Control                     */
    byte          : 1;        /* Reserved. Always zero                    */
    byte  mfr     : 1;        /* Mailbox Flag Reset                       */
    byte  a2pfr   : 1;        /* Add-on to PCI FIFO Status Reset          */
    byte  p2afr   : 1;        /* PCI to Add-on FIFO Status Reset          */
    byte  aor     : 1;        /* Add-on pin Reset                         */
  } bit;
  dword all;
} s5933_mcsr;

typedef struct {           /* S5933 PCI bus Operation Registers */
  s5933_mbox  omb[4];         /* Outgoing Mailbox Registers           (OMB) */
  const s5933_mbox  imb[4];   /* Incoming Mailbox Registers           (IMB) */
  dword         fifo;         /* bidirectional FIFO Register Port     (FIFO) */
  dword         mwar;         /* Master Write Address Register        (MWAR) */
  dword         mwtc;         /* Master Write Transfer Count Register (MWTC) */
  dword         mrar;         /* Master Read Address Register         (MRAR) */
  dword         mrtc;         /* Master Read Transfer Count Register  (MRTC) */
  s5933_mbef    mbef;         /* Mailbox Empty/Full Status Register   (MBEF) */
  s5933_intcsr  intcsr;       /* Interrupt Control/Status Register  (INTCSR) */
  s5933_mcsr    mcsr;         /* Bus Master Control/Status Register   (MCSR) */
} s5933_regs; 

enum s5933_returned_code {  /* function returned codes     */
  S5933_SUCCESSFUL = 0,       /* successful                */
  S5933_NOT_SUCCESSFUL        /* anything else             */
};


/* function prototypes */
int  S5933_Open          (s5933_regs *); 
int  S5933_Close         (s5933_regs *);
int  S5933_Reset         (s5933_regs *); 
int  S5933_Dump          (s5933_regs *); 
int  S5933_Write_Mailbox (s5933_regs *, int mbox, int mbyte, dword data);
int  S5933_Read_Mailbox  (s5933_regs *, int mbox, int byte,  dword *data);
int  S5933_Read_FIFO     (s5933_regs *, register dword *dest, int nwords);
int  S5933_Write_FIFO    (s5933_regs *, register dword *src, int nwords);
int  S5933_DMA_Read      (s5933_regs *, dword addr, int size); 
int  S5933_DMA_Write     (s5933_regs *, dword addr, int size);
int  S5933_DMA_Read_Buffer  (s5933_regs *, void *base, int size); 
int  S5933_DMA_Write_Buffer (s5933_regs *, void *base, int size);
int  S5933_Wait_Endof_DMA_Read  (s5933_regs *, int timeout);
int  S5933_Wait_Endof_DMA_Write (s5933_regs *, int timeout);

#endif /*_s5933lib_h_*/
