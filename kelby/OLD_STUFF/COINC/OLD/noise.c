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
   int sset[96],sdac[96],scount[96];
   double svolt[96];
main()
 { FILE *fpch,*fpth;
   char xstring[80],ystring[80];
   unsigned long *adr,k;
   int dac,scaler;
   int count,ovfl;
   double fdac,fvalue,volt;

     fpch = fopen("/h0/kja/coinc/channel.dat","w");
     fpth = fopen("/h0/kja/coinc/threshold.dat","w");
 
       sprintf(ystring,"min threshold (mv)");
       sprintf(xstring,"channel");
       dxyset(0,100.0,0.0,20.0,0.0,0,0);
       dxylbl(0,xstring,ystring);

       for(k=0;k<96;k++) sset[k] = 0;

         for(dac=25;dac>0;dac=dac-1)
          { fdac = (double)dac;
            volt = 1.24*fdac/4095.0;
            for(k=0;k<96;k++) setdac(k,dac);
              adr = (unsigned long *)0x80ff000c;
              *adr = 0;  /* disable scalers */
              adr = (unsigned long *)0x80ff0000;
              *adr = 0;  /* clear scalers */
              adr = (unsigned long *)0x80ff0030;
              *adr = 0;  /* disable gate */
              adr = (unsigned long *)0x80ff0008;
              *adr = 0;  /* enable scalers */
              sleep(2);          
              adr = (unsigned long *)0x80ff000c;
              *adr = 0;  /* disabler scalers */
         /* print out nonzero scalers */
            for(scaler=0;scaler<96;scaler++)
              { get_scaler(scaler,&ovfl,&count);
                fvalue = (double)count;
                if(scaler == 18) 
                  printf("scaler=%d abcd=%d ldac=%d dac=%d volt=%f count=%d\n",
                      scaler,abcd[scaler],ldac[scaler],dac,volt,count);
                if(sset[scaler] == 0)
                  if(count>2)
                   { sset[scaler] = 1;
                     svolt[scaler] = volt;
                     sdac[scaler] = dac;
                     scount[scaler] = fvalue;
                   }
             }
       }
      for(k=0;k<96;k++) 
       { printf(" scaler = %d   volts=%f  dac=%d  count=%d\n",
          k,svolt[k],sdac[k],scount[k]);
         fdac = (double)k;
         fvalue = 1000.0*svolt[k];
         dxyacc(0,fdac,fvalue,0.0,0);
         fprintf(fpch,"%d\n",k);
         fprintf(fpth,"%d\n",sdac[k]);
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
