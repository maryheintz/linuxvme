#include <stdio.h>
#include <math.h>
main()
{ 
   int zone,sector,tube,i,k;
   unsigned int i1,i2,itst;
   unsigned short shrt;
   long stat;
   ttcvi_map();
   can_init();
   zone=0;
   sector=0;
/*   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d",&zone,&sector);   */
strt:
   can_zone(zone);
   can_sector(sector);
   can_multi_low();
   itst = 0x1eaa;
   for(k=0;k<49;k++)
     { can_tube(k);
         i=0xaaa;
         i1 = 0xfc | ((i>>8)&3);
         i2 = i & 0xff;
         tsleep(0x80000005);
         shrt=(i1<<8) | i2;
 	 stat=can_3in1set(shrt);       /* send to 3in1 card */
         tsleep(0x80000005);
 	 stat=can_3in1set(0x9c00);       /* load back */
         tsleep(0x80000005);
 	 stat=can_3in1get(&shrt);
	 if((int)shrt == itst) printf(" card %d found\n",k);
	 if(sw(7) == 1)
               { if((int)shrt != itst)
                 printf(" card %d not found  shrt=%x  itst=%x\n",k,shrt,itst);}
         if(sw(8) == 1) printf(" card %d  back=%x\n",k,(int)shrt);
      }
      if(sw(1) == 1) 
        { printf(" \n");
          sleep(1);
          goto strt;
        }
      
}


