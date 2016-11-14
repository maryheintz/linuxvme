#ifndef _pcilib_h_
#define _pcilib_h_ 
/*****************************************************************************/
/*                                                                           */
/* File             : pcilib.c                                               */
/* Version          : 1.00                                                   */
/* Author           : D.j. Francis & P. Kapinos                              */
/*                                                                           */
/* Last modified by : DjF                                                    */
/* Date modified    : 21-02-97                                               */
/* Reason           : Do away with dword, byte                               */
/*                    include P2P csh typedefs                               */
/*                                                                           */
/************ Copyright Forty Acres and a Mule Limited 1996 ******************/

typedef union {
  struct {                       /*-------- PCI Command Register ------*/
    char  waitcycle     : 1;     /* Wait Cycle Enable                  */
    char  parerr        : 1;     /* Parity Error Enable                */
    char  paletsnoop    : 1;     /* Palette Snoop Enable               */
    char  memwrtinv     : 1;     /* Memory Write and Invalidate Enable */
    char  specycle      : 1;     /* Special Cycle Enable               */
    char  busmaster     : 1;     /* Bus Master Enable                  */
    char  memacc        : 1;     /* Memory Access Enable               */
    char  ioacc         : 1;     /* I/O Access Enable                  */
    char                : 6;       
    char  fastbben      : 1;     /* Fast Back-to-Back Enable           */
    char  syserr        : 1;     /* System Error Enable                */

                                 /*-------- PCI Status Register -------*/
    char  fastbb        : 1;     /* Fast Back-to-Back Capable          */
    char                : 7;
    char  detectparerr  : 1;     /* Detected Parity Error              */
    char  signsyserr    : 1;     /* Signaled System Error              */
    char  recvmastabort : 1;     /* Received Master Abort              */
    char  recvtargabort : 1;     /* Received Target Abort              */
    char  signtargabort : 1;     /* Signaled Target Abort              */
    char  devseltim     : 2;     /* Device Select Timing               */
    char  datparrep     : 1;     /* Data Parity Reported               */
  } bit;
  unsigned int all; 
} stscmd_t;

typedef struct {
  const unsigned int vdid;       /* Vendor and Device ID               */
  stscmd_t           stscmd;     /* Command and Status Register        */
  const unsigned int clcrev;     /* Class Code and Rev ID              */
  char               caln;       /* Cache Line Size                    */
  char               lat;        /* Master Latency Timer               */
  const char         hdr;        /* Header Type                        */
  char               bist;       /* Built-in Self-test                 */
} field0;
  
typedef struct {
  unsigned int    badr[6];       /* Base Address Registers             */
  unsigned int    cardbus;       /* Cardbus CIS pointer                */
  unsigned int    ssvid;         /* Subsystem and Subsystem Vendor ID  */
  unsigned int    exrom;         /* Expansion ROM Base Address         */
  unsigned int    reserved[2];   /* Reserved                           */
  char            intln;         /* Interrupt Line                     */
  const char      intpin;        /* Interrupt Pin                      */
  const char      mingnt;        /* Minimum Grant                      */
  const char      maxlat;        /* Maximum Latency                    */
} header1;
  
typedef struct {
  unsigned int   badr[2];        /* Base Address Registers             */
  char           pbusnum;        /* Primary bus number                 */
  char           sbusnum;        /* Secondary bus number               */
  char           subbusnum;      /* Subordinate bus number             */
  char           seclat;         /* Secondary latency timer            */
  char           iobase;         /* IO base                            */
  char           iolimit;        /* IO Limit                           */
  short          secsts    : 16; /* Secondary status                   */
  short          membase   : 16; /* Memory base                        */
  short          memlimit  : 16; /* Memory Limit                       */
  short          pmembase  : 16; /* Memory base                        */
  short          pmemlimit : 16; /* Memory Limit                       */
  unsigned int   pbaseup;        /* Prefetchable base upper 32 bits    */
  unsigned int   plimitup;       /* Prefetchable limit upper 32 bits   */
  short          iobaseup;       /* IO base upper 16 bits              */ 
  short          iolimitup;      /* IO limit upper 16 bits             */ 
  unsigned int   reserved;       /* Reserved                           */
  unsigned int   exrom;          /* Expansion ROM Base Address         */
  char           intln;          /* Interrupt Line                     */
  const char     intpin;         /* Interrupt Pin                      */
  short          bctrl     : 16; /* Bridge control                     */
} header2;

