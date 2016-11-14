/******************************************************************************
*
* Filename: 	vme_api.h
* 
* Description:	Header file for VME application programmers interface
*
* $Revision: 1.4 $
*
* $Date: 2006/08/24 09:03:11 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme/vme_api.h,v $
*
* Copyright 2000-2005 Concurrent Technologies.
*
******************************************************************************/

#ifndef __INCvme_api
#define __INCvme_api

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/* Universe II Register Offsets */

#define PCI_ID		0x0000
#define PCI_CSR		0x0004
#define PCI_CLASS	0x0008
#define PCI_MISC0	0x000C
#define PCI_BS0		0x0010
#define PCI_BS1		0x0014
#define PCI_MISC1	0x003C

#define LSI0_CTL	0x0100
#define LSI0_BS		0x0104
#define LSI0_BD		0x0108
#define LSI0_TO		0x010C

#define LSI1_CTL	0x0114
#define LSI1_BS		0x0118
#define LSI1_BD		0x011C
#define LSI1_TO		0x0120

#define LSI2_CTL	0x0128
#define LSI2_BS		0x012C
#define LSI2_BD		0x0130
#define LSI2_TO		0x0134

#define LSI3_CTL	0x013C
#define LSI3_BS		0x0140
#define LSI3_BD		0x0144
#define LSI3_TO		0x0148

#define SCYC_CTL	0x0170
#define SCYC_ADDR	0x0174
#define SCYC_EN		0x0178
#define SCYC_CMP	0x017C
#define SCYC_SWP	0x0180
#define LMISC		0x0184
#define SLSI		0x0188
#define L_CMDERR	0x018C
#define LAERR		0x0190

#define LSI4_CTL	0x01A0
#define LSI4_BS		0x01A4
#define LSI4_BD		0x01A8
#define LSI4_TO		0x01AC

#define LSI5_CTL	0x01B4
#define LSI5_BS		0x01B8
#define LSI5_BD		0x01BC
#define LSI5_TO		0x01C0

#define LSI6_CTL	0x01C8
#define LSI6_BS		0x01CC
#define LSI6_BD		0x01D0
#define LSI6_TO		0x01D4

#define LSI7_CTL	0x01DC
#define LSI7_BS		0x01E0
#define LSI7_BD		0x01E4
#define LSI7_TO		0x01E8

#define DCTL		0x0200
#define DTBC		0x0204
#define DLA			0x0208
#define DVA			0x0210
#define DCPP		0x0218
#define DGCS		0x0220
#define D_LLUE		0x0224

#define LINT_EN		0x0300
#define LINT_STAT	0x0304
#define LINT_MAP0	0x0308
#define LINT_MAP1	0x030C
#define VINT_EN		0x0310
#define VINT_STAT	0x0314
#define VINT_MAP0	0x0318
#define VINT_MAP1	0x031C
#define STATID		0x0320
#define V1_STATID	0x0324
#define V2_STATID	0x0328
#define V3_STATID	0x032C
#define V4_STATID	0x0330
#define V5_STATID	0x0334
#define V6_STATID	0x0338
#define V7_STATID	0x033C

#define LINT_MAP2	0x340
#define VINT_MAP2	0x344
#define MBOX0		0x348
#define MBOX1		0x34C
#define MBOX2		0x350
#define MBOX3		0x354
#define SEMA0		0x358
#define SEMA1		0x35C

#define MAST_CTL	0x0400
#define MISC_CTL	0x0404
#define MISC_STAT	0x0408
#define USER_AM		0x040C

#define VSI0_CTL	0x0F00
#define VSI0_BS		0x0F04
#define VSI0_BD		0x0F08
#define VSI0_TO		0x0F0C

#define VSI1_CTL	0x0F14
#define VSI1_BS		0x0F18
#define VSI1_BD		0x0F1C
#define VSI1_TO		0x0F20

#define VSI2_CTL	0x0F28
#define VSI2_BS		0x0F2C
#define VSI2_BD		0x0F30
#define VSI2_TO		0x0F34

#define VSI3_CTL	0x0F3C
#define VSI3_BS		0x0F40
#define VSI3_BD		0x0F44
#define VSI3_TO		0x0F48

#define LM_CTL		0xF64
#define LM_BS		0xF68

#define VRAI_CTL	0x0F70
#define VRAI_BS		0x0F74
#define VCSR_CTL	0x0F80
#define VCSR_TO		0x0F84
#define V_AMERR		0x0F88
#define VAERR		0x0F8C

