
#include <stdio.h>
#include <time.h>
#include "vme.h"

time_t usec;
unsigned long vmebase,badr;
main()
 { FILE *fp;
   char xstring[80],ystring[80];
   unsigned long *adr,k;
   int mode,abcd,ldac,dac,scaler,fpmode,base,kk;
   int count,ovfl,step;
   unsigned long value,value1,value2,value3;
   double fdac,fvalue,volt,mv;
   double plateau[8],psum[8],sum[8],half1[8],half2[8],half[8];
   double a,b,dhalf[8],dac1[8],dac2[8];
   printf(" input card base address <fc, fd, fe, or ff>\n");
   fscanf(stdin,"%x",&kk);
   badr = (kk<<16);
   vmebase = vme_a24_base(); 
   printf("vmebase=0x%x  badr=%x\n",vmebase,badr);
       fp = fopen("plat.dat","w");
       printf("input starting channel\n");
       fscanf(stdin,"%d",&base);
       for(k=0;k<8;k++)
         { 
           sprintf(xstring,"threshold (volts)");
           sprintf(ystring,"EVENTS");
           dxyset(k,150.0,0.0,5000.0,0.0,0,0);
           dxylbl(k,xstring,ystring);
           plateau[k] = 0.0;
           sum[k] = 0.0;
           half1[k] = 0.0;
           half2[k] = 0.0;
           half[k] = 0.0;
         }
         step=5;       
         for(dac=5;dac<400;dac=dac+step)
          {  if(dac>=50) step=10;
              for(abcd=0;abcd<4;abcd++)
              for(ldac=0;ldac<24;ldac++) 
                setdac(abcd,ldac,dac);
              adr = (unsigned long *)(vmebase + badr + 0x0c);
              *adr = 0;  /* disable scalers */
              adr = (unsigned long *)(vmebase + badr + 0x00);
              *adr = 0;  /* clear scalers */
              if(sw(7) == 0) adr = (unsigned long *)(vmebase + badr + 0x2c); /* enable gate */
              if(sw(7) == 1) adr = (unsigned long *)(vmebase + badr + 0x30); /* disable gate */
              *adr = 0;  /* enable gate */
              adr = (unsigned long *)(vmebase + badr + 0x08);
              *adr = 0;  /* enable ot disable scalers */
              usleep(5000);          
              adr = (unsigned long *)(vmebase + badr + 0x0c);
              *adr = 0;  /* disabler scalers */
         /* print out nonzero scalers */
            for(scaler=base;scaler<base+8;scaler++)
              { adr = (unsigned long *)(vmebase + badr + 0x10);
                *adr = scaler;
                adr = (unsigned long *)(vmebase + badr + 0x04);
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
                if(dac>50 && dac<75)
                 { sum[k] = sum[k] + 1.0;
                   psum[k] = psum[k] + fvalue;
                 }
                if(dac == 100 && sum[k]>0.0) plateau[k] = psum[k] / sum[k];
                if(dac > 100 && fvalue > 0.5*plateau[k]) 
                  { half1[k] = fvalue;
                    dac1[k] = (double)dac;
                  }
                if(dac > 100 && fvalue < 0.5*plateau[k] && half2[k] == 0.0)
                  { half2[k] = fvalue;
                    dac2[k] = (double)dac;
                  }
             }
       }
         for(k=0;k<8;k++)
            { if(sw(4) == 1) printf("k=%d  plateau=%g dac1=%g  dac2=%g  half1=%g  half2=%g\n",
                    k,plateau[k],dac1[k],dac2[k],half1[k],half2[k]);
              if(half1[k] == half2[k] || dac1[k] == dac2[k] || dac1[k]==0) 
               { printf(" channel %d problem\n",k);
                 continue;
               }
              half[k] = 0.5*plateau[k];
            /*  printf("h1=%f  h2=%f  d1=%f  d2=%f  h=%f\n",
               half1[k],half2[k],dac1[k],dac2[k],half[k]);  */
              a = (half1[k] - half2[k]) / (dac1[k] - dac2[k]);
              b = half1[k] - a*dac1[k];
              dhalf[k] = (half[k] - b) / a;
              printf(" channel %d  edge %f\n",k,dhalf[k]);
            }
      for(k=0;k<8;k++) dxywrt(k);
  }

 setdac(abcd,ldac,dac)
 int abcd,ldac,dac;
  { unsigned long k1,k2,*adr;
/*    printf(" setdac dat=%d\n",dac); */
/*  abcd 0=A  1=C  2=B  3=D  */
    k1 = (abcd<<12) | dac;
    adr = (unsigned long *)(vmebase + badr + 0x14);
    *adr = k1;      /* latch dac value and abcd select */
    k2 = (1<<ldac);
    adr = (unsigned long *)(vmebase + badr + 0x18);
    *adr = k2;  /* strobe dac line */
  }
double dxyfit(l,k,x)
int l,k;
double x;
  { double dxyf;
    dxyf=0.0;
    return(dxyf);
  }
  
  sw(int i)
  { int k,l,shift;
    unsigned long *adr;
    adr = (unsigned long *)(vmebase+0x00aa000c);
    k = *adr;
    usleep(usec);
    shift = i-1;
    l = (k>>shift) & 1;
    return(l);
  }