typedef struct {         /* PCI Bus Type 00 Configuration Space Header */ 
  field0       header;
  union {
    header1    type1;
    header2    type2;
  } field1;
  unsigned int not_used[48];
} pci_csh;

typedef struct {
  const unsigned int vdid;       /* Vendor and Device ID                */
        unsigned int stscmd;     /* Command and Status Register         */
  const unsigned int clcrev;     /* Class Code and Rev ID               */
        unsigned int caln;       /* Cache Line Size           caln      */
                                 /* Master Latency Timer      lat       */
                                 /* Header Type               hdr       */
                                 /* Built-in Self-test        bist      */
  union {
    struct {
      unsigned int badr[6];      /* Base Address Registers              */
      unsigned int cardbus;      /* Cardbus CIS pointer                 */
      unsigned int ssvid;        /* Subsystem and Subsystem Vendor ID   */
      unsigned int exrom;        /* Expansion ROM Base Address          */
      unsigned int reserved[2];  /* Reserved                            */
      unsigned int intln;        /* Interrupt Line            intln     */
                                 /* Interrupt Pin             intpin    */
                                 /* Minimum Grant             mingnt    */
                                 /* Maximum Latency           maxlat    */
    } type0;
    struct {
      unsigned int badr[2];      /* Base Address Registers              */
      unsigned int pbusnum;      /* Primary bus#                        */
                                 /* Secondary bus #                     */
                                 /* Subordinate bus #                   */
                                 /* Secondary latency timer             */
      unsigned int iobase;       /* IO base                             */
                                 /* IO Limit                            */
                                 /* Secondary status                    */
      unsigned int membase;      /* Memory Base & Limit                 */
      unsigned int pmembase;     /* Preetech memory base                */
      unsigned int pbaseup;      /* Prefetchable base upper 32 bits     */
      unsigned int plimitup;     /* Prefetchable limit upper 32 bits    */
      unsigned int iobaseup;     /* IO Base & Limit upper 16 bits       */
      unsigned int reserved;     /* Reserved                            */
      unsigned int exrom;        /* Expansion ROM Base Address          */
      unsigned int intln;        /* Interrupt Line                      */
                                 /* Interrupt Pin                       */
                                 /* Bridge Control                      */
    } type1; 
  } field1; 
  unsigned int not_used[48];
} pci_csh_t;

enum pci_csh_field {     /* fields of the PCI Configuration Space      */
  pci_csh_vid,                   /* Vendor Identification              */
  pci_csh_did,                   /* Device Identification              */
  pci_csh_vdid,                  /* Vendor and Device Identification   */
  pci_csh_cmd,                   /* PCI Command Register               */
  pci_csh_sts,                   /* PCI Status Register                */
  pci_csh_rid,                   /* Revision Identification Register   */
  pci_csh_clcd,                  /* Class Code Register                */
  pci_csh_caln,                  /* Cache Line Size Register           */
  pci_csh_lat,                   /* Master Latency Timer               */
  pci_csh_hdr,                   /* Header Type                        */
  pci_csh_bist,                  /* Built-in Self-test                 */
  pci_csh_badr0,                 /* Base Address Register 0            */
  pci_csh_badr1,                 /* Base Address Register 1            */
  pci_csh_badr2,                 /* Base Address Register 2            */
  pci_csh_badr3,                 /* Base Address Register 3            */
  pci_csh_badr4,                 /* Base Address Register 4            */
  pci_csh_badr5,                 /* Base Address Register 5            */
  pci_csh_cbus,                  /* Cardbus CIS Pointer                */
  pci_csh_ssvid,                 /* Subsystem Vendor Identification    */
  pci_csh_ssid,                  /* Subsystem Identification           */
  pci_csh_exrom,                 /* Expansion ROM Base Address         */
  pci_csh_reserved_1,            /* Reserved                           */
  pci_csh_reserved_2,            /* Reserved                           */
  pci_csh_intln,                 /* Interrupt Line                     */
  pci_csh_intpin,                /* Interrupt Pin                      */
  pci_csh_mingnt,                /* Minimum Grant                      */
  pci_csh_maxlat,                /* Maximum Latency                    */

