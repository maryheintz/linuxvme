/******************************************************************************
*
* Filename: 	main.c
* 
* Description:	Linux VME Utility Program.
*
* $Revision: 1.8 $
*
* $Date: 2007/02/09 13:57:45 $
*
* $Source: z:\\cvsroot/Linuxvme2/linuxvme_util/main.c,v $
*
* Copyright 2000 - 2005 Concurrent Technologies.
*
******************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curses.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include <asm/page.h>

#include "main.h"
#include "thread.h"

extern int errno;


/**************************** Local Variables ******************************/

static const char *appTitle 		= "Linux VME Utility Program";
static const char *appVersion		= "Version V2.06-01";
static const char *appCreationDate  = __DATE__ ", " __TIME__;
static const char *appCopyright		= "Copyright 2000-2007 Concurrent Technologies Plc";

static const char *devNotOpen		= "Error device not open";

static int devHandle = -1;
static UINT32 dmaBufferSize = 0;
static const char *procRoot = "/proc/vme";
static char procPath[40];
static char deviceName[20];
static int imageNum = 0;
static int imageNumbers[] = {0,0,0,1,2,3,4,5,6,7,0,1,2,3,4,5,6,7};
static char tmpStr[MAX_STRING];
static char tmpBuf[MAX_BUFFER_SIZE];


/************************** Option String Arrays *****************************
*
* An options value is determined by the index of its string in the array.
* "?" is used for a value that is not used or invalid and is skipped in the
* getStrValue() function.
*
******************************************************************************/

static char *amStrs[] =
{
	"A16",
	"A24",
	"A32",
	"?",	/* not used */
	"?",	/* not used */
	"CR/CSR",
    "User1",
	"User2",
    0,
};

static char *dwStrs[] =
{
	"D8",
	"D16",
	"D32",
	"D64",
    0,
};

static char *dmaModeStrs[] =
{
	"direct",
	"list",
    0,
};

static char *enableStrs[] =
{
	"disable",
	"enable",
    0,
};

static char *yesNoStrs[] =
{
	"no",
	"yes",
    0,
};

static char *directionStrs[] =
{
	"read",
	"write",
    0,
};

static char *lasStrs[] =
{
	"no BLT's",
	"BLT's",
    0,
};

static char *pgmAM1Strs[] =
{
	"data",
	"program",
    0,
};

static char *superAM1Strs[] =
{
	"user",
	"supervisor",
    0,
};

static char *vct1Strs[] =
{
	"memory",
	"I/O",
    0,
};


static char *pgmAM2Strs[] =
{
	"?",	/* not used */
	"data",
	"program",
	"both",
    0,
};

static char *superAM2Strs[] =
{
	"?",	/* not used */
	"user",
	"supervisor",
	"both",
    0,
};

static char *vct2Strs[] =
{
	"memory",
	"I/O",
	"configuration",
    0,
};

static char *vonStrs[] =
{
	"until done",
	"256 bytes",
	"512 bytes",
	"1024 bytes",
	"2048 bytes",
	"4096 bytes",
	"8192 bytes",
	"16384 bytes",
    0,
};

static char *voffStrs[] =
{
	"0 us",
	"16 us",
	"32 us",
	"64 us",
	"128 us",
	"256 us",
	"512 us",
	"1024 us",
	"2 us",
	"4 us",
	"8 us",
    0,
};

static char *intModeStrs[] =
{
	"ROAK",
	"RORA",
    0,
};


/******************************** Menu Items *********************************/

static ENTRYITEM pciImageData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"PCI address  : 0x",'x',32,0x00000000, 0, 0xffffffff, NULL},
	{"VME address  : 0x",'x',32,0x1000000, 0, 0xffffffff, NULL},
	{"Image size   : 0x",'x',32,0x10000, 0, 0x100000, NULL},
	{"Ioremap image: ",'s',8, 1, 0, 1, yesNoStrs},
	{"Data width   : ",'s',8, 2, 0, 3, dwStrs},
	{"Address space: ",'s', 8, 2, 0, 7, amStrs},
	{"Posted writes  : ",'s',8, 1, 0, 1, enableStrs},
	{"Program/data   : ",'s',8, 0, 0, 1, pgmAM1Strs},
	{"Supervisor/user: ",'s',8, 0, 0, 1, superAM1Strs},
	{"VME cycle type : ",'s',8, 0, 0, 1, lasStrs},
	{"PCI bus space  : ",'s',8, 0, 0, 1, vct1Strs},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM vmeImageData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"VME address  : 0x",'x',32,0x2000000, 0, 0xffffffff, NULL},
	{"Image size   : 0x",'x',32,0x10000, 0, 0x100000, NULL},
	{"Posted writes: ",'s',8, 1, 0, 1, enableStrs},
	{"Prefetch read: ",'s',8, 1, 0, 1, enableStrs},
	{"Address space: ",'s', 8, 2, 0, 7, amStrs},
	{"Ioremap image: ",'s',8, 1, 0, 1, yesNoStrs},
	{"Program/data   : ",'s',8, 3, 0, 3, pgmAM2Strs},
	{"Supervisor/user: ",'s',8, 3, 0, 3, superAM2Strs},
	{"PCI bus lock   : ",'s',8, 0, 0, 1, enableStrs},
	{"PCI bus space  : ",'s',8, 0, 0, 1, vct2Strs},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM readImageData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Offset          : 0x",'x',32, 0, 0, 0xfffff, NULL},
	{"Number of bytes : ",'d',32, 256, 0, 0x100000, NULL},
	{"Memory map      : ",'s',8, 0, 0, 1, yesNoStrs},
	{"Number of cycles : ",'d',16, 1, 0, 65535, NULL},
	{"Cycle rate (ms)  : ",'d',16, 100, 50, 10000, NULL},
	{"Number of threads: ",'d',8, 1, 1, 4, NULL},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM writeImageData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Offset         : 0x",'x',32, 0, 0, 0xfffff, NULL},
	{"Number of bytes: ",'d',32, 256, 0, 0x100000, NULL},
	{"Memory map     : ",'s',8, 0, 0, 1, yesNoStrs},
	{"Byte value     : 0x",'x',8, 0, 0, 0xff, NULL},
	{"Number of cycles : ",'d',16, 1, 0, 65535, NULL},
	{"Cycle rate (ms)  : ",'d',16, 100, 50, 10000, NULL},
	{"Number of threads: ",'d',8, 1, 1, 4, NULL},
	{"Use test pattern : ",'s',8, 1, 0, 1, yesNoStrs},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM readRegData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Register offset    : 0x",'x',16, 0, 0, 0xffc, NULL},
	{"Number of registers: ",'d',16, 4, 1, 32, NULL},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM writeRegData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Register offset: 0x",'x',16, 0x348, 0, 0xffc, NULL},
	{"Register value : 0x",'x',32, 0x12345678, 0, 0xffffffff, NULL},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM intsCtrlData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"00 VOWN     : ",'s',8, 0, 0, 1, enableStrs},
	{"01 VIRQ1    : ",'s',8, 0, 0, 1, enableStrs},
	{"02 VIRQ2    : ",'s',8, 0, 0, 1, enableStrs},
	{"03 VIRQ3    : ",'s',8, 0, 0, 1, enableStrs},
	{"04 VIRQ4    : ",'s',8, 0, 0, 1, enableStrs},
	{"05 VIRQ5    : ",'s',8, 0, 0, 1, enableStrs},
	{"06 VIRQ6    : ",'s',8, 0, 0, 1, enableStrs},
	{"07 VIRQ7    : ",'s',8, 0, 0, 1, enableStrs},
	{"08 DMA      : ",'s',8, 0, 0, 1, enableStrs},
	{"09 LERR     : ",'s',8, 0, 0, 1, enableStrs},
	{"10 VERR     : ",'s',8, 0, 0, 1, enableStrs},
	{"11 Reserved : ",'s',8, 0, 0, 0, enableStrs},
	{"12 SW_IACK  : ",'s',8, 0, 0, 1, enableStrs},
	{"13 SW_INT   : ",'s',8, 0, 0, 1, enableStrs},
	{"14 SYSFAIL  : ",'s',8, 0, 0, 1, enableStrs},
	{"15 ACFAIL   : ",'s',8, 0, 0, 1, enableStrs},
	{"16 MBOX0    : ",'s',8, 0, 0, 1, enableStrs},
	{"17 MBOX1    : ",'s',8, 0, 0, 1, enableStrs},
	{"18 MBOX2    : ",'s',8, 0, 0, 1, enableStrs},
	{"19 MBOX3    : ",'s',8, 0, 0, 1, enableStrs},
	{"20 LM0      : ",'s',8, 0, 0, 1, enableStrs},
	{"21 LM1      : ",'s',8, 0, 0, 1, enableStrs},
	{"22 LM2      : ",'s',8, 0, 0, 1, enableStrs},
	{"23 LM3      : ",'s',8, 0, 0, 1, enableStrs},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM generateIntData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Interrupt number: ",'d',8, 1, 1, 7, NULL},
	{"Status ID value : 0x",'x',8, 0xfe, 0, 0xff, NULL},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM waitIntData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"00 VOWN     : ",'s',8, 0, 0, 1, enableStrs},
	{"01 VIRQ1    : ",'s',8, 1, 0, 1, enableStrs},
	{"02 VIRQ2    : ",'s',8, 1, 0, 1, enableStrs},
	{"03 VIRQ3    : ",'s',8, 1, 0, 1, enableStrs},
	{"04 VIRQ4    : ",'s',8, 1, 0, 1, enableStrs},
	{"05 VIRQ5    : ",'s',8, 1, 0, 1, enableStrs},
	{"06 VIRQ6    : ",'s',8, 1, 0, 1, enableStrs},
	{"07 VIRQ7    : ",'s',8, 1, 0, 1, enableStrs},
	{"08 DMA      : ",'s',8, 0, 0, 1, enableStrs},
	{"09 LERR     : ",'s',8, 0, 0, 1, enableStrs},
	{"10 VERR     : ",'s',8, 0, 0, 1, enableStrs},
	{"11 Reserved : ",'s',8, 0, 0, 0, enableStrs},
	{"12 SW_IACK  : ",'s',8, 0, 0, 1, enableStrs},
	{"13 SW_INT   : ",'s',8, 0, 0, 1, enableStrs},
	{"14 SYSFAIL  : ",'s',8, 0, 0, 1, enableStrs},
	{"15 ACFAIL   : ",'s',8, 0, 0, 1, enableStrs},
	{"16 MBOX0    : ",'s',8, 0, 0, 1, enableStrs},
	{"17 MBOX1    : ",'s',8, 0, 0, 1, enableStrs},
	{"18 MBOX2    : ",'s',8, 0, 0, 1, enableStrs},
	{"19 MBOX3    : ",'s',8, 0, 0, 1, enableStrs},
	{"20 LM0      : ",'s',8, 0, 0, 1, enableStrs},
	{"21 LM1      : ",'s',8, 0, 0, 1, enableStrs},
	{"22 LM2      : ",'s',8, 0, 0, 1, enableStrs},
	{"23 LM3      : ",'s',8, 0, 0, 1, enableStrs},
	{"?",'h',0,0,0,0,NULL},
	{"Timeout (jiffies): ",'d',32, 200, 0, 10000, NULL},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM intInfoData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Interrupt number: ",'d',8, 1, 1, 7, NULL},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM regAccessData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Image          : ",'s',8, 1, 0, 1, enableStrs},
	{"VME address    : 0x",'x',32,0x2000000, 0, 0xffffffff, NULL},
	{"Address space  : ",'s', 8, 2, 0, 7, amStrs},
	{"Program/data   : ",'s',8, 3, 0, 3, pgmAM2Strs},
	{"Supervisor/user: ",'s',8, 3, 0, 3, superAM2Strs},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM csrImageData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Image        : ",'s',8, 1, 0, 1, enableStrs},
	{"Image number : ",'d',8, 0, 0, 31, NULL},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM locMonData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Image          : ",'s',8, 1, 0, 1, enableStrs},
	{"VME address    : 0x",'x',32,0x2000000, 0, 0xffffffff, NULL},
	{"Address space  : ",'s', 8, 2, 0, 7, amStrs},
	{"Program/data   : ",'s',8, 3, 0, 3, pgmAM2Strs},
	{"Supervisor/user: ",'s',8, 3, 0, 3, superAM2Strs},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM userAmData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"User1 AM code: ",'d',8, 16, 16, 31, NULL},
	{"User2 AM code: ",'d',8, 16, 16, 31, NULL},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM byteSwapData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Master: ",'s',8, 0, 0, 1, enableStrs},
	{"Slave : ",'s',8, 1, 0, 1, enableStrs},
	{"Fast  : ",'s',8, 0, 0, 1, enableStrs},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM dmaBufferData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Buffer size: 0x",'x',32, 0x10000, 0x1000, 0x1000000, NULL}, /* 4 KB - 2 MB */
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM dmaAccessData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Data width     : ",'s',8, 3, 0, 3, dwStrs},
	{"Address space  : ",'s', 8, 2, 0, 7, amStrs},
	{"Program/data   : ",'s',8, 0, 0, 1, pgmAM1Strs},
	{"Supervisor/user: ",'s',8, 0, 0, 1, superAM1Strs},
	{"VME cycle type : ",'s',8, 1, 0, 1, lasStrs},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM cmdPktData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Number to add  : ",'d',8, 1, 1, 100, NULL},
	{"Direction      : ",'s',8, 0, 0, 1, directionStrs},
	{"Offset         : 0x",'x',32, 0, 0, 0xfffff, NULL},
	{"Number of bytes: ",'d',32, 256, 1, 0x100000, NULL},
	{"VME address    : 0x",'x',32,0x1000000, 0, 0xffffffff, NULL},
	{"Data width     : ",'s',8, 3, 0, 3, dwStrs},
	{"Address space  : ",'s', 8, 2, 0, 7, amStrs},
	{"Program/data   : ",'s',8, 0, 0, 1, pgmAM1Strs},
	{"Supervisor/user: ",'s',8, 0, 0, 1, superAM1Strs},
	{"VME cycle type : ",'s',8, 1, 0, 1, lasStrs},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM readDmaData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Offset         : 0x",'x',32, 0, 0, 0xfffff, NULL},
	{"Number of bytes: ",'d',32, 256, 0, 0x100000, NULL},
	{"Memory map     : ",'s',8, 0, 0, 1, yesNoStrs},
	{"VME address    : 0x",'x',32,0x1000000, 0, 0xffffffff, NULL},
	{"VON            : ",'s',8, 0, 0, 0x7, vonStrs},
	{"VOFF           : ",'s',8, 0, 0, 0xa, voffStrs},
	{"Mode             : ",'s',8, 0, 0, 1, dmaModeStrs},
	{"Create list      : ",'s',8, 0, 0, 1, yesNoStrs},
	{"Number of cycles : ",'d',16, 1, 0, 65535, NULL},
	{"Cycle rate (ms)  : ",'d',16, 100, 50, 10000, NULL},
	{"Number of threads: ",'d',8, 1, 1, 4, NULL},
	{"Timeout (jiffies): ",'d',32, 200, 2, 1000, NULL},
	{"\0",0,0,0,0,0,NULL},
};