#define VSI4_CTL	0x0F90
#define VSI4_BS		0x0F94
#define VSI4_BD		0x0F98
#define VSI4_TO		0x0F9C

#define VSI5_CTL	0x0FA4
#define VSI5_BS		0x0FA8
#define VSI5_BD		0x0FAC
#define VSI5_TO		0x0FB0

#define VSI6_CTL	0x0FB8
#define VSI6_BS		0x0FBC
#define VSI6_BD		0x0FC0
#define VSI6_TO		0x0FC4

#define VSI7_CTL	0x0FCC
#define VSI7_BS		0x0FD0
#define VSI7_BD		0x0FD4
#define VSI7_TO		0x0FD8

#define VCSR_CLR	0x0FF4
#define VCSR_SET	0x0FF8
#define VCSR_BS		0x0FFC


/* DMA Constants */
#define UNI_DMA_READ	0
#define UNI_DMA_WRITE	1


/* VME bus constants */
#define VME_D8			0
#define VME_D16 		1
#define VME_D32 		2
#define VME_D64 		3
#define VME_A16			0
#define VME_A24 		1
#define VME_A32 		2
#define VME_USER1_AM 	6
#define VME_USER2_AM 	7

#define LSI_DATA		0
#define LSI_PGM			1
#define LSI_USER		0
#define LSI_SUPER		1

#define VSI_DATA		1
#define VSI_PGM			2
#define VSI_USER		1
#define VSI_SUPER		2
#define VSI_BOTH		3


/* Byte Swap constants */
#define SWAP_MASTER	0x08
#define SWAP_SLAVE	0x10
#define SWAP_FAST	0x20
#define SWAP_MASK	(SWAP_MASTER | SWAP_SLAVE | SWAP_FAST)
#define SWAP_PORT	0x210

/* Interrupt Modes */
#define INT_MODE_ROAK 0
#define INT_MODE_RORA 1


/* Ioctl Functions */
enum ioctl_nums
{
	IOCTL_GET_POS = 0,
	IOCTL_ENABLE_INT,
	IOCTL_DISABLE_INT,
	IOCTL_ENABLE_PCI_IMAGE,
	IOCTL_DISABLE_PCI_IMAGE,
	IOCTL_ENABLE_VME_IMAGE,
	IOCTL_DISABLE_VME_IMAGE,
	IOCTL_SET_STATUSID,
	IOCTL_GET_VME_INT_INFO,
	IOCTL_ENABLE_REG_IMAGE,
	IOCTL_DISABLE_REG_IMAGE,
	IOCTL_ENABLE_CSR_IMAGE,
	IOCTL_DISABLE_CSR_IMAGE,
	IOCTL_ENABLE_LM_IMAGE,
	IOCTL_DISABLE_LM_IMAGE,
	IOCTL_ALLOC_DMA_BUFFER,
	IOCTL_FREE_DMA_BUFFER,
	IOCTL_DMA_DIRECT_TXFER,
	IOCTL_DMA_ADD_CMD_PKT,
	IOCTL_DMA_CLEAR_CMD_PKTS,
	IOCTL_DMA_LIST_TXFER,
	IOCTL_WAIT_LINT,
	IOCTL_SET_USER_AM_CODES,
	IOCTL_SET_BYTE_SWAP,
	IOCTL_CLEAR_STATS,
	IOCTL_RESERVE_MEMORY,
	IOCTL_GET_VERR_INFO,
	IOCTL_SET_INT_MODE,
	IOCTL_GET_VME_EXTINT_INFO,
	IOCTL_GENERATE_INT,
};


/* Interrupt Numbers */
enum lint_nums
{
	LINT_VOWN = 0,
	LINT_VIRQ1,
	LINT_VIRQ2,
	LINT_VIRQ3,
	LINT_VIRQ4,
	LINT_VIRQ5,
	LINT_VIRQ6,
	LINT_VIRQ7,
	LINT_DMA,
	LINT_LERR,
	LINT_VERR,
	LINT_RESERVED,
	LINT_SW_IACK,
	LINT_SW_INT,
	LINT_SYSFAIL,
	LINT_ACFAIL,
	LINT_MBOX0,
	LINT_MBOX1,
	LINT_MBOX2,
	LINT_MBOX3,
	LINT_LM0,
	LINT_LM1,
	LINT_LM2,
	LINT_LM3,
};


/* Type Definitions */

