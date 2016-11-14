#include <stdio.h>
#include <math.h>
#include <types.h>
#include <time.h>
char string[1000];
unsigned char gpib_adr = 3;
int ent=0;
time_t usec;
main()
{ 
   int isw[6],i1,i2,ll,i,mode,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,sw1,dacset;
   int card,zone,sector,tube,l;
   unsigned short cadr,sval,dval,tval,t1val,t2val;
   double fmin,ratio,volt[6],ped[6],expr[6];
   map_3in1();
   zone = 1;
   sector = 1;
   card = 48;
   l = (zone<<12) | (sector<<6) | card;
   printf(" zone=%d sector=%d card=%d cadr=%x\n",zone,sector,card,l);
   cadr = (unsigned short)l;
   card_sel(cadr);
   tval = (unsigned short)100;
   tim1_set(tval);
   tval = (unsigned short)50;
   tim2_set(tval);
   rtime();
     for(i=0;i<49;i++)
        { cadr = (unsigned short)( (zone<<12) | (sector<<6) | i );
          card_sel(cadr);
          send4_3in1(4,0);   /* disable small C */
          send4_3in1(5,0);   /* disable large C */
         }
       cadr = (unsigned short)( (zone<<12) | (sector<<6) | card );
       card_sel(cadr);
       send4_3in1(4,0);   /* disable small C */
       send4_3in1(5,1);   /* enable large C */
       multi_low();
       dval = (unsigned short)1023;
       send13_3in1(6,dval);  /* set dac value */
loop25:
       usec=30;
       usleep(usec);
       send4_3in1(7,1);
       tp_high();
       usec=10;
       usleep(usec);
       stime();
       usec=10;
       usleep(usec);
       rtime();
       usec=30;
       usleep(usec);
       send4_3in1(7,0);
       tp_high();
       usec=10;
       usleep(usec);
       stime();
       usec=10;
       usleep(usec);
       rtime();
       sw1=sw(1);
       if(sw1 == 0) goto loop25;
}