static ENTRYITEM writeDmaData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Offset         : 0x",'x',32, 0, 0, 0xfffff, NULL},
	{"Number of bytes: ",'d',32, 256, 0, 0x100000, NULL},
	{"Memory map     : ",'s',8, 0, 0, 1, yesNoStrs},
	{"VME address    : 0x",'x',32,0x1000000, 0, 0xffffffff, NULL},
	{"VON            : ",'s',8, 0, 0, 0x7, vonStrs},
	{"VOFF           : ",'s',8, 0, 0, 0xa, voffStrs},
	{"Byte value     : 0x",'x',8, 0, 0, 0xff, NULL},
	{"Mode             : ",'s',8, 0, 0, 1, dmaModeStrs},
	{"Create list      : ",'s',8, 0, 0, 1, yesNoStrs},
	{"Number of cycles : ",'d',16, 1, 0, 65535, NULL},
	{"Cycle rate (ms)  : ",'d',16, 100, 50, 10000, NULL},
	{"Number of threads: ",'d',8, 1, 1, 4, NULL},
	{"Use test pattern : ",'s',8, 1, 0, 1, yesNoStrs},
	{"Timeout (jiffies): ",'d',32, 200, 2, 1000, NULL},
	{"\0",0,0,0,0,0,NULL},
};


static ENTRYITEM resMemData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Physical address: 0x",'x',32, 0x302000, 0x0, 0xffffffff, NULL},
	{"Block size      : 0x",'x',32, 0x100000, 0x0, 0x1000000, NULL}, /* 4K - 16M */
	{"\0",0,0,0,0,0,NULL},
};


static ENTRYITEM intModeData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Interrupt Mode: ",'s',8, 0, 0, 1, intModeStrs},
	{"\0",0,0,0,0,0,NULL},
};


static ENTRYITEM dmaBufferOffsetData[] =
{
	/* Item parameters: */
	/* name, format, value size, default value, min value, max value, option strings */

	{"Offset: 0x",'x',32, 0x0, 0x0, 0x1000000, NULL}, /* 4 KB - 2 MB */
	{"\0",0,0,0,0,0,NULL},
};



/****************************** Menu Structures *******************************
*
* The first character is the character to return when the
* choice is selected, the remaining text is displayed.
*
******************************************************************************/

static char *interruptCtlMenu[] =
{
	"0Set Interrupt Mode",
	"1Wait For Interrupt",
	"2Enable/Disable Interrupt",
	"3Generate Software Interrupt",
	"4Read VME Interrupt Information",
	"5Read VME Extended Interrupt Information",
	"6Read VME Bus Error Information",
    "qQuit",
    0,
};


static char *imageCtlMenu[] =
{
	"0CR/CSR Image Window",
	"1Location Monitor Window",
	"2Register Access Image Window",
    "qQuit",
    0,
};


static char *devCtlMenu[] =
{
    "0Read Registers",
	"1Write Register",
	"2Set User AM Codes",
	"3Set Hardware Byte Swap",
	"4Interrupt Control",
	"5Enable/Disable Image Windows",
	"6Clear Statistic Counters",
	"7Reserve Memory",
    "qQuit",
    0,
};

static char *imageMenu[] =
{
	"0Enable Image Window",
	"1Disable Image Window",
	"2Read Image Data",
	"3Write Image Data",
    "qQuit",
    0,
};

static char *dmaMenu[] =
{
	"0Allocate Buffer",
    "1Free Buffer",
	"2Configure Access Parameters",
	"3Add Command Packet To List",
	"4Clear Command Packet List",
	"5Read Data Using DMA",
	"6Write Data Using DMA",
	"7View DMA Buffer",
    "qQuit",
    0,
};

static char *procFileMenu[] =
{
	"0Device Status",
    "1Interrupt Status",
	"2LSI Statistics",
	"3VSI Statistics",
	"4DMA Statistics",
	"5PCI Allocation",
#ifdef DEBUG
	"6DMA Command List",
#endif
    "qQuit",
    0,
};

static char *mainMenu[] =
{
	"0Open Device",
	"1Close Device",
    "2Control Device",
	"3PCI Image Windows",
    "4VME Image Windows",
	"5DMA Access",
	"6Driver Information",
	"7About",
    "qQuit",
    0,
};


static char *devFiles[] = 
{
	"/ctl",
	"/lsi0",
	"/lsi1",
	"/lsi2",
	"/lsi3",
	"/lsi4",
	"/lsi5",
	"/lsi6",
	"/lsi7",
	"/dma",
	"/vsi0",
	"/vsi1",
	"/vsi2",
	"/vsi3",
	"/vsi4",
	"/vsi5",
	"/vsi6",
	"/vsi7",
	0,
};


/******************************************************************************
*
* printBuf
*
* Display the given buffer on the screen in Hex and ASCII.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
void printBuf( WINDOW *wPtr, int startrow, UINT32 addr, UINT8 *buf, UINT32 size )
{
	int screenrow;
    int screencol;
	UINT32 i;
	UINT32 j;
	UINT32 n = 0;
	UINT32 offset = addr;


	screenrow = startrow;
    
	while ( (n < size) && ( screenrow < STATUS_LINE) )
	{
		screencol = 2;
		mvwprintw( wPtr, screenrow, screencol, "%08X", offset );
		screencol = 11;
		
		offset += 16;    
		j = n;
        
		for (i = 0; i < 16; i++, j++)
		{
			if ( (n + i) < size )
			{
				mvwprintw( wPtr, screenrow, screencol, "%02X", (int)buf[j] & 0xff );
			}
			else
			{
				mvwprintw( wPtr, screenrow, screencol, ".." );
			}
			screencol += 3;   
		}
        
		j = n;
		for (i = 0; i < 16; i++, j++)    /* print ASCII */
		{
			if ( (n + i) < size )
			{
				mvwprintw( wPtr, screenrow, screencol, "%c", ( (buf[j] > 33) && (buf[j] < 127) ) ? buf[j] : '.' );
			}
			else
			{
				mvwprintw( wPtr, screenrow, screencol, "." );
			}
			screencol++;
		}
		
		n+=16;
		screenrow++;    
    }    
    
}


/******************************************************************************
*
* getReturn
*
* Prompt for and read a carriage return.
* Ignore other characters.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
void getReturn( int column )
{
	int key;


    mvprintw(PROMPT_LINE, column, "%s", "Press Return to continue ");
	clrtoeol();
    refresh();

	do
	{
		key = getch();
	}
	while ( (key != KEY_ENTER) && (key != '\n') && (key != 'q') );

}


/******************************************************************************
*
* getConfirm
*
* Prompt for and read confirmation.
* Read a string and check first character for Y or y.
* On error or other character return no confirmation.
*
*
*  
*
* RETURNS: 1 if confirmed else 0.
*
******************************************************************************/
int getConfirm( int column )
{
    int confirmed = 0;
    int key = 'N';


    mvprintw(PROMPT_LINE, column, "Are you sure (y/n) ?");
    clrtoeol();
    refresh();

    key = getch();
    if ((key == 'Y') || (key == 'y')) 
	{
		confirmed = 1;
    }

    if (confirmed != 1) 
	{
		mvprintw(PROMPT_LINE, column, "Cancelled");
		clrtoeol();
		refresh();
		sleep(1);
    }

    return confirmed;
}


