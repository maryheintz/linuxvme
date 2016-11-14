#include <stdio.h>
main()
 {
   unsigned long *adr,k;
   int mode,abcd,ldac,dac,scaler,fpmode,count,ovfl;
   unsigned long value,value1,value2,value3;
   double fval;
   int second;
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
   printf(" 9 - find pulsed channel\n");
   printf(" 10 - enable gate\n");
   printf(" 11 - disable gate\n");
   printf(" 12 - exit\n");
   fscanf(stdin,"%d",&mode);   
  
    if(mode == 0)
       { printf(" clearing scalers\n");
         adr = (unsigned long *)0x80ff0000;
         *adr = 0;
         goto loop;
       }

    if(mode == 1)
       { printf(" enabling scalers\n");
         adr = (unsigned long *)0x80ff0008;
         *adr = 0;
         goto loop;
       }

    if(mode == 2)
       { printf(" disabling scalers\n");
         adr = (unsigned long *)0x80ff000c;
         *adr = 0;
         goto loop;
       }

    if(mode == 3)
       { printf(" select scalers  0-95=tower  96=trigger counter\n");
         fscanf(stdin,"%d",&scaler);
         printf("writing to 0x80ff0010\n");
         adr = (unsigned long *)0x80ff0010;
         *adr = scaler;
         printf("reading from 0x80ff0004\n");
         adr = (unsigned long *)0x80ff0004;
         value = *adr;
         count = value & 0x7ffff;
         ovfl = (value>>19) & 1;
         printf(" scaler %d value is %x  count=%d  ovfl=%d\n",
          scaler,value,count,ovfl);
         goto loop;
       }

    if(mode == 4)
       { for(scaler=0;scaler<97;scaler++)
          { adr = (unsigned long *)0x80ff0010;
            *adr = scaler;
            adr = (unsigned long *)0x80ff0004;
            value = *adr;
            count = value & 0x7ffff;
            ovfl = (value>>19) & 1;
            printf(" scaler %d value is %x  count=%d  ovfl=%d\n",
             scaler,value,count,ovfl);
          }
         goto loop;
       }

    if(mode == 5)
       { printf(" trigger words\n");
         adr = (unsigned long *)0x80ff001c;
         value1 =*adr;
         adr = (unsigned long *)0x80ff0020;
         value2 =*adr;
         adr = (unsigned long *)0x80ff0024;
         value3 =*adr;
         printf("trig1=%x  trig2=%x  trig3=%x\n",value1,value2,value3);
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
         fscanf(stdin,"%d",&fpmode);
         adr = (unsigned long *)0x80ff0028;
         *adr = fpmode;
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
       { printf(" enter [dac value] 0-4095\n");
         fscanf(stdin,"%d",&dac);
         for(abcd=0;abcd<4;abcd++)
          { for(ldac=0;ldac<24;ldac++)
             {  setdac(abcd,ldac,dac);
             }
          }
         goto loop;
       }
    if(mode == 9)
       { printf(" enter time in seconds\n");
         fscanf(stdin,"%d",&second);
         adr = (unsigned long *)0x80ff000c;
         *adr = 0;  /* disable scalers */
         adr = (unsigned long *)0x80ff0000;
         *adr = 0;  /* clear scalers */
         adr = (unsigned long *)0x80ff0008;
         *adr = 0;  /* enable scalers */
         sleep(second);          
         adr = (unsigned long *)0x80ff000c;
         *adr = 0;  /* disable scalers */
         /* print out nonzero scalers */
         for(scaler=0;scaler<97;scaler++)
          { adr = (unsigned long *)0x80ff0010;
            *adr = scaler;
            fval = sin(0.13284);
            adr = (unsigned long *)0x80ff0004;
            value = *adr;
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

    if(mode == 10)
       { printf(" enabling gate\n");
         adr = (unsigned long *)0x80ff002c;
         *adr = 0;
         goto loop;
       }

    if(mode == 11)
       { printf(" disabling gate\n");
         adr = (unsigned long *)0x80ff0030;
         *adr = 0;
         goto loop;
       }

    if(mode == 12) exit(0);

    goto loop;
  }

 setdac(abcd,ldac,dac)
 int abcd,ldac,dac;
  { unsigned long k1,k2,*adr;
/*  abcd 0=A  1=C  2=B  3=D  */
    k1 = (abcd<<12) | dac;
    adr = (unsigned long *)0x80ff0014;
    *adr = k1;      /* latch dac value and abcd select */
    k2 = (1<<ldac);
    adr = (unsigned long *)0x80ff0018;
    *adr = k2;  /* strobe dac line */
  }
