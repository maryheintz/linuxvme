#define CANSET 11
#define CANGET 12
#define ACK 51
#define GO_FB 101
#define INIT 1
#include <stdio.h>
#include <math.h>
#include <time.h>
#define swch 0    /* value of dip switch on integrator adc card */
static int Base={swch<<6};
;
time_t usec;
main()
{ 
   int zone,sector,tube,mode,zone_bar,sector_bar,dac,i;
   unsigned int k,i1,i2,subadr,data;
   unsigned short shrt,shrt2,i16;
   char version[8];
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   ttcvi_map();
   printf("calling reset_can\n");
   reset_ctrl_can();
   sleep(1);
   reset_can(1); /* set can reset bit on via TTC */
   sleep(1);
   reset_can(0); /* release can reset bit */
   sleep(1);
   printf("calling can_adc_init\n");
   can_adc_init(GO_FB,&version[1]);
   for (i=0;i<7;i++) printf("%c", version[i]);
   printf(" - VERSION %d\n", (int)version[7]);
   printf("reset_can return\n");
   printf(" enter zone sector tube\n");
   fscanf(stdin,"%d %d %d",&zone,&sector,&tube);
   printf("zone=%d  sector=%d  tube=%d\n",zone,sector,tube);
   can_zone(zone);
   can_sector(sector);
   can_tube(tube);
   zone_bar = 0;
   if(zone == 0) zone_bar = 3;
   sector_bar = 0;
   if(sector == 0) sector_bar = 63;
   can_multi_low();
loop:
   printf(" select desired test - loop\n");
   printf("  1 = unused\n");
   printf("  2 = unused\n");
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
   printf(" 24 = unused\n");
   printf(" 25 = test backload\n");
   printf(" 26 = test can readback from 3in1 with rxw high\n");
   printf(" 27 = unused\n");
   printf(" 28 = load mb1 timer hex value\n");

   fscanf(stdin,"%d",&mode);

/* toggle multi_sel  */
   if(mode == 3)
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
    { printf("starting s4 test\n");
    loop15:
      can_intg_gain(8);
      sleep(1);
      can_intg_gain(0);
      sleep(1);
      if(sw(1) != 1) goto loop15;
    }

   if(mode == 16)
    {loop16:
     printf("enter hex dac value\n");
     fscanf(stdin,"%x",&k);
     can_dac(k);
     if(sw(1) != 1) goto loop16;
    }

   if(mode == 17)
    {loop17:
     printf("starting set dac loop\n");
      for(k=0;k<0x3ff;k=k+10)
        { printf("k=%d\n",k);
	  can_dac(k);
          usleep(1);
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
     { usleep(5);
       can_large_cap_enable();
       usleep(5);
       can_small_cap_disable();
       usleep(5);
       printf("enter dac hex value\n");
       fscanf(stdin,"%x",&dac);
       can_dac(dac);
       usleep(5);
loop21:
      can_tp_high();
      usleep(5);
      can_tp_low();
      usleep(5);
      if(sw(1) != 1) goto loop21;
     }

/* pulse small C */
   if(mode == 22)
     { usleep(5);
       can_small_cap_enable();
       usleep(5);
       can_large_cap_disable();
       usleep(5);
       printf("enter dac hex value\n");
       fscanf(stdin,"%x",&dac);
       can_dac(dac);
       usleep(5);
loop22:
      can_tp_high();
      usleep(5);
      can_tp_low();
      usleep(5);
      if(sw(1) != 1) goto loop22;
     }

/* pulse using multi_sel - alternate size */
   if(mode == 23)
     { for(i=1;i<49;i++)
         {can_tube(i);
	  can_mse_low();
          usleep(5);
          can_small_cap_disable();
          usleep(5);
          can_large_cap_disable();
          usleep(5);
	 }
       can_tube(tube);
       usleep(5);
       can_large_cap_enable();
       usleep(5);
       can_small_cap_disable();
       usleep(5);
       can_mse_high();
       usleep(5);
       can_multi_high();
       usleep(5);
       can_tube(0);
       usleep(5);
       can_multi_low();
       usleep(5);
       can_multi_low();
loop23:
       can_dac(0x3ff);
       usleep(5);
       can_tp_high();
       usleep(5);
       can_tp_low();
       usleep(5);
       can_dac(0x200);
       usleep(5);
       can_tp_high();
       usleep(5);
       can_tp_low();
       usleep(5);
       if(sw(1) != 1) goto loop23;
     }
     can_multi_low();
     can_tube(tube);
     
   if(mode == 24)
    {
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
       can_3in1set(0x9c00);   /* fetch from 3in1 and load can output reg */
       usleep(5);
       can_3in1get(&shrt);
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
       can_3in1set(0x9c00);  /* fetch from 3in1 and load can output reg */
       usleep(5);
       can_3in1get(&shrt);
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
       can_3in1set(0x9c00);  /* fetch from 3in1 and load can output reg */
       usleep(5);
       can_3in1get(&shrt);
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
       can_3in1set(0x9c00);  /* fetch from 3in1 and load can output reg */
       usleep(5);
       can_3in1get(&shrt);
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
    {can_3in1set(0xa000); /* reset state machine */
     i=0xaaa;
     i1 = 0xfc | ((i>>8)&3);
     i2 = i & 0xff;
     usleep(5);
     can_3in1set((i1<<8)|i2);  /* send to 3in1 card */
     can_rxw_high();
     printf("starting closed loop\n");
loop26:
     usleep(5);
     can_3in1set(0x9c00);  /* fetch from 3in1 and load can output reg */
     usleep(5);
     can_3in1get(&shrt);
     printf(" sent i1=%x  i2=%x  back=%x\n",i1,i2,(int)shrt);
     if(sw(1) != 1) goto loop26;

    }

   if(mode == 27)
    {
    }

   if(mode == 28)
    {loop28:
     printf("enter hex timer value\n");
     fscanf(stdin,"%x",&k);
     can_tube(0x38);  /* send the data */
     usleep(5);
     can_3in1set(0xe000|k);
     usleep(5);
     can_tube(0x39);   /* latch it */
     usleep(5);
     can_tube(tube);
     if(sw(1) != 1) goto loop28;
    }
    can_tube(tube);
    goto loop;
}

reset_ctrl_can()
 { unsigned char *adr;
   int k,sjw,brp,sam,tseg2,tseg1;
   int octp1,octn1,ocpol1,octp0,octn0,ocpol0,ocmode1,ocmode0;


   k = 0xdffe6001; /* CONTROL REGISTER */
   adr = (unsigned char*) k;
   *adr = 0x01;  /*RESET HIGH*/
   
   k = 0xdffe6009; /* ACCEPTANCE CODE REGISTER */
   adr = (unsigned char*) k;
   *adr = 0;  
   
   k = 0xdffe600b; /* ACCEPTANCE MASK */
   adr = (unsigned char*) k;
   *adr = 0xff;  
   
   k = 0xdffe600d; /* BTR0 */
   adr = (unsigned char*) k;
   sjw = 2;
   brp = 0;  
   *adr = (sjw<<6) | brp;  

   k = 0xdffe600f; /* BTR1 */
   adr = (unsigned char*) k;
   sam = 0;
   tseg2 = 6;
   tseg1 = 7;
   *adr = (sam<<7) | (tseg2<<4) | tseg1;     /*BTR 1*/

   k = 0xdffe6011;  /* OCR */
   adr = (unsigned char*) k;
   octp1 = 0;
   octn1 = 0;
   ocpol1 = 0;
   octp0 = 1;
   octn0 = 1;
   ocpol0 = 0;
   ocmode1 = 1;
   ocmode0 = 0;
   *adr = (octp1<<7) | (octn1<<6) | (ocpol1<<5) | (octp0<<4)
        | (octn0<<3) | (ocpol0<<2) | (ocmode1<<1) | ocmode0;
 
   k = 0xdffe6001;  /* COMMAND REGISTER */
   adr = (unsigned char*) k;
   *adr = 0;  /* Reset Request low */
}
can_adc_init(action,version)
 char action;
 char version[];
  { long status;
    long id;
    char dlc, rtr;
    char data[8];
    int i;

/* Send the init command  */
    data[0] = INIT;
    data[1] = action;
    data[2] = 0;
    data[3] = 0;
    data[4] = 0;
    data[5] = 0;
    data[6] = 0;
    data[7] = 0;
    can_tx(Base+1, 2, 0 , data);

/* Wait for the adc acknowledgement  */
    can_rx(&id, &dlc, &rtr, version);  

/* Wait for the transition indicator  */
    can_rx(&id, &dlc, &rtr, data);  
	
/* If Status equals 0 command was processed -> return 0
	   Otherwise there was a problem -> return -1   */
}

can_tx(id,dlc,rtr,data)
int id;
char dlc,rtr;
char data[8];
 { int i;
   unsigned int k;
   unsigned char *adr,value;
   adr = (unsigned char *)0xdffe6015;
   value = (id >> 3);
   *adr = value;
   printf(" adr=%x  *adr= %x\n",(int)adr,value);
   adr = (unsigned char *)0xdffe6017;
   value = (id << 5) | ((rtr&1)<<4) | (dlc&0xf);
   *adr = value;
   printf(" adr=%x  *adr= %x\n",(int)adr,value);
   k = 0xdffe6019;
   for(i=0;i<8;i++)
    {adr = (unsigned char *)k;
     *adr = data[i];
   printf(" adr=%x  *adr= %x   data=%x\n",(int)adr,*adr,data[i]);
     k = k+2;
    }
   adr = (unsigned char *)0xdffe6003;
/*   *adr = 0xf;   */
   *adr = 1;
   }
can_rx(id,dlc,rtr,data)
int *id;
char *dlc,*rtr;
char data[8];
 { int i,k,count;
   unsigned int kk;
   unsigned char status,dat1,dat2;
   unsigned char *adr;
   count=0;
   adr = (unsigned char *)0xdffe6005;
wait_rx:
   status = *adr;
   count++;
   if(count>1000)
     { printf("can_rx hung\n");
       printf("can_rx status=%x\n",status);
       return;
     }
   if((status&1) == 0) goto wait_rx;  
   printf("can_rx status=%x\n",status);
   adr = (unsigned char *)0xdffe6029;
   dat1 = *adr;
   adr = (unsigned char *)0xdffe602b;
   dat2 = *adr;
   *id = (dat1<<3) | (dat2>>5);
   *rtr = (dat2>>4) & 1;
   k = (dat2 & 0xf);
   *dlc = k;
   printf("can_rx id=%x  rtr=%x  dlc=%x\n",*id,*rtr,*dlc);
   printf("can_rx dat1=%x  dat2=%x\n",dat1,dat2);
   kk = 0xdffe602f;
   for(i=0;i<k;i++)
    { adr = (unsigned char *)kk;
      data[i] = *adr;
      printf("can_rx  i=%d  data=%x\n",i,data[i]);
      kk = kk+2;
    }
}

can_3in1set(dat)
unsigned short dat;
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];

	/* Send the command  */
	data[0] = CANSET;
	data[1] = (unsigned char)( (dat >> 8) &0x00FF );
	data[2] = (unsigned char)(  dat       &0x00FF );
	status  = can_tx(Base+1, 3, 0, data);

	/* Wait for the acknowledgement  */
	/* if we don't get ACK then status is set to -1  */
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	if (status == 0) if (data[0] != ACK) status = -1;

	/* If the command was written and acknowledged properly, return 0
	   Otherwise return -1  */
        if(status != 0) printf("can_3in1set status=%d\n",status);
}

/* Read a 3in1 pattern form the mother board epld  */

can_3in1get(dat)
unsigned short *dat;
{
	long status;
	long id;
	char dlc, rtr;
	char data[8];
	unsigned short var1, var2;

	/* Send the read command  */
	data[0] = CANGET;
	status  = can_tx(Base+1, 1, 0, data);
	/* Get back the result  */
	if (status == 0) status = can_rx(&id, &dlc, &rtr, data);
	/* If we don't get back a REQUEST command  */
	/* If we don't get back the right parameter code  */
	/*		then error is set to 1  */
	if (status == 0) if (data[0] != CANGET) status = 1;
	
	/* Format the returned parameter  */
	var1 = ((unsigned short)data[1])&0x00FF;
	var2 = ((unsigned short)data[2])&0x00FF;
	*dat = (var1 << 8) + var2;

	/* Returns 0 if all was OK
	   Returns 1 if the command or code parameter that were received back are wrong
	   Returns -1 if there was a communication error  */
        if(status != 0) printf("can_3in1get status=%d\n",status);
}