/******************************************************************************
*
* getString
*
* At the current screen position prompt for and read a string
* Delete any trailing newline.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
void getString( char *string )
{
    int len;

    wgetnstr(stdscr, string, MAX_STRING);
    len = strlen(string);
    if (len > 0 && string[len - 1] == '\n')
	{
		string[len - 1] = '\0';
	}
}


/******************************************************************************
*
* clearAllScreen
*
* Clear the screen and re-write the title message
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
void clearAllScreen( const char *message )
{
	int screencol;


    clear();
	screencol = ((MAX_COLUMN / 2) - (strlen( message ) / 2)) - 1;
    mvprintw( 0, screencol, "%s", message );
    refresh();
}


/******************************************************************************
*
* drawItems
*
* Draw the given item strings on the screen and highlight the selected item.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
void drawItems( ENTRYITEM *items, int highlight, int spacing, int itemsPerCol )
{
	int screenrow = START_LINE + 2;
	int screencol = 10;
	int count = 0;
	int current = 0;
	ENTRYITEM *iPtr;
	

	iPtr = items;

	while( *iPtr->name != 0 )
	{	
		if (current == highlight) 
		{
			attrset(A_REVERSE);
		}

		if ( iPtr->format == 'x')
		{
			mvprintw( screenrow, screencol, "%s%X", iPtr->name, iPtr->value );
		}
		else if ( iPtr->format == 'c')
		{
			mvprintw( screenrow, screencol, "%s%c", iPtr->name, (char) (iPtr->value & 0xff) );
		}
		else if ( iPtr->format == 's')
		{
			mvprintw( screenrow, screencol, "%s%s", iPtr->name, iPtr->sPtr[iPtr->value] );
		}
		else
		{
			if ( iPtr->format != 'h')
			{
				mvprintw( screenrow, screencol, "%s%u", iPtr->name, iPtr->value );
			}
		}

		attrset(A_NORMAL);
		clrtoeol();

		screenrow += spacing;
		iPtr++;
		count++;
		current++;

		if ( count >= itemsPerCol )
		{
			screenrow = START_LINE + 2;
			screencol += 30;
			count = 0;
		}
	} 

	refresh();
}


/******************************************************************************
*
* getStrValue
*
* Allows the user to select a value from an array of strings.
* The index of the selected string is returned as the value.
*
*  
*
* RETURNS: value selected.
*
******************************************************************************/
static UINT32 getStrValue( ENTRYITEM *iPtr )
{
	int key = 0;
	UINT32 value;
	int count = 0;


	value = iPtr->value;

	while ( iPtr->sPtr[count] != 0 ) 
	{
		count++;
   	}
/*
	mvprintw(PROMPT_LINE, 10, "Change value with the arrow keys then press Return");
	clrtoeol();
*/
   	while ( (key != KEY_ENTER) && (key != '\n') )
	{
		if ((key == KEY_UP) || (key == KEY_RIGHT)) 
		{
			do
			{
    			if (value == 0)
				{
					value = count - 1;
				}
    			else
				{
					value--;
				}
			}
			while ( strcmp( iPtr->sPtr[value], "?" ) == 0 );
		}

		if ((key == KEY_DOWN) || (key == KEY_LEFT)) 
		{
			do
			{
				if (value == (count - 1))
				{
					value = 0;
				}
				else
				{
					value++;
				}
			}
			while ( strcmp( iPtr->sPtr[value], "?" ) == 0 );
		}

		mvprintw(STATUS_LINE, 10, "%s %s", iPtr->name, iPtr->sPtr[value]);
		clrtoeol();
		mvprintw(PROMPT_LINE, 10, "Change value with the arrow keys then press Return");
		clrtoeol();

		refresh();

		key = getch();
	}

	return value;
}


/******************************************************************************
*
* editItem
*
* Allow user to edit given item.
* Check edited value is within range, display error if not.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void editItem( ENTRYITEM *iPtr )
{
	UINT32 value;


	if ( iPtr->format == 's')
	{
		value = getStrValue( iPtr );
	}
	else
	{
		keypad(stdscr, FALSE);
   		echo();
		curs_set(1); /* turn cursor on */

		mvprintw(PROMPT_LINE, 10, "Enter new value then press Return");
		clrtoeol();

		mvprintw(STATUS_LINE, 10, "%s", iPtr->name);
		clrtoeol();
		getString( tmpStr );

		curs_set(0); /* turn cursor off */
		keypad(stdscr, TRUE);
    	noecho();	

		if ( iPtr->format == 'x')
		{
			value = (UINT32) (strtoul( tmpStr, (char **)NULL, 16));
		}
		else if ( iPtr->format == 'c')
		{
			value = tmpStr[0];
		}
		else
		{
			value = (UINT32) (strtoul( tmpStr, (char **)NULL, 10));
		}
	}

	if ( iPtr->size == 8 )
	{
		value &= 0xff;
	}
	else if ( iPtr->size == 16 )
	{
		value &= 0xffff;
	}
	else
	{
		value &= 0xffffffff;
	}
	
	if ( (value >= iPtr->min) && (value <= iPtr->max) )
	{
		iPtr->value = value;
	}
	else
	{
		mvprintw(STATUS_LINE, 10, "Error value out of range");
		clrtoeol();
		getReturn(10);
	}
}


/******************************************************************************
*
* selectItem
*
* Display given items and allow user selection.
*
*
*  
*
* RETURNS: key value.
*
******************************************************************************/
static int selectItem( ENTRYITEM *items, int spacing, int itemsPerCol )
{
	int numRows = 0;
	int selected_row = 0;
    int key = 0;
	ENTRYITEM *iPtr;
	

	mvprintw( START_LINE, 10, "Parameters:" );
	mvprintw( START_LINE, DEVICE_INDENT, "Device: %s", deviceName);
	
	iPtr = items;
		
    while ( *iPtr->name != 0 ) 
	{
		numRows++;
		iPtr++;
    }

    while ( (key != 'q') && (key != KEY_F(1)) && (key != 'a') ) 
	{
		if (key == KEY_UP) 
		{
	    	if (selected_row == 0)
			{
				selected_row = numRows - 1;
			}
	    	else
			{
				selected_row--;
			}
		}

		if ( numRows > itemsPerCol )
		{
			if (key == KEY_RIGHT) 
			{
	    		if ( selected_row < itemsPerCol )
				{
					if ( (selected_row + itemsPerCol) < numRows )
					{
						selected_row += itemsPerCol;
					}
					else
					{
						selected_row = numRows - 1;
					}
				}
			}

			if (key == KEY_LEFT) 
			{
	    		if ( selected_row >= itemsPerCol )
				{
					selected_row -= itemsPerCol;
				}
			}
		}

		if (key == KEY_DOWN) 
		{
	    	if (selected_row == (numRows - 1))
			{
				selected_row = 0;
			}
	    	else
			{
				selected_row++;
			}
		}

		while ( items[selected_row].format == 'h' )
		{
			if (selected_row == 0)
			{
				selected_row = numRows - 1;
			}
			else if (selected_row == (numRows - 1))
			{
				selected_row = 0;
			}
			else
			{
				selected_row++;
			}
		}

		if ( numRows > 1 )
		{
			drawItems( items, selected_row, spacing, itemsPerCol );

			mvprintw(PROMPT_LINE, 10, "Move highlight then press Return to edit parameter");
			clrtoeol();
		}
		else
		{
			/* single item so don't highlight */
			drawItems( items, -1, spacing, itemsPerCol );

			mvprintw(PROMPT_LINE, 10, "Press Return to edit parameter");
			clrtoeol();
		}

		mvprintw(STATUS_LINE, 10, "Press F1 or 'a' to accept current setting and continue");
		clrtoeol();

		key = getch();

		if ( (key == KEY_ENTER) || (key == '\n') )
		{
			editItem( items + selected_row );
		}
    }

	return key;

}


/******************************************************************************
*
* showRegisters
*
* Display the given number of registers on the screen.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void showRegisters( int offset, int num )
{
	int screenrow = START_LINE;
    int screencol = 10;
	UINT32 *bPtr;
	int i = 0;
	int nextOffset;


	bPtr = (UINT32 *) tmpBuf;
	nextOffset = offset;

	mvprintw( screenrow, screencol, "Offset  Value" );
	screenrow++;
	
	while ( (i < num) && (i <= 32) )
	{
		mvprintw( screenrow, screencol, "0x%03X   0x%08X", nextOffset, *bPtr );
		screenrow++;
		bPtr++;
		nextOffset += 4;
		i++;

		if ( screenrow == 20 )
		{
			screenrow = START_LINE;
			screencol = 40;
			mvprintw( screenrow, screencol, "Offset  Value" );
			screenrow++;
		}
	}
}


/******************************************************************************
*
* showIntInfo
*
* Display the given interrupt information on the screen.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void showIntInfo( int startIndex, UINT8 intNum, UINT32 vecCount, 
							UINT8 numOfInts, UINT8 *vectors )
{
	int screenrow = START_LINE;
    int screencol = 10;
	int i = 0;


	mvprintw(screenrow, 10, "VIRQ: %u", intNum);
	mvprintw(screenrow, 21, "Vector count: %u", vecCount);
	mvprintw(screenrow, 40, "Number: %u", numOfInts);
	screenrow += 2;

	mvprintw( screenrow, screencol, "Index" );
	mvprintw( screenrow, (screencol + 7), "Vector" );
	screenrow++;
	
	while ( i < 32 )
	{
		mvprintw( screenrow, screencol, "%02d", (startIndex + i) );
		mvprintw( screenrow, (screencol + 7), "0x%02X", vectors[i] );
		screenrow++;
		i++;

		if ( i == 16 )
		{
			screenrow = START_LINE + 2;
			screencol = 40;
			mvprintw( screenrow, screencol, "Index" );
			mvprintw( screenrow, (screencol + 7), "Vector" );
			screenrow++;
		}
	}
}


/******************************************************************************
*
* drawMenu
*
* Draw the given memu strings on the screen and highlight the selected row.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void drawMenu( char *options[], int highlight, int itemsPerCol, int spacing )
{
	int screenrow = START_LINE + 2;
	int screencol = 10;
	int count = 0;
	int current = 0;
    char **option_ptr;
    char *txt_ptr;


    option_ptr = options;
	
    while ( *option_ptr ) 
	{

    	txt_ptr = options[current];
		txt_ptr++;

		if (current == highlight) 
		{
			attrset(A_REVERSE);
	    	mvprintw(screenrow, screencol, "%s", txt_ptr);
	    	attrset(A_NORMAL);
		} 
		else 
		{
	    	mvprintw(screenrow, screencol, "%s", txt_ptr);
		}
	
		screenrow++;
		option_ptr++;
		current++;
		count++;

		if ( count >= itemsPerCol )
		{
			screenrow = START_LINE + 2;
			screencol += spacing;
			count = 0;
		}
    }

	mvprintw( PROMPT_LINE, 10, "Move highlight then press Return to select" ); 
    refresh();
}


/******************************************************************************
*
* getChoice
*
* Display given menu choices and allow user selection.
*
*
*  
*
* RETURNS: Selected choice character.
*
******************************************************************************/
static int getChoice( char *title, char *choices[], int *index , 
						int itemsPerCol, int spacing )
{
    int selected_row;
    int numRows = 0;
	int screenrow = START_LINE;
    char **option;
    int selected;
    int key = 0;


	selected_row = *index;

    option = choices;
    while (*option) 
	{
		numRows++;
		option++;
    }

    /* protect against menu getting shortened */
    if (selected_row >= numRows)
	{
		selected_row = 0;
	}

    mvprintw( screenrow, 10, title );
	mvprintw( screenrow, DEVICE_INDENT, "Device: %s", deviceName);
	screenrow += 2;

    while ( (key != 'q') && (key != KEY_ENTER) && (key != '\n')) 
	{
		if (key == KEY_UP) 
		{
	    	if (selected_row == 0)
			{
				selected_row = numRows - 1;
			}
	    	else
			{
				selected_row--;
			}
		}

		if ( numRows > itemsPerCol )
		{
			if (key == KEY_RIGHT) 
			{
	    		if ( selected_row < itemsPerCol )
				{
					if ( (selected_row + itemsPerCol) < numRows )
					{
						selected_row += itemsPerCol;
					}
					else
					{
						selected_row = numRows - 1;
					}
				}
			}

			if (key == KEY_LEFT) 
			{
	    		if ( selected_row >= itemsPerCol )
				{
					selected_row -= itemsPerCol;
				}
			}
		}

		if (key == KEY_DOWN) 
		{
	    	if (selected_row == (numRows - 1))
			{
				selected_row = 0;
			}
	    	else
			{
				selected_row++;
			}
		}

		selected = *choices[selected_row];
		drawMenu( choices, selected_row, itemsPerCol, spacing );
		key = getch();
		
		/* mvprintw(PROMPT_LINE-1, 10, "key: %d selected: %d [%d]", key, selected_row, KEY_DOWN); */
    }

	*index = selected_row;

    if (key == 'q')
	{
		selected = 'q';
	}

    return selected;
}


