#include <stdio.h>
#include <time.h>
#include "vme.h"

time_t usec;
unsigned long vmebase;

main()
 { unsigned short *adr,reg,uni;
   int mode;
   vmebase = vme_a24_base(); 
   printf("vmebase=0x%x\n",vmebase);
   
loop:
   printf(" enter operation number\n");
   printf(" 1 - write to 32 bit register\n");
   printf(" 2 - read from 32 bit register\n");
   printf(" 3 - read/write test (loop print on error)\n");
   printf(" 4 - exit\n");
   fscanf(stdin,"%d",&mode);   
  
    if(mode == 1)
       { printf(" enter register value (32 bit hex)\n");
         fscanf(stdin,"%x",&reg);
         adr = (unsigned short *)(vmebase+0x00de100a);
         *adr = reg;
         goto loop;
       }

    if(mode == 2)
       { adr = (unsigned short *)(vmebase+0x00de100c);
         reg = *adr;
	 printf(" register value=%x\n",reg);
         goto loop;
       }

    if(mode == 3)
       { loop15:
         usec=2;
         for(uni=0;uni<0xffff;uni++)
	   { adr = (unsigned short *)(vmebase+0x00de100a);
	     *adr = uni;
	     usleep(usec);
	     adr = (unsigned short *)(vmebase+0x00de100c);	     
             reg = *adr;
	     usleep(usec);
	     printf(" OK %x %x\n",uni,reg);
	     if(uni != reg) printf(" register error %x %x\n",uni,reg);
	   }
	 goto loop15;
       }

    if(mode == 4) exit(0);

    goto loop;
  }