#ifndef __KERNEL__
typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned int	UINT32;
typedef char	INT8;
typedef short	INT16;
typedef int		INT32;
#endif


typedef struct 
{
	UINT32 pciAddress;		/* PCI address */
	UINT32 vmeAddress;		/* VME address */
	UINT32 size;			/* image size */
	UINT32 postedWrites;	/* posted writes 0=disable, 1=enable */
	UINT32 dataWidth;		/* data width 0=8 bit, 1=16 bit, 2=32 bit, 3=64 bit */
	UINT32 addrSpace;		/* address space 0=A16, 1=A24, 2=A32, 5=CR/CSR, 6=User1, 7=User2 */
	UINT32 type;			/* type 0=data, 1=program */
	UINT32 mode;			/* mode 0=non-privileged, 1=supervisor */
	UINT32 vmeCycle;		/* VME bus cycle type 0=no BLT's, 1=single BLT's */
	UINT32 pciBusSpace;		/* PCI bus space 0=PCI memory space, 1=PCI I/O space */
	UINT32 ioremap;			/* ioremap  0=no, 1=yes */

} PCI_IMAGE_DATA;


typedef struct 
{
	UINT32 vmeAddress;		/* VME address */
	UINT32 size;			/* image size */
	UINT32 postedWrites;	/* posted writes 0=disable, 1=enable */
	UINT32 prefetchRead;	/* prefetch read 0=disable, 1=enable */
	UINT32 type;			/* type 1=data, 2=program 3=both */
	UINT32 mode;			/* mode 1=non-privileged, 2=supervisor 3=both */
	UINT32 addrSpace;		/* address space 0=A16, 1=A24, 2=A32, 6=User1, 7=User2 */
	UINT32 pciBusLock;		/* PCI bus lock of VME read modify write  0=disable, 1=enable */
	UINT32 pciBusSpace;		/* PCI bus space 0=PCI memory space, 1=PCI I/O space */
	UINT32 ioremap;			/* ioremap  0=no, 1=yes */

} VME_IMAGE_DATA;


typedef struct
{
	UINT8 intNum;		/* VME interrupt number 1 - 7 */
	UINT32 numOfInts;	/* Number of VME interrupts since last call */
	UINT8 vecCount;		/* number of vectors stored in vectors array */
	UINT8 vectors[32]; 	/* array to hold the STATUSID vectors */

} VME_INT_INFO;

typedef struct
{
	UINT8 intNum;		/* VME interrupt number 1 - 7 */
	UINT32 numOfInts;	/* Number of VME interrupts since last call */
	UINT8 vecCount;		/* number of vectors stored in vectors array */
	UINT8 vectors[128]; /* array to hold the STATUSID vectors */

} VME_EXTINT_INFO;


typedef struct
{
	UINT32 intNum;		/* interrupt number selection, bit 31 is used for validation */
						/* bit 0 = VOWN	bit 1 = VIRQ1 bit 2 = VIRQ2 etc. */
	UINT32 timeout;		/* wait timeout in jiffies */

} VME_WAIT_LINT;


typedef struct
{
	UINT32 vmeAddress;		/* VME address, lowest address in range to monitor */
	UINT32 type;			/* type 1=data, 2=program 3=both */
	UINT32 mode;			/* mode 1=non-privileged, 2=supervisor 3=both */
	UINT32 addrSpace;		/* address space 0=A16, 1=A24, 2=A32 */

} VME_IMAGE_ACCESS;


typedef struct
{
	UINT32 user1;			/* User1 address modifier code, values range from 16 - 31 */
	UINT32 user2;			/* User2 address modifier code, values range from 16 - 31 */

} VME_USER_AM;


typedef struct 
{
	UINT32 dataWidth;		/* data width 0=8 bit, 1=16 bit, 2=32 bit, 3=64 bit */
	UINT32 addrSpace;		/* address space 0=A16, 1=A24, 2=A32 */
	UINT32 type;			/* type 0=data, 1=program */
	UINT32 mode;			/* mode 0=non-privileged, 1=supervisor */
	UINT32 vmeCycle;		/* VME bus cycle type 0=no BLT's, 1=single BLT's */

} VME_ACCESS_PARAMS;


typedef struct 
{
	UINT32 ownership;		/* VME bus on/off counters */
	UINT32 timeout;			/* transfer timeout in jiffies */

} VME_TXFER_PARAMS;


