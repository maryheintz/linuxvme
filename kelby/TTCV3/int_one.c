#include <stdio.h>
#include <math.h>
#include <time.h>
#include "vme_base.h"
char xstring[80],ystring[80],string[80],err[80];
unsigned int id,ttcadr,itype,nall,ngood,nbad;
double v1[50],v2[50],v3[50],v4[50];
double ped[50][6],sped[50][6],vlt[50][6][100],dc[50][6][100];
double nonlin[50][6],aa[50][6],bb[50][6];
int imx[6],ii[6],dok[50],ok[50];
unsigned int card;
   double slp[6] ={0.013,0.12,0.132,0.252,0.348,0.467};
   double dslp[6] = {0.001,0.02,0.010,0.020,0.035,0.050};
   double lnonl[6] = {3.0,5.0,5.0,8.0,10.0,20.0};
   double lsped[6] = {3.0,3.0,5.0,8.0,10.0,10.0};
char adcno[7];
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{  FILE *fpadc;
   unsigned char wd,*adr;
   int board;
   int isw[6],i,kl;
   int sw1,dacset,max[6],del[6],k,kkk;
   double volt,expr[6];
   double a,b,delta,dac,factor;
   double fdc,fvlt,s,sx,sy,sxy,sx2;
   double mean,sigma,sqr,val;
   int zone,sector,l,gain,pause;
   unsigned int itst,command;
   unsigned short shrt;
   unsigned long status;
   status = vme_a24_base();
   status = vme_a16_base(); 
   
/*   k = vme16base + 0x6081;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 01 wd=%x\n",wd);
   
   k = vme16base + 0x6083;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 03 wd=%x\n",wd);
   
   k = vme16base + 0x6085;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 05 wd=%x\n",wd);
   
   k = vme16base + 0x6087;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 07 wd=%x\n",wd);
   
   k = vme16base + 0x6089;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 09 wd=%x\n",wd);
   
   k = vme16base + 0x608b;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 0b wd=%x\n",wd);
   
   k = vme16base + 0x608d;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 0d wd=%x\n",wd);
   
   k = vme16base + 0x608f;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 0f wd=%x\n",wd);
   
   k = vme16base + 0x6091;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 11 wd=%x\n",wd);
   
   k = vme16base + 0x6093;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 13 wd=%x\n",wd);
   
   k = vme16base + 0x6095;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 15 wd=%x\n",wd);
   
   k = vme16base + 0x6097;
   adr = (unsigned char *) k;
   wd = *adr;
   printf(" 17 wd=%x\n",wd); */
   
   printf("enter <ttc address>\n");
   fscanf(stdin,"%x",&ttcadr);  
   fpadc=fopen("/home/kelby/TTCV3/adc.num","r");
   fscanf(fpadc,"%s",adcno);
   printf("enter <card number>\n");
   fscanf(stdin,"%d",&card);  
   printf("card=%d   ttcadr=%x   adc=%s\n",card,ttcadr,adcno);

   board = 1;
   id = (board<<6) | 1; 
   ttcvi_mapr(ttcadr);
   reset_ctrl_can();
   printf("calling can_reset - this takes a few seconds\n");
   can_reset(ttcadr);
   can_init(board,adcno);
   kkk = cardchk(card);
   if(kkk != 1) 
     { printf("no card found\n");
       return(0);
     }
found:
   printf("card found = %d\n",card);
   set_tube(ttcadr,card);
   multi_low(ttcadr);
   rxw_low(ttcadr);
   back_low(ttcadr);
   mse_high(ttcadr);
       
    multi_high(ttcadr);
    factor = 9.6/4096.0;  /* volts per count  */

    max[0]=300;
    max[1]=60;
    max[2]=60;
    max[3]=35;
    max[4]=20;
    max[5]=18;

    del[0]=15;
    del[1]=5;
    del[2]=5;
    del[3]=3;
    del[4]=2;
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


   for(gain=0;gain<6;gain++)
    {printf("starting gain=%d\n",gain);
     if(gain==0) status=adc_dacset(id,100);
     if(gain==1) status=adc_dacset(id,100);
     if(gain==2) status=adc_dacset(id,100);
     if(gain==3) status=adc_dacset(id,80);
     if(gain==4) status=adc_dacset(id,70);
     if(gain==5) status=adc_dacset(id,50);
     if(status != 0) printf(" adc_dacset status=%x\n",status);
     intg_rd_low(ttcadr);
     itr_low(ttcadr);
     set_dac(ttcadr,0);  /* dac=0 */
     set_intg_gain(ttcadr,isw[gain]);
     usleep(50);
        s=0.0;
         sx=0.0;
	 sx2=0.0;
         set_tube(ttcadr,card);
	 for(i=0;i<25;i++)
           { command = 0xc400 | card;
             status=adc_convert(id,command,&shrt);
	     val = factor * (double)shrt;
	     s=s+1.0;;
	     sx = sx + val;
	     sx2 = sx2 + val*val;
	     if(sw(4) == 1) printf("i=%d val=%f  shrt=%x\n",i,val,shrt);
	   }
         mean=sx/s;
	 sqr = sx2/s - (mean*mean);
	 sigma=0.0;
	 if(sqr>0.0) sigma = sqrt(sqr);
	 ped[card][gain] = mean;
	 sped[card][gain] = sigma;
	 if(sped[card][gain]/factor>lsped[gain]) ok[card] = 20+gain;
         printf("card=%d  gain=%d  ped=%f sped=%f  shrt=%x\n",
	         card,gain,mean,sigma,shrt);
       
     ii[gain]=0;
     dacset = max[gain];
     for(i=0;i<dacset;i=i+del[gain])       
       { set_dac(ttcadr,i);
         usleep(50);
	/* card=kard; */
	    command = 0xc400 | card;
             status=adc_convert(id,command,&shrt);
             volt = factor * (double)shrt;
	     if(i==0) ped[card][gain] = volt;  /* ----- kja ----- */
             vlt[card][gain][ii[gain]]=volt-ped[card][gain];
	     dc[card][gain][ii[gain]]=i;
	       printf("card=%d dac=%d  volt=%f  ped=%f  vlt=%f shrt=%d\n",
                card,i,volt,ped[card][gain],vlt[card][gain][ii[gain]],shrt);
	   
        imx[gain] =ii[gain];
        ii[gain]++;
       }
/*   printf("card=%d  gain=%d  dacset=%d  del=%d  imx=%d\n",
      card,gain,dacset,del[gain],imx[gain]);  */
     }
        
   /* card=kard; */
   set_tube(ttcadr,card);
   usleep(2);
   set_dac(ttcadr,48);
   usleep(2);
   intg_rd_low(ttcadr);
   usleep(2);
   itr_low(ttcadr);
   sleep(1);
   /* card=kard;  */
      command = 0xc400 | card;
      status=adc_convert(id,command,&shrt);
      volt = factor * (double)shrt;
      v1[card] = volt;
      printf("card=%d intg_rd=0 itr=0 on volt=%f\n",
          card,volt);
    
    
   set_dac(ttcadr,0);
   usleep(20);
   intg_rd_low(ttcadr);
   usleep(2);
   itr_high(ttcadr);
   usleep(2);
   set_dac(ttcadr,48);
   sleep(1);
   /* card=kard;  */
      command = 0xc400 | card;
      status=adc_convert(id,command,&shrt);
      volt = factor * (double)shrt;
      v2[card] = volt;
      printf("card=%d intg_rd=0 itr=1 on volt=%f\n",
          card,volt);
   set_dac(ttcadr,0);
   usleep(2);
   intg_rd_high(ttcadr);
   usleep(2);
   itr_low(ttcadr);
   usleep(2);
   set_dac(ttcadr,48);
   sleep(1);
   /* card=kard; */
      command = 0xc400 | card;
      status=adc_convert(id,command,&shrt);
      volt = factor * (double)shrt;
      v3[card] = volt;
      printf("card=%d intg_rd=1 itr=0 on volt=%f\n",
          card,volt);
    

   intg_rd_low(ttcadr);
   usleep(2);
   itr_low(ttcadr);
   sleep(1);
   /*card=kard; */
      command = 0xc400 | card;
      status=adc_convert(id,command,&shrt);
      volt = factor * (double)shrt;
      v4[card] = volt;
      printf("card=%d intg_rd=0 itr=0 on volt=%f\n",
          card,volt);
     
   hstset(0,1.0,0.0,50,1);
   hstset(1,1.0,0.0,50,1);
   hstset(2,1.0,0.0,50,1);
   hstset(3,1.0,0.0,50,1);
   hstset(4,1.0,0.0,50,1);
   hstset(5,1.0,0.0,50,1);
   ngood=0;
   nbad=0;
   nall=0;
   /*  card=kard; */
   usleep(2);
      dxyzro();
      dxyft_set(0,1,5);
      itype=0;
      ok[card]=0;
      for(gain=0;gain<6;gain++)
       { s=0.0;
         sx=0.0;
         sy=0.0;
         sxy=0.0;
         sx2=0.0;
	 if(gain == 0) dxyset(gain,(double)max[gain],0.0,4.0,0.0,0,0);
         if(gain != 0) dxyset(gain,(double)max[gain],0.0,10.0,0.0,0,0);
         sprintf(xstring,"isw=%x xaxis=dac value",isw[gain]);
         sprintf(ystring,"yaxis=volts");
         dxylbl(i,xstring,ystring);
	  if(sw(6)==1) printf("card %d  imx=%d\n",card,imx[gain]);
          for(i=0;i<=imx[gain];i++)
	    { fdc = dc[card][gain][i];
	      fvlt = vlt[card][gain][i];
	      dxyacc(gain,fdc,fvlt,0.0,0);
	      s=s+1.0;
	      sx=sx+fdc;
	      sy=sy+fvlt;
	      sxy=sxy+(fdc*fvlt);
	      sx2=sx2+(fdc*fdc);
              if(sw(6) == 1) printf(" i=%d  gain=%d  dc=%f  vlt=%f\n",
	               i,gain,fdc,fvlt);
	    }	       
      
         delta = (s*sx2) - (sx*sx);
         if(delta < 1.0) printf("error delta=%f\n",delta);
         if(sw(6)==1) printf("s=%f  sx=%f  sy=%f\n  sxy=%f  sx2=%f  delta=%f\n",
             s,sx,sy,sxy,sx2,delta);
         a = ((sx2*sy) - (sx*sxy)) / delta;
         b = ((s*sxy) - (sx*sy)) / delta;
	 aa[card][gain]=a;
	 bb[card][gain]=b;
         hstacc(gain,b,1.0);
         if(sw(6) == 1) printf(" card %d gain %d fit results a=%f b=%f volts=a+b*dac  ped=%f  sped=%f\n",
            card,gain,a,b,ped[card][gain],sped[card][gain]);
         if(gain<5 && (a<-0.10 || a>0.10)) ok[card]=6+gain;
         if(gain==5 && (a<-0.25 || a>0.25)) ok[card]=6+gain;
         if(b<(slp[gain]-dslp[gain]) || b>(slp[gain]+dslp[gain]) ) 
	         ok[card]=gain;
	 if(v1[card] < 9.0) ok[card] = 12;
	 if(v2[card] > 1.0) ok[card] = 13;
	 if(v3[card] > 1.0) ok[card] = 14;
	 if(v4[card] < 9.0) ok[card] = 15;
	 if(dok[card] !=0 ) ok[card]=16;
       
 if(sw(6) == 0) 
     { sidev(1);
       dxymwr(6,0,5);
     }  
   }
   if(sw(4) == 1) 
     { hstwrt(0);
       hstwrt(1);
       hstwrt(2);
       hstwrt(3);
       hstwrt(4);
       hstwrt(5);
     }
 /* ------------ calculate the non linearities ----------------- */
      for(gain=0;gain<6;gain++)
       { /* card=kard;  */
             s=0.0;
	     sx=0.0;       
             for(i=0;i<=imx[gain];i++)
	       { fdc = dc[card][gain][i];
	         fvlt = vlt[card][gain][i];
	         a=aa[card][gain];
	         b=bb[card][gain];
		 volt = a + b*fdc;
		 delta = fvlt-volt;
/*   if(card==20 && gain==3)   printf("gain=%d  card=%d  fdc=%f  fvlt=%f  volt=%f delta=%f\n",
               gain,card,fdc,fvlt,volt,delta);  
*/
		 s=s+1.0;
		 sx = sx+(delta*delta);
	       }
	      mean = sqrt(sx)/s;
	      nonlin[card][gain] = mean;
	      if(mean/factor>lnonl[gain]) ok[card] = gain+30;
/*	      if(mean/factor>lnonl[gain]) 
	         printf(" lnonl error card %d  gain %d  mean(cnt) %f  mean(v) %f\n",
		 card,gain,mean/factor,mean);  */
	      if(sw(8)==1) printf(" gain=%d  card=%d  nonlin=%f\n",gain,card,mean);
	   
      printf(" a= %f  b=%f  b-expected=%f %f\n",a,b,slp[gain]-dslp[gain],
                      slp[gain]+dslp[gain]);    
	 } 
 /* card=kard; */
   if(ok[card] == 0) printf(" card %d is OK\n",card);    
   nall++;
   if(ok[card] == 0) ngood++; 
   if(ok[card] != 0) nbad++;
   if(ok[card] != 0 && ok[card]<6 ) 
      printf(" card %d is BAD  ok=%d  b=%f  expected=%f\n",
               card,ok[card],bb[card][ok[card]],slp[ok[card]]);    
   if(ok[card] != 0 && ok[card]<12 && ok[card]>5 ) 
        printf(" card %d is BAD  ok=%d  a=%f\n",
	      card,ok[card],aa[card][ok[card]-6]);    
   if(ok[card] != 0 && ok[card] == 12 ) 
        printf(" card %d is BAD  ok=%d  v1=%f\n",card,ok[card],v1[card]);    
   if(ok[card] != 0 && ok[card] == 13 ) 
        printf(" card %d is BAD  ok=%d  v2=%f\n",card,ok[card],v2[card]);    
   if(ok[card] != 0 && ok[card] == 14 ) 
        printf(" card %d is BAD  ok=%d  v3=%f\n",card,ok[card],v3[card]);    
   if(ok[card] != 0 && ok[card] == 15 ) 
        printf(" card %d is BAD  ok=%d  v4=%f\n",card,ok[card],v4[card]);    
   if(ok[card] != 0 && ok[card] == 16 ) 
        printf(" card %d is BAD  ok=%d  no card found\n",card,ok[card]);
   if(ok[card] > 19 && ok[card]< 30)
      { gain = ok[card]-20; 
        printf(" card %d is bad %d failed gain %d sped(cnt) %f  sped(v) %f\n",
	     card,ok[card],gain,sped[card][gain]/factor,sped[card][gain]);
      }    
   if(ok[card] > 29 && ok[card]< 40)
      { gain = ok[card]-30; 
        printf(" card %d is bad %d failed gain %d nonlin(cnt) %f  nonlin(v) %f\n",
	     card,ok[card],gain,nonlin[card][gain]/factor,nonlin[card][gain]);
      }    
  
/* ---------------plot the slopes ----------------------------- */
   if(sw(5)==1)
     { dotset(0,50.0,0.0,0.02,0.0,0,0);
       dotset(1,50.0,0.0,0.2,0.0,0,0);
       dotset(2,50.0,0.0,0.2,0.0,0,0);
       dotset(3,50.0,0.0,0.4,0.0,0,0);
       dotset(4,50.0,0.0,0.4,0.0,0,0);
       dotset(5,50.0,0.0,0.6,0.0,0,0);
       for(gain=0;gain<6;gain++)
         { /* card=kard; */
	   dotacc(gain,(double)card,bb[card][gain]);
         }
       dotmwr(6,0,6);
     }
/* ---------- plot the deviations from a straight line --------------- */
   /* card=kard; */
    { dxyzro();
      itype=1;
      for(gain=0;gain<6;gain++)
       { dxyset(gain,(double)max[gain],0.0,0.1,-0.1,0,0);
             for(i=0;i<=imx[gain];i++)
	       { fdc = dc[card][gain][i];
	         fvlt = vlt[card][gain][i];
	         a=aa[card][gain];
	         b=bb[card][gain];
		 volt = a + b*fdc;
		 delta = fvlt-volt;
		 dxyacc(gain,fdc,delta,0.0,8);
	       }
       }
 if(sw(6) == 1) 
     { sidev(1);
       dxymwr(6,0,5);
     }  
    }
/* printf("%d cards   %d good    %d bad\n",nall,ngood,nbad); */
/* --------------------------------------------------------------------
   patch values into data base here
       card=1 to 48  gain=0 to 5
      dok[card] - 0=good   1=bad   digital check
      ok[card] 0=integrator is ok  nonzero(n)=failed test number n
      sped[card][gain] - pedistal noise (volts)
      bb[card][gain] - slope (volts/dac count)
      nonlin[card][gain] - nonlinearity (volts)	 
      v2[card] - feedthrough (volts)
 ----------------------------------------------------------------------*/
 }
