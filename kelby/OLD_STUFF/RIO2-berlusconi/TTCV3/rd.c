#include <stdio.h>
#include <math.h>
main()
{  unsigned int basef = 0xdedd0000;
   unsigned short *adr,data;
   adr = (unsigned short *)(basef | 0xa);
   data = *adr;
   printf("data=%x\n",data);
 }
