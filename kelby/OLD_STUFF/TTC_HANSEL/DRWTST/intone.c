#include <stdio.h>
#include <math.h>
#include <time.h>
char xstring[80],ystring[80],string[80],err[80];
unsigned int ttcadr,id,board;
main(int argc,char *argv[])
{ 
   int isw[6],i1,i2,ll,i,mode,s1,s2,s3,s4,itr,ire,mse,tpe,lcal;
   int sw1,dacset,max[6],del[6],k,kkk;
   short cadr,sval,dval,tval,t1val,t2val;
   double fmin,ratio,volt,ped,expr[6];
   double a,b,ped0,ped100,dac,factor;
   int l,gain,pause;
   unsigned int itst;
   unsigned short shrt;
   unsigned int card,command;
   long status;
   factor=9.6/4095.0;
   ttcadr = 0x3009;
   board = 1;
   id = (board<<6) | 1;
   ttcvi_map(ttcadr);
   reset_ctrl_can();
   can_reset(ttcadr);
   can_init(board,"S10117");  
   status=adc_delay(id,200);
   if(status != 0) printf(" adc_delay status=%x\n",status);
   status=adc_dacset(id,100);
   if(status != 0) printf(" adc_dacset status=%x\n",status);
   status=adc_get_delay(id,&shrt);
   printf(" status=%x  delay back=%d\n",status,(int)shrt);
   if(argc != 2)
    {for(k=0;k<49;k++) 
       { if(cardchk(k) == 1)
          {card = k; 
           printf("card found = %d\n",card);
           goto found; }
       }
      printf("no card %d\n",k);
      return(0);
    }
   if(argc == 2)
     { sscanf(argv[1],"%d",&card);
       printf("card %d requested\n",card);
       if(cardchk(k) == 1) 
         { printf("card found = %d\n",card);
           goto found;
	 }
       printf("no card %d\n",k);
       return(0);
     } 
found:
   set_tube(ttcadr,card);
   multi_low(ttcadr);
   rxw_low(ttcadr);

   max[0]=280;
   max[1]=60;
   max[2]=60;
   max[3]=32;
   max[4]=20;
   max[5]=16;

   del[0]=10;
   del[1]=2;
   del[2]=2;
   del[3]=2;
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
   { if(i==0) dxyset(i,(double)max[i],0.0,3.0,0.0,0,0);
     if(i==1) dxyset(i,(double)max[i],0.0,8.0,0.0,0,0);
     if(i==2) dxyset(i,(double)max[i],0.0,8.0,0.0,0,0);
     if(i==3) dxyset(i,(double)max[i],0.0,8.0,0.0,0,0);
     if(i==4) dxyset(i,(double)max[i],0.0,6.0,0.0,0,0);
     if(i==5) dxyset(i,(double)max[i],0.0,6.0,0.0,0,0);
     sprintf(xstring,"isw=%x xaxis=dac value",isw[i]);
     sprintf(ystring,"yaxis=volts");
     dxylbl(i,xstring,ystring);
    }
   for(gain=0;gain<6;gain++)
    {set_intg_gain(ttcadr,isw[gain]);
     set_dac(ttcadr,0);  /* dac=0 */
     intg_rd_low(ttcadr);
     itr_low(ttcadr);
/*     usleep(5000);  */
     sleep(1);
     command = 0xc400 | card;
     status=adc_convert(id,command,&shrt);
     command = 0xc400 | card;
     status=adc_convert(id,command,&shrt);
     ped = factor * (double)shrt;
     printf("gain setting %d  ped=%f\n",gain,ped);
     dacset = max[gain];
     for(i=0;i<dacset;i=i+del[gain])       
       { sw1 = sw(1);
         set_dac(ttcadr,i);
         usleep(500);
	 command = 0xc400 | card;
         status=adc_convert(id,command,&shrt);
         volt = factor * (double)shrt;
         fmin=volt-ped;
         if(sw1 == 0) printf("dac=%d  volt=%f  ped=%f  shrt=%x\n",
             i,volt,ped,shrt);
         dxyacc(gain,(double)i,fmin,0.0,0);
       }
    }
   set_dac(ttcadr,20);
   sleep(1);
   intg_rd_low(ttcadr);
   itr_low(ttcadr);
   sleep(1);
   command = 0xc400 | card;
   status=adc_convert(id,command,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=0 on volt=%f  %x  %f\n",volt,shrt,factor);

   intg_rd_low(ttcadr);
   itr_high(ttcadr);
   sleep(1);
   command = 0xc400 | card;
   status=adc_convert(id,command,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=1 on volt=%f  %x  %f\n",volt,shrt,factor);

llll:
   intg_rd_high(ttcadr);
   itr_low(ttcadr);
   sleep(1);
   command = 0xc400 | card;
   status=adc_convert(id,command,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=1 itr=0 on volt=%f  %x  %f\n",volt,shrt,factor);
if(sw(4) == 1) goto llll;
   intg_rd_low(ttcadr);
   itr_low(ttcadr);
   sleep(1);
   command = 0xc400 | card;
   status=adc_convert(id,command,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=0 on volt=%f  %x  %f\n",volt,shrt,factor);
   printf("\n\n\n");
   fflush(stdout);
   sidev(1);
   dxymwr(6,0,5);
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
      { double dxyf;
        dxyf=0.0;  /* dummy routine */
        return(dxyf);
      }
