#include <stdio.h>
#include <time.h>
#include "vme.h"

time_t usec;
unsigned long vmebase,badr;
FILE *fpsw;
main()
 { int i,kja;
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
   printf(" 20 - exit\n");
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
         printf(" 1 - tower[7..0]  input A\n");
         printf(" 2 - tower[15..8]  input B\n");
         printf(" 3 - tower[23..16]  input C\n");
         printf(" 4 - tower[31..24]  input D\n");
         printf(" 5 - tower[39..32]  input E\n");
         printf(" 6 - tower[47..40]  input F\n");
         printf(" 7 - tower[55..48]input G\n");
         printf(" 8 - tower[63..56]  input H\n");
         printf(" 9 - tower[71..64]  input I\n");
         printf(" 10 - tower[79..72]  input J\n");
         printf(" 11 - tower[87..80]  input K\n");
         printf(" 12 - tower[95..88]  input L\n");
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
         adr = (unsigned long *)(vmebase + badr + 0x08);
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
         i=0;
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
         printf("%d trig bits %8.8x%8.8x%8.8x\n",i,value3,value2,value1);
	 i++;
         if(sw(1) == 0) goto loop19;
	 goto loop;
       }

    if(mode == 20) exit(0);

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
  
  sw(int i)
  { int k,l,shift;
    unsigned long *adr;
    adr = (unsigned long *)(vmebase+0x00aa000c);
    k = *adr;
/*    usleep(usec);  */
    shift = i-1;
    l = (k>>shift) & 1;
    return(l);
  }

