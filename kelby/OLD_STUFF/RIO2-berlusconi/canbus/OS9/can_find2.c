#include <stdio.h>
#include <math.h>
main()
{ 
   int zone,sector,tube,i,k;
   unsigned int i1,i2,itst;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   long status;
   ttcvi_map();
   can_init();
strt:
   zone=0;
   sector=0;
/*   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d",&zone,&sector);   */
   set_zone(zone);
   tsleep(0x80000002);
   set_sector(sector);
   tsleep(0x80000002);
   for(k=0;k<49;k++) 
     { if(cardchk(k) == 1) printf(" card %d found\n",k);
     }
   if(sw(1) == 1) 
     { printf(" \n");
       sleep(1);
       goto strt;
     }
      
}


