#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
time_t usec;
int id,ttcadr;
main(ttcadr)
{ 
   int tube,mode,zone_bar,sector_bar,dac,i;
   unsigned int k;
   unsigned long klong;
   printf(" enter ttcadr in hex\n");
   fscanf(stdin,"%x",&ttcadr);
   printf("ttcadr = %x\n",ttcadr);  
   id=1;
   ttcvi_map(ttcadr);
   can_init(ttcadr,id);
loop:
   reset_can(ttcadr,1);
   sleep(1);
   reset_can(ttcadr,0);
   sleep(1);
   goto loop;
}


