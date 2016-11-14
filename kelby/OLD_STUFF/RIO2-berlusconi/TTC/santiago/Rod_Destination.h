#include <ces/vmelib.h>
#include <ces/uiocmd.h>
#include "atdaq_types.h"
#include "s5933lib.h"
#include "SLINK.h"

/* READOUT CONSTANTs */
#define NUM_OF_SAMPLES 15
#define WHOLE_READOUT 30
#define CHANNELs_PER_BLOCK 3
#define BITs_PER_DATA 31

#define WORDs_PER_BLOCK (NUM_OF_SAMPLES+2)
#define NUM_OF_BLOCKs   (WHOLE_READOUT/CHANNELs_PER_BLOCK)
#define EVENT_SIZE      (WORDs_PER_BLOCK*NUM_OF_BLOCKs)
#define EVENTbyte_SIZE  (WORDs_PER_BLOCK*4)

/* Slink to PMC Constant */
#define MAX_BUF_SIZE 9000
#define SSP_IN_REG  imb[3]
#define SSP_OUT_REG omb[0]
#define DMA_ON 1
/*#define SLIDAS*/
#define _DIGITIZER_
#define SLOT1 0

/* ROD NEMONICs */
#define ROD_TRUE 1
#define ROD_FALSE 0
#define ROD_CLEAR 0
#define ROD_SET 1
#define PARITY_SIZE 30
#define VERTICAL_PARITY 31

/* ERROR & SUCCES Codes */
#define ALLOC_FAILED 0
#define ALLOC_SUCCESS 1
#define INIT_FAILED 0
#define INIT_SUCCESS 1
#define RELEASE_SUCCESS 1
#define RELEASE_FAIL 0
#define EVENT_RELEASED 1

/* Basic Types */
typedef unsigned int word32;
typedef unsigned short int word16;
typedef unsigned char word8;


/* Single 32 bits words from FrontEnd */
typedef struct{
  word8  Code:1;
  word8  Parity:1;
  word32 Data:30;
}PAccess;

typedef union{
  PAccess ParityAccess;
  word32  all;
}ParityType;



/* Data Pattern */
typedef union{

  PAccess  ParityAccess;
  struct{
    word8 Code:1;
    word8 Parity:1;
    word16 Data1:10;
    word16 Data2:10;
    word16 Data3:10;
  }bit;
  word32 all;

}SMPLdata;


/* Control Pattern */
typedef union{

  struct{
    word16 Code:1;
    word16 Parity:1;
    word16 ParityError:1;
    word16 ReadError:1;
    word16 TECError:1;
    word16 DEstr:2;
    word16 SEstr:1;
    word16 HighLow:1;
    word16 Mode:2;
    word16 BoardID:2;
    word16 DataSetID:1;
    word16 dummy2:1;
    word16 dummy1:1;
    word16 BCID:12;
    word16 mode:2;
    word16 BOARD_id: 2;
    word16 DATA_group_id: 2;
    word16 BCID_id: 12;
  }bit;

  PAccess ParityAccess;
  word32 all;

}CTRLdata;


typedef struct{
  word32 *FEWord;/* Pointer to index block */
  word32 *Vdata; /* Vertical data buffer to calculate vertical parity */
  word16  SizeOfBlock;
}Block;


/* Generic PCI Type */
typedef struct{
  word32 *VIRTaddr;
  word32 *PCIaddr;
  word32  PageSize;
}PCIitems;


/* Two types to have the PCI features of the event */
typedef struct{
  PCIitems item;
  word32   VMEAddress;
}RaidWindow;

typedef struct{
  PCIitems item;
}SlinkWindow;  


/* Complete features of the event */
typedef struct{
  word16      NumOfBlocks;
  Block      *FEBlock;
  word32     *Vparity;
  word32     *Hparity;
  word16      EventSize;     
  word16      BitsPerData;
  SlinkWindow SlinkWin;
  RaidWindow  RaidWin;
}Event;


/* Parametters of the FrontEnd readout */
typedef struct{
  word16 NumOfBlocks;
  word16 EventSize;
  word16 WordsPerBlock;
  word16 BitsPerData;
  word16 SizeOfBlock;
}ReadOutParams;

/* If something is wrong I want to know which has failed to release 
   memory of others */
typedef union{
  struct{
    word8 dummy:3;
    word8 event:1;
    word8 slink_dev:1;
    word8 slink_PCIdesc:1;
    word8 slvVME_PCIdesc:1;
    word8 slvVME_VMEmap:1;
  }field;
  word8 all;
}Item;

typedef struct{
  Item          item;
  SLINK_device  *slink_dev;
  Event         *event;
  uio_mem_t     *slink_PCIdesc;
  uio_mem_t     *slvVME_PCIdesc;
  word32         vme_addr;
}EventResources;


/* Function Declaration */
int DataProcessing(Event *event);

int ParityCheck(word32 data, int width);
int Base2(int N);

Event* AllocMemory(ReadOutParams *ReadOut, Event *event);
int FreeMemory(Event *event);
int ReleaseResources(EventResources *resource);
int TestPointers(Event *event);
