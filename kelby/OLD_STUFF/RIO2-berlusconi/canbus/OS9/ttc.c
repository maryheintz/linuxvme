#include <stdio.h>
#include <math.h>
main()
{ 
   int zone,sector,tube,mode,zone_bar,sector_bar,dac,i;
   unsigned int k,pause;
   unsigned long klong;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   long status;
   ttcvi_map();
   can_init();
   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d %d",&zone,&sector,&tube);
   set_zone(zone);
   set_sector(sector);
   set_tube(tube);
   zone_bar = 0;
   if(zone == 0) zone_bar = 3;
   sector_bar = 0;
   if(sector == 0) sector_bar = 63;
   multi_low();
loop:
   printf(" select desired test - loop\n");
   printf("  1 = toggle set bits one at a time\n");
   printf("  2 = toggle unused\n");
   printf("  3 = toggle multi_sel\n");
   printf("  4 = toggle rxw\n");
   printf("  5 = toggle back_load\n");
   printf("  6 = toggle tp\n");
   printf("  7 = toggle tube\n");
   printf("  8 = toggle zone\n");
   printf("  9 = toggle sector\n");
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
   printf(" 24 = set tp high\n");
   printf(" 25 = set tp low\n");
   printf(" 26 = send long word\n");
   printf(" 27 = cycle phase of clock40des1\n");
   printf(" 28 = cycle phase of clock40des2\n");
   printf(" 29 = toggle tp cycling phase of clock40des2\n");
   printf(" 30 = set MB1 timer to hex value\n");
   printf(" 31 = sent via TTC - read back via CANF\n");
   printf(" 32 = toggle multi TTC and CANBUS\n");
   printf(" 33 = exit\n");
   fscanf(stdin,"%d",&mode);

   if(mode == 1)
    {loop1:
       can_multi_low();
       printf("set s1\n");
       set_intg_gain(1);
       intg_rd_low();
       itr_low();
       mse_low();
       small_cap_disable();
       large_cap_disable();
       trig_disable();
       sleep(1);
       stat3in1();

       printf("set s2\n");
       set_intg_gain(2);
       stat3in1();

       printf("set s3\n");
       set_intg_gain(4);
       stat3in1();

       printf("set s4\n");
       set_intg_gain(8);
       stat3in1();

       printf("set ire\n");
       set_intg_gain(0); 
       intg_rd_high();
       stat3in1();

       printf("set itr\n");
       intg_rd_low();
       itr_high();
       stat3in1();

       printf("set mse\n");
       itr_low();
       mse_high();
       stat3in1();

       printf("set tplo\n");
       mse_low();
       small_cap_enable();
       stat3in1();

       printf("set tphi\n");
       small_cap_disable();
       large_cap_enable();
       stat3in1();

       printf("set trig\n");
       large_cap_disable();
       trig_enable();
       stat3in1();

       goto loop1;
    }
        
   if(mode == 2)
    {
    }

   if(mode == 3)
    {loop3:
      multi_high();
      sleep(1);
      multi_low();
      sleep(1);
      if(sw(1) != 1) goto loop3;
    }
    multi_low();

   if(mode == 4)
    {loop4:
      rxw_high();
      sleep(1);
      rxw_low();
      sleep(1);
      if(sw(1) != 1) goto loop4;
    }
    rxw_low();

   if(mode == 5)
    {loop5:
      back_high();
      sleep(1);
      back_low();
      sleep(1);
      if(sw(1) != 1) goto loop5;
    }
    back_low();

   if(mode == 6)
    {reset_state();
loop6:
      tp_high();
      tsleep(0x80000005);
      tp_low();
      tsleep(0x80000005);
      if(sw(1) != 1) goto loop6;
    }
    tp_low();

   if(mode == 7)
    {loop7:
      set_tube(0);
      sleep(1);
      set_tube(tube);
      sleep(1);
      if(sw(1) != 1) goto loop7;
    }
    set_tube(tube);

   if(mode == 8)
    {loop8:
      set_zone(zone_bar);
      sleep(1);
      set_zone(zone);
      sleep(1);
      if(sw(1) != 1) goto loop8;
    }
    set_zone(zone);

   if(mode == 9)
    {loop9:
      set_sector(sector_bar);
      sleep(1);
      set_sector(sector);
      sleep(1);
      if(sw(1) != 1) goto loop9;
    }
    set_sector(sector);

   if(mode == 10)
    {loop10:
      intg_rd_high();
      sleep(1);
      intg_rd_low();
      sleep(1);
      if(sw(1) != 1) goto loop10;
    }

   if(mode == 11)
    {printf("starting itr test\n");
    loop11:
      itr_high();
      sleep(1);
      itr_low();
      sleep(1);
      if(sw(1) != 1) goto loop11;
    }

   if(mode == 12)
    {printf("starting s1 test\n");
    loop12:
      set_intg_gain(1);
      sleep(1);
      set_intg_gain(0);
      sleep(1);
      if(sw(1) != 1) goto loop12;
    }

   if(mode == 13)
    {printf("starting s2 test\n");
    loop13:
      set_intg_gain(2);
      sleep(1);
      set_intg_gain(0);
      sleep(1);
      if(sw(1) != 1) goto loop13;
    }

   if(mode == 14)
    {printf("starting s3 test\n");
    loop14:
      set_intg_gain(4);
      sleep(1);
      set_intg_gain(0);
      sleep(1);
      if(sw(1) != 1) goto loop14;
    }

   if(mode == 15)
    { 
      printf("starting s4 test\n");
    loop15:
      set_intg_gain(8);
      sleep(1);
      set_intg_gain(0);
      sleep(1);
      if(sw(1) != 1) goto loop15;
    }

   if(mode == 16)
    {
    loop16:
     printf("enter hex dac value\n");
     fscanf(stdin,"%x",&k);
     set_dac(k);
     if(sw(1) != 1) goto loop16;
    }

   if(mode == 17)
    {
    loop17:
     printf("starting set dac loop\n");
      for(k=0;k<0x3ff;k=k+10)
        { printf("k=%d\n",k);
	  set_dac(k);
          tsleep(0x80000020);
        }
      if(sw(1) != 1) goto loop17;
    }

   if(mode == 18)
    {printf("toggle small C enable\n");
    loop18:
      small_cap_enable();
      sleep(1);
      small_cap_disable();
      sleep(1);
      if(sw(1) != 1) goto loop18;
    }

   if(mode == 19)
    {printf("toggle large C enable\n");
    loop19:
      large_cap_enable();
      sleep(1);
      large_cap_disable();
      sleep(1);
      if(sw(1) != 1) goto loop19;
    }

   if(mode == 20)
    {printf("toggle trig enable\n");
    loop20:
      trig_enable();
      sleep(1);
      trig_disable();
      sleep(1);
      if(sw(1) != 1) goto loop20;
    }

/* pulse large C */
   if(mode == 21)
     { 
       tsleep(0x80000005);
       large_cap_enable();
       tsleep(0x80000005);
       small_cap_disable();
       tsleep(0x80000005);
       printf("enter dac hex value\n");
       fscanf(stdin,"%x",&dac);
       set_dac(dac);
       tsleep(0x80000005);
loop21:
      tp_high();
      tsleep(0x80000050);
      tp_low_bc();  
      /*  tp_low();  */
      tsleep(0x80000050);
      if(sw(1) != 1) goto loop21;
     }

/* pulse small C */
   if(mode == 22)
     { 
       tsleep(0x80000005);
       small_cap_enable();
       tsleep(0x80000005);
       large_cap_disable();
       tsleep(0x80000005);
       printf("enter dac hex value\n");
       fscanf(stdin,"%x",&dac);
       set_dac(dac);
       tsleep(0x80000005);
loop22:
      tp_high();
      tsleep(0x80000005);
      tp_low_bc();
      tsleep(0x80000005);
      if(sw(1) != 1) goto loop22;
     }

/* pulse using multi_sel - alternate size */
   if(mode == 23)
     { for(i=1;i<49;i++)
         {set_tube(i);
	  mse_low();
          small_cap_disable();
          large_cap_disable();
	 }
       set_tube(tube);
       tsleep(0x80000005);
       large_cap_enable();
       tsleep(0x80000005);
       small_cap_disable();
       tsleep(0x80000005);
       mse_high();
       multi_high();
       set_tube(0);
loop23:
       set_dac(0x3ff);
       tsleep(0x80000005);
       tp_high();
       tsleep(0x80000005);
       tp_low_bc();
       tsleep(0x80000005);
       set_dac(0x200);
       tsleep(0x80000005);
       tp_high();
       tsleep(0x80000005);
       tp_low_bc();
       tsleep(0x80000005);
       if(sw(1) != 1) goto loop23;
     }
     multi_low();
     set_tube(tube);
     
   if(mode == 24)
    {tp_high();
    }

   if(mode == 25)
    {tp_low();
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
         tsleep(0x80000002);
      }
     if(sw(1) != 1) goto loop27;
    }


   if(mode == 28)
    {
    loop28:
      for(i=0;i<240;i++)
      {  SetTimeDes2(0,i);
         tsleep(0x80000002);
      }
     if(sw(1) != 1) goto loop28;
    }

   if(mode == 29)
    {
    loop29:
      for(i=0;i<240;i++)
      {  SetTimeDes2(0,i);
         tsleep(0x80000002);
	 tp_high();
	 trig_enable();
         tsleep(0x80000002);
	 tp_low_bc();
	 trig_disable();
         tsleep(0x80000002);
	 printf("i=%x\n",i);
      }
     if(sw(1) != 1) goto loop29;
    }

   if(mode == 30)
    {loop30:
     printf("enter hex timer value\n");
     fscanf(stdin,"%x",&k);
     set_mb_time(1,k);
     set_tube(tube);
     if(sw(1) != 1) goto loop30;
    }

   if(mode == 31)
    {loop31:
       can_multi_low();
       printf("\ns1..4=0 0 0 0 itr=0 ire=0 mse=0 tplo=0 tphi=0 trig=0 sent\n");
       set_intg_gain(0);   /* s1=s2=s3=s4=0 */
       intg_rd_low();   /* ire=0 */
       itr_low();   /* itr=0 */
       mse_low();   /* mse=0 */
       small_cap_disable();   /* tplo=0 */
       large_cap_disable();   /* tphi=0 */
       trig_disable();   /* trig=0 */
       sleep(1);
       can_bkld_high();
       can_bkld_low();
       status=can_3in1set(0x9c00);  /* fetch from 3in1 and load can output reg */
       tsleep(0x80000005);
       status=can_3in1get(&shrt);
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
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       printf("\ns1..4=1 1 1 1 itr=1 ire=1 mse=1 tplo=1 tphi=1 trig=1 sent\n");
       set_intg_gain(0xf);   /* s1=s2=s3=s4=1 */
       intg_rd_high();   /* itr=1 */
       itr_high();   /* ire=1 */
       mse_high();   /* mse=1 */
       small_cap_enable();   /* tplo=1 */
       large_cap_enable();   /* tphi=1 */
       trig_enable();   /* trig=1 */
       can_bkld_high();
       can_bkld_low();
       status=can_3in1set(0x9c00);  /* fetch from 3in1 and load can output reg */
       tsleep(0x80000005);
       status=can_3in1get(&shrt);
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
      printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       printf("\ns1..4=1 0 1 0 itr=1 ire=0 mse=1 tplo=0 tphi=1 trig=0 sent\n");
       set_intg_gain(0x5);   /* s1=s3=1 s2=s4=0 */
       itr_high();   /* itr=1 */
       intg_rd_low();   /* ire=0 */
       mse_high();   /* mse=1 */
       small_cap_disable();   /* tplo=0 */
       large_cap_enable();   /* tphi=1 */
       trig_disable();   /* trig=0 */
       can_bkld_high();
       can_bkld_low();
       status=can_3in1set(0x9c00);  /* fetch from 3in1 and load can output reg */
       tsleep(0x80000005);
       status=can_3in1get(&shrt);
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
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       printf("\ns1..4=0 1 0 1 itr=0 ire=1 mse=0 tplo=1 tphi=0 trig=1 sent\n");
       set_intg_gain(0xa);   /* s1=s3=1 s2=s4=0 */
       itr_low();   /* itr=1 */
       intg_rd_high();   /* ire=0 */
       mse_low();   /* mse=1 */
       small_cap_enable();   /* tplo=0 */
       large_cap_disable();   /* tphi=1 */
       trig_enable();   /* trig=0 */
       can_bkld_high();
       can_bkld_low();
       status=can_3in1set(0x9c00);  /* fetch from 3in1 and load can output reg */
       tsleep(0x80000005);
       status=can_3in1get(&shrt);
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
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       if(sw(1) != 1) goto loop31;
    }

   if(mode == 32)
    { 
loop32:
      multi_high();
      printf(" multi_high()\n");
      printf(" paused\n");
      fscanf(stdin,"%d",&pause);
      multi_low();
      printf(" multi_low()\n");
      printf(" paused\n");
      fscanf(stdin,"%d",&pause);
      can_multi_high();
      printf(" can_multi_high()\n");
      printf(" paused\n");
      fscanf(stdin,"%d",&pause);
      can_multi_low();
      printf(" can_multi_low()\n");
      printf(" paused\n");
      fscanf(stdin,"%d",&pause);
      if(sw(1) == 0) goto loop32;
    }

   if(mode == 33) return(0);

   goto loop;
}

stat3in1()
  {    unsigned short shrt;
       int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,i;
       long status;
       can_bkld_high();
       can_bkld_low();
       status=can_3in1set(0x9c00);  /* fetch from 3in1 and load can output reg */
       tsleep(0x80000005);
       status=can_3in1get(&shrt);
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
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
  }
