#include <stdio.h>main(){    short *adr1,*adr2;   unsigned long k1,k2;   unsigned short kshort;   k1=0xf0df0004;   k2=0xf0df0006;   adr1 = (short *)k1;   adr2 = (short *)k2;   FPermit(0xf0df0000,0x100,3);loop:   *adr1=0;   sleep(1);   *adr2=0;   sleep(1);   if(k1 != 0) goto loop;   return(0); }