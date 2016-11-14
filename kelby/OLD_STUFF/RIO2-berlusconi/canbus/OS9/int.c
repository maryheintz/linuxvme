#include <stdio.h>
#include <math.h>
#include <time.h>
char xstring[80],ystring[80],string[80],err[80];
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   int isw[6],i1,i2,ll,i,mode,s1,s2,s3,s4,itr,ire,mse,tpe,lcal;
   int sw1,dacset,max[6],del[6],k,kkk;
   short cadr,sval,dval,tval,t1val,t2val;
   double fmin,ratio,volt,ped,expr[6];
   double a,b,ped0,ped100,dac,factor;
   int zone,sector,card,l,gain,pause;
   unsigned int itst;
   unsigned short shrt;
   unsigned char z,s,c;
   long status;
   int bits[12];
   factor=1.0;
   ttcvi_map();
   can_init();
   status=adc_delay(20);
   if(status != 0) printf(" adc_delay status=%x\n",status);
   status=adc_dacset(120);
   if(status != 0) printf(" adc_dacset status=%x\n",status);
   status=adc_get_delay(&shrt);
   printf(" status=%x  delay back=%d\n",status,(int)shrt);
   zone = 0;
   sector = 0;  
  for(i=0;i<12;i++) bits[i]=0;
   set_zone(zone);
   set_sector(sector);
   if(argc != 2)
    {for(k=0;k<49;k++)
       { if(cardchk(k))
          { card = k;
            goto found;}
       }
   printf("no card found\n");
   return(0);
found:
   printf("card found = %d\n",card);
   set_tube(card);
   }
   if(argc == 2)
     { sscanf(argv[1],"%d",&card);
       printf("card %d requested\n",card);
       set_tube(card);
     }
   z = (unsigned char)zone;
   s = (unsigned char)sector;
   c = (unsigned char) card;   
   multi_low();
   rxw_low();

   max[0]=250;
   max[1]=100;
   max[2]=70;
   max[3]=40;
   max[4]=25;
   max[5]=20;

   del[0]=25;
   del[1]=2;
   del[2]=2;
   del[3]=1;
   del[4]=1;
   del[5]=1;

   expr[0]=1.0;
   expr[1]=2.7;
   expr[2]=3.7;
   expr[3]=7.2;
   expr[4]=9.7;
   expr[5]=13.2;

   isw[0]=0xe;
   isw[1]=0xd;
   isw[2]=0xf;
   isw[3]=0xb;
   isw[4]=0x7;
   isw[5]=0x3;

   for(i=0;i<6;i++)
   { dxyset(i,(double)max[i],0.0,5000.0,0.0,0,0);
     sprintf(xstring,"isw=%x xaxis=dac value",isw[i]);
     sprintf(ystring,"yaxis=volts");
     dxylbl(i,xstring,ystring);
    }
   for(gain=0;gain<6;gain++)
    {set_intg_gain(isw[gain]);
     set_dac(0);  /* dac=0 */
     intg_rd_low();
     itr_low();
     tsleep(0x80000010);
     status=adc_convert(z,s,c,&shrt);
     ped = factor * (double)shrt;
     printf("gain setting %d  ped=%f\n",gain,ped);
     dacset = max[gain];
     for(i=0;i<dacset;i=i+del[gain])       
       { sw1 = sw(1);
         set_dac(i);
         tsleep(0x80000010);
         status=adc_convert(z,s,c,&shrt);
         volt = factor * (double)shrt;
         fmin=volt-ped;
         if(sw1 == 0) printf("dac=%d  volt=%f  ped=%f  shrt=%x\n",
             i,volt,ped,shrt);
         dxyacc(gain,(double)i,fmin,0.0,0);
         for(k=0;k<12;k++)
           { kkk = (shrt>>k) & 1;
             if(kkk == 1) bits[k]++; }
       }
    }
    for(k=0;k<12;k++) printf("k=%d  bits[k]=%d\n",k,bits[k]);
   set_dac(48);
   tsleep(0x80000020);
   intg_rd_low();
   itr_low();
   sleep(1);
   status=adc_convert(z,s,c,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=0 on volt=%f\n",volt);

   intg_rd_low();
   itr_high();
   sleep(1);
   status=adc_convert(z,s,c,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=1 on volt=%f\n",volt);

   intg_rd_high();
   itr_low();
   sleep(1);
   status=adc_convert(z,s,c,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=1 itr=0 on volt=%f\n",volt);

   intg_rd_low();
   itr_low();
   sleep(1);
   status=adc_convert(z,s,c,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=0 on volt=%f\n",volt);
   printf("card=%d\n",card);
   printf("\n\n\n");
   fflush(stdout);
   sidev(1);
   dxymwr(6,0,5);
}
      double dxyfit(l,k,x)
      int l,k;
      double x;
      { double dxyf;
        dxyf=0.0;  /* dummy routine */
        return(dxyf);
      }
