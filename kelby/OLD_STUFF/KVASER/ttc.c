#include <stdio.h>
#include <math.h>
#include "vme_base.h"
int id,ttcadr;
char string[80];
char adcno[7];
main()
{  char name[7];
   int tube,mode,dac,i;
   unsigned int k;
   unsigned long klong;
   int board,cd1,cd2;
   unsigned long status;
   FILE *fp;
   status = vme_a16_base();
   status = vme_a24_base();
   printf("enter ttc address\n");
   fscanf(stdin,"%x",&ttcadr);  
   fp=fopen("/home/kelby/TTCV3/adc.num","r");
   fscanf(fp,"%s",adcno);
   printf("%x  %s\n",ttcadr,adcno);
   board = 1;
   id = (board<<6) | 1; 
   ttcvi_mapr(ttcadr);
   reset_ctrl_can();
   printf("calling can_reset - this takes a few seconds\n");
   can_reset(ttcadr);
   printf("step 1\n");
   can_init(board,adcno);

   printf(" enter tube\n");
   fscanf(stdin,"%d",&tube);
   printf("tube = %d\n",tube);  
   set_tube(ttcadr,tube);
   multi_low(ttcadr);
loop:
   printf(" select desired test - loop\n");
   printf("  1 = toggle tp CANF\n");
   printf("  2 = set tp to state\n");
   printf("  3 = toggle multi_sel\n");
   printf("  4 = toggle rxw\n");
   printf("  5 = toggle back_load\n");
   printf("  6 = toggle tp LONG\n");
   printf("  7 = toggle tube\n");
   printf("  8 = toggle tp short\n");
   printf("  9 = set can_reset line\n");
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
   printf(" 31 = loop through tube adr\n");
   printf(" 32 = loop send can resets\n");
   printf(" 33 = toggle dac 0 or 0x3ff\n");
   printf(" 34 = toggle between 2 tubes selected\n");
   printf(" 35 = toggle everything 1 sec interval\n");

   fscanf(stdin,"%d",&mode);

   if(mode == 1)
    {loop1:
      can_tp_low(0);
      sleep(1);
      can_tp_high(0);
      sleep(1);
      if(sw(1) != 1) goto loop1;
    }

   if(mode == 2)
    { loop2:
      printf("enter mode\n");
      printf("    1- tp_high_long\n");
      printf("    2- tp_low_long\n");
      printf("    3- tp_high_short\n");
      printf("    4- tp_low_short\n");
      fscanf(stdin,"%d",&i);
      if(i == 1) tp_high(ttcadr);
      if(i == 2) tp_low(ttcadr);
      if(i == 3) tp_high_bc();
      if(i == 4) tp_low_bc();
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
      usleep(5000);
      tp_low(ttcadr);
      usleep(5000);
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
    { loop8:
      tp_low_bc();
      sleep(1);
      tp_high_bc();
      sleep(1);
      goto loop8;
    }

   if(mode == 9)
    { loop9:
      printf("enter mode\n");
      printf("    1- can_reset high long\n");
      printf("    2- can_reset low long\n");
      printf("    3- can_reset high short\n");
      printf("    4- can_reset low short\n");
      fscanf(stdin,"%d",&i);
      if(i == 1) SendLongCommand(ttcadr,1,0xdc,1);
      if(i == 2) SendLongCommand(ttcadr,1,0xdc,0);;
      if(i == 3) BroadcastShortCommand(0xcc);
      if(i == 4) BroadcastShortCommand(0xc8);
      goto loop9;
    }

   if(mode == 10)
    {loop10:
      intg_rd_high(ttcadr);
      usleep(1000);
      intg_rd_low(ttcadr);
      usleep(1000);
      if(sw(1) != 1) goto loop10;
    }

   if(mode == 11)
    {printf("starting itr test\n");
    loop11:
      itr_high(ttcadr);
      sleep(1);
      itr_low(ttcadr);
      sleep(1);
      if(sw(1) != 1) goto loop11;
    }

   if(mode == 12)
    {printf("starting s1 test\n");
    set_dac(ttcadr,0x3ff);
    loop12:
      set_intg_gain(ttcadr,1);
      sleep(1);
      set_intg_gain(ttcadr,0);
      sleep(1);
      if(sw(1) != 1) goto loop12;
    }

   if(mode == 13)
    {printf("starting s2 test\n");
    set_dac(ttcadr,0x3ff);
    loop13:
      set_intg_gain(ttcadr,2);
      sleep(1);
      set_intg_gain(ttcadr,0);
      sleep(1);
      if(sw(1) != 1) goto loop13;
    }

   if(mode == 14)
    {printf("starting s3 test\n");
    set_dac(ttcadr,0x3ff);
    loop14:
      set_intg_gain(ttcadr,4);
      sleep(1);
      set_intg_gain(ttcadr,0);
      sleep(1);
      if(sw(1) != 1) goto loop14;
    }

   if(mode == 15)
    { printf("starting s4 test\n");
    set_dac(ttcadr,0x3ff);
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
      for(k=0;k<0x400;k=k+1)
        { printf("k=%x\n",k);
	  set_dac(ttcadr,k);
          usleep(1000);
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
     { usleep(5000);
       large_cap_enable(ttcadr);
       usleep(5000);
       small_cap_disable(ttcadr);
       usleep(5000);
       printf("enter dac hex value\n");
       fscanf(stdin,"%x",&dac);
       set_dac(dac);
       usleep(5000);
loop21:
      tp_high(ttcadr);
      usleep(5000);
      tp_low_bc(ttcadr);  
      /*  tp_low(ttcadr);  */
      usleep(5000);
      if(sw(1) != 1) goto loop21;
     }

/* pulse small C */
   if(mode == 22)
     { usleep(5000);
       small_cap_enable(ttcadr);
       usleep(5000);
       large_cap_disable(ttcadr);
       usleep(5000);
       printf("enter dac hex value\n");
       fscanf(stdin,"%x",&dac);
       set_dac(dac);
       usleep(5000);
loop22:
      tp_high(ttcadr);
      usleep(5000);
      tp_low_bc(ttcadr);
      usleep(5000);
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
       usleep(5000);
       large_cap_enable(ttcadr);
       usleep(5000);
       small_cap_disable(ttcadr);
       usleep(5000);
       mse_high(ttcadr);
       multi_high(ttcadr);
       set_tube(ttcadr,0);
loop23:
       set_dac(0x3ff);
       usleep(5000);
       tp_high(ttcadr);
       usleep(5000);
       set_dac(0x200);
       usleep(5000);
       tp_high(ttcadr);
       usleep(5000);
       tp_low_bc(ttcadr);
       usleep(5000);
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
         usleep(5000);
      }
     if(sw(1) != 1) goto loop27;
    }


   if(mode == 28)
    {
    loop28:
      for(i=0;i<240;i++)
      {  SetTimeDes2(0,i);
         usleep(5000);
      }
     if(sw(1) != 1) goto loop28;
    }

   if(mode == 29)
    {
    loop29:
      for(i=0;i<240;i++)
      {  SetTimeDes2(0,i);
       usleep(5000);
	 tp_high(ttcadr);
	 trig_enable(ttcadr);
       usleep(5000);
	 tp_low_bc(ttcadr);
	 trig_disable(ttcadr);
       usleep(5000);
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

   if(mode == 31)
    {loop31:
      for(i=0;i<64;i++)
        { set_tube(ttcadr,i);
          usleep(1000);
        }
      if(sw(1) != 1) goto loop31;
      set_tube(ttcadr,tube);
    }

   if(mode == 32)
    {
loop32:
      reset_can(ttcadr,1);
      usleep(5000);
      reset_can(ttcadr,0);
      usleep(5000);
      if(sw(1) != 1) goto loop32;
    }

   if(mode == 33)
    {
     printf("starting set dac toggle\n");
      itr_low(ttcadr);
      intg_rd_low(ttcadr);
      set_intg_gain(ttcadr,0x7);
     loop33:
	  set_dac(ttcadr,0);
          usleep(0250);
	  set_dac(ttcadr,0x3ff);
          usleep(0250);
      if(sw(1) != 1) goto loop33;
    }

   if(mode == 34)
    {
     printf("enter tube numbers\n");
     fscanf(stdin,"%d %d",&cd1,&cd2);
loop34:
   set_tube(ttcadr,cd1);
   usleep(0100);
   set_tube(ttcadr,cd2);
   usleep(0100);
   if(sw(1) != 1) goto loop34;
    }

   if(mode == 35)
    { loop35:
      set_dac(ttcadr,0x3ff);
      set_intg_gain(ttcadr,0xf);
      tp_high(ttcadr);
      multi_high(ttcadr);
      rxw_high(ttcadr);
      back_high(ttcadr);
      intg_rd_high(ttcadr);
      itr_high(ttcadr);
      small_cap_enable(ttcadr);
      large_cap_enable(ttcadr);
      trig_enable(ttcadr);
      set_tube(ttcadr,tube);

      sleep(1);
      set_dac(ttcadr,0);
      set_intg_gain(ttcadr,0);
      tp_low(ttcadr);
      multi_low(ttcadr);
      rxw_low(ttcadr);
      back_low(ttcadr);
      intg_rd_low(ttcadr);
      itr_low(ttcadr);
      small_cap_disable(ttcadr);
      large_cap_disable(ttcadr);
      trig_disable(ttcadr);
      set_tube(ttcadr,0);

      sleep(1);
      if(sw(1) != 1) goto loop35;
    }

   goto loop;
}

