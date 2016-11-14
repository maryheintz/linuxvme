#include <stdio.h>
main()
 { FILE *fp;
   char xstring[80],ystring[80];
   unsigned long *adr,k;
   int mode,abcd,ldac,dac,scaler,fpmode,base;
   int count,ovfl,step;
   unsigned long value,value1,value2,value3;
   double fdac,fvalue,volt,mv;
   double plateau[8],psum[8],sum[8],half1[8],half2[8],half[8];
   double a,b,dhalf[8],dac1[8],dac2[8];
       fp = fopen("/h0/kja/coinc/plat.dat","w");
       printf("input starting channel\n");
       fscanf(stdin,"%d",&base);
       for(k=0;k<8;k++)
         { 
           sprintf(xstring,"threshold (volts)");
           sprintf(ystring,"EVENTS");
           dxyset(k,100.0,0.0,20000.0,0.0,0,0);
           dxylbl(k,xstring,ystring);
           plateau[k] = 0.0;
           sum[k] = 0.0;
           half1[k] = 0.0;
           half2[k] = 0.0;
           half[k] = 0.0;
         }
         step=1;       
         for(dac=15;dac<400;dac=dac+step)
          {  if(dac==20) step=10;
              for(abcd=0;abcd<4;abcd++)
              for(ldac=0;ldac<24;ldac++) 
                setdac(abcd,ldac,dac);
              adr = (unsigned long *)0x80ff000c;
              *adr = 0;  /* disable scalers */
              adr = (unsigned long *)0x80ff0000;
              *adr = 0;  /* clear scalers */
              adr = (unsigned long *)0x80ff002c;
              *adr = 0;  /* enable gate */
              adr = (unsigned long *)0x80ff0008;
              *adr = 0;  /* enable scalers */
              tsleep(0x80000095);          
              adr = (unsigned long *)0x80ff000c;
              *adr = 0;  /* disabler scalers */
         /* print out nonzero scalers */
            for(scaler=base;scaler<base+8;scaler++)
              { adr = (unsigned long *)0x80ff0010;
                *adr = scaler;
                adr = (unsigned long *)0x80ff0004;
                value = *adr;
                count = value & 0x7ffff;
                ovfl = (value>>19) & 1;
                fdac = (double)dac;
                fvalue = (double)value;
                volt = 1.24*fdac/4095.0;
                mv = 1000.0*volt;
        /*        if(scaler ==28) fprintf(fp,"dac=%d count=%d\n",dac,value); */
                if(scaler == base) fprintf(fp,"%d\n",value);   
             if(sw(1) == 1 || scaler==base) 
                  printf(" dac=%d  value=%d  ovfl=%d  mv=%f\n",
                    dac,value,ovfl,mv);
                k=scaler-base;
                dxyacc(k,mv,fvalue,0.0,1);
                if(dac>100 && dac<200)
                 { sum[k] = sum[k] + 1.0;
                   psum[k] = psum[k] + fvalue;
                 }
                if(dac == 200 && sum[k]>0.0) plateau[k] = psum[k] / sum[k];
                if(dac > 200 && fvalue > 0.5*plateau[k]) 
                  { half1[k] = fvalue;
                    dac1[k] = (double)dac;
                  }
                if(dac > 200 && fvalue < 0.5*plateau[k] && half2[k] == 0.0)
                  { half2[k] = fvalue;
                    dac2[k] = (double)dac;
                  }
             }
       }
      for(k=0;k<8;k++) dxywrt(k);
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