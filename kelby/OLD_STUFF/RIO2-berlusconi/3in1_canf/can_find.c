#include <stdio.h>
#include <math.h>
#include <types.h>
#include <time.h>
time_t usec;
main()
{ 
   int zone,sector,tube,i,k;
   unsigned int i1,i2,itst;
   unsigned short shrt;
   zone=0;
   sector=0;
/*   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d",&zone,&sector);   */
   can_zone(zone);
   can_sector(sector);
   can_multi_low();
   can_swln2(1);
   can_swln1(0);
   itst = 0x1eaa;
   for(k=0;k<49;k++)
     { can_tube(k);
         i=0xaaa;
         i1 = 0xfc | ((i>>8)&3);
         i2 = i & 0xff;
         usleep(10);
 	 can_set(i1,i2);  /* send to 3in1 card */
	 usleep(100);
 	 can_set(0x9c,0);  /* fetch from 3in1 and load can output reg */
         usleep(100);
 	 can_shft_rd(&shrt);
	 if((int)shrt == itst) printf(" card %d found\n",k);
         if(sw(8) == 1) printf(" card %d  back=%x\n",k,(int)shrt);
      }
      
}