  pci_csh_pbusnum,               /* Primary bus number                 */
  pci_csh_sbusnum,               /* Secondary bus number               */
  pci_csh_subbusnum,             /* Subordinate bus number             */
  pci_csh_seclat,                /* Secondary latency timer            */
  pci_csh_iobase,                /* IO base                            */
  pci_csh_iolimit,               /* IO Limit                           */
  pci_csh_secsts,                /* Secondary status                   */
  pci_csh_membase,               /* Memory base                        */
  pci_csh_memlimit,              /* Memory Limit                       */
  pci_csh_pmembase,              /* Memory base                        */
  pci_csh_pmemlimit,             /* Memory Limit                       */
  pci_csh_pbaseup,               /* Prefetchable base upper 32 bits    */
  pci_csh_plimitup,              /* Prefetchable limit upper 32 bits   */
  pci_csh_iobaseup,              /* IO base upper 16 bits              */ 
  pci_csh_iolimitup,             /* IO limit upper 16 bits             */ 
  pci_csh_bctrl                  /* Bridge control                     */
};

enum pci_cmd_bit {              /* PCI Command Register bits           */
  pci_cmd_ioacc         = 1,     /* I/O Access Enable                  */
  pci_cmd_memacc        = 1<<1,  /* Memory Access Enable               */
  pci_cmd_busmaster     = 1<<2,  /* Bus Master Enable                  */
  pci_cmd_specycle      = 1<<3,  /* Special Cycle Enable               */
  pci_cmd_memwrtinv     = 1<<4,  /* Memory Write and Invalidate Enable */
  pci_cmd_paletsnoop    = 1<<5,  /* Palette Snoop Enable               */
  pci_cmd_parerr        = 1<<6,  /* Parity Error Enable                */
  pci_cmd_waitcycle     = 1<<7,  /* Wait Cycle Enable                  */
  pci_cmd_syserr        = 1<<8,  /* System Error Enable                */
  pci_cmd_fastbben      = 1<<9   /* Fast Back-to-Back Enable           */
};

enum pci_sts_bit {              /* PCI Status Register bits            */
  pci_sts_fastbb        = 1<<7,  /* Fast Back-to-Back Capable          */
  pci_sts_datparrep     = 1<<8,  /* Data Parity Reported               */
  pci_sts_devseltim     = 3<<9,  /* Device Select Timing               */
  pci_sts_signtargabort = 1<<11, /* Signaled Target Abort              */
  pci_sts_recvtargabort = 1<<12, /* Received Target Abort              */
  pci_sts_recvmastabort = 1<<13, /* Received Master Abort              */
  pci_sts_signsyserr    = 1<<14, /* Signaled System Error              */
  pci_sts_detectparerr  = 1<<15  /* Detected Parity Error              */
}; 

enum pci_returned_code {        /* function returned codes             */
  PCI_SUCCESSFUL,                /* successful                         */
  PCI_NOT_SUCCESSFUL             /* anything else                      */
};

/* Library function prototypes */
int  PCI_Open              (void);
int  PCI_Close             (void); 
int  PCI_Attach_Device     (int slot, unsigned int vdid, pci_csh **csh); 
int  PCI_Detach_Device     (int slot); 
int  PCI_Find_Device       (unsigned int vdid, int index, int *slot);
int  PCI_Read_CSH          (int slot, int field, unsigned int *data); 
int  PCI_Write_CSH         (int slot, int field, unsigned int data); 
int  PCI_Dump_CSH          (int slot); 
int  PCI_Alloc_Space       (int slot, int badr, void **base);
int  PCI_Alloc_Space_Type  (int slot, int badr, int type, void **base);

#endif /* _pcilib_h_ */
