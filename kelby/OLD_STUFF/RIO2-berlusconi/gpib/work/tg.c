#include <types.h>
#include <stdio.h>
#include <ces/vmelib.h>
#include <time.h>
#define GPIBout(x,v) (ib->x = v)
static struct pdparam_master param=
      { 1,   /* VME Iack (must be 1 for all normal pages) */
        0,   /* VME Read Prefetch Option */
        0,   /* VME Write Posting Option */
        1,   /* VME Swap Option */
        0,   /* page number return */
        0,   /* reserved */
        0,   /* reserved */
        0 };
 struct block {                /*  hardware registers                  */
	char      ch0[64];     /*   +0 (x000) dma channel 0            */
	char      ch1[64];     /*  +64 (x040) dma channel 1            */
	char      ch2[64];     /* +128 (x080) dma channel 2            */
	char      ch3[64];    /* +192 (x0C0) dma channel 3            */
	char      f0B;            /* +256 (x100) PADDING                  */
	char           cfg1;   /* +257 (x101) configuration register 1 */
	char   f0C[3];         /* +258 (x102) PADDING                  */
	char           cfg2;   /* +261 (x105) configuration register 2 */
	char   f0D[10];        /* +262 (x106) PADDING                  */
	char   f0E,    cdor;   /* +273 (x111) byte out register        */
	char   f0F,    imr1;   /* +275 (x113) interrupt mask reg. 1    */
	char   f10,    imr2;   /* +277 (x115) interrupt mask reg. 2    */
	char   f11,    spsr;   /* +279 (x117) serial poll mode reg.    */
	char   f12,    adsr;   /* +281 (x119) address mode register    */
	char   f13,    auxmr;  /* +283 (x11B) auxiliary mode register  */
	char   f14,    adr;    /* +285 (x11D) address register 0/1     */
	char   f15,    eosr;   /* +287 (x11F) end of string register   */
};
struct block *ib;
#define dir cdor
#define isr1 imr1
#define isr2 imr2
#define cptr auxmr

/* Read-only register mnemonics corresponding to write-only registers */
time_t usec = 1;

main()
 { int i;
   char b8;
   setbase();
   printf("start test 1 - initialize TLC\n");
   ib->cfg2 = 0xa;
   
   sleep(1);
   ib->cfg2 = 8;
   usleep(usec);
   ib->auxmr = 2;
   ib->auxmr = 0;
   printf("start test 2 \n");
   ib->cfg2 = 0xa;
   sleep(1);
   ib->cfg2 = 8;
   usleep(usec);
   ib->cdor = 0;
   b8 = ib->dir;
   printf("dir=0? %x\n",(int)b8);
   b8 = ib->isr1;
   printf("isr1=0? %x\n",(int)b8);
   b8 = ib->isr2;
   printf("isr2=0? %x\n",(int)b8);
   b8 = ib->spsr;
   printf("spsr=0? %x\n",(int)b8);
   b8 = ib->adsr;
   printf("adsr=40? %x\n",(int)b8);
   b8 = ib->cptr;
   printf("cptr=0? %x\n",(int)b8);
   b8 = ib->gcr;
   printf("gcr=0? %x\n",(int)b8);

}

setbase()
 { unsigned long am,vmead,offset,size,len;
    unsigned int base;
    int l,shft;
    static unsigned short *swtch;
    am=0x29;
    vmead=0x2000;
    offset=0;
    size=0;
    len=400;
    ib = 
      (struct block *)( find_controller(vmead,len,am,offset,size,&param) );
    if( ib == (struct block *)(-1) ) 
     { printf("unable to map vme address\n");
       exit(0);
	}
       printf("ib=%x\n",(int)ib);
       return(0);	  
 }      
