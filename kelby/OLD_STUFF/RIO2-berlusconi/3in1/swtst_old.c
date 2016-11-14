

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
main()
  { int l,i,mode;
    l = sw(1); /* initialize switch reading */

restart:
   printf(" select desired test - loop\n");
   printf("  1 = read switches and latch\n");
   printf("  2 = reset latch\n");
   printf("  3 = toggle f1\n");
   fscanf(stdin,"%d",&mode);

   if(mode == 1)
    {  for(i=1;i<9;i++)
        {l = sw(i);
         printf("i=%d  sw=%d\n",i,l);
        }
    l = latch();
    printf("latch bit=%d\n",l);
    }
   if(mode == 2) rlatch();
   if(mode == 3) trigger();
   goto restart;
  }
sw(i)
  int i;
  { unsigned long am,vmead,offset,size,len,k;
    int l,shft;
    static int ent=0;
    if(ent==0)
     { ent = 1;
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
     if(i>0 && i<9)	  
      { k=*swtch;
        printf(" k=%x\n",k);
        shft=23+i;
        l=(k>>shft)&1;
	return(l); 
      }
   }
   latch()
     { unsigned long k;
       int l;
       k = *swtch;
       l = k & 1;
       return(l);
     }
   rlatch()
     { unsigned long k;
       k = (unsigned long)swtch | 8;
       (unsigned long *)adr = (unsigned long *)k;
       *adr = 0;
     }
   trigger()
    { unsigned long k;
       k = (unsigned long)swtch | 4;
       (unsigned long *)adr = (unsigned long *)k;
       *adr = 0;
     }
