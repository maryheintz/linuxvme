#include <stdio.h>
#include <math.h>
main()
{ 
   int zone,sector,tube,i,k;
   unsigned int i1,i2,itst;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   ttcvi_map();
strt:
   zone=0;
   sector=0;
/*   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d",&zone,&sector);   */
   set_zone(zone);
   set_sector(sector);
   multi_low();
   itst = 0x1eaa;
   for(k=0;k<49;k++)
     { set_tube(k);
       set_intg_gain(0x5);   /* s1=s3=1 s2=s4=0 */
       itr_high();   /* itr=1 */
       intg_rd_low();   /* ire=0 */
       mse_high();   /* mse=1 */
       small_cap_disable();   /* tplo=0 */
       large_cap_enable();   /* tphi=1 */
       trig_disable();   /* trig=0 */
       can_bkld_high();
       can_bkld_low();
       can_set(0x9c,0);  /* fetch from 3in1 and load can output reg */
       tsleep(0x80000005);
       can_shft_rd(&shrt);
       i = (int)shrt;
       trig = i & 1;
       s1 = (i>>1) & 1;
       s2 = (i>>2) & 1;
       s3 = (i>>3) & 1;
       s4 = (i>>4) & 1;
       itr = (i>>5) & 1;
       ire = (i>>6) & 1;
       mse = (i>>7) & 1;
       tplo = (i>>8) & 1;
       tphi = (i>>9) & 1;
       if((s1==1) && (s2==0)  && (s3==1) && (s4==0) && (itr==1) &&
          (ire==0) && (mse==1) && (tplo==0) && (tphi==1) && 
          (trig==0) ) printf(" card %d found\n",k);;
       if(sw(7)==1)
       printf("card=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
      }
      if(sw(1) == 1) 
        { printf(" \n");
          sleep(1);
          goto strt;
        }
      
}


