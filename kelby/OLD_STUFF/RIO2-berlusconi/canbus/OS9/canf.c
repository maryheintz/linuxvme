#include <stdio.h>
#include <math.h>
main()
{ 
   int zone,sector,tube,mode,zone_bar,sector_bar,dac,i;
   unsigned int k,i1,i2,subadr,data;
   unsigned short shrt,shrt2,i16;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   ttcvi_map();
   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d %d",&zone,&sector,&tube);
   can_zone(zone);
   can_sector(sector);
   can_tube(tube);
   zone_bar = 0;
   if(zone == 0) zone_bar = 3;
   sector_bar = 0;
   if(sector == 0) sector_bar = 63;
   can_multi_low();
   can_swln2(1);
   can_swln1(0);
loop:
   printf(" select desired test - loop\n");
   printf("  1 = toggle sw_ln1\n");
   printf("  2 = toggle sw_ln2\n");
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
   printf(" 24 = test can readback local\n");
   printf(" 25 = test backload\n");
   printf(" 26 = test can readback from 3in1 with rxw high\n");
   printf(" 27 = set can_clk high or low\n");
   printf(" 28 = load mb1 timer hex value\n");

   fscanf(stdin,"%d",&mode);

/* toggle swln1  */
   if(mode == 1)
    {can_swln2(1);
    loop1:
      can_swln1(0);
      sleep(1);
      can_swln1(1);
      sleep(1);
      if(sw(1) != 1) goto loop1;
    }
    can_swln1(1);
        
/* toggle swln2  */
   if(mode == 2)
    {can_swln1(1);
    loop2:
      can_swln2(0);
      sleep(1);
      can_swln2(1);
      sleep(1);
      if(sw(1) != 1) goto loop2;
    }
    can_swln2(1);

/* toggle multi_sel  */   if(mode == 3)
    {loop3:
      can_multi_high();
      sleep(1);
      can_multi_low();
      sleep(1);
      if(sw(1) != 1) goto loop3;
    }
    can_multi_low();

/* toggle rxw  */
   if(mode == 4)
    {loop4:
      can_rxw_high();
      sleep(1);
      can_rxw_low();
      sleep(1);
      if(sw(1) != 1) goto loop4;
    }
    can_rxw_low();

/* toggle backload  */
   if(mode == 5)
    {loop5:
      can_bkld_high();
      sleep(1);
      can_bkld_low();
      sleep(1);
      if(sw(1) != 1) goto loop5;
    }
    can_bkld_low();

   if(mode == 6)
    {loop6:
      can_tp_high();
      sleep(1);
      can_tp_low();
      sleep(1);
      if(sw(1) != 1) goto loop6;
    }
    can_tp_low();

   if(mode == 7)
    {loop7:
      can_tube(0);
      sleep(1);
      can_tube(tube);
      sleep(1);
      if(sw(1) != 1) goto loop7;
    }
    can_tube(tube);

   if(mode == 8)
    {loop8:
      can_zone(zone_bar);
      sleep(1);
      can_zone(zone);
      sleep(1);
      if(sw(1) != 1) goto loop8;
    }
    can_zone(zone);

   if(mode == 9)
    {loop9:
      can_sector(sector_bar);
      sleep(1);
      can_sector(sector);
      sleep(1);
      if(sw(1) != 1) goto loop9;
    }
    can_sector(sector);

   if(mode == 10)
    {loop10:
      can_intg_rd_high();
      sleep(1);
      can_intg_rd_low();
      sleep(1);
      if(sw(1) != 1) goto loop10;
    }

   if(mode == 11)
    {printf("starting itr test\n");
    loop11:
      can_itr_high();
      sleep(1);
      can_itr_low();
      sleep(1);
      if(sw(1) != 1) goto loop11;
    }

   if(mode == 12)
    {printf("starting s1 test\n");
    loop12:
      can_intg_gain(1);
      sleep(1);
      can_intg_gain(0);
      sleep(1);
      if(sw(1) != 1) goto loop12;
    }

   if(mode == 13)
    {printf("starting s2 test\n");
    loop13:
      can_intg_gain(2);
      sleep(1);
      can_intg_gain(0);
      sleep(1);
      if(sw(1) != 1) goto loop13;
    }

   if(mode == 14)
    {printf("starting s3 test\n");
    loop14:
      can_intg_gain(4);
      sleep(1);
      can_intg_gain(0);
      sleep(1);
      if(sw(1) != 1) goto loop14;
    }

   if(mode == 15)
    { can_swln2(1);
      can_swln1(0);
      printf("starting s4 test\n");
    loop15:
      can_intg_gain(8);
      sleep(1);
      can_intg_gain(0);
      sleep(1);
      if(sw(1) != 1) goto loop15;
    }

   if(mode == 16)
    {can_swln2(1);
     can_swln1(0);
    loop16:
     printf("enter hex dac value\n");
     fscanf(stdin,"%x",&k);
     can_dac(k);
     if(sw(1) != 1) goto loop16;
    }

   if(mode == 17)
    {can_swln2(1);
     can_swln1(0);
    loop17:
     printf("starting set dac loop\n");
      for(k=0;k<0x3ff;k=k+10)
        { printf("k=%d\n",k);
	  can_dac(k);
          tsleep(0x80000020);
        }
      if(sw(1) != 1) goto loop17;
    }

   if(mode == 18)
    {printf("toggle small C enable\n");
    loop18:
      can_small_cap_enable();
      sleep(1);
      can_small_cap_disable();
      sleep(1);
      if(sw(1) != 1) goto loop18;
    }

   if(mode == 19)
    {printf("toggle large C enable\n");
    loop19:
      can_large_cap_enable();
      sleep(1);
      can_large_cap_disable();
      sleep(1);
      if(sw(1) != 1) goto loop19;
    }

   if(mode == 20)
    {printf("toggle trig enable\n");
    loop20:
      can_trig_enable();
      sleep(1);
      can_trig_disable();
      sleep(1);
      if(sw(1) != 1) goto loop20;
    }

/* pulse large C */
   if(mode == 21)
     { can_swln2(1);
       can_swln1(0);
       tsleep(0x80000005);
       can_large_cap_enable();
       tsleep(0x80000005);
       can_small_cap_disable();
       tsleep(0x80000005);
       printf("enter dac hex value\n");
       fscanf(stdin,"%x",&dac);
       can_dac(dac);
       tsleep(0x80000005);
loop21:
      can_tp_high();
      tsleep(0x80000005);
      can_tp_low();
      tsleep(0x80000005);
      if(sw(1) != 1) goto loop21;
     }

/* pulse small C */
   if(mode == 22)
     { can_swln2(1);
       can_swln1(0);
       tsleep(0x80000005);
       can_small_cap_enable();
       tsleep(0x80000005);
       can_large_cap_disable();
       tsleep(0x80000005);
       printf("enter dac hex value\n");
       fscanf(stdin,"%x",&dac);
       can_dac(dac);
       tsleep(0x80000005);
loop22:
      can_tp_high();
      tsleep(0x80000005);
      can_tp_low();
      tsleep(0x80000005);
      if(sw(1) != 1) goto loop22;
     }

/* pulse using multi_sel - alternate size */
   if(mode == 23)
     { for(i=1;i<49;i++)
         {can_tube(i);
	  can_mse_low();
          tsleep(0x80000005);
          can_small_cap_disable();
          tsleep(0x80000005);
          can_large_cap_disable();
          tsleep(0x80000005);
	 }
       can_tube(tube);
       tsleep(0x80000005);
       can_swln2(1);
       tsleep(0x80000005);
       can_swln1(0);
       tsleep(0x80000005);
       can_large_cap_enable();
       tsleep(0x80000005);
       can_small_cap_disable();
       tsleep(0x80000005);
       can_mse_high();
       tsleep(0x80000005);
       can_multi_high();
       tsleep(0x80000005);
       can_tube(0);
       tsleep(0x80000005);
       can_multi_low();
       tsleep(0x80000005);
       can_multi_low();
loop23:
       can_dac(0x3ff);
       tsleep(0x80000005);
       can_tp_high();
       tsleep(0x80000005);
       can_tp_low();
       tsleep(0x80000005);
       can_dac(0x200);
       tsleep(0x80000005);
       can_tp_high();
       tsleep(0x80000005);
       can_tp_low();
       tsleep(0x80000005);
       if(sw(1) != 1) goto loop23;
     }
     can_multi_low();
     can_tube(tube);
     
   if(mode == 24)
    {loop24:
       can_set(0xa0,0); /* reset state machine */
/*       for(i=0;i<0x10;i++)  */  i=2;
       { subadr = 0xfc | ((i>>8)&3);
         data = i & 0xff;
         i16 = (subadr<<8) | data;
         can_shft_wr(i16);
         for(i=0;i<16;i++)
          { can_clkh();
            can_clkl();
            can_int_clk();
          }
 	 can_shft_rd(&shrt);  /* read back command just sent */
	 printf(" first sent subadr=%x  data=%x  back=%x\n",
	    subadr,data,(int)shrt);
       }
       if(sw(1) != 1) goto loop24;

    }

   if(mode == 25)
    {loop25:
       can_multi_low();
       printf("\ns1..4=0 0 0 0 itr=0 ire=0 mse=0 tplo=0 tphi=0 trig=0 sent\n");
       can_intg_gain(0);   /* s1=s2=s3=s4=0 */
       can_intg_rd_low();   /* ire=0 */
       can_itr_low();   /* itr=0 */
       can_mse_low();   /* mse=0 */
       can_small_cap_disable();   /* tplo=0 */
       can_large_cap_disable();   /* tphi=0 */
       can_trig_disable();   /* trig=0 */
       sleep(1);
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
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       printf("\ns1..4=1 1 1 1 itr=1 ire=1 mse=1 tplo=1 tphi=1 trig=1 sent\n");
       can_intg_gain(0xf);   /* s1=s2=s3=s4=1 */
       can_intg_rd_high();   /* itr=1 */
       can_itr_high();   /* ire=1 */
       can_mse_high();   /* mse=1 */
       can_small_cap_enable();   /* tplo=1 */
       can_large_cap_enable();   /* tphi=1 */
       can_trig_enable();   /* trig=1 */
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
      printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       printf("\ns1..4=1 0 1 0 itr=1 ire=0 mse=1 tplo=0 tphi=1 trig=0 sent\n");
       can_intg_gain(0x5);   /* s1=s3=1 s2=s4=0 */
       can_itr_high();   /* itr=1 */
       can_intg_rd_low();   /* ire=0 */
       can_mse_high();   /* mse=1 */
       can_small_cap_disable();   /* tplo=0 */
       can_large_cap_enable();   /* tphi=1 */
       can_trig_disable();   /* trig=0 */
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
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       printf("\ns1..4=0 1 0 1 itr=0 ire=1 mse=0 tplo=1 tphi=0 trig=1 sent\n");
       can_intg_gain(0xa);   /* s1=s3=1 s2=s4=0 */
       can_itr_low();   /* itr=1 */
       can_intg_rd_high();   /* ire=0 */
       can_mse_low();   /* mse=1 */
       can_small_cap_enable();   /* tplo=0 */
       can_large_cap_disable();   /* tphi=1 */
       can_trig_enable();   /* trig=0 */
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
       printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
       sleep(1);
       if(sw(1) != 1) goto loop25;
    }

   if(mode == 26)
    {can_set(0xa0,0); /* reset state machine */
         i=0xaaa;
         i1 = 0xfc | ((i>>8)&3);
         i2 = i & 0xff;
         tsleep(0x80000005);
 	 can_set(i1,i2);  /* send to 3in1 card */
         can_rxw_high();
	 printf("starting closed loop\n");
loop26:
         tsleep(0x80000005);
 	 can_set(0x9c,0);  /* fetch from 3in1 and load can output reg */
         tsleep(0x80000005);
 	 can_shft_rd(&shrt);
	 printf(" sent i1=%x  i2=%x  back=%x\n",i1,i2,(int)shrt);
       if(sw(1) != 1) goto loop26;

    }

   if(mode == 27)
    {loop27:
     printf("enter 1=can_clkh or 0 can_clkl\n");
     fscanf(stdin,"%d",&k);
     if(k == 0) can_clkl();
     if(k == 1) can_clkh();
     if(sw(1) != 1) goto loop27;
    }

   if(mode == 28)
    {loop28:
     printf("enter hex timer value\n");
     fscanf(stdin,"%x",&k);
     can_tube(0x38);  /* send the data */
     tsleep(0x80000005);
     can_set(0xe0,k);
     tsleep(0x80000005);
     can_tube(0x39);   /* latch it */
     tsleep(0x80000005);
     can_tube(tube);
     if(sw(1) != 1) goto loop28;
    }
    can_tube(tube);
    goto loop;
}