/******************************************************************************
*
* displayStrBuf
*
* Displays a string buffer.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void displayStrBuf( char *strBuf, int len, int startrow, int startcol )
{
	int count = 0;
	int i = 0;
	int screenrow;


	if ( len > 0 )
	{
		screenrow = startrow;

		/* ignore first newline character */
		if ( strBuf[count] == '\n')
		{
			count++;			
		}

		while ( (count < len) && (screenrow < (STATUS_LINE - 1)) )
		{
			if ( (strBuf[count] != '\n') && ( i < (MAX_STRING - 1)) )
			{
				tmpStr[i] = strBuf[count];
				i++;
				
				/* end of string so print it */
				if ( count == (len - 1) )
				{
					tmpStr[i] = 0;
					mvprintw(screenrow, startcol, "%s", tmpStr);
					screenrow++;
					i = 0;
				}
			}
			else
			{
				tmpStr[i] = 0;
				mvprintw(screenrow, startcol, "%s", tmpStr);
				screenrow++;
				i = 0;
			}

			count++;
		}
	}
	else
	{
		mvprintw(STATUS_LINE, 10, "Error no data available");
	}

}


/******************************************************************************
*
* getProcFile
*
* Read device driver status information via the proc file system.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void getProcFile( char *title, char* filename, int startcol )
{
	int fh;
	int result;


	clearAllScreen( title );

	strcpy( procPath, procRoot);
	strcat( procPath, filename );

	fh = open( procPath, O_RDONLY, 0 );
	if (fh == -1 )
	{
		mvprintw(STATUS_LINE, 10, "Error unable to open %s", procPath);
	}
	else
	{
		result = read( fh, tmpBuf, MAX_BUFFER_SIZE );
		displayStrBuf( tmpBuf, result, START_LINE, startcol );
		
		close( fh );
	}

}


/******************************************************************************
*
* getDeviceStatus
*
* Get device driver status information.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void getDeviceStatus( char *title )
{
	int choice;
	int selected_row = 0;


	clearAllScreen( title );

	choice = getChoice( "Select device:", devFiles, &selected_row, 9, 10 );

	if ( choice != 'q' )
	{
		getProcFile( title, devFiles[selected_row], 10 );
		clrtoeol();
		getReturn( 10 );
	}
}



/******************************************************************************
*
* openDevice
*
* Opens a device file.
* Prompt user to select filename from list.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void openDevice( char *title )
{
	int choice;
	char *cPtr;
	int selected_row = 0;


	clearAllScreen( title );

	choice = getChoice( "Select device:", devFiles, &selected_row, 9, 10 );

	if ( choice != 'q' )
	{
		if ( devHandle != -1 )
		{
			vme_closeDevice( devHandle );
			devHandle = -1;
			strcpy( deviceName, "closed" );
		}

		imageNum = imageNumbers[selected_row];
		cPtr = devFiles[selected_row];
		cPtr++;

		devHandle = vme_openDevice( cPtr );

		if ( devHandle < 0 )
		{
			mvprintw(STATUS_LINE, 10, "Error failed to open %s", cPtr);
		}
		else
		{
			strcpy( deviceName, cPtr );
			mvprintw( START_LINE, DEVICE_INDENT, "Device: %s", deviceName);
			clrtoeol();
			mvprintw(STATUS_LINE, 10, "%s opened", deviceName);
		}
		
		getReturn( 10 );
	}
}


/******************************************************************************
*
* closeDevice
*
* Closes a device file.
* Prompt user for confirmation.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void closeDevice( char *title )
{
	int screenrow = START_LINE;
	

	clearAllScreen( title );

	if ( devHandle != -1 )
	{
		mvprintw( screenrow, 10, "Device: %s", deviceName);
	
		if ( getConfirm( 10 ) ) 
		{
			vme_closeDevice( devHandle );
			devHandle = -1;

			mvprintw(STATUS_LINE, 10, "%s closed", deviceName);
			strcpy( deviceName, "closed" );
		}
	}
	else
	{
		mvprintw(STATUS_LINE, 10, "Error no devices open");
	}

	clrtoeol();
	getReturn( 10 );	
}


/******************************************************************************
*
* readRegisters
*
* Read registers from the device.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void readRegisters( char *title )
{
	int key;
	int num;
	UINT16 offset;
	UINT32 *rPtr;
	int count = 0;
	int result = 0;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &readRegData[0], 2, 2 );

		if ( key != 'q' )
		{
			offset = (UINT16) readRegData[0].value;
			num = readRegData[1].value;				
			
			if ( (num * 4) <= MAX_SIZE )
			{				
				rPtr = (UINT32 *) tmpBuf;

				while ( (count < num) && ( result == 0 ) )
				{
					result = vme_readRegister( devHandle, offset, rPtr );
					count++;
					offset += 4;
					rPtr++;
				}

				if ( result < 0 )
				{
					mvprintw(STATUS_LINE, 10, "Error (%d) read registers failed (%s)", 
								result, strerror(errno));
				}
				else
				{
					clearAllScreen( title );
					showRegisters( readRegData[0].value, count );
				}
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error too many registers entered");
			}

			clrtoeol();
			getReturn( 10 );
    	}
	}
}


/******************************************************************************
*
* writeRegisters
*
* Write to device register.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void writeRegister( char *title )
{
	int screenrow = START_LINE; 
	int num;
	int key;
	UINT16 offset;
	UINT32 reg;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &writeRegData[0], 2, 2 );

		if ( key != 'q' )
		{
			offset = (UINT16) writeRegData[0].value;
			reg = writeRegData[1].value;

			num = vme_writeRegister( devHandle, offset, reg );
				
			if ( num < 0 )
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) write register failed (%s)", 
							num, strerror(errno));
			}
			else
			{
				num = vme_readRegister( devHandle, offset, &reg );

				if ( num < 0 )
				{
					mvprintw(STATUS_LINE, 10, "Error (%d) read registers failed (%s)",
								num, strerror(errno));
					
				}
				else
				{
					clearAllScreen( title );
					mvprintw( screenrow, 10, "Offset  Value" );
					screenrow++;
					mvprintw( screenrow, 10, "0x%03X   0x%08X", offset, reg );
					screenrow++;
				}
			}
			
			clrtoeol();
			getReturn( 10 );
    	}
	}
}


/******************************************************************************
*
* setInterruptMode
*
* Sets device interrupt mode ROAK or RORA.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void setInterruptMode( char *title )
{
	int key;
	int result = 0;
	UINT8 mode;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &intModeData[0], 1, 1 );

		if ( key != 'q' )
		{
			mode = intModeData[0].value;
			
			result = vme_setInterruptMode( devHandle, mode );
			if ( result == 0 )
			{
				mvprintw( STATUS_LINE, 10, "Interrupt mode set");
			}
			else
			{
				mvprintw( STATUS_LINE, 10, "Error (%d) failed to set interrupt mode (%s)", 
							result, strerror(errno) );
			}

			clrtoeol();
			getReturn( 10 );
    	}
	}
}


/******************************************************************************
*
* enableUnvInt
*
* Enables/disables device interrupts.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void enableUnvInt( char *title )
{
	int key;
	int result = 0;
	UINT8 count = 0;
	UINT32 reg;
	ENTRYITEM *iPtr;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		result = vme_readRegister( devHandle, (UINT16) LINT_EN, &reg );

		if ( result  == 0 )
		{
			while ( count < 24 )
			{
				intsCtrlData[count].value = (reg >> count) & 1;
				count++;
			}

			iPtr = &intsCtrlData[0];
			key = selectItem( iPtr, 1, 12 );

			if ( key != 'q' )
			{
				count = 0;

				while( (*iPtr->name != 0) && (count < 24) && (result == 0) )
				{
					if ( iPtr->value != 0 )
					{
						iPtr->value = 1;	/* force incase other value entered */
						result = vme_enableInterrupt( devHandle, count );				
					}
					else
					{
						result = vme_disableInterrupt( devHandle, count );
					}

					iPtr++;
					count++;
				}

				clearAllScreen( title );

				if ( result >= 0 )
				{
					/* read and display LINT_EN register */
					vme_readRegister( devHandle, (UINT16) LINT_EN, &reg );
					mvprintw( START_LINE, 10, "LINT_EN: 0x%08X", reg );
				}
				else
				{
					mvprintw(STATUS_LINE, 10, "Error (%d) failed to set interrupt %u (%s)",
								result, count, strerror(errno));
				}

				clrtoeol();
				getReturn( 10 );
			}
		}
		else
		{
			mvprintw(STATUS_LINE, 10, "Error (%d) failed to read LINT_EN register (%s)",
								result, strerror(errno));
			clrtoeol();
			getReturn( 10 );
		}
	}
}


