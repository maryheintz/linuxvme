#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <time.h>
time_t usec;
int id,ttcadr;
main(ttcadr)
{ 
   int tube,mode,zone_bar,sector_bar,dac,i;
   unsigned int k,tpmode;
   unsigned long klong;
   int board;
   char serial[7] = {"S10117\0"};
   ttcadr = 0x300a;
/*   printf(" enter ttcadr in hex\n");
   fscanf(stdin,"%x",&ttcadr);
   printf("ttcadr = %x\n",ttcadr);  */ 
   board = 1;
   id = (board<<6) | 1;
   ttcvi_map(ttcadr);
   reset_ctrl_can();
   printf("calling can_init\n");
   can_reset(ttcadr);  
   can_init(board,serial);  
   printf(" enter tube\n");
   fscanf(stdin,"%d",&tube);
   printf("tube = %d\n",tube);  
   set_tube(ttcadr,tube);
   multi_low(ttcadr);
loop:
   printf(" select desired test - loop\n");
   printf("  1 = toggle tp bcst\n");
   printf("  2 = toggle tp modes\n");
   printf("  3 = toggle multi_sel\n");
   printf("  4 = toggle rxw\n");
   printf("  5 = toggle back_load\n");
   printf("  6 = toggle tp LONG\n");
   printf("  7 = toggle tube\n");
   printf("  8 = set tp high LONG\n");
   printf("  9 = set tp low BC\n");
   printf(" 10 = toggle intg_rd\n");
   printf(" 11 = toggle itr\n");
   printf(" 12 = toggle s1\n");
   printf(" 13 = toggle s2\n");
   printf(" 14 = toggle s3\n");
   printf(" 15 = toggle s4\n");
   printf(" 16 = set dac to hex value\n");
   printf(" 17 = set dac loop\n");
   printf(" 18 = toggle small C enable\n");
   printf(" 19 = toggle large C enable\n");
   printf(" 20 = toggle trig enable\n");
   printf(" 21 = pulse with large C\n");
   printf(" 22 = pulse with small C\n");
   printf(" 23 = pulse with multi_sel alternate size\n");
   printf(" 24 = toggle TP LONG-BC\n");
   printf(" 25 = toggle cards 1 & 2 intg_rd with multi_sel\n");
   printf(" 26 = send long word\n");
   printf(" 27 = cycle phase of clock40des1\n");
   printf(" 28 = cycle phase of clock40des2\n");
   printf(" 29 = toggle tp cycling phase of clock40des2\n");
   printf(" 30 = set MB1 timer to hex value\n");
   printf(" 31 = pulse with large C ramp dac\n");

   fscanf(stdin,"%d",&mode);

   if(mode == 1)
    {loop1:
      tp_low_bc();
      sleep(1);
      tp_high_bc();
      sleep(1);
      if(sw(1) != 1) goto loop1;
    }

   if(mode == 2)
    {
loop2:
     printf(" enter 1=low_bc 2=high_bc 3=low_long 4=high_long\n");
     fscanf(stdin,"%d",&tpmode);
      if(tpmode == 1) tp_low_bc();
      if(tpmode == 2) tp_high_bc();
      if(tpmode == 3) tp_low(ttcadr);
      if(tpmode == 4) tp_high(ttcadr);
      goto loop2;
    }

   if(mode == 3)
    {loop3:
      multi_high(ttcadr);
      sleep(1);
      multi_low(ttcadr);
      sleep(1);
      if(sw(1) != 1) goto loop3;
      multi_low(ttcadr);
    }

   if(mode == 4)
    {loop4:
      rxw_high(ttcadr);
      sleep(1);
      rxw_low(ttcadr);
      sleep(1);
      if(sw(1) != 1) goto loop4;
      rxw_low(ttcadr);
    }

   if(mode == 5)
    {loop5:
      back_high(ttcadr);
      sleep(1);
      back_low(ttcadr);
      sleep(1);
      if(sw(1) != 1) goto loop5;
      back_low(ttcadr);
    }

   if(mode == 6)
    {reset_state(ttcadr);
loop6:
      tp_high(ttcadr);
      sleep(1);
      tp_low(ttcadr);
      sleep(1);
      if(sw(1) != 1) goto loop6;
      tp_low(ttcadr);
    }

   if(mode == 7)
    {loop7:
      set_tube(ttcadr,0);
      sleep(1);
      set_tube(ttcadr,tube);
      sleep(1);
      if(sw(1) != 1) goto loop7;
      set_tube(ttcadr,tube);
    }

   if(mode == 8)
    { tp_high(ttcadr);
      goto loop;
    }

   if(mode == 9)
    { tp_low_bc();
      goto loop;
    }

   if(mode == 10)
    { set_dac(ttcadr,0xff);
      itr_low(ttcadr);
      set_intg_gain(ttcadr,0xe);
loop10:
      intg_rd_high(ttcadr);
      sleep(1);
      intg_rd_low(ttcadr);
      sleep(1);
      if(sw(1) != 1) goto loop10;
    }

   if(mode == 11)
    {printf("starting itr test\n");
    loop11:
      set_dac(ttcadr,0xff);
      itr_high(ttcadr);
      sleep(1);
      itr_low(ttcadr);
      sleep(1);
      if(sw(1) != 1) goto loop11;
    }

   if(mode == 12)
    {printf("starting s1 test\n");
    loop12:
      set_intg_gain(ttcadr,1);
      sleep(1);
      set_intg_gain(ttcadr,0);
      sleep(1);
      if(sw(1) != 1) goto loop12;
    }

   if(mode == 13)
    {printf("starting s2 test\n");
    loop13:
      set_intg_gain(ttcadr,2);
      sleep(1);
      set_intg_gain(ttcadr,0);
      sleep(1);
      if(sw(1) != 1) goto loop13;
    }

   if(mode == 14)
    {printf("starting s3 test\n");
    loop14:
      set_intg_gain(ttcadr,4);
      sleep(1);
      set_intg_gain(ttcadr,0);
      sleep(1);
      if(sw(1) != 1) goto loop14;
    }

   if(mode == 15)
    { printf("starting s4 test\n");
    loop15:
      set_intg_gain(ttcadr,8);
      sleep(1);
      set_intg_gain(ttcadr,0);
      sleep(1);
      if(sw(1) != 1) goto loop15;
    }

   if(mode == 16)
    {loop16:
     printf("enter hex dac value\n");
     fscanf(stdin,"%x",&k);
     set_dac(ttcadr,k);
     if(sw(1) != 1) goto loop16;
    }

   if(mode == 17)
    {loop17:
     printf("starting set dac loop\n");
      for(k=0;k<0x3ff;k=k+10)
        { printf("k=%d\n",k);
	  set_dac(ttcadr,k);
	  usec=500000;
          usleep(usec);
        }
      if(sw(1) != 1) goto loop17;
    }

   if(mode == 18)
    {printf("toggle small C enable\n");
    loop18:
      small_cap_enable(ttcadr);
      sleep(1);
      small_cap_disable(ttcadr);
      sleep(1);
      if(sw(1) != 1) goto loop18;
    }

   if(mode == 19)
    {printf("toggle large C enable\n");
    loop19:
      large_cap_enable(ttcadr);
      sleep(1);
      large_cap_disable(ttcadr);
      sleep(1);
      if(sw(1) != 1) goto loop19;
    }

   if(mode == 20)
    {printf("toggle trig enable\n");
    loop20:
      trig_enable(ttcadr);
      sleep(1);
      trig_disable(ttcadr);
      sleep(1);
      if(sw(1) != 1) goto loop20;
    }

/* pulse large C */
   if(mode == 21)
     { usleep(1000);
       large_cap_enable(ttcadr);
       usleep(1000);
       small_cap_disable(ttcadr);
       usleep(1000);
       printf("enter dac hex value\n");
       fscanf(stdin,"%x",&dac);
       set_dac(ttcadr,dac);
       usleep(1000);
loop21:
      tp_high(ttcadr);
      usleep(10000);
      tp_low_bc(ttcadr);  
      /*  tp_low(ttcadr);  */
      usleep(100000);
      if(sw(1) != 1) goto loop21;
     }

/* pulse small C */
   if(mode == 22)
     { usleep(1000);
       small_cap_enable(ttcadr);
       usleep(1000);
       large_cap_disable(ttcadr);
       usleep(1000);
       printf("enter dac hex value\n");
       fscanf(stdin,"%x",&dac);
       set_dac(ttcadr,dac);
       usleep(1000);
loop22:
      tp_high(ttcadr);
      usleep(1000);
      tp_low_bc(ttcadr);
      usleep(1000);
      if(sw(1) != 1) goto loop22;
     }

/* pulse using multi_sel - alternate size */
   if(mode == 23)
     { for(i=1;i<49;i++)
         {set_tube(ttcadr,i);
	  mse_low(ttcadr);
          small_cap_disable(ttcadr);
          large_cap_disable(ttcadr);
	 }
       set_tube(ttcadr,tube);
       usleep(1000);
       large_cap_enable(ttcadr);
       usleep(1000);
       small_cap_disable(ttcadr);
       usleep(1000);
       mse_high(ttcadr);
       multi_high(ttcadr);
       set_tube(ttcadr,0);
loop23:
       set_dac(ttcadr,0x3ff);
       usleep(1000);
       tp_high(ttcadr);
       usleep(1000);
       tp_low_bc(ttcadr);
       usleep(1000);
       set_dac(ttcadr,0x200);
       usleep(1000);
       tp_high(ttcadr);
       usleep(1000);
       tp_low_bc(ttcadr);
       usleep(1000);
       if(sw(1) != 1) goto loop23;
       multi_low(ttcadr);
       set_tube(ttcadr,tube);
     }
     
   if(mode == 24)
    { printf(" toggle tp LONG-BC\n");
loop24:
      tp_high(ttcadr);
      sleep(1);
      tp_low_bc();
      sleep(1);
      if(sw(1) != 1) goto loop24;
      tp_low(ttcadr);
    }

   if(mode == 25)
    { multi_low(ttcadr);
      set_tube(ttcadr,1);
      mse_high(ttcadr);
      set_tube(ttcadr,2);
      mse_high(ttcadr);
      multi_high(ttcadr);
 loop25:
      set_tube(ttcadr,0);
      intg_rd_high(ttcadr);
      set_tube(ttcadr,1);
      sleep(1);
      set_tube(ttcadr,2);
      sleep(1);
      set_tube(ttcadr,0);
      intg_rd_low(ttcadr);
      set_tube(ttcadr,1);
      sleep(1);
      set_tube(ttcadr,2);
      sleep(1);
      goto loop25;
    }

   if(mode == 26)
     {
/*    printf("enter hex dac value\n");
     fscanf(stdin,"%x",&klong);  */
loop26:
     SendLongCommand(0,1,0x55,0xaa);
     if(sw(1) != 1) goto loop26;
    }

   if(mode == 27)
    {
    loop27:
      for(i=0;i<240;i++)
      {  SetTimeDes1(0,i);
         usleep(10);
      }
     if(sw(1) != 1) goto loop27;
    }


   if(mode == 28)
    {
    loop28:
      for(i=0;i<240;i++)
      {  SetTimeDes2(0,i);
         usleep(10);
      }
     if(sw(1) != 1) goto loop28;
    }

   if(mode == 29)
    {
    loop29:
      for(i=0;i<240;i++)
      {  SetTimeDes2(0,i);
         usleep(2);
	 tp_high(ttcadr);
	 trig_enable(ttcadr);
	 usleep(2);
	 tp_low_bc(ttcadr);
	 trig_disable(ttcadr);
	 usleep(2);
	 printf("i=%x\n",i);
      }
     if(sw(1) != 1) goto loop29;
    }

   if(mode == 30)
    {loop30:
     printf("enter hex timer value\n");
     fscanf(stdin,"%x",&k);
     set_mb_time(ttcadr,1,k);
     set_tube(ttcadr,tube);
     if(sw(1) != 1) goto loop30;
    }

/* pulse large C */
   if(mode == 31)
     { usleep(1);
       large_cap_enable(ttcadr);
       usleep(1000);
       small_cap_disable(ttcadr);
loop31:
       usleep(1);
       for(dac=0;dac<0xff;dac++)
        { set_dac(ttcadr,dac);
          usleep(1);
          tp_high(ttcadr);
          usleep(10);
          tp_low_bc(ttcadr);  
          usleep(1);
	}
      if(sw(1) != 1) goto loop31;
     }

   goto loop;
}
pstat()
  {int i,status;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,ok;
   ok=0;
   status=0;
   back_high(ttcadr);
   back_low(ttcadr);
   load_can(ttcadr);   /* fetch from 3in1 */
   status=can_3in1get(id,&shrt);    /* and load can output reg */
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
      (trig==0) ) ok++;
   printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x %d\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i,ok);
  }


