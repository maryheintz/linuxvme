#include <stdio.h>
#include <math.h>
#include <types.h>
#include <time.h>
time_t usec;
main()
{ 
   int zone,sector,tube,mode;
   ttcvi_map();
   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d %d",&zone,&sector,&tube);
   set_zone(zone);
   set_sector(sector);
   set_tube(tube);
   set_swln2(1);
   set_swln1(0);
   usleep(1000);
   large_cap_enable();
   usleep(1000);
   small_cap_disable();
   usleep(1000);
   set_dac(0x3ff);
   usleep(1000);
loop:
      tp_high();
      usleep(1000);
      tp_low_bc();
      usleep(1000);
      if(sw(1) != 1) goto loop;
}