/******************************************************************************
*
* generateInt
*
* Generates a device interrupt via software.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void generateInt( char *title )
{
	int key;
	int result;
	UINT8 intNum;
	UINT8 statusId;
	UINT32 reg;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &generateIntData[0], 2, 2 );		

		if ( key != 'q' ) 
		{
			intNum = (UINT8) generateIntData[0].value;
			statusId = (UINT8) generateIntData[1].value;

			clearAllScreen( title );

			result = vme_setStatusId( devHandle, statusId );

			if ( result >= 0 )
			{
				result = vme_generateInterrupt( devHandle, intNum );

				if ( result >= 0 )
				{
					mvprintw(START_LINE, 10, "Interrupt %u generated", intNum);

					/* read and display STATUSID register */
					vme_readRegister( devHandle, (UINT16) STATID, &reg );
					mvprintw( START_LINE + 2, 10, "STATID: 0x%08X", reg );
				}
				else
				{
					mvprintw(STATUS_LINE, 10, "Error (%d) failed to generate interrupt %d (%s)",
							result, intNum, strerror(errno));
				}
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) failed to set STATID: 0x%08X (%s)",
							result, statusId, strerror(errno));
			}

			clrtoeol();
			getReturn( 10 );
		}
	}
}


/******************************************************************************
*
* waitForInterrupt
*
* Wait for a Universe II interrupt.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void waitForInterrupt( char *title )
{
	int key;
	UINT8 count = 0;
	TINTDATA tData;

	clearAllScreen( title );
	memset( &tData, 0, sizeof(TINTDATA) );
	
	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &waitIntData[0], 1, 13 );		

		if ( key != 'q' ) 
		{    
			clearAllScreen( "Waiting For Interrupt" );

			tData.devHandle = devHandle;
	
			while ( count < 24 )
			{
				if ( waitIntData[count].value == 1 )
				{
					tData.selectedInts |= 1L << count;
				}
				count++;
			}

			tData.timeout = waitIntData[25].value;

			startWaitInt( &tData );
		}
	}
}


/******************************************************************************
*
* getVmeIntInfo
*
* Gets VME interrupt information from the device driver.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void getVmeIntInfo( char *title )
{
	int key;
	VME_INT_INFO info;
	int result;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &intInfoData[0], 1, 1 );

		if ( key != 'q' ) 
		{
			info.intNum = (UINT8) intInfoData[0].value;

			clearAllScreen( title );

			result = vme_readInterruptInfo( devHandle, &info );
			if ( result >= 0 )
			{
				showIntInfo( 0, info.intNum, info.vecCount, 
							 info.numOfInts, &info.vectors[0] );
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) failed to get interrupt information (%s)",
						 result, strerror(errno));
			}

			clrtoeol();
			getReturn( 10 );
		}
	}
}


/******************************************************************************
*
* getVmeExtIntInfo
*
* Gets VME extended interrupt information from the device driver.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void getVmeExtIntInfo( char *title )
{
	int key;
	int count;
	VME_EXTINT_INFO info;
	int result;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &intInfoData[0], 1, 1 );

		if ( key != 'q' ) 
		{
			info.intNum = (UINT8) intInfoData[0].value;

			clearAllScreen( title );

			result = vme_readExtInterruptInfo( devHandle, &info );
			if ( result >= 0 )
			{
				count = 0;

				do
				{
					showIntInfo( count, info.intNum, info.vecCount, 
						 			info.numOfInts, &info.vectors[count] );

					clrtoeol();
					getReturn( 10 );

					count += 32;
				}
				while( count < info.vecCount );
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) failed to get interrupt information (%s)",
						 result, strerror(errno));

				clrtoeol();
				getReturn( 10 );
			}
		}
	}
}


/******************************************************************************
*
* getVerrInfo
*
* Gets VERR information from the device driver.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void getVerrInfo( char *title )
{
	int result;
	UINT32 vmeAddress;
	UINT8 direction;
	UINT8 amCode;
	int screenrow = START_LINE;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		result = vme_readVerrInfo( devHandle, &vmeAddress, &direction, &amCode);
		if ( result >= 0 )
		{	
			mvprintw( screenrow, 10, "VME address: 0x%08X", vmeAddress );
			screenrow++;
			mvprintw( screenrow, 10, "Direction  : %s", 
						(direction ? "read" : "write") );
			screenrow++;
			mvprintw( screenrow, 10, "AM code    : 0x%02X", amCode );
		}
		else
		{
			mvprintw(STATUS_LINE, 10, "Error (%d) failed to read VERR information (%s)",
					 result, strerror(errno));
		}

		clrtoeol();
		getReturn( 10 );
	}
}


/******************************************************************************
*
* regAccessImage
*
* Enables/disables the register acccess image.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void regAccessImage( char *title )
{
	int key;
	UINT8 enable;
	VME_IMAGE_ACCESS idata;
	UINT32 reg;
	int result;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &regAccessData[0], 2, 5 );

    	if ( key != 'q' ) 
		{
			enable = (UINT8) regAccessData[0].value;
			idata.vmeAddress = regAccessData[1].value;
			idata.addrSpace = regAccessData[2].value;
			idata.type = regAccessData[3].value;
			idata.mode = regAccessData[4].value;

			clearAllScreen( title );

			if ( enable == 1 )
			{
				result = vme_enableRegAccessImage( devHandle, &idata );
			}
			else
			{
				result = vme_disableRegAccessImage( devHandle );
			}

			if ( result >= 0 )
			{
				/* read and display register access image registers */
				vme_readRegister( devHandle, (UINT16) VRAI_CTL, &reg );
				mvprintw( START_LINE + 2, 10, "VRAI_CTL: 0x%08X", reg );
				vme_readRegister( devHandle, (UINT16) VRAI_BS, &reg );
				mvprintw( START_LINE + 4, 10, "VRAI_BS : 0x%08X", reg );

				mvprintw(STATUS_LINE, 10, "Image window %s", 
							(enable ? "enabled" : "disabled") );
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) setting register access image (%s)",
							 result, strerror(errno));
			}

			clrtoeol();
			getReturn( 10 );
    	}
	}
}


/******************************************************************************
*
* csrImage
*
* Enable/disable a Universe II CR/CSR image window.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void csrImage( char *title )
{
	int key;
	UINT32 reg;
	UINT8 enable;
	UINT8 num;
	int result;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &csrImageData[0], 2, 2 );

		if ( key != 'q' ) 
		{
			enable = (UINT8) csrImageData[0].value;
			num = (UINT8) csrImageData[1].value;

			clearAllScreen( title );

			if ( enable == 1 )
			{
				result = vme_enableCsrImage( devHandle, num );
			}
			else
			{
				result = vme_disableCsrImage( devHandle, num );
			}

			if ( result >= 0 )
			{
				/* read and display VCSR registers */
				vme_readRegister( devHandle, (UINT16) VCSR_CTL, &reg );
				mvprintw( START_LINE + 2, 10, "VCSR_CTL: 0x%08X", reg );
				vme_readRegister( devHandle, (UINT16) VCSR_TO, &reg );
				mvprintw( START_LINE + 4, 10, "VCSR_TO : 0x%08X", reg );
				vme_readRegister( devHandle, (UINT16) VCSR_BS, &reg );
				mvprintw( START_LINE + 6, 10, "VCSR_BS : 0x%08X", reg );

				mvprintw(STATUS_LINE, 10, "Image window %s", 
							(enable ? "enabled" : "disabled") );
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) setting CR/CSR image (%s)",
							 result, strerror(errno));
			}

			clrtoeol();
			getReturn( 10 );
		}
	}
}


/******************************************************************************
*
* locationMon
*
* Enables/disables a Location monitor window.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void locationMon( char *title )
{
	int key;
	UINT8 enable;
	VME_IMAGE_ACCESS idata;
	UINT32 reg;
	int result;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &locMonData[0], 2, 5 );

    	if ( key != 'q' ) 
		{
			enable = (UINT8) locMonData[0].value;
			idata.vmeAddress = locMonData[1].value;
			idata.addrSpace = locMonData[2].value;
			idata.type = locMonData[3].value;
			idata.mode = locMonData[4].value;

			clearAllScreen( title );

			if ( enable == 1 )
			{
				result = vme_enableLocationMon( devHandle, &idata );
			}
			else
			{
				result = vme_disableLocationMon( devHandle );
			}

			if ( result >= 0 )
			{
				/* read and display Location Monitor registers */
				vme_readRegister( devHandle, (UINT16) LM_CTL, &reg );
				mvprintw( START_LINE + 2, 10, "LM_CTL: 0x%08X", reg );
				vme_readRegister( devHandle, (UINT16) LM_BS, &reg );
				mvprintw( START_LINE + 4, 10, "LM_BS : 0x%08X", reg );

				mvprintw(STATUS_LINE, 10, "Location monitor %s", 
							(enable ? "enabled" : "disabled") );
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) setting location monitor (%s)",
							 result, strerror(errno));
			}

			clrtoeol();
			getReturn( 10 );
    	}
	}
}


/******************************************************************************
*
* setUserAmCodes
*
* Sets the User address modifier codes.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void setUserAmCodes( char *title )
{
	int key;
	UINT32 reg;
	VME_USER_AM amdata;
	int result;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &userAmData[0], 2, 2 );

		if ( key != 'q' ) 
		{
			amdata.user1 = userAmData[0].value;		
			amdata.user2 = userAmData[1].value;

			clearAllScreen( title );

			result = vme_setUserAmCodes( devHandle, &amdata );

			if ( result >= 0 )
			{
				/* read and display USER_AM register */
				vme_readRegister( devHandle, (UINT16) USER_AM, &reg );
				mvprintw( START_LINE, 10, "USER_AM: 0x%08X", reg );

				mvprintw(STATUS_LINE, 10, "User AM codes set");
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) User AM codes (%s)",
							 result, strerror(errno));
			}

			clrtoeol();
			getReturn( 10 );
		}
	}
}