cardchk(k)
int k;
  {int i,status;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,ok;
   ok=0;
   status=0;
   multi_low(ttcadr);
   back_low(ttcadr);
   set_tube(ttcadr,k);
   set_intg_gain(ttcadr,5);   /* s1=s3=1 s2=s4=0 */
   itr_high(ttcadr);           /* itr=1 */
   intg_rd_low(ttcadr);        /* ire=0 */
   mse_high(ttcadr);           /* mse=1 */
   small_cap_disable(ttcadr);  /* tplo=0 */
   large_cap_enable(ttcadr);   /* tphi=1 */
   trig_disable(ttcadr);       /* trig=0 */
   back_high(ttcadr);
   back_low(ttcadr);
   status=can_3in1set(id,0xd400);   /* fetch from 3in1 */
   status=can_3in1get(id,&shrt);    /* and load can output reg */
   i = (int)shrt;
   trig = i & 1;
   s1 = (i>>1) & 1;
   s2 = (i>>2) & 1;
   s3 = (i>>3) & 1;
   s4 = (i>>4) & 1;
   itr = (i>>5) & 1;
   ire = (i>>6) & 1;
   mse = (i>>7) & 1;
   tplo = (i>>8) & 1;
   tphi = (i>>9) & 1;
   if((s1==1) && (s2==0)  && (s3==1) && (s4==0) && (itr==1) &&
      (ire==0) && (mse==1) && (tplo==0) && (tphi==1) && 
      (trig==0) ) ok++;
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);

   set_intg_gain(ttcadr,0xa);   /* s1=s3=0 s2=s4=1 */
   itr_low(ttcadr);           /* itr=0 */
   intg_rd_high(ttcadr);        /* ire=1 */
   mse_low(ttcadr);           /* mse=0 */
   small_cap_enable(ttcadr);  /* tplo=1 */
   large_cap_disable(ttcadr);   /* tphi=0 */
   trig_enable(ttcadr);       /* trig=1 */
   back_high(ttcadr);
   back_low(ttcadr);
   status=can_3in1set(id,0xd400);   /* fetch from 3in1 */
   status=can_3in1get(id,&shrt);    /* and load can output reg */
   i = (int)shrt;
   trig = i & 1;
   s1 = (i>>1) & 1;
   s2 = (i>>2) & 1;
   s3 = (i>>3) & 1;
   s4 = (i>>4) & 1;
   itr = (i>>5) & 1;
   ire = (i>>6) & 1;
   mse = (i>>7) & 1;
   tplo = (i>>8) & 1;
   tphi = (i>>9) & 1;
   if((s1==0) && (s2==1)  && (s3==0) && (s4==1) && (itr==0) &&
      (ire==1) && (mse==0) && (tplo==1) && (tphi==0) && 
      (trig==1) ) ok++;
   if(sw(2)==1) printf("k=%d s1..4=%d %d %d %d itr=%d ire=%d mse=%d tplo=%d tphi=%d trig=%d %x\n"
           ,k,s1,s2,s3,s4,itr,ire,mse,tplo,tphi,trig,i);
  if(ok == 2) status=1;
   return(status);
  }
      double dxyfit(l,k,x)
      int l,k;
      double x;
      { char text[80];
        double dxyf;
	int ent=0,i,ix,iy;
	FILE *fp;
        dxyf=0.0;  /* dummy routine */
        if(sw(1) == 0) return(0);
	if(ent == 0 && itype == 0)
	  {  getp(&ix,&iy);
             sprintf(text,"card %d  %5.1f  %5.1f  %5.1f  %5.1f",
	       card,v1[card],v2[card],v3[card],v4[card]);
	     symb(500,3100,0.0,text);
             mova(ix,iy);  
             ent=1;
          }  
	if(ent == 0 && itype == 1)
	  {  getp(&ix,&iy);
             sprintf(text,"card %d  linearity deviations",
	       card);
	     symb(500,3100,0.0,text);
             mova(ix,iy);  
             ent=1;
             dxyf = -1.0;
          }  
        return(dxyf);
      }
    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=0.0;
     return(y); 
    }


