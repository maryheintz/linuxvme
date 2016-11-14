#include <stdio.h>
#include <time.h>
#include "vme.h"

time_t usec;
unsigned long vmebase,badr;
FILE *fpsw;
main()
 { int i,kja,l,ch[5],iev;
   unsigned long *adr,k,test;
   int mode,abcd,ldac,dac,scaler,fpmode,count,ovfl,mv,sec,kk,nloop;
   unsigned long value,value1,value2,value3,reg,uni,tbit,rcnt1,rcnt2;
   double fval;
   printf(" input card base address <fc, fd, fe, or ff>\n");
   fscanf(stdin,"%x",&kk);
   badr = (kk<<16);
   vmebase = vme_a24_base(); 
   printf("vmebase=0x%x\n",vmebase);
   
loop:
   printf(" enter operation number\n");
   printf(" 0 - clear all scalers, trigger counter, and fifo\n");
   printf(" 1 - enable scalers and trigger counter\n");
   printf(" 2 - disable scalers and trigger counter\n");
   printf(" 3 - read out a scaler\n");
   printf(" 4 - read all scalers\n");
   printf(" 5 - read the trigger words\n");
   printf(" 6 - select signals on front panel output lines\n");
   printf(" 7 - set a dac\n");
   printf(" 8 - set all dacs to same value\n");
   printf(" 9 - take data\n");
   printf(" 10 - find pulsed channel\n");
   printf(" 11 - enable gate\n");
   printf(" 12 - disable gate\n");
   printf(" 13 - write to 32 bit register\n");
   printf(" 14 - read from 32 bit register\n");
   printf(" 15 - read/write test (loop print on error)\n");
   printf(" 16 - print switch values\n");
   printf(" 17 - read switch fast loop\n");
   printf(" 18 - write once reread\n");
   printf(" 19 - read latch if 1 read trig words\n");
   printf(" 20 - read latch if 1 read trig words no gate\n");
   printf(" 21 - printout nonzero scalers\n");
   printf(" 22 - set to back to back coincidence mode\n");
   printf(" 23 - set to single tower trigger mode\n");
   printf(" 24 - reset FIFO only\n");
   printf(" 25 - call arbmuon\n");
   printf(" 99 - exit\n");
   fscanf(stdin,"%d",&mode);   
   usec=1;

    if(mode == 0)
       { printf(" clearing scalers\n");
         adr = (unsigned long *)(vmebase + badr + 0x00);
         *adr = 0;
       /*  usleep(usec); */
         goto loop;
       }

    if(mode == 1)
       { printf(" enabling scalers\n");
         adr = (unsigned long *)(vmebase + badr + 0x08);
         *adr = 0;
       /*  usleep(usec); */
         goto loop;
       }

    if(mode == 2)
       { printf(" disabling scalers\n");
         adr = (unsigned long *)(vmebase + badr + 0x0c);
         *adr = 0;
       /*  usleep(usec); */
         goto loop;
       }

    if(mode == 3)
       { printf(" select scalers  0-95=tower  96=trigger counter\n");
         fscanf(stdin,"%d",&scaler);
         printf("writing to 0x80ff0010\n");
         adr = (unsigned long *)(vmebase + badr + 0x10);
         *adr = scaler;
       /*  usleep(usec); */
         printf("reading from 0x80ff0004\n");
         adr = (unsigned long *)(vmebase + badr + 0x04);
         value = *adr;
       /*  usleep(usec); */
         count = value & 0x1ffff;
         ovfl = (value>>17) & 1;
         printf(" scaler %d value is %x  count=%d  ovfl=%d\n",
          scaler,value,count,ovfl);
         goto loop;
       }

    if(mode == 4)
       { for(scaler=0;scaler<97;scaler++)
          { adr = (unsigned long *)(vmebase + badr + 0x10);
            *adr = scaler;
       /*  usleep(usec); */
            adr = (unsigned long *)(vmebase + badr + 0x04);
            value = *adr;
       /*  usleep(usec); */
            count = value & 0x1ffff;
            ovfl = (value>>17) & 1;
            if(scaler<96) printf(" scaler %d value is %x  count=%d  ovfl=%d\n",
             scaler,value,count,ovfl);
            if(scaler==96) printf(" trgcnt %d value is %x  count=%d  ovfl=%d\n",
             scaler,value,count,ovfl);
          }
         goto loop;
       }

    if(mode == 5)
       { loop5:
         adr = (unsigned long *)(vmebase + badr + 0x1c);
         value1 =*adr;
       /*  usleep(usec); */
         adr = (unsigned long *)(vmebase + badr + 0x20);
         value2 =*adr;
       /*  usleep(usec); */
         adr = (unsigned long *)(vmebase + badr + 0x24);
         value3 =*adr;
       /*  usleep(usec); */
         printf("trig bits %8.8x%8.8x%8.8x\n",value3,value2,value1);
         if(sw(8) == 1) goto loop5;
	 goto loop;
       }


    if(mode == 6)
       { printf(" select front panel output signals\n");
         printf(" 0 - tst[7..0]  - pld test points\n");
         printf("        trg[95..0] - extended tower signals\n");
         printf(" 1 - fp_out[8..1] = trg[91..88]-trg[3..0]\n");
         printf(" 2 - fp_out[8..1] = trg[95..92]-trg[7..4]\n");
         printf(" 3 - fp_out[8..1] = trg[83..80]-trg[11..8]\n");
         printf(" 4 - fp_out[8..1] = trg[87..84]-trg[15..12]\n");
         printf(" 5 - fp_out[8..1] = trg[75..72]-trg[19..16]\n");
         printf(" 6 - fp_out[8..1] = trg[79..76]-trg[23..20]\n");
         printf(" 7 - fp_out[8..1] = trg[67..64]-trg[27..24]\n");
         printf(" 8 - fp_out[8..1] = trg[71..68]-trg[31..28]\n");
         printf(" 9 - fp_out[8..1] = trg[59..56]-trg[35..32]\n");
         printf(" 10 - fp_out[8..1] = trg[63..60]-trg[39..36]\n");
         printf(" 11 - fp_out[8..1] = trg[51..48]-trg[43..40]\n");
         printf(" 11 - fp_out[8..1] = trg[55..52]-trg[47..44]\n");
         printf("        tg[47..0] are the 48 coincidence bits\n");
         printf(" 13 - tg[7..0]   A*L\n");
         printf(" 14 - tg[15..8]   B*K\n");
         printf(" 15 - tg[23..16]   C*J\n");
         printf(" 16 - tg[31..24]   D*I\n");
         printf(" 17 - tg[39..32]   E*H\n");
         printf(" 18 - tg[47..40]   F*G\n");
         printf(" 19 - fpga testpoints 0-7 ==> fp_out[8..1]\n");
         printf(" 20 - fp_in[8..1] ==> fp_out[8..1]\n");
         printf(" 21 - pulse twrite bits[7..0] ==> fp_out[8..1]\n");
	 
         fscanf(stdin,"%d",&fpmode);
         adr = (unsigned long *)(vmebase + badr + 0x28);
         *adr = fpmode;
     /*    usleep(usec); */
         goto loop;
       }

    if(mode == 7)
       { printf(" enter [section ABCD] 0-A 1-C 2-B 3-D\n");
         fscanf(stdin,"%d",&abcd);
         printf(" enter [dac number] 0 to 23\n");
         fscanf(stdin,"%d",&ldac);
         printf(" enter [dac value] 0-4095\n");
         fscanf(stdin,"%d",&dac);
         printf(" abcd=%d  dac number=%d  dac value=%d\n",abcd,ldac,dac);
         setdac(abcd,ldac,dac);
       }
    if(mode == 8)
       { printf(" enter [dac value (mV)] 0-1239mV\n");
         fscanf(stdin,"%d",&mv);
         dac = mv*(4096.0/1240.0);
         printf(" mv=%d    dac=%d\n",mv,dac);
         for(abcd=0;abcd<4;abcd++)
          { for(ldac=0;ldac<24;ldac++)
             {  setdac(abcd,ldac,dac);
             }
          }
         goto loop;
       }
    if(mode == 9)
       { /* disable scalers -------------------------------- */
         adr = (unsigned long *)(vmebase + badr + 0x0c);
         *adr = 0;
       /*  usleep(usec); */
         /* set threshold and acquisition time ------------- */
         printf(" enter threshold in mv 0-1239 and time in seconds\n");
         fscanf(stdin,"%d %d",&mv,&sec);
 next9:
         /* clear scalers-- -------------------------------- */
         adr = (unsigned long *)(vmebase + badr + 0x00);
         *adr = 0;
       /*  usleep(usec); */
        dac = mv*(4096.0/1240.0);
         printf("\n  mv=%d  dac=%d -------------------\n",mv,dac);
         for(abcd=0;abcd<4;abcd++)
          { for(ldac=0;ldac<24;ldac++)
             {  setdac(abcd,ldac,dac);
             }
          }
         /* enable scalers-- -------------------------------- */
         adr = (unsigned long *)(vmebase + badr + 0x08);
         *adr = 0;
       /*  usleep(usec); */
         /* take data for time interval --------------------- */
         sleep(sec);
         /* disable scalers --------------------------------- */
         adr = (unsigned long *)(vmebase + badr + 0x0c);
         *adr = 0;
       /*  usleep(usec); */
         /* print out data ---------------------------------- */
         for(scaler=0;scaler<97;scaler++)
          { adr = (unsigned long *)(vmebase + badr + 0x10);
            *adr = scaler;
       /*  usleep(usec); */
            adr = (unsigned long *)(vmebase + badr + 0x04);
            value = *adr;
       /*  usleep(usec); */
            count = value & 0x7ffff;
            ovfl = (value>>19) & 1;
            if(scaler==7) printf(" scaler %d value is %x  count=%d  ovfl=%d\n",
             scaler,value,count,ovfl);
            if(scaler==95) printf(" scaler %d value is %x  count=%d  ovfl=%d\n",
             scaler,value,count,ovfl);
            if(scaler==96) printf(" trgcnt %d value is %x  count=%d  ovfl=%d\n",
             scaler,value,count,ovfl);
          }
          mv = mv - 5;
          /*if(sw(8)==0) goto next9;*/
         goto loop;
       }
    if(mode == 10)
       { adr = (unsigned long *)(vmebase + badr + 0x0c);
         *adr = 0;  /* disable scalers */
       /*  usleep(usec); */
         adr = (unsigned long *)(vmebase + badr + 0x00);
         *adr = 0;  /* clear scalers */
       /*  usleep(usec); */
         adr = (unsigned long *)(vmebase + badr + 0x08);
         *adr = 0;  /* enable scalers */
       /*  usleep(usec); */
         adr = (unsigned long *)(vmebase + badr + 0x0c);
         *adr = 0;  /* disable scalers */
       /*  usleep(usec); */
         /* print out nonzero scalers */
         for(scaler=0;scaler<97;scaler++)
          { adr = (unsigned long *)(vmebase + badr + 0x10);
            *adr = scaler;
       /*  usleep(usec); */
            fval = sin(0.13284);
            adr = (unsigned long *)(vmebase + badr + 0x04);
            value = *adr;
       /*  usleep(usec); */
            count = value & 0x7ffff;
            ovfl = (value>>19) & 1;
            if((value != 0) && (scaler<96)) 
             printf(" scaler %d value is %x  count=%d  ovfl=%d\n",
             scaler,value,count,ovfl);
            if((value != 0) && (scaler == 96)) 
             printf(" trigger scaler value is %x  count=%d  ovfl=%d\n",
             value,count,ovfl);
          }
       }

    if(mode == 11)
       { printf(" enabling gate\n");
         adr = (unsigned long *)(vmebase + badr + 0x2c);
         *adr = 0;
       /*  usleep(usec); */
         goto loop;
       }

    if(mode == 12)
       { printf(" disabling gate\n");
         adr = (unsigned long *)(vmebase + badr + 0x30);
         *adr = 0;
       /*  usleep(usec); */
         goto loop;
       }

    if(mode == 13)
       { printf(" enter register value (32 bit hex)\n");
         fscanf(stdin,"%x",&reg);
	 rewrite:
         adr = (unsigned long *)(vmebase + badr + 0x38);
         *adr = reg;
         if(sw(6) == 1) goto rewrite;
	 goto loop;
       }

    if(mode == 14)
       { adr = (unsigned long *)(vmebase + badr + 0x34);
         reg = *adr;
       /*  usleep(usec); */
	 printf(" register value=%x\n",reg);
         goto loop;
       }

    if(mode == 15)
       { nloop=0;
       loop15:
         usec=5;
         for(uni=0;uni<0xffff;uni++)
	   { test = uni | uni<<16;
	     adr = (unsigned long *)(vmebase + badr + 0x38);
	     *adr = test;
	/*     usleep(usec);  */
	     adr = (unsigned long *)(vmebase + badr + 0x34);	     
             reg = *adr;
       /*  usleep(usec); */
	     if(test != reg) 
	       { printf(" register error %x %x\n",test,reg);
	         if(sw(3) ==1 ) exit(1);
	       }
	     if(sw(2) == 1) printf("send=%x  read=%x\n",test,reg);
	   }
	 nloop++;
	 printf(" nloop=%d\n",nloop);
	 if(sw(1) == 0) goto loop15;
         goto loop;
       }

    if(mode == 16)
       { for(i=1;i<9;i++)
          { kja = sw(i);
	    printf(" switch %d = %d\n",i,kja);
          }
         sleep(2);	
         goto loop;
       }

    if(mode == 17)
       { adr = (unsigned long *)(vmebase+0x00aa000c);
loop17:  k = *adr;
       /*  usleep(usec); */
	 goto loop17;
       }

    if(mode == 18)
       { loop18:
	 kja = 0x12345678;
	 adr = (unsigned long *)(vmebase + badr + 0x38);
	 *adr = kja;
       /*  usleep(usec); */
         for(uni=0;uni<0xffffffff;uni++)
	   { adr = (unsigned long *)(vmebase + badr + 0x34);	     
             reg = *adr;
       /*      usleep(usec); */
	     if(kja != reg) printf(" register error %x %x\n",uni,reg);
	     if(sw(2) == 1) printf("kja=%x  reg=%x\n",uni,reg);
	   }
	 if(sw(1) == 0) goto loop18;
         goto loop;
       }

    if(mode == 19)
       {  adr = (unsigned long *)(vmebase + badr + 0x2c);
         *adr = 0; /* enable gate and trigger fifo */
         adr = (unsigned long *)(vmebase + badr + 0x0c);
         *adr = 0;  /* disable scalers */
         adr = (unsigned long *)(vmebase + badr + 0x00);
         *adr = 0; /* clear scalers and trigger fifo */
         printf(" enter [dac value (mV)] 0-1239mV\n");
         fscanf(stdin,"%d",&mv);
         dac = mv*(4096.0/1240.0);
         printf(" mv=%d    dac=%d\n",mv,dac);
         for(abcd=0;abcd<4;abcd++)
          { for(ldac=0;ldac<24;ldac++)
             {  setdac(abcd,ldac,dac);
             }
          }
         iev=0;
         adr = (unsigned long *)(vmebase + badr + 0x08);
         *adr = 0;  /* enable scalers and trigger fifo */
  loop19:
	 adr = (unsigned long *)(vmebase + badr + 0x3c);
         tbit = *adr;
	 if(sw(2) == 1) printf(" tbit=%x\n",tbit);
	 if(tbit == 0) goto loop19;
         adr = (unsigned long *)(vmebase + badr + 0x1c);
         value1 =*adr;
         adr = (unsigned long *)(vmebase + badr + 0x20);
         value2 =*adr;
         adr = (unsigned long *)(vmebase + badr + 0x24);
         value3 =*adr;
         l=0;
	 for(i=0;i<5;i++) ch[i] = 0;
	 for(i=0;i<32;i++)
	   { kk = (value1>>i) & 1;
	     if(kk != 0)
	       { ch[l] = i;
	         if(l<5) l++;
	       }
	    }
         value3 =*adr;
	 for(i=0;i<32;i++)
	   { kk = (value2>>i) & 1;
	     if(kk != 0)
	       { ch[l] = i+32;
	         if(l<5) l++;
	       }
	    }
         value3 =*adr;
	 for(i=0;i<32;i++)
	   { kk = (value3>>i) & 1;
	     if(kk != 0)
	       { ch[l] = i+64;
	         if(l<5) l++;
	       }
	    }
         printf("%d trig bits %8.8x%8.8x%8.8x",iev,value3,value2,value1);
	 if(l == 1) printf("  %d\n", ch[0]);
	 if(l == 2) printf("  %d %d\n", ch[0],ch[1]);
	 if(l == 3) printf("  %d %d %d\n", ch[0],ch[1],ch[2]);
	 if(l == 4) printf("  %d %d %d %d\n", ch[0],ch[1],ch[2],ch[3]);
	 if(l == 5) printf("  %d %d %d %d %d\n", ch[0],ch[1],ch[2],ch[3],ch[4]);
	 iev++;
         if(sw(1) == 0) goto loop19;
	 goto loop;
       }


    if(mode == 20)
       {  adr = (unsigned long *)(vmebase + badr + 0x30);
         *adr = 0; /* disable gate */
         adr = (unsigned long *)(vmebase + badr + 0x0c);
         *adr = 0;  /* disable scalers */
         adr = (unsigned long *)(vmebase + badr + 0x00);
         *adr = 0; /* clear scalers and trigger fifo */
         printf(" enter [dac value (mV)] 0-1239mV\n");
         fscanf(stdin,"%d",&mv);
         dac = mv*(4096.0/1240.0);
         printf(" mv=%d    dac=%d\n",mv,dac);
         for(abcd=0;abcd<4;abcd++)
          { for(ldac=0;ldac<24;ldac++)
             {  setdac(abcd,ldac,dac);
             }
          }
	 /* =========================== bad tower ================= */
/*         abcd=1;
         ldac=18;
         setdac(abcd,ldac,500);
         abcd=3;
         ldac=18;
         setdac(abcd,ldac,500);  */
	 /* =========================== bad tower ================= */
         iev=0;
         adr = (unsigned long *)(vmebase + badr + 0x08);
         *adr = 0;  /* enable scalers and trigger fifo */
  loop20:
	 adr = (unsigned long *)(vmebase + badr + 0x3c);
         tbit = *adr;
	 if(sw(2) == 1) printf(" tbit=%x\n",tbit);
	 if(tbit == 0) goto loop20;
         adr = (unsigned long *)(vmebase + badr + 0x1c);
         value1 =*adr;
         adr = (unsigned long *)(vmebase + badr + 0x20);
         value2 =*adr;
         adr = (unsigned long *)(vmebase + badr + 0x24);
         value3 =*adr;
         l=0;
	 for(i=0;i<5;i++) ch[i] = 0;
	 for(i=0;i<32;i++)
	   { kk = (value1>>i) & 1;
	     if(kk != 0)
	       { ch[l] = i;
	         if(l<5) l++;
	       }
	    }
         value3 =*adr;
	 for(i=0;i<32;i++)
	   { kk = (value2>>i) & 1;
	     if(kk != 0)
	       { ch[l] = i+32;
	         if(l<5) l++;
	       }
	    }
         value3 =*adr;
	 for(i=0;i<32;i++)
	   { kk = (value3>>i) & 1;
	     if(kk != 0)
	       { ch[l] = i+64;
	         if(l<5) l++;
	       }
	    }
         printf("%d trig bits %8.8x%8.8x%8.8x",iev,value3,value2,value1);
	 if(l == 1) printf("  %d\n", ch[0]);
	 if(l == 2) printf("  %d %d\n", ch[0],ch[1]);
	 if(l == 3) printf("  %d %d %d\n", ch[0],ch[1],ch[2]);
	 if(l == 4) printf("  %d %d %d %d\n", ch[0],ch[1],ch[2],ch[3]);
	 if(l == 5) printf("  %d %d %d %d %d\n", ch[0],ch[1],ch[2],ch[3],ch[4]);
	 iev++;
	 fflush(stdout);
         if(sw(1) == 0) goto loop20;
	 goto loop;
       }

    if(mode == 21)
       { for(scaler=0;scaler<97;scaler++)
          { adr = (unsigned long *)(vmebase + badr + 0x10);
            *adr = scaler;
       /*  usleep(usec); */
            adr = (unsigned long *)(vmebase + badr + 0x04);
            value = *adr;
       /*  usleep(usec); */
            count = value & 0x1ffff;
            ovfl = (value>>17) & 1;
            if(scaler<96 && count>0) printf(" scaler %d value is %x  count=%d  ovfl=%d\n",
             scaler,value,count,ovfl);
            if(scaler==96) printf(" trgcnt %d value is %x  count=%d  ovfl=%d\n",
             scaler,value,count,ovfl);
          }
         goto loop;
       }

    if(mode == 22)
       { adr = (unsigned long *)(vmebase + badr + 0x40);
         *adr=0;
	 printf(" set to back to back coincidence mode\n");
         goto loop;
       }
    if(mode == 23)
       { adr = (unsigned long *)(vmebase + badr + 0x44);
         *adr=0;
	 printf(" set to single tower coincidence mode\n");
         goto loop;
       }

    if(mode == 24)
       { printf(" reset FIFO\n");
         adr = (unsigned long *)(vmebase + badr + 0x48);
         *adr = 0;
         goto loop;
       }

    if(mode == 25)
       { printf(" calling arbmuon\n");
         arbmuon();
         goto loop;
       }

    if(mode == 99) exit(0);

    goto loop;
  }

 setdac(abcd,ldac,dac)
 int abcd,ldac,dac;
  { unsigned long k1,k2,*adr;
/*  abcd 0=A  1=C  2=B  3=D  */
    k1 = (abcd<<12) | dac;
    adr = (unsigned long *)(vmebase + badr + 0x14);
    *adr = k1;      /* latch dac value and abcd select */
       /*  usleep(usec); */
    k2 = (1<<ldac);
    adr = (unsigned long *)(vmebase + badr + 0x18);
    *adr = k2;  /* strobe dac line */
       /*  usleep(usec); */
  }
  
  sw_old(int i)
  { int k,l,shift;
    unsigned long *adr;
    adr = (unsigned long *)(vmebase+0x00aa000c);
    k = *adr;
/*    usleep(usec);  */
    shift = i-1;
    l = (k>>shift) & 1;
    return(l);
  }

  sw(int i)
  { int k,l,shift;
    unsigned short *adr;
    adr = (unsigned short *)(vmebase+0x00aa000c);
    k = *adr;
    usleep(usec);
    shift = i-1;
    l = (k>>shift) & 1;
    return(l);
    } 
     
  arbmuon()
    { int mode,i;
      unsigned short klow,khigh,test;
      unsigned short *adr,scaler;
 loop:
      printf(" enter operation number\n");
      printf(" 0 - clear all scalers\n");
      printf(" 1 - read muon scaler\n");
      printf(" 2 - read scintillator scaler\n");
      printf(" 3 - read coincidence scaler\n");
      printf(" 4 - toggle half.q\n");
      printf(" 99 - exit back\n");
      fscanf(stdin,"%d",&mode);   
      if(mode == 0)
       { printf(" clearing scalers\n");
         adr = (unsigned short *)(vmebase + 0x00de1000);
         *adr = 0;
         goto loop;
       }

      if(mode == 1)
       { printf(" read muon scaler\n");
         adr = (unsigned short *)(vmebase + 0x00de1008);
         *adr = 0;  /* set half to zero */
         adr = (unsigned short *)(vmebase + 0x00de1002);
	 klow = *adr;
  
         adr = (unsigned short *)(vmebase + 0x00de1008);
         *adr = 1;  /* set half to one */
         adr = (unsigned short *)(vmebase + 0x00de1002);
	 khigh = *adr;
	 
	 scaler = (khigh<<16) + klow;
	 printf(" muon scaler = %d  %x %x\n", scaler,khigh,klow);
         goto loop;
       }

      if(mode == 2)
       { printf(" read scintillator scaler\n");
         adr = (unsigned short *)(vmebase + 0x00de1008);
         *adr = 0;  /* set half to zero */
         adr = (unsigned short *)(vmebase + 0x00de1004);
	 klow = *adr;
  
         adr = (unsigned short *)(vmebase + 0x00de1008);
         *adr = 1;  /* set half to one */
         adr = (unsigned short *)(vmebase + 0x00de1004);
	 khigh = *adr;
	 
	 scaler = (khigh<<16) + klow;
	 printf(" scintillator scaler = %d  %x %x\n", scaler,khigh,klow);
         goto loop;
       }

      if(mode == 3)
       { printf(" read coincidence scaler\n");
         adr = (unsigned short *)(vmebase + 0x00de1008);
         *adr = 0;  /* set half to zero */
         adr = (unsigned short *)(vmebase + 0x00de1006);
	 klow = *adr;
  
         adr = (unsigned short *)(vmebase + 0x00de1008);
         *adr = 1;  /* set half to one */
         adr = (unsigned short *)(vmebase + 0x00de1006);
	 khigh = *adr;
	 
	 scaler = (khigh<<16) + klow;
	 printf(" coincidence scaler = %d  %x %x\n", scaler,khigh,klow);
         goto loop;
       }

      if(mode == 4)
       { next4:
         for (i=0;i<0xffff;i++)
	   { adr = (unsigned short *)(vmebase + 0x00de1008);
             *adr = i;  /* write to 16 bit register */
	     adr = (unsigned short *)(vmebase + 0x00de100a);
	     test = *adr;
	     if(test != i) printf(" i=%x  test=%x\n",i,test);  
	     if(sw(1) == 1) printf(" i=%x  test=%x\n",i,test);  
           }
	 if(sw(6) == 0) goto next4;
         goto loop;
       }

      if(mode == 99) return(0);
      goto loop;
    }
