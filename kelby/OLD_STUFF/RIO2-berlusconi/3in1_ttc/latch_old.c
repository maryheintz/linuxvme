

#include <types.h>
#include <stdio.h>
#include <ces/vmelib.h>
static struct pdparam_master param=
      { 1,   /* VME Iack (must be 1 for all normal pages) */
        0,   /* VME Read Prefetch Option */
        0,   /* VME Write Posting Option */
        1,   /* VME Swap Option */
        0,   /* page number return */
        0,   /* reserved */
        0,   /* reserved */
        0 };
unsigned long *swtch, *adr;
  init_old()
  { unsigned long am,vmead,offset,size,len,k;
    int l,shft;
       am=0x39;
       vmead=0xcf0000;
       offset=0;
       size=0;
       len=400;
       swtch = 
         (unsigned long *)( find_controller(vmead,len,am,offset,size,&param) );
       if( swtch == (unsigned long *)(-1) ) 
        { printf("unable to map vme address\n");
          exit(0);}
       return(0);	  
  }
   latch_old()
     { unsigned long k;
       int l;
       k = *swtch;
       l = k & 1;
       return(l);
     }
   rlatch_old()
     { unsigned long k;
       k = (unsigned long)swtch | 8;
       (unsigned long *)adr = (unsigned long *)k;
       *adr = 0;
     }