/******************************************************************************
*
* byteSwap
*
* Enable/disable hardware byte swapping on VP PSE/C1x and VP PSE/P3x boards.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void byteSwap( char *title )
{
	int key;
	UINT8 enable;
	int result;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &byteSwapData[0], 2, 3 );

		if ( key != 'q' ) 
		{
			enable = (UINT8) (byteSwapData[0].value << 3) |
								(byteSwapData[1].value << 4) |
								(byteSwapData[2].value << 5);		

			result = vme_setByteSwap( devHandle, enable );

			if ( result >= 0 )
			{
				mvprintw(STATUS_LINE, 10, "Byte swap %s", 
							(enable ? "enabled" : "disabled") );
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) setting byte swap (%s)",
							 result, strerror(errno));
			}

			clrtoeol();
			getReturn( 10 );
		}
	}
}


/******************************************************************************
*
* clearStats
*
* Clears driver statistic counters.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void clearStats( char *title )
{	
	int result;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
	}
	else
	{	
		if ( getConfirm( 10 ) ) 
		{
			result = vme_clearStats( devHandle );
			if ( result == 0 )
			{
				mvprintw(STATUS_LINE, 10, "Statistic counters cleared");
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) failed to clear statistics (%s)", 
							result, strerror(errno));
			}
		}
	}

	clrtoeol();
	getReturn( 10 );	
}


/******************************************************************************
*
* reserveMemory
*
* Allows a user defined memory area to be reserved for DMA buffer and
* VME window use. The new memeory area will be used as an alternative to the
* reserved memory area configured by the driver when it is loaded.
*
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void reserveMemory( char *title )
{	
	int result;
	UINT32 physAddress;
	UINT32 size;
	int key;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );	
	}
	else
	{	
		key = selectItem( &resMemData[0], 2, 2 );

		if ( key != 'q' )
		{
			physAddress = resMemData[0].value;
			size = resMemData[1].value;
				
			result = vme_reserveMemory( devHandle, physAddress, size );
			if ( result == 0 )
			{
				mvprintw( STATUS_LINE, 10, "Memory reserved, %u bytes available", size);
			}
			else
			{
				mvprintw( STATUS_LINE, 10, "Error (%d) failed to reserve memory (%s)", 
							result, strerror(errno) );
			}

			clrtoeol();
			getReturn( 10 );
    	}
	}
}


/******************************************************************************
*
* enablePciImage
*
* Enables a PCI image window.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void enablePciImage( char *title )
{
	int key;
	int result;
	PCI_IMAGE_DATA idata;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &pciImageData[0], 2, 6 );

    	if ( key != 'q' ) 
		{
			idata.pciAddress = pciImageData[0].value;
			idata.vmeAddress = pciImageData[1].value;
			idata.size = pciImageData[2].value;
			idata.ioremap = pciImageData[3].value;
			idata.dataWidth = pciImageData[4].value;
			idata.addrSpace = pciImageData[5].value;
			idata.postedWrites = pciImageData[6].value;
			idata.type = pciImageData[7].value;
			idata.mode = pciImageData[8].value;
			idata.vmeCycle = pciImageData[9].value;
			idata.pciBusSpace = pciImageData[10].value;

			clearAllScreen( title );

			result = vme_enablePciImage( devHandle, &idata );
				
			if ( result == 0 )
			{
				strcpy( tmpStr, "/" );
				strcat( tmpStr, deviceName );
				getProcFile( title, tmpStr, 10 );

				mvprintw(STATUS_LINE, 10, "Image window enabled");
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) failed to enable PCI image %u (%s)",
								result, imageNum, strerror(errno));
			}

			clrtoeol();
			getReturn( 10 );
    	}
	}
}


/******************************************************************************
*
* enableVmeImage
*
* Enables a VME image window.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void enableVmeImage( char *title )
{
	int key;
	int result;
	VME_IMAGE_DATA idata;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &vmeImageData[0], 2, 6 );

    	if ( key != 'q' ) 
		{
			idata.vmeAddress = vmeImageData[0].value;
			idata.size = vmeImageData[1].value;
			idata.postedWrites = vmeImageData[2].value;
			idata.prefetchRead = vmeImageData[3].value;
			idata.addrSpace = vmeImageData[4].value;
			idata.ioremap = vmeImageData[5].value;
			idata.type = vmeImageData[6].value;
			idata.mode = vmeImageData[7].value;
			idata.pciBusLock = vmeImageData[8].value;
			idata.pciBusSpace = vmeImageData[9].value;	

			clearAllScreen( title );

			result = vme_enableVmeImage( devHandle, &idata );
				
			if ( result == 0 )
			{
				strcpy( tmpStr, "/" );
				strcat( tmpStr, deviceName );
				getProcFile( title, tmpStr, 10 );

				mvprintw(STATUS_LINE, 10, "Image window enabled");
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) failed to enable VME image %u (%s)",
								result, imageNum, strerror(errno));
			}

			clrtoeol();
			getReturn( 10 );
    	}
	}
}


/******************************************************************************
*
* disableImage
*
* Disables a PCI or VME image window.
* Prompt user for confirmation.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void disableImage( char *title )
{
	int result;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		mvprintw( START_LINE, 10, "Image number: %d", imageNum);

    	if ( getConfirm( 10 ) ) 
		{
			clearAllScreen( title );

			if ( strncmp (deviceName, "lsi", 3) == 0 )
			{
				result = vme_disablePciImage( devHandle );
			}
			else if ( strncmp (deviceName, "vsi", 3) == 0 )
			{
				result = vme_disableVmeImage( devHandle );
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error invalid device");
				result = -1;
			}

			if ( result == 0 )
			{
				strcpy( tmpStr, "/" );
				strcat( tmpStr, deviceName );
				getProcFile( title, tmpStr, 10 );

				mvprintw(STATUS_LINE, 10, "Image window disabled");
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) failed to disable image %u (%s)",
								result, imageNum, strerror(errno));
			}

			clrtoeol();
			getReturn( 10 );
    	}
	}
}


/******************************************************************************
*
* readImageWindow
*
* Reads data from a PCI or VME image window.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void readImageWindow( char *title )
{
	int key;
	TDATA tData;
	

	clearAllScreen( title );
	memset( &tData, 0, sizeof(TDATA) );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &readImageData[0], 2, 3 );

		if ( key != 'q' )
		{
			clearAllScreen( title );

			tData.devHandle = devHandle;
			tData.mode = RD_DEVICE;
			tData.offset = readImageData[0].value;
			tData.length = readImageData[1].value;
			tData.mapping = readImageData[2].value;

			tData.numCycles = readImageData[3].value;
			tData.cycleRate = readImageData[4].value;
			tData.numThreads = readImageData[5].value;

			/* setup by the thread */
			tData.threadNum = 0;
			tData.userMemAddrs = 0;
			tData.bPtr = NULL;

			startDeviceAccess( &tData );
		}
	}
}


/******************************************************************************
*
* writeImageWindow
*
* Writes data to a PCI or VME image window.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void writeImageWindow( char *title )
{
	int key;
	TDATA tData;


	clearAllScreen( title );
	memset( &tData, 0, sizeof(TDATA) );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &writeImageData[0], 2, 4 );

		if ( key != 'q' )
		{
			clearAllScreen( title );

			/* setup thread data */
			tData.devHandle = devHandle;
			tData.mode = (RD_DEVICE | WR_DEVICE);
			tData.offset = writeImageData[0].value;
			tData.length = writeImageData[1].value;
			tData.mapping = writeImageData[2].value;
			tData.userData = (UINT8) (writeImageData[3].value & 0xff);
			tData.numCycles = writeImageData[4].value;
			tData.cycleRate = writeImageData[5].value;
			tData.numThreads = writeImageData[6].value;
			tData.useTestPattern = writeImageData[7].value;

			/* setup by the thread */
			tData.threadNum = 0;
			tData.userMemAddrs = 0;
			tData.bPtr = NULL;

			startDeviceAccess( &tData );
			
		}
	}
}



/******************************************************************************
*
* allocDmaBuffer
*
* Allocate a DMA buffer.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void allocDmaBuffer( char *title )
{
	int key;
	int result;

	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &dmaBufferData[0], 1, 1 );

		if ( key != 'q' )
		{
			dmaBufferSize = dmaBufferData[0].value;
				
			result = vme_allocDmaBuffer( devHandle, &dmaBufferSize );
			if ( result == 0 )
			{
				mvprintw( STATUS_LINE, 10, "DMA buffer allocated, %u bytes available", dmaBufferSize);
			}
			else
			{
				mvprintw( STATUS_LINE, 10, "Error (%d) failed to allocate DMA buffer (%s)", 
							result, strerror(errno) );
			}

			clrtoeol();
			getReturn( 10 );
    	}
	}
}


/******************************************************************************
*
* freeDmaBuffer
*
* Free's a previously allocated DMA buffer.
* Prompt user for confirmation.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void freeDmaBuffer( char *title )
{	
	int result;

	
	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
	}
	else
	{
		mvprintw( START_LINE, 10, "Free DMA buffer (%u bytes)", dmaBufferSize);
	
		if ( getConfirm( 10 ) ) 
		{
			result = vme_freeDmaBuffer( devHandle );
			if ( result == 0 )
			{
				dmaBufferSize = 0;

				mvprintw(STATUS_LINE, 10, "DMA buffer freed");
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) failed to free DMA buffer (%s)", 
							result, strerror(errno));
			}
		}
	}

	clrtoeol();
	getReturn( 10 );	
}


/******************************************************************************
*
* setupCmdPkt
*
* Sets up a DMA linked list command packet.
*
*
*  
*
* RETURNS: 0 if successful else an error code.
*
******************************************************************************/
static int setupCmdPkt( UINT32 direction, UINT32 offset, UINT32 length, 
											UINT32 vmeAddress )
{
	VME_CMD_DATA cmdPkt;
	int result;


	cmdPkt.direction = direction;
	cmdPkt.offset = offset;
	cmdPkt.size = length;
	cmdPkt.vmeAddress = vmeAddress;

	cmdPkt.access.dataWidth = dmaAccessData[0].value;
	cmdPkt.access.addrSpace = dmaAccessData[1].value;
	cmdPkt.access.type = dmaAccessData[2].value;
	cmdPkt.access.mode = dmaAccessData[3].value;
	cmdPkt.access.vmeCycle = dmaAccessData[4].value;

	result = vme_addDmaCmdPkt( devHandle, &cmdPkt );
	
	if ( result != 0 )
	{
		mvprintw(STATUS_LINE, 10, "Error (%d) failed to add command packet (%s)",
								result, strerror(errno));
	}

	return result;
}