typedef struct 
{
	UINT32 direction;			/* 0=read (VME to PCI bus), 1=write (PCI to VME bus) */
	UINT32 vmeAddress;			/* VME address */
	UINT32 offset;				/* offset from start of DMA buffer */
	UINT32 size;				/* size in bytes to transfer */
	VME_TXFER_PARAMS txfer;		/* transfer parameters */
	VME_ACCESS_PARAMS access;	/* access parameters */

} VME_DIRECT_TXFER;


typedef struct 
{
	UINT32 direction;			/* 0=read (VME to PCI bus), 1=write (PCI to VME bus) */
	UINT32 vmeAddress;			/* VME address */
	UINT32 offset;				/* offset from start of DMA buffer */
	UINT32 size;				/* size in bytes to transfer */
	VME_ACCESS_PARAMS access;	/* access parameters */
	
} VME_CMD_DATA;


typedef struct 
{	
	UINT32 physAddress;			/* physical start address */
	UINT32 size;				/* size in bytes */
	
} VME_MEM_ALLOC;


/* Function Prototypes */
#ifndef __KERNEL__

int vme_getApiVersion( char *buffer );

int vme_openDevice( INT8 *deviceName );
int vme_closeDevice( INT32 deviceHandle );

int vme_readRegister( INT32 deviceHandle, UINT16 offset, UINT32 *reg );
int vme_writeRegister( INT32 deviceHandle, UINT16 offset, UINT32 reg );
int vme_setInterruptMode( INT32 deviceHandle, UINT8 mode );
int vme_enableInterrupt( INT32 deviceHandle, UINT8 intNumber );
int vme_disableInterrupt( INT32 deviceHandle, UINT8 intNumber );
int vme_generateInterrupt( INT32 deviceHandle, UINT8 intNumber );
int vme_readInterruptInfo( INT32 deviceHandle, VME_INT_INFO *iPtr );
int vme_readExtInterruptInfo( INT32 deviceHandle, VME_EXTINT_INFO *iPtr );
int vme_setStatusId( INT32 deviceHandle, UINT8 statusId );
int vme_waitInterrupt( INT32 deviceHandle, UINT32 selectedInts, UINT32 timeout, UINT32 *intNum );
int vme_setUserAmCodes( INT32 deviceHandle, VME_USER_AM *amPtr );
int vme_setByteSwap( INT32 deviceHandle, UINT8 enable );
int vme_readVerrInfo( INT32 deviceHandle, UINT32 *Address, UINT8 *Direction, UINT8 *AmCode);

int vme_enableRegAccessImage( INT32 deviceHandle, VME_IMAGE_ACCESS *iPtr );
int vme_disableRegAccessImage( INT32 deviceHandle );
int vme_enableCsrImage( INT32 deviceHandle, UINT8 imageNumber );
int vme_disableCsrImage( INT32 deviceHandle, UINT8 imageNumber );
int vme_enableLocationMon( INT32 deviceHandle, VME_IMAGE_ACCESS *iPtr );
int vme_disableLocationMon( INT32 deviceHandle );
int vme_enablePciImage( INT32 deviceHandle, PCI_IMAGE_DATA *iPtr );
int vme_disablePciImage( INT32 deviceHandle );
int vme_enableVmeImage( INT32 deviceHandle, VME_IMAGE_DATA *iPtr );
int vme_disableVmeImage( INT32 deviceHandle );
int vme_read( INT32 deviceHandle, UINT32 offset, UINT8 *buffer, UINT32 length );
int vme_write( INT32 deviceHandle, UINT32 offset, UINT8 *buffer, UINT32 length );

int vme_mmap( INT32 deviceHandle, UINT32 offset, UINT32 length, UINT32 *userAddress );
int vme_unmap( INT32 deviceHandle, UINT32 userAddress, UINT32 length );

int vme_allocDmaBuffer( INT32 deviceHandle, UINT32 *size );
int vme_freeDmaBuffer( INT32 deviceHandle );

int vme_dmaDirectTransfer( INT32 deviceHandle, VME_DIRECT_TXFER *dPtr );
int vme_dmaListTransfer( INT32 deviceHandle, VME_TXFER_PARAMS *tPtr );
int vme_addDmaCmdPkt( INT32 deviceHandle, VME_CMD_DATA *cmdPtr );
int vme_clearDmaCmdPkts( INT32 deviceHandle );

int vme_clearStats( INT32 deviceHandle );

int vme_reserveMemory( INT32 deviceHandle, UINT32 physicalAddress, UINT32 size );

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif	/* __INCvme_api */
