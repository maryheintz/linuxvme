#include <stdio.h>
main()
 { char xstring[80],ystring[80];
   unsigned long *adr,k;
   int mode,abcd,ldac,dac,scaler,fpmode,count,ovfl;
   unsigned long value,value1,value2,value3;
   double fdac,fvalue,volt;
   int c10,c4000;
         for(abcd=0;abcd<4;abcd++)
           for(ldac=0;ldac<24;ldac++)
              setdac(abcd,ldac,4000);
         printf(" enter [tower]\n");
         fscanf(stdin,"%d",&scaler);
         for(abcd=0;abcd<4;abcd++)
           for(ldac=0;ldac<24;ldac++)
             {   dac = 10;
                setdac(abcd,ldac,dac);
              adr = (unsigned long *)0x80ff000c;
              *adr = 0;  /* disable scalers */
              adr = (unsigned long *)0x80ff0000;
              *adr = 0;  /* clear scalers */
              adr = (unsigned long *)0x80ff002c;
              *adr = 0;  /* enable gate */
              adr = (unsigned long *)0x80ff0008;
              *adr = 0;  /* enable scalers */
              tsleep(0x80000050);          
              adr = (unsigned long *)0x80ff000c;
              *adr = 0;  /* disabler scalers */
         /* print out nonzero scalers */
            adr = (unsigned long *)0x80ff0010;
            *adr = scaler;
            adr = (unsigned long *)0x80ff0004;
            value = *adr;
            count = value & 0x7ffff;
            c10 = count;
                dac = 4000;
                setdac(abcd,ldac,dac);
              adr = (unsigned long *)0x80ff000c;
              *adr = 0;  /* disable scalers */
              adr = (unsigned long *)0x80ff0000;
              *adr = 0;  /* clear scalers */
              adr = (unsigned long *)0x80ff002c;
              *adr = 0;  /* enable gate */
              adr = (unsigned long *)0x80ff0008;
              *adr = 0;  /* enable scalers */
              tsleep(0x80000050);          
              adr = (unsigned long *)0x80ff000c;
              *adr = 0;  /* disabler scalers */
         /* print out nonzero scalers */
            adr = (unsigned long *)0x80ff0010;
            *adr = scaler;
            adr = (unsigned long *)0x80ff0004;
            value = *adr;
            count = value & 0x7ffff;
            c4000 = count;
            printf("k=%d abcd=%d  ldac=%d  c10=%d  c4000=%d\n",
             scaler,abcd,ldac,c10,c4000);
       }
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
double dxyfit(l,k,x)
int l,k;
double x;
  { double dxyf;
    dxyf=0.0;
    return(dxyf);
  }