/******************************************************************************
*
* readDmaBuffer
*
* Reads data from the DMA buffer.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void readDmaBuffer( char *title )
{	
	int key;
	TDATA tData;
	VME_DIRECT_TXFER *tdPtr;
	VME_TXFER_PARAMS *tpPtr;
	UINT32 offset;
	UINT32 vmeAddress;
	int count = 0;
	int result;
	

	clearAllScreen( title );
	memset( &tData, 0, sizeof(TDATA) );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &readDmaData[0], 2, 6 );

		if ( key != 'q' )
		{
			/* setup thread data */
			tData.devHandle = devHandle;
			tData.offset = readDmaData[0].value;
			tData.length = readDmaData[1].value;
			tData.mapping = readDmaData[2].value;
			tData.numCycles = readDmaData[8].value;
			tData.cycleRate = readDmaData[9].value;
			tData.numThreads = readDmaData[10].value;

			/* setup by the thread */
			tData.threadNum = 0;
			tData.userMemAddrs = 0;
			tData.bPtr = NULL;

			if ( readDmaData[6].value == 0 )	/* direct mode */
			{
				clearAllScreen( "Read Data Using Direct Mode DMA" );

				/* setup thread data */
				tData.mode = (DMA_DIRECT | RD_DEVICE);
				tdPtr = (VME_DIRECT_TXFER *) &tData.txfer;

				/* setup DMA transfer data */
				tdPtr->direction = 0;		/* setup by the thread */
				tdPtr->offset = tData.offset;
				tdPtr->size = tData.length;
				tdPtr->vmeAddress = readDmaData[3].value;
				tdPtr->txfer.ownership = (readDmaData[4].value << 4) + readDmaData[5].value;
				tdPtr->txfer.timeout = readDmaData[11].value;
				tdPtr->access.dataWidth = dmaAccessData[0].value;
				tdPtr->access.addrSpace = dmaAccessData[1].value;
				tdPtr->access.type = dmaAccessData[2].value;
				tdPtr->access.mode = dmaAccessData[3].value;
				tdPtr->access.vmeCycle = dmaAccessData[4].value;

				result = 0;
			}
			else	/* linked list mode */
			{
				clearAllScreen( "Read Data Using List Mode DMA" );

				tData.mode = (DMA_LIST | RD_DEVICE);
				tpPtr = (VME_TXFER_PARAMS *) &tData.txfer;
				
				tpPtr->ownership = (readDmaData[4].value << 4) + readDmaData[5].value;

				tpPtr->timeout = readDmaData[11].value;

				/* create a new command linked list */
				if ( readDmaData[7].value == 1 )
				{
					result = vme_clearDmaCmdPkts( devHandle );
					if ( result == 0 )
					{
						while ( (result == 0) && (count < tData.numThreads) )
						{
							vmeAddress = readDmaData[3].value;
							offset = tData.offset + (tData.length * count);
							vmeAddress += offset;
							result = setupCmdPkt( 0, offset, tData.length, vmeAddress );
							count++;
						}
					}
					else
					{
						mvprintw(STATUS_LINE, 10, "Error (%d) clearing list (%s)",
									result, strerror(errno));
					}
				}
				else /* use existing command linked list */
				{
					result = 0;
    			}
			}

			if ( result == 0 )
			{
				startDeviceAccess( &tData );
			}
			else
			{
				clrtoeol();
				getReturn( 10 );
			}
		}
	}
}


/******************************************************************************
*
* writeDmaBuffer
*
* Writes data to the DMA buffer.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void writeDmaBuffer( char *title )
{	
	int key;
	TDATA tData;
	VME_DIRECT_TXFER *tdPtr;
	VME_TXFER_PARAMS *tpPtr;
	UINT32 offset;
	UINT32 vmeAddress;
	int count = 0;
	int result;


	clearAllScreen( title );
	memset( &tData, 0, sizeof(TDATA) );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &writeDmaData[0], 2, 7 );

		if ( key != 'q' )
		{
			/* setup thread data */
			tData.devHandle = devHandle;
			tData.offset = writeDmaData[0].value;
			tData.length = writeDmaData[1].value;
			tData.mapping = writeDmaData[2].value;
			tData.userData = (UINT8) (writeDmaData[6].value & 0xff);
			tData.numCycles = writeDmaData[9].value;
			tData.cycleRate = writeDmaData[10].value;
			tData.numThreads = writeDmaData[11].value;
			tData.useTestPattern = writeDmaData[12].value;
	
			/* setup by the thread */
			tData.threadNum = 0;
			tData.userMemAddrs = 0; 
			tData.bPtr = NULL;

			if ( writeDmaData[7].value == 0 )	/* direct mode */
			{
				clearAllScreen( "Write Data Using Direct Mode DMA" );

				tData.mode = (DMA_DIRECT | RD_DEVICE | WR_DEVICE);
				tdPtr =  (VME_DIRECT_TXFER *) &tData.txfer;

				/* setup DMA transfer data */ 
				tdPtr->direction = 0; 	/* setup by the thread */
				tdPtr->offset = tData.offset;
				tdPtr->size = tData.length;
				tdPtr->vmeAddress = writeDmaData[3].value;
				tdPtr->txfer.ownership = (writeDmaData[4].value << 4) + writeDmaData[5].value;
				tdPtr->txfer.timeout = writeDmaData[13].value;
				tdPtr->access.dataWidth = dmaAccessData[0].value;
				tdPtr->access.addrSpace = dmaAccessData[1].value;
				tdPtr->access.type = dmaAccessData[2].value;
				tdPtr->access.mode = dmaAccessData[3].value;
				tdPtr->access.vmeCycle = dmaAccessData[4].value;

				result = 0;
			}
			else	/* linked list mode */
			{
				clearAllScreen( "Write Data Using List Mode DMA" );

				tData.mode = (DMA_LIST | RD_DEVICE | WR_DEVICE);
				tpPtr = (VME_TXFER_PARAMS *) &tData.txfer;
				
				tpPtr->ownership = (writeDmaData[4].value << 4) + writeDmaData[5].value;
				tpPtr->timeout = writeDmaData[13].value;

				/* create a new command linked list */
				if ( writeDmaData[8].value == 1 )
				{
					result = vme_clearDmaCmdPkts( devHandle );
					if ( result == 0 )
					{
						while ( (result == 0) && (count < tData.numThreads) )
						{
							vmeAddress = writeDmaData[3].value;
							offset = tData.offset + (tData.length * count);
							vmeAddress += offset;
					
							/* add the read command packet first as this will be executed last */
							result = setupCmdPkt( 0, offset, tData.length, vmeAddress );

							/* next, add the write data command packet */
							if ( result == 0 )
							{
								result = setupCmdPkt( 1, offset, tData.length, vmeAddress );
							}

							count++;
						}
					}
					else
					{
						mvprintw(STATUS_LINE, 10, "Error (%d) clearing list (%s)",
									result, strerror(errno));
					}
				}
				else /* use existing command linked list */
				{
					result = 0;
    			}
			}

			if ( result == 0 )
			{
				startDeviceAccess( &tData );
			}
			else
			{
				clrtoeol();
				getReturn( 10 );
			}
		}
	}	
}


/******************************************************************************
*
* getDmaOffset
*
* Get DMA buffer offset from user.
* 
*
*
*  
*
* RETURNS: offset value.
*
******************************************************************************/
static UINT32 getDmaOffset( char *title )
{
	int key;
	UINT32 offset;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &dmaBufferOffsetData[0], 1, 1 );
		clearAllScreen( title );
	}

	offset = (dmaBufferOffsetData[0].value + 16) / 16;

	return offset;
}


/******************************************************************************
*
* viewDmaBuffer
*
* Displays the DMA buffer.
* 
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void viewDmaBuffer( char *title )
{	
	int key = 0;
	UINT32 userMemAddrs;
	UINT32 numOfPages;
	int result = 0;
	UINT8 *rPtr;
	UINT32 offset;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		if ( dmaBufferSize > 0 )
		{
			numOfPages = (dmaBufferSize / PAGE_SIZE);
			if ( (dmaBufferSize % PAGE_SIZE) != 0 )
			{
				numOfPages++;
			}

			result = vme_mmap( devHandle, 0, 
								(numOfPages * PAGE_SIZE), &userMemAddrs );

			if ( result != 0 )
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) memory mapping failed (%s)", 
							result, strerror(errno));
				clrtoeol();
				getReturn( 10 );
			}
			else
			{
				offset = 0;

				while ( key != 'q' ) 
				{
					if ( dmaBufferSize > 256 )
					{
						switch (key)
						{
							case KEY_UP: 	if ( offset >= 16 )
											{
												offset -= 16;
											}
											break;

							case KEY_DOWN:	if ( offset < (dmaBufferSize - 256) )
											{
												offset += 16;
											}
											break;

							case KEY_PPAGE:	if ( offset >= 16  )
											{
												if ( offset >= 256 )
												{
													offset -= 256;
												}
												else
												{
													offset = 0;
												}												
											}
											break;

							case KEY_NPAGE: if ( offset < (dmaBufferSize - 256) )
											{
												if (offset + 512 <= dmaBufferSize)
												{
													offset += 256;
												}
												else
												{
													offset = dmaBufferSize - 256;
												}
											}
											break;

							case KEY_HOME: 	offset = 0;
											break;

							case KEY_END: 	offset = dmaBufferSize - 256;
											break;

							case 'o':		offset = getDmaOffset( title );
											break;

							default:		break;
						}
					}					

					rPtr = (UINT8 *) userMemAddrs + offset;

					if ( dmaBufferSize <= 256 ) /* can only display 256 bytes */
					{
						printBuf( stdscr, START_LINE + 2, offset, rPtr, dmaBufferSize );
					}
					else
					{
						printBuf( stdscr, START_LINE + 2, offset, rPtr, 256 );
					}

					mvprintw(PROMPT_LINE, 10, "Use the arrow keys to scroll");
					clrtoeol();

					key = getch();
    			}

				if ( userMemAddrs != 0 )
				{
					result = vme_unmap( devHandle, userMemAddrs,
										(numOfPages * PAGE_SIZE) );

					if ( result != 0 )
					{
						mvprintw(STATUS_LINE, 10, "Error (%d) unmap failed (%s)", 
									result, strerror(errno));
						clrtoeol();
						getReturn( 10 );
					}
				}
			}
		}
		else
		{
			mvprintw(STATUS_LINE, 10, "Error DMA buffer not allocated");
			clrtoeol();
			getReturn( 10 );
		}
	}
}


/******************************************************************************
*
* configAccessParams
*
* Configure DMA access parameters.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void configAccessParams( char *title )
{	
	int key;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );
	}
	else
	{
		key = selectItem( &dmaAccessData[0], 2, 5 );
	}
	
}


/******************************************************************************
*
* addCmdPkt
*
* Adds a DMA command packet to the list.
* Prompt user for parameters and display results.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void addCmdPkt( char *title )
{
	int key;	
	VME_CMD_DATA cmdPkt;
	int count = 0;
	int result = 0;


	clearAllScreen( title );

	if (devHandle == -1 )
	{
		mvprintw(STATUS_LINE, 10, "%s", devNotOpen);
		clrtoeol();
		getReturn( 10 );	
	}
	else
	{
		key = selectItem( &cmdPktData[0], 2, 5 );

    	if ( key != 'q' ) 
		{
			clearAllScreen( title );

			cmdPkt.direction = cmdPktData[1].value;
			cmdPkt.offset = cmdPktData[2].value;
			cmdPkt.size = cmdPktData[3].value;
			cmdPkt.vmeAddress = cmdPktData[4].value;

			cmdPkt.access.dataWidth = cmdPktData[5].value;
			cmdPkt.access.addrSpace = cmdPktData[6].value;
			cmdPkt.access.type = cmdPktData[7].value;
			cmdPkt.access.mode = cmdPktData[8].value;
			cmdPkt.access.vmeCycle = cmdPktData[9].value;
	
			while ( (result == 0) && (count < cmdPktData[0].value) )
			{
				result = vme_addDmaCmdPkt( devHandle, &cmdPkt );
				cmdPkt.offset += cmdPktData[3].value;
				count++;
			}

			if ( result == 0 )
			{
				getProcFile( title, "/dmaList", 5 );
				mvprintw(STATUS_LINE, 10, "%d command packet(s) added to list", count);
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) failed to add command packet(s) (%s)",
								result, strerror(errno));
			}

			clrtoeol();
			getReturn( 10 );
    	}
	}
}


/******************************************************************************
*
* clearCmdPktList
*
* Clears the DMA command packet list.
* Prompt user for confirmation.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void clearCmdPktList( char *title )
{
	int result;


	clearAllScreen( title );

	if ( devHandle != -1 )
	{
		/* mvprintw( START_LINE, 10, "Device: %s", deviceName); */
	
		if ( getConfirm( 10 ) ) 
		{	
			result = vme_clearDmaCmdPkts( devHandle );		
			if ( result == 0)
			{
				mvprintw(STATUS_LINE, 10, "Command packet list cleared");
			}
			else
			{
				mvprintw(STATUS_LINE, 10, "Error (%d) clearing list (%s)",
								result, strerror(errno));
			}
		}
	}
	else
	{
		mvprintw(STATUS_LINE, 10, "Error no devices open");
	}

	clrtoeol();
	getReturn( 10 );	
}


