#include <stdio.h>
#include <time.h>
#include "vme.h"

time_t usec;
unsigned long vmebase,badr;
int ldac_set[96]={0,1,1,1,1,0,0,0,
                  2,3,3,3,3,2,2,2,
                  4,5,5,5,5,4,4,4,
                  6,7,7,7,7,6,6,6,
                  8,9,9,9,9,8,8,8,
                  10,11,11,11,11,10,10,10,
                  12,13,13,13,13,12,12,12,
                  14,15,15,15,15,14,14,14,
                  16,17,17,17,17,16,16,16,
                  18,19,19,19,19,18,18,18,
                  20,21,21,21,21,20,20,20,
                  22,23,23,23,23,22,22,22};
int abcd_set[96]={0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                  0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                  0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                  0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                  0,3,1,2,0,3,1,2,0,3,1,2,0,3,1,2,
                  0,3,2,2,0,3,1,2,0,3,1,2,0,3,1,2};
int dacset[96],set[96];
main()
 { FILE *fp;
   char xstring[80],ystring[80];
   unsigned long *adr,k;
   int mode,abcd,ldac,dac,scaler,fpmode,base,kk;
   int count,ovfl,step,dacmax,dacmin,extra;
   unsigned long value,value1,value2,value3;
   double fdac,fvalue,volt,mv;
   printf(" input card base address <fc, fd, fe, or ff>\n");
   fscanf(stdin,"%x",&kk);
/*   kk = 0xff;  */
   badr = (kk<<16);
   vmebase = vme_a24_base(); 
   printf("vmebase=0x%x  badr=%x\n",vmebase,badr);
   printf(" enter 0=fixed global threshold   1=set to edge individual channels\n");
   fscanf(stdin,"%d%",&mode);
   if(mode == 0) goto fixed;
   printf("enter extra dac counts above zero count edge\n");
   fscanf(stdin,"%d%",&extra);
   dacmax=0;
   dacmin=1000;
   base=48;
   fp = fopen("threshold.dat","w");
   sprintf(xstring,"tower number");
   sprintf(ystring,"Threshold setting");
   dxyset(0,100.0,0.0,50.0,0.0,0,0);
   dxylbl(0,xstring,ystring);
   step=1;       
   for(scaler=0;scaler<96;scaler++) set[scaler]=0;
   for(dac=0;dac<400;dac=dac+step)
    { /* printf("dac=%d\n",dac);  */
      k = dac - dac%10;
/*      printf("dac=%d   k=%d\n",dac,k);  */
      if(k == dac) 
        { printf("\r dac=%d",dac);
	  fflush(stdout);
	}
      for(abcd=0;abcd<4;abcd++)
        { for(ldac=0;ldac<24;ldac++) 
            { setdac(abcd,ldac,dac);
	    }
	}
  /*    printf("taking data\n");  */
      adr = (unsigned long *)(vmebase + badr + 0x0c);
      *adr = 0;  /* disable scalers */
      adr = (unsigned long *)(vmebase + badr + 0x00);
      *adr = 0;  /* clear scalers */
      adr = (unsigned long *)(vmebase + badr + 0x30);
      *adr = 0;  /* disable gate */
      adr = (unsigned long *)(vmebase + badr + 0x08);
      *adr = 0;  /* enable scalers */
      usleep(10);          
      adr = (unsigned long *)(vmebase + badr + 0x0c);
      *adr = 0;  /* disabler scalers */
  /*    printf("taking data done\n");  */

      for(scaler=0;scaler<96;scaler++)
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
    /*     if(dac<100) printf(" dac=%d  scaler=%d  value=%x\n",dac,scaler,value);  */
         if(count==0 && dac>10 && set[scaler]==0)
           { dacset[scaler] = dac;
             set[scaler]=1;
           }
         if(count > 10 && set[scaler]>0) set[scaler]=0;
      }  /* end of scaler */
    } /* end of dac */
  for(scaler=0;scaler<96;scaler++) 
   {  fdac = (double)dacset[scaler];
      volt = 1.24*fdac/4095.0;
      mv = 1000.0*volt;
      abcd = abcd_set[scaler];
      ldac = ldac_set[scaler];
      setdac(abcd,ldac,dacset[scaler]+extra);
      printf(" channel=%d %d %d  threshold  dac=%d  mv=%f\n",scaler,ldac,abcd,dacset[scaler]+extra,mv);
      fprintf(fp," channel=%d %d %d  threshold  dac=%d  mv=%f\n",scaler,ldac,abcd,dacset[scaler]+extra,mv);
      dxyacc(0,(double)scaler,fdac,0.0,1);
      if( (dacset[scaler]+extra) < dacmin ) dacmin=dacset[scaler]+extra;
      if( (dacset[scaler]+extra) > dacmax ) dacmax=dacset[scaler]+extra;
   }
   printf("dacmin=%d  dacmax=%d\n",dacmin,dacmax);
   fprintf(fp,"dacmin=%d  dacmax=%d\n",dacmin,dacmax);
    dxywrt(0);
    goto run;
fixed:
    printf(" enter global threshold value in mV\n");
    fscanf(stdin,"%lf",&mv);
    fdac=4095.0*mv/1250.0;
    dac = (int)fdac;
    printf(" mV=%d  dac=%d\n",mv,dac);
      for(abcd=0;abcd<4;abcd++)
        { for(ldac=0;ldac<24;ldac++) 
            { setdac(abcd,ldac,dac);
	    }
	}
run:    
      adr = (unsigned long *)(vmebase + badr + 0x0c);
      *adr = 0;  /* disable scalers */
      adr = (unsigned long *)(vmebase + badr + 0x00);
      *adr = 0;  /* clear scalers */
      printf(" enter 0=gate disabled mode  1=gate enabled mode\n");
      fscanf(stdin,"%d",&mode);
      if(mode == 0)
        { adr = (unsigned long *)(vmebase + badr + 0x30);
          *adr = 0;  /* disable gate */
	}
      if(mode != 0)
        { adr = (unsigned long *)(vmebase + badr + 0x2c);
          *adr = 0;  /* disable gate */
	}
      printf("enter 1 to enable data taking\n");
      fscanf(stdin,"%d",&mode);
      adr = (unsigned long *)(vmebase + badr + 0x08);
      *adr = 0;  /* enable scalers */
      printf("data taking enabled\n");
} /* end of main */

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
  
/*  sw(int i)
  { int k,l,shift;
    unsigned long *adr;
    adr = (unsigned long *)(vmebase+0x00aa000c);
    k = *adr;
    usleep(usec);
    shift = i-1;
    l = (k>>shift) & 1;
    return(l);
  }  */

  sw(int i)
  { int l;
    l = 0;
    return(l);
  }

