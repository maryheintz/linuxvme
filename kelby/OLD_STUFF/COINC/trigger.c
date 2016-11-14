#include <stdio.h>
    int abcd[96] ={0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                   0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                   0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                   0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                   0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                   0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2};

    int ldac[96] ={0,1,1,1,1,0,0,0,2,3,3,3,3,2,2,2,
                   4,5,5,5,5,4,4,4,6,7,7,7,7,6,6,6,
                   8,9,9,9,9,8,8,8,10,11,11,11,11,10,10,10,
                   12,13,13,13,13,12,12,12,14,15,15,15,15,14,14,14,
                   16,17,17,17,17,16,16,16,18,19,19,19,19,18,18,18,
                   20,21,21,21,21,20,20,20,22,23,23,23,23,22,22,22};

main()
 { FILE *fpdac,*fpc0,*fpc7,*fpc95,*fptg;
   char xstring[80],ystring[80];
   unsigned long *adr,k;
   int mode,dac,scaler,fpmode,base;
   int count,ovfl,step;
   unsigned long value,value1,value2,value3;
   double fdac,fvalue,volt,mv,eff,c0,c7,c95,c96;

        fpdac = fopen("/h0/kja/coinc/dac.dat","w");
        fpc0 = fopen("/h0/kja/coinc/c0.dat","w");
        fpc7 = fopen("/h0/kja/coinc/c7.dat","w");
        fpc95 = fopen("/h0/kja/coinc/c95.dat","w");
        fptg = fopen("/h0/kja/coinc/ctg.dat","w");
        sprintf(xstring,"threshold [mV]");
        sprintf(ystring,"Efficiency");
        dxyset(0,50.0,0.0,2.0,0.0,0,0);
        dxylbl(0,xstring,ystring);

         /* set up the referencd channel */
         setdac(0,450);
 
         step=5;
         for(dac=200;dac>4;dac=dac-step)
          { 
             setdac(7,dac);
             setdac(95,dac);

              volt = 1.24*fdac/4095.0;
              mv = 1000.0*volt;
              fdac = (double)dac;

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

         /* get the reference scaler value */

              get_scaler(0,&ovfl,&count);
              fvalue = (double)count;
              c0 = fvalue;

              get_scaler(7,&ovfl,&count);
               fvalue = (double)count;
              c7 = fvalue;

              get_scaler(95,&ovfl,&count);
               fvalue = (double)count;
              c95 = fvalue;

              get_scaler(96,&ovfl,&count);
               fvalue = (double)count;
              c96 = fvalue;

              eff = c96/c0;
              dxyacc(0,mv,eff,0.0,1);
             printf("dac=%d  c0=%f  c7=%f  c95=%f  trig=%f\n",
               dac,c0,c7,c95,c96);
             fprintf(fpdac,"%d\n",dac);
             fprintf(fpc0,"%f\n",c0);
             fprintf(fpc7,"%f\n",c7);
             fprintf(fpc95,"%f\n",c95);
             fprintf(fptg,"%f\n",c96);
          }
          dxywrt(0);
      }

 setdac(k,dac)
 int k,dac;
  { unsigned long k1,k2,*adr;
/*  abcd 0=A  1=C  2=B  3=D  */
    k1 = (abcd[k]<<12) | dac;
    adr = (unsigned long *)0x80ff0014;
    *adr = k1;      /* latch dac value and abcd select */
    k2 = (1<<ldac[k]);
    adr = (unsigned long *)0x80ff0018;
    *adr = k2;  /* strobe dac line */
  }

   get_scaler(k,ovfl,count)
   int k,*ovfl,*count;
    { unsigned long *adr,value;
      adr = (unsigned long *)0x80ff0010;
      *adr = k;
      adr = (unsigned long *)0x80ff0004;
      value = *adr;
      *count = value & 0x7ffff;
      *ovfl = (value>>19) & 1;
    }
double dxyfit(l,k,x)
int l,k;
double x;
  { double dxyf;
    dxyf=0.0;
    return(dxyf);
  }
