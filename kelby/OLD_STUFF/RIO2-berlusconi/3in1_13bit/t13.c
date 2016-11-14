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
   printf(" enter zone sector card\n");
   fscanf(stdin,"%d %d %d",&zone,&sector,&card);
   l = (zone<<12) | (sector<<6) | card;
   printf(" zone=%d sector=%d card=%d cadr=%x\n",zone,sector,card,l);
   cadr = (unsigned short)l;
   card_sel(cadr);
   tval = (unsigned short)100;
   tim1_set(tval);
   tval = (unsigned short)50;
   tim2_set(tval);
   rtime();
   printf(" select desired test - loop\n");
   printf("  1 = toggle intg_rd\n");
   printf("  2 = toggle itr\n");
   printf("  3 = mse test\n");
   printf("  4 = test tp\n");
   printf("  5 = toggle s1 s2 s3 or s4\n");
   printf("  6 = dac set loop\n");
   printf("  7 = read current status\n");
   printf("  8 = step shaper cal pulses\n");
   printf("  9 = read back status\n");
   printf(" 10 = large cal pulses\n");
   printf(" 11 = test timers\n");
   printf(" 12 = cal pulses (choose dac values)\n");
   printf(" 13 = test integtator gains\n");
   printf(" 14 = automated integtator gains\n");
   printf(" 15 = large cal pulses using multi_sel\n");
   printf(" 16 = controller read/write test\n");
   printf(" 17 = controller-3in1 read/write test\n");
   printf(" 18 = toggle card address\n");
   printf(" 19 = test status selection\n");
   printf(" 20 = dac set loop intg on\n");
   printf(" 21 = set dac values\n");
   printf(" 22 = toggle dac 127-255\n");
   printf(" 23 = controller-3in1 read/write test aaa-1555\n");
   printf(" 24 = toggle 3in1 clock\n");
   printf(" 25 = large cal pulses alternate trig output\n");
   printf(" 26 = send/receive 0x1000\n");

   fscanf(stdin,"%d",&mode);

   if(mode == 1)
    {loop1:
       send4_3in1(0,1);     /* set intg_rd=1 */
       sleep(1);   /* wait 1 sec */
       send4_3in1(0,0);     /* set intg_rd=0 */
       sleep(1);   /* wait 1 sec */
       if(cadr != 0xfff) goto loop1;
    }

   if(mode == 2)
    {loop2:
       send4_3in1(1,1);     /* set itr=1 */
       sleep(1);   /* wait 1 sec */
       send4_3in1(1,0);     /* set itr=0 */
       sleep(1);   /* wait 1 sec */
       if(cadr != 0xfff) goto loop2;
    }

   if(mode == 3)
    {printf(" s1 s2 s3 s4 test using multi_sel instead of card_sel\n");
     loop3:
       send4_3in1(3,1);     /* set mse (multi select enable */
       cadr = 0;
       card_sel(cadr);
       multi_high();
       send13_3in1(2,0x5);   /* set s1=s3=1 s2=s4=0 */
       send13_3in1(2,0xa);   /* set s1=s3=0 s2=s4=1 */
       send13_3in1(2,0xf);     /* set s1=s2=s3=s4=1   */
       send13_3in1(2,0);     /* set s1=s2=s3=s4=0   */
       if(cadr != 0xfff) goto loop3;
    }

   if(mode == 4)
    {  send4_3in1(4,1);   /* enable tp pulses */
       send4_3in1(5,0);   /* disable ltp pulses */
       tval = (unsigned short)255;
       tim1_set(tval);
       tim2_set(tval);
loop4:
       rtime();
       tp_high();
       tp_low();
       usec=20;
       usleep(usec);
       if(cadr != 0xfff) goto loop4;
    }

   if(mode == 5)
    {  re5:
       printf("toggle which switch 1,2,3,or 4?\n");
       fscanf(stdin,"%d",&ll);
       if(ll<1) goto re5;
       if(ll>4) goto re5;
       i1=0xf;
       if(ll == 1) i2=0xe;
       if(ll == 2) i2=0xd;
       if(ll == 3) i2=0xb;
       if(ll == 4) i2=0x7;
       printf(" i1=%x  i2=%x\n",i1,i2);
loop5:
       send13_3in1(2,i1);
       sleep(1);
       send13_3in1(2,i2);
       sleep(1);
       if(cadr != 0xfff) goto loop5;
    }

   if(mode == 6)
    {loop6:
       for(i=0;i<1024;i++)
         {send13_3in1(6,i);  /* set dac value */
	  usec=2;
          usleep(usec);
         }   
       sleep(1);
       if(cadr != 0xfff) goto loop6;
    }

   if(mode == 7)
    {  multi_low();
       read_status(&sval);   /* read back the controller shift register */
       i = (int)sval;
       trig = (i>>3) & 1;
       s1 = (i>>4) & 1;
       s2 = (i>>5) & 1;
       s3 = (i>>6) & 1;
       s4 = (i>>7) & 1;
       itr = (i>>8) & 1;
       ire = (i>>9) & 1;
       mse = (i>>10) & 1;
       tplo = (i>>11) & 1;
       tphi = (i>>12) & 1;
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
    }

   if(mode == 8)
    {  send4_3in1(4,1);   /* enable calibration */
       printf("enter 0=standard 1 = 1/20 cal\n");
       fscanf(stdin,"%x",&ll);
       printf("level set to %x\n",ll);
       send4_3in1(5,ll);  
       printf("enter fine dac value (integer)\n");
       fscanf(stdin,"%d",&i);
       printf("fine dac set to %x\n",i);
       dval = (unsigned short)i;
       send13_3in1(6,dval);  /* set fine dac value */

loop8:
       usec=30;
       usleep(usec);
       for(i=0;i<256;i++)
        {loops:
         dval = (unsigned short)i;
         send13_3in1(6,dval);  /* set dac value */
         tp_high();
	 usec=10;
         usleep(usec);
         stime();
	 usec=10;
         usleep(usec);
         rtime();
         sw1=sw(1);
         if(sw1 == 1) goto loops;
         }
       sleep(1);
       if(cadr != 0xfff) goto loop8;
    }
 
   if(mode == 9)
    {loop9:
       multi_low();
       printf("\ns1..4=0 0 0 0 itr=0 ire=0 mse=0 tplo=0 tphi=0 trig=0 sent\n");
       send13_3in1(2,0);   /* s1=s2=s3=s4=0 */
       send4_3in1(0,0);   /* ire=0 */
       send4_3in1(1,0);   /* itr=0 */
       send4_3in1(3,0);   /* mse=0 */
       send4_3in1(4,0);   /* tplo=0 */
       send4_3in1(5,0);   /* tphi=0 */
       send4_3in1(7,0);   /* trig=0 */
       sleep(1);
       read_status(&sval);   /* read back the controller shift register */
       i = (int)sval;
       trig = (i>>3) & 1;
       s1 = (i>>4) & 1;
       s2 = (i>>5) & 1;
       s3 = (i>>6) & 1;
       s4 = (i>>7) & 1;
       itr = (i>>8) & 1;
       ire = (i>>9) & 1;
       mse = (i>>10) & 1;
       tplo = (i>>11) & 1;
       tphi = (i>>12) & 1;
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       printf("\ns1..4=1 1 1 1 itr=1 ire=1 mse=1 tplo=1 tphi=1 trig=1 sent\n");
       send13_3in1(2,0xf);   /* s1=s2=s3=s4=1 */
       send4_3in1(1,1);   /* itr=1 */
       send4_3in1(0,1);   /* ire=1 */
       send4_3in1(3,1);   /* mse=1 */
       send4_3in1(4,1);   /* tplo=1 */
       send4_3in1(5,1);   /* tphi=1 */
       send4_3in1(7,1);   /* trig=1 */
       read_status(&sval);   /* read back the controller shift register */
       i = (int)sval;
       trig = (i>>3) & 1;
       s1 = (i>>4) & 1;
       s2 = (i>>5) & 1;
       s3 = (i>>6) & 1;
       s4 = (i>>7) & 1;
       itr = (i>>8) & 1;
       ire = (i>>9) & 1;
       mse = (i>>10) & 1;
       tplo = (i>>11) & 1;
       tphi = (i>>12) & 1;
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       printf("\ns1..4=1 0 1 0 itr=1 ire=0 mse=1 tplo=0 tphi=1 trig=0 sent\n");
       send13_3in1(2,0x5);   /* s1=s3=1 s2=s4=0 */
       send4_3in1(1,1);   /* itr=1 */
       send4_3in1(0,0);   /* ire=0 */
       send4_3in1(3,1);   /* mse=1 */
       send4_3in1(4,0);   /* tplo=0 */
       send4_3in1(6,1);   /* tphi=1 */
       send4_3in1(7,0);   /* trig=0 */
       read_status(&sval);   /* read back the controller shift register */
       i = (int)sval;
       trig = (i>>3) & 1;
       s1 = (i>>4) & 1;
       s2 = (i>>5) & 1;
       s3 = (i>>6) & 1;
       s4 = (i>>7) & 1;
       itr = (i>>8) & 1;
       ire = (i>>9) & 1;
       mse = (i>>10) & 1;
       tplo = (i>>11) & 1;
       tphi = (i>>12) & 1;
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       printf("\ns1..4=0 1 0 0 itr=0 ire=0 mse=0 tplo=0 tphi=0 trig=1 sent\n");
       send13_3in1(2,2);   /* s2=1 s1=s3=s4=0 */
       send4_3in1(0,0);   /* ire=0 */
       send4_3in1(1,0);   /* itr=0 */
       send4_3in1(3,0);   /* mse=0 */
       send4_3in1(4,0);   /* tplo=0 */
       send4_3in1(5,0);   /* tphi=0 */
       send4_3in1(7,1);   /* trig=1 */
       sleep(1);
       read_status(&sval);   /* read back the controller shift register */
       i = (int)sval;
       trig = (i>>3) & 1;
       s1 = (i>>4) & 1;
       s2 = (i>>5) & 1;
       s3 = (i>>6) & 1;
       s4 = (i>>7) & 1;
       itr = (i>>8) & 1;
       ire = (i>>9) & 1;
       mse = (i>>10) & 1;
       tplo = (i>>11) & 1;
       tphi = (i>>12) & 1;
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       printf("\ns1..4=0 0 0 0 itr=0 ire=1 mse=0 tplo=0 tphi=0 trig=1 sent\n");
       send13_3in1(2,0);   /* s1=s2=s3=s4=0 */
       send4_3in1(0,1);   /* ire=1 */
       send4_3in1(1,0);   /* itr=0 */
       send4_3in1(3,0);   /* mse=0 */
       send4_3in1(4,0);   /* tplo=0 */
       send4_3in1(5,0);   /* tphi=0 */
       send4_3in1(7,1);   /* trig=1 */
       sleep(1);
       read_status(&sval);   /* read back the controller shift register */
       i = (int)sval;
       trig = (i>>3) & 1;
       s1 = (i>>4) & 1;
       s2 = (i>>5) & 1;
       s3 = (i>>6) & 1;
       s4 = (i>>7) & 1;
       itr = (i>>8) & 1;
       ire = (i>>9) & 1;
       mse = (i>>10) & 1;
       tplo = (i>>11) & 1;
       tphi = (i>>12) & 1;
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       if(cadr != 0xfff) goto loop9;
    }
 
   if(mode == 10)
    {  for(i=0;i<49;i++)
        { cadr = (unsigned short)( (zone<<12) | (sector<<6) | i );
          card_sel(cadr);
          send4_3in1(4,0);   /* disable tp pulses */
          send4_3in1(5,0);   /* disable ltp pulses */
         }
       cadr = (unsigned short)( (zone<<12) | (sector<<6) | card );
       card_sel(cadr);
       send4_3in1(4,1);   /* enable tp pulses */
       send4_3in1(5,0);   /* disable ltp pulses */
       multi_low();
loop10:
       dval = (unsigned short)255;
       send13_3in1(6,dval);  /* set fine dac value */
       usec=30;
       usleep(usec);
       send13_3in1(6,dval);  /* set dac value */
       tp_high();
       usec=10;
       usleep(usec);
       stime();
       usec=10;
       usleep(usec);
       rtime();
       sw1=sw(1);
       if(sw1 == 0) goto loop10;
    }

   if(mode == 11)
    {  send4_3in1(4,1);   /* enable small C */
       send4_3in1(5,0);   /* disable large C */
       printf("input coarse timer value (0-255)\n");
       fscanf(stdin,"%d",&mode);
       t1val = (unsigned short)mode;
       printf("input fine timer value (0-255)\n");
       fscanf(stdin,"%d",&mode);
       t2val = (unsigned short)mode;
       printf(" coarse=%x fine=%x\n",(int)t1val,(int)t2val);
loop11:
       tim1_set(t1val);
       tim2_set(t2val);
       rtime();
       usec=10;
       usleep(usec);
       tp_high();   /* set tp high  */
       stime();     /* start timers - tp goes low when timers finish*/
       usec=50;
       usleep(usec);
       if(cadr != 0xfff) goto loop11;
    }

   if(mode == 12)
    {  send4_3in1(4,1);   /* enable small C pulses */
       send4_3in1(5,0);   /* disable large C pulses */
       printf("input dac setting (integer)\n");
       fscanf(stdin,"%d",&ll);
       dval = (unsigned short)ll;      
       send13_3in1(6,dval);  /* set fine dac value */

loop12:
       usec=30;
       usleep(usec);
       tp_high();
       usec=10;
       usleep(usec);
       stime();
       usec=10;
       usleep(usec);
       rtime();
       sw1=sw(1);
       if(sw1 == 0) goto loop12;
    }
 
   if(mode == 13)
    {  expr[0]=1.0;
       expr[1]=2.7;
       expr[2]=3.7;
       expr[3]=7.2;
       expr[4]=9.7;
       expr[5]=13.2;
       printf(" input dac value\n");
       fscanf(stdin,"%d",&ll);
       send13_3in1(6,ll);  /* set dac value */
       printf(" input intg_rd value 0 or 1\n");
       fscanf(stdin,"%d",&ll);
       send4_3in1(0,ll);
       printf(" input itr value 0 or 1\n");
       fscanf(stdin,"%d",&ll);
       send4_3in1(1,ll);
loop13:
       isw[0]=1;
       send13_3in1(2,1);
       printf(" enter s4=0 s3=0 s2=0 s1=1 output volt[0] value\n");
       fscanf(stdin,"%lf",&volt[0]);
       fmin=volt[0];

       isw[1]=2;
       send13_3in1(2,2);
       printf(" enter s4=0 s3=0 s2=1 s1=0 output volt[0] value\n");
       fscanf(stdin,"%lf",&volt[1]);
       if(volt[1]<fmin) fmin=volt[1];

       isw[2]=0;
       send13_3in1(2,0);
       printf(" enter s4=0 s3=0 s2=0 s1=0 output volt[0] value\n");
       fscanf(stdin,"%lf",&volt[2]);
       if(volt[2]<fmin) fmin=volt[2];

       isw[3]=4;
       send13_3in1(2,4);
       printf(" enter s4=0 s3=1 s2=0 s1=0 output volt[0] value\n");
       fscanf(stdin,"%lf",&volt[3]);
       if(volt[3]<fmin) fmin=volt[3];

       isw[4]=8;
       send13_3in1(2,8);
       printf(" enter s4=1 s3=0 s2=0 s1=0 output volt[0] value\n");
       fscanf(stdin,"%lf",&volt[4]);
       if(volt[4]<fmin) fmin=volt[4];

       isw[5]=0xc;
       send13_3in1(2,0xc);
       printf(" enter s4=1 s3=1 s2=0 s1=0 output volt[0] value\n");
       fscanf(stdin,"%lf",&volt[5]);
       if(volt[5]<fmin) fmin=volt[5];

       for(i=0;i<6;i++)
        { ratio=volt[i]/fmin;
          printf(" i=%d  isw=%x volt=%f  ratio=%f expected ratio=%f\n",
            i,isw[i],volt[i],ratio,expr[i]);}

       if(cadr != 0xfff) goto loop13;
    }

   if(mode == 14)
    {  expr[0]=1.0;
       expr[1]=2.7;
       expr[2]=3.7;
       expr[3]=7.2;
       expr[4]=9.7;
       expr[5]=13.2;
loop14:
       printf(" input dac value\n");
       fscanf(stdin,"%d",&dacset);
       send4_3in1(0,1);  /* intg_rd=1 */
       send4_3in1(1,1);  /* itr=1 */

       isw[0]=1;
       send13_3in1(2,1);
       send13_3in1(6,0);  /* set dac value=0 */
       sleep(2);
       go_get(&ped[0]);
       send13_3in1(6,dacset);  /* set dac value */
       sleep(2);
       go_get(&volt[0]);
       printf("returned volt[0]=%f ped[0]=%f\n",volt[0],ped[0]);
       fmin=volt[0]-ped[0];

       isw[1]=2;
       send13_3in1(2,2);
       send13_3in1(6,0);  /* set dac value=0 */
       sleep(2);
       go_get(&ped[1]);
       send13_3in1(6,dacset);  /* set dac value */
       sleep(2);
       go_get(&volt[1]);
       printf("returned volt[1]=%f ped[1]=%f\n",volt[1],ped[1]);
       if(volt[1]-ped[1]<fmin) fmin=volt[1]-ped[1];

       isw[2]=0;
       send13_3in1(2,0);
       send13_3in1(6,0);  /* set dac value=0 */
       sleep(2);
       go_get(&ped[2]);
       send13_3in1(6,dacset);  /* set dac value */
       sleep(2);
       go_get(&volt[2]);
       printf("returned volt[2]=%f ped[2]=%f\n",volt[2],ped[2]);
       if(volt[2]-ped[2]<fmin) fmin=volt[2]-ped[2];

       isw[3]=4;
       send13_3in1(2,4);
       send13_3in1(6,0);  /* set dac value=0 */
       sleep(2);
       go_get(&ped[3]);
       send13_3in1(6,dacset);  /* set dac value */
       sleep(2);
       go_get(&volt[3]);
       printf("returned volt[3]=%f ped[3]=%f\n",volt[3],ped[3]);
       if(volt[3]-ped[3]<fmin) fmin=volt[3]-ped[3];

       isw[4]=8;
       send13_3in1(2,8);
       send13_3in1(6,0);  /* set dac value=0 */
       sleep(2);
       go_get(&ped[4]);
       send13_3in1(6,dacset);  /* set dac value */
       sleep(2);
       go_get(&volt[4]);
       printf("returned volt[4]=%f ped[4]=%f\n",volt[4],ped[4]);
       if(volt[4]-ped[4]<fmin) fmin=volt[4]-ped[4];

       isw[5]=0xc;
       send13_3in1(2,0xc);
       send13_3in1(6,0);  /* set dac value=0 */
       sleep(2);
       go_get(&ped[5]);
       send13_3in1(6,dacset);  /* set dac value */
       sleep(2);
       go_get(&volt[5]);
       printf("returned volt[5]=%f ped[5]=%f\n",volt[5],ped[5]);
       if(volt[5]-ped[5]<fmin) fmin=volt[5]-ped[5];

       for(i=0;i<6;i++)
        { ratio=(volt[i]-ped[i])/fmin;
          printf(" i=%d  isw=%x volt=%f ped=%f  ratio=%f expected ratio=%f\n",
            i,isw[i],volt[i],ped[i],ratio,expr[i]);}

       send4_3in1(0,1);  /* intg_rd=1 */
       sleep(2);
       go_get(&volt[5]);
       send4_3in1(0,0);  /* intg_rd=0 */
       sleep(2);
       go_get(&ped[5]);
       printf("intg_rd on=%f intg_rd off=%f\n",volt[5],ped[5]);

       send4_3in1(0,1);  /* intg_rd=1 */
       send4_3in1(1,0);  /* itr=0 */
       sleep(2);
       go_get(&ped[5]);
       printf("itr on=%f itr off=%f\n",volt[5],ped[5]);
       if(cadr != 0xfff) goto loop14;
    }

   if(mode == 15)
    {  for(i=0;i<49;i++)
       { cadr = (unsigned short)( (zone<<12) | (sector<<6) | i );
         card_sel(cadr);
         send4_3in1(3,1);     /* set mse (multi select enable) */
        }
       cadr = 0;
       card_sel(cadr);
       multi_high();
       send4_3in1(4,1);   /* enable tp pulses */
       send4_3in1(5,0);   /* disable ltp pulses */
loop15:
       dval = (unsigned short)255;
       send13_3in1(6,dval);  /* set fine dac value */
       usec=30;
       usleep(usec);
       send13_3in1(6,dval);  /* set dac value */
       tp_high();
       usec=10;
       usleep(usec);
       stime();
       usec=10;
       usleep(usec);
       rtime();
       sw1=sw(1);
       if(sw1 == 0) goto loop15;
    }
   if(mode == 16)
    { cadr = (unsigned short)( (zone<<12) | (sector<<6) | card );
      card_sel(cadr);
      printf("test 16\n");
loop16:
      for(i=0;i<=0x1fff;i++)
        { sval = (unsigned short)0x1fff;
          shift_set(sval);
          shift_read(&dval);
/*          printf(" error sval=%x  dval=%x\n",(int)sval,(int)dval);  */
          if(sval != (dval&0x1fff) ) printf(" error sval=%x  dval=%x\n",
                  (int)sval,(int)dval);
         }
      goto loop16;
    }

   if(mode == 17)
    { while(sw(1)==0)
      {for(i=0;i<=0x1fff;i++)
        { sval = (unsigned short)i;
          wrt_shift(13,sval);
          read_shift(&dval);
          if(sval != (dval&0x1fff) ) printf(" error i=%x  sval=%x  dval=%x\n",
                  (int)i,(int)sval,(int)dval);
         }
      }
    }

   if(mode == 18)
     { loop18:
       cadr = (unsigned short)( (zone<<12) | (sector<<6) | card );
       card_sel(cadr);
       usec=10;
       usleep(usec);
       cadr = (unsigned short) 0;
       card_sel(cadr);
       usec=10;
       usleep(usec);
       if(card != 99) goto loop18;
     }

   if(mode == 19)
      {multi_low();
loop19:
       for(l=0;l<49;l++)
        { cadr = (unsigned short)( (zone<<12) | (sector<<6) | l );
          card_sel(cadr);
          send13_3in1(2,0x5);   /* s1=s3=1 s2=s4=0 */
          send4_3in1(1,1);   /* itr=1 */
          send4_3in1(0,0);   /* ire=0 */
          send4_3in1(3,1);   /* mse=1 */
          send4_3in1(4,0);   /* tplo=0 */
          send4_3in1(5,1);   /* tphi=1 */
	  send4_3in1(7,1);   /* trig=1 */
          usec=5;
          usleep(usec);
          read_status(&sval);   /* read back the controller shift register */
          i = (int)sval;
       trig = (i>>3) & 1;
       s1 = (i>>4) & 1;
       s2 = (i>>5) & 1;
       s3 = (i>>6) & 1;
       s4 = (i>>7) & 1;
       itr = (i>>8) & 1;
       ire = (i>>9) & 1;
       mse = (i>>10) & 1;
       tplo = (i>>11) & 1;
       tphi = (i>>12) & 1;
         if(l == card) 
       sw1 = sw(1);
       if(sw1 == 1) 
       printf("card=%d s1-4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d\n",
              l,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig); 
         if( (s1==1) && (s2==0) && (s3==1) && (s4==0) && (ire==0)
            && (itr==1) && (mse==1) && (tplo==0) && (tphi==1) && (trig==1) )
             printf("card %d is there %x\n",l,i);
        }
       printf("end of loop\n");
       sleep(1);
       if(card != 99) goto loop19;
      }

   if(mode == 20)
    {loop20:
       send4_3in1(0,1);     /* set intg_rd=1 */
       send4_3in1(1,1);     /* set itr=1 */
       for(i=0;i<256;i++)
         {send13_3in1(6,i);  /* set dac value */
          usec=10;
          usleep(usec);
         }  
       usec=20;
       usleep(usec);
       if(cadr != 0xfff) goto loop20;
    }
   if(mode == 21)
    {loop21:
       printf("enter dac value (hex)\n");
       fscanf(stdin,"%x",&i);
       printf("dac set to %x hex\n",i);
       dval = (unsigned short)i;
       send13_3in1(6,dval);  /* set dac value */
       goto loop21;
    }
   if(mode == 22)
    { send13_3in1(6,255);  /* set fine dac value */
loop22:
       send13_3in1(6,255);  /* set dac value */
       usec=5;
       usleep(usec);
       send13_3in1(6,127);  /* set dac value */
       usec=5;
       usleep(usec);
       goto loop22;
    }
    
   if(mode == 23)
    { while(sw(1)==0)
      {for(i=0;i<=0xfff;i++)
        { sval = (unsigned short)0x1555;
          wrt_shift(13,sval);
          read_shift(&dval);
          if(sval != (dval&0x1fff) ) printf(" error sval=%x  dval=%x\n",
                  (int)sval,(int)dval);
          sval = (unsigned short)0xaaa;
          wrt_shift(13,sval);
          read_shift(&dval);
          if(sval != (dval&0x1fff) ) printf(" error sval=%x  dval=%x\n",
                  (int)sval,(int)dval);
         }
      }
    }

   if(mode == 24)
    {loop24:
       clk_high();
       usleep((time_t)1);
       clk_low();
       usleep((time_t)1);
       goto loop24;
    }
   if(mode == 25)
    {  for(i=0;i<49;i++)
        { cadr = (unsigned short)( (zone<<12) | (sector<<6) | i );
          card_sel(cadr);
          send4_3in1(4,0);   /* disable tp pulses */
          send4_3in1(5,0);   /* disable ltp pulses */
         }
       cadr = (unsigned short)( (zone<<12) | (sector<<6) | card );
       card_sel(cadr);
       send4_3in1(4,1);   /* enable tp pulses */
       send4_3in1(5,0);   /* disable ltp pulses */
       multi_low();
       dval = (unsigned short)255;
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
   if(mode == 26)
    { while(sw(1)==0)
      {for(i=0;i<=0x1fff;i++)
        { sval = (unsigned short)0x1800;
          wrt_shift(13,sval);
          read_shift(&dval);
          if(sval != (dval&0x1fff) ) printf(" error i=%x  sval=%x  dval=%x\n",
                  (int)i,(int)sval,(int)dval);
         }
      }
    }
   return(0);
}

    go_get(v)
      double *v;
      {double volts;
       if(ent == 0)
         {gpib_reset();
          usleep((time_t)100000);

/*          printf("setting T9\n");
          sendmessage(gpib_adr,"T9 X");
          usleep((time_t)100000);
          sendmessage(gpib_adr,"T? X");
          getmessage(gpib_adr, &string[0] );
          printf("T9 back=%s\n",string);  */

          sendmessage(gpib_adr,"A2 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"A? X");
          getmessage(gpib_adr, &string[0] );
          printf("A2 back=%s\n",string);  */

          sendmessage(gpib_adr,"C1 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"C? X");
          getmessage(gpib_adr, &string[0] );
          printf("C1 back=%s\n",string);  */
  
          sendmessage(gpib_adr,"R3 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"R? X");
          getmessage(gpib_adr, &string[0] );
          printf("R3 back=%s\n",string);    */

          sendmessage(gpib_adr,"N1 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"N? X");
          getmessage(gpib_adr, &string[0] );
          printf("N1 back=%s\n",string);   */ 

          sendmessage(gpib_adr,"G0 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"G? X");
          getmessage(gpib_adr, &string[0] );
          printf("G0 back=%s\n",string);   */
  
          sendmessage(gpib_adr,"Q0 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"Q? X");
          getmessage(gpib_adr, &string[0] );
          printf("Q0 back=%s\n",string);    */

          ent=1;
         }

          sendmessage(gpib_adr,"T6 X");   /* trigger voltage reading */

          usleep((time_t)5000);
          getmessage(gpib_adr, &string[0] );
/*          printf("first readback=%c%c%c%c%c%c%c%c%c%c\n",
              string[0],string[1],string[2],string[3],string[4],
	      string[5],string[6],string[7],string[8],string[9]);   */    
/*          printf("first readback=%s\n",string);  */
          sscanf(string,"%le",&volts);
/*          printf("volts=%e\n",volts);   */ 

          *v=volts;
          return(0);
}