/******************************************************************************
*
* showAbout
*
* Displays version information.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void showAbout( char *title )
{
	int screenrow = START_LINE;
	int len;


	clearAllScreen( title );

#ifdef DEBUG
	mvprintw(screenrow, 10, "++WARNING++ Development Version");
	screenrow += 2;
#endif

	mvprintw(screenrow, 10, "%s", appTitle);
	screenrow++;
	mvprintw(screenrow, 10, "%s %s", appVersion, appCreationDate);
	screenrow++;
	mvprintw(screenrow, 10, "%s", appCopyright);
	screenrow += 4;

	len = vme_getApiVersion( tmpBuf );
	displayStrBuf( tmpBuf, len, screenrow, 10 );
	
	clrtoeol();
	getReturn( 10 );

}


/******************************************************************************
*
* intCtlSubMenu
*
* Interrupt control sub-memu.
* Display menu and prompt user for selection.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void intCtlSubMenu( char *title )
{
	int choice;
	int selected_row = 0;
	char *cPtr;

	do 
	{
		clearAllScreen( title );
		choice = getChoice("Options:", interruptCtlMenu, &selected_row, 10, 40 );
		cPtr = interruptCtlMenu[selected_row];
		cPtr++;

		switch ( choice ) 
		{
			case '0':
				setInterruptMode( cPtr );
	    		break;

			case '1':
				waitForInterrupt( cPtr );
	    		break;

			case '2':
				enableUnvInt( cPtr );
	    		break;

			case '3':
				generateInt( cPtr );
	    		break;

			case '4':
				getVmeIntInfo( cPtr );
	    		break;

			case '5':
				getVmeExtIntInfo( cPtr );
	    		break;

			case '6':
				getVerrInfo( cPtr );
				break;

			case 'q':
				clear();
				refresh();
	    		break;

			default:
	    		break;
		}
    } 
	while (choice != 'q');
}


/******************************************************************************
*
* imageCtlSubMenu
*
* Image control sub-memu.
* Display menu and prompt user for selection.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void imageCtlSubMenu( char *title )
{
	int choice;
	int selected_row = 0;
	char *cPtr;

	do 
	{
		clearAllScreen( title );
		choice = getChoice("Options:", imageCtlMenu, &selected_row, 10, 40 );
		cPtr = imageCtlMenu[selected_row];
		cPtr++;

		switch ( choice ) 
		{
			case '0':
				csrImage( cPtr );
	    		break;

			case '1':
				locationMon( cPtr );
	    		break;

			case '2':
				regAccessImage( cPtr );
	    		break;

			case 'q':
				clear();
				refresh();
	    		break;

			default:
	    		break;
		}
    } 
	while (choice != 'q');
}


/******************************************************************************
*
* ctlDeviceSubMenu
*
* Control device sub-memu.
* Display menu and prompt user for selection.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void ctlDeviceSubMenu( char *title )
{
	int choice;
	int selected_row = 0;
	char *cPtr;

	do 
	{
		clearAllScreen( title );
		choice = getChoice("Options:", devCtlMenu, &selected_row, 10, 40 );
		cPtr = devCtlMenu[selected_row];
		cPtr++;

		switch ( choice ) 
		{
			case '0':
				readRegisters( cPtr );
				break;

			case '1':
				writeRegister( cPtr );
				break;
	
			case '2':
				setUserAmCodes( cPtr );
	    		break;

			case '3':
				byteSwap( cPtr );
	    		break;

			case '4':
				intCtlSubMenu( cPtr );
	    		break;

			case '5':
				imageCtlSubMenu( cPtr );
	    		break;

			case '6':
				clearStats( cPtr );
	    		break;

			case '7':
				reserveMemory( cPtr );
	    		break;

			case 'q':
				clear();
				refresh();
	    		break;

			default:
	    		break;
		}
    } 
	while (choice != 'q');
}


/******************************************************************************
*
* pciImageSubMenu
*
* PCI image sub-memu.
* Display menu and prompt user for selection.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void pciImageSubMenu( char *title )
{
	int choice;
	int selected_row = 0;
	char *cPtr;

	do 
	{
		clearAllScreen( title );
		choice = getChoice("Options:", imageMenu, &selected_row, 11, 40 );
		cPtr = imageMenu[selected_row];
		cPtr++;

		switch ( choice ) 
		{
			case '0':
				enablePciImage( cPtr );
				break;
	
			case '1':
				disableImage( cPtr );
				break;

			case '2':
				readImageWindow( cPtr );
				break;

			case '3':
				writeImageWindow( cPtr );
				break;
			
			case 'q':
				clear();
				refresh();
	    		break;

			default:
	    		break;
		}
    } 
	while (choice != 'q');
}


/******************************************************************************
*
* vmeImageSubMenu
*
* VME image sub-memu.
* Display menu and prompt user for selection.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void vmeImageSubMenu( char *title )
{
	int choice;
	int selected_row = 0;
	char *cPtr;

	do 
	{
		clearAllScreen( title );
		choice = getChoice("Options:", imageMenu, &selected_row, 11, 40 );
		cPtr = imageMenu[selected_row];
		cPtr++;

		switch ( choice ) 
		{
			case '0':
				enableVmeImage( cPtr );
				break;
	
			case '1':
				disableImage( cPtr );
				break;

			case '2':
				readImageWindow( cPtr );
				break;

			case '3':
				writeImageWindow( cPtr );
				break;
			
			case 'q':
				clear();
				refresh();
	    		break;

			default:
	    		break;
		}
    } 
	while (choice != 'q');
}


/******************************************************************************
*
* dmaSubMenu
*
* DMA sub-memu.
* Display menu and prompt user for selection.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void dmaSubMenu( char *title )
{
	int choice;
	int selected_row = 0;
	char *cPtr;

	do 
	{
		clearAllScreen( title );
		choice = getChoice("Options:", dmaMenu, &selected_row, 11, 40 );
		cPtr = dmaMenu[selected_row];
		cPtr++;

		switch ( choice ) 
		{
			case '0':
				allocDmaBuffer( cPtr );
				break;
	
			case '1':
				freeDmaBuffer( cPtr );
				break;
			
			case '2':
				configAccessParams( cPtr );
				break;

			case '3':
				addCmdPkt( cPtr );
				break;
			
			case '4':
				clearCmdPktList( cPtr );
				break;

			case '5':
				readDmaBuffer( cPtr );
				break;

			case '6':
				writeDmaBuffer( cPtr );
				break;

			case '7':
				viewDmaBuffer( cPtr );
				break;

			case 'q':
				clear();
				refresh();
	    		break;

			default:
	    		break;
		}
    } 
	while (choice != 'q');
}


/******************************************************************************
*
* driverInfoSubMenu
*
* Device driver information sub-memu.
* Display menu and prompt user for selection.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void driverInfoSubMenu( char *title )
{
	int choice;
	int selected_row = 0;
	char *cPtr;

	do 
	{
		clearAllScreen( title );
		choice = getChoice("Options:", procFileMenu, &selected_row, 11, 40 );
		cPtr = procFileMenu[selected_row];
		cPtr++;

		switch ( choice ) 
		{
			case '0':
				getDeviceStatus( cPtr );
				break;

			case '1':
				getProcFile( cPtr, "/ints", 10 );
				clrtoeol();
				getReturn( 10 );
				break;
			
			case '2':
				getProcFile( cPtr, "/lsiStats", 5 );
				clrtoeol();
				getReturn( 10 );
				break;

			case '3':
				getProcFile( cPtr, "/vsiStats", 5 );
				clrtoeol();
				getReturn( 10 );
				break;

			case '4':
				getProcFile( cPtr, "/dmaStats", 5 );
				clrtoeol();
				getReturn( 10 );
				break;

			case '5':
				getProcFile( cPtr, "/pciAlloc", 5 );
				clrtoeol();
				getReturn( 10 );
				break;
#ifdef DEBUG
			case '6':
				getProcFile( cPtr, "/dmaList", 5 );
				clrtoeol();
				getReturn( 10 );
				break;
#endif
	
			case 'q':
				clear();
				refresh();
	    		break;

			default:
	    		break;
		}
    } 
	while (choice != 'q');
}


/******************************************************************************
*
* displayMainMenu
*
* Main memu.
* Display menu and prompt user for selection.
*
*
*  
*
* RETURNS: None.
*
******************************************************************************/
static void displayMainMenu( void )
{
    int choice;
	char *cPtr;
	int selected_row = 0;


    initscr();

	if( ((stdscr->_maxx+1) < 80) || ((stdscr->_maxy+1) < 24) )
	{
		endwin();

		printf("\nError attempt to run program in a window smaller than 80x24\n\n");
	}
	else
	{
		curs_set(0); /* turn cursor off */
		keypad(stdscr, TRUE);
		noecho();

		strcpy( deviceName, "closed" );

    	do 
		{
			clearAllScreen( appTitle );
			choice = getChoice("Options:", mainMenu, &selected_row, 11, 40 );
			cPtr = mainMenu[selected_row];
			cPtr++;

			switch ( choice ) 
			{
				case '0':
					openDevice( cPtr );
	    			break;

				case '1':
					closeDevice( cPtr );
					break;

				case '2':
					ctlDeviceSubMenu( cPtr );
	    			break;

				case '3':
					pciImageSubMenu( cPtr );
					break;

				case '4':				
					vmeImageSubMenu( cPtr );
					break;
	
				case '5':
					dmaSubMenu( cPtr );
					break;

				case '6':
					driverInfoSubMenu( cPtr );
					break;

				case '7':
					showAbout( cPtr );
					break;
			
				case 'q':
					clear();
					refresh();
	    			break;

				default:
	    			break;
			}
    	} 
		while (choice != 'q');

		curs_set(1); /* turn cursor on */
		keypad(stdscr, FALSE);
		echo();
    	endwin();
	}
}


/******************************************************************************
*
* main
*
* Program entry point.
*
*
*  
*
* RETURNS: SUCCESS.
*
******************************************************************************/
int main( void )
{

	displayMainMenu();

	exit(EXIT_SUCCESS);
}
