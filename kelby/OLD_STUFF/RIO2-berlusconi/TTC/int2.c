#include <stdio.h>
#include <math.h>
#include <time.h>
char xstring[80],ystring[80],string[80],err[80];
unsigned int id,ttcadr;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   int i,k,board;
   int tadr[5] = {0x2008,0x2009,0x2002,0x200a,0x2005};
   int isw[6],i1,i2,ll,mode,s1,s2,s3,s4,itr,ire,mse,tpe,lcal;
   int dacset,max[6],del[6];
   double fmin,ratio,volt,ped,expr[6];
   double a,b,ped0,ped100,dac,factor;
   int card,l,gain,pause;
   unsigned int itst,command;
   unsigned short shrt;
   long status;
   unsigned char serial[6];
   factor=1.0;
   printf("enter board number (1-5)\n");
   fscanf(stdin,"%d",&board);
   ttcadr = tadr[board-1];
   id = (board<<6) | 1;
   printf(" board %d  id=%x  ttcadr=%x\n",board,id,ttcadr);
   printf("enter board serial number\n");
   fscanf(stdin,"%s",serial);

   ttcvi_map(ttcadr);
   reset_ctrl_can();    /* set up the TIP810 canbus controller */
   can_init(ttcadr,board,serial);  
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
   set_tube(ttcadr,card);
   }
   if(argc == 2)
     { sscanf(argv[1],"%d",&card);
       printf("card %d requested\n",card);
       set_tube(ttcadr,card);
     }
   command = (card<<8) | 0xc4;
   status=adc_delay(id,20);
   if(status != 0) printf(" adc_delay status=%x\n",status);
   status=adc_dacset(id,120);
   if(status != 0) printf(" adc_dacset status=%x\n",status);
   status=adc_get_delay(id,&shrt);
   printf(" status=%x  delay back=%d\n",status,(int)shrt);  
   multi_low(ttcadr);
   rxw_low(ttcadr);

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
     set_dac(ttcadr,0);  /* dac=0 */
     intg_rd_low(ttcadr);
     itr_low(ttcadr);
     usleep(5);
     set_tube(ttcadr,0);
     status=adc_convert(id,command,&shrt);
     ped = factor * (double)shrt;
     printf("gain setting %d  ped=%f  shrt=%x\n",gain,ped,shrt);
     dacset = max[gain];
     for(i=0;i<dacset;i=i+del[gain])       
       { set_dac(ttcadr,i);
         usleep(1);
         status=adc_convert(id,command,&shrt);
         volt = factor * (double)shrt;
         fmin=volt-ped;
         if(sw(1) == 0) printf("dac=%d  volt=%f  ped=%f  shrt=%x\n",
             i,volt,ped,shrt);
         dxyacc(gain,(double)i,fmin,0.0,0);
       }
     }
   set_dac(ttcadr,48);
   usleep(20);
   intg_rd_low(ttcadr);
   itr_low(ttcadr);
   sleep(1);
   status=adc_convert(id,command,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=0 on volt=%f\n",volt);

   intg_rd_low(ttcadr);
   itr_high(ttcadr);
   sleep(1);
   status=adc_convert(id,command,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=1 on volt=%f\n",volt);

   intg_rd_high(ttcadr);
   itr_low(ttcadr);
   sleep(1);
   status=adc_convert(id,command,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=1 itr=0 on volt=%f\n",volt);

   intg_rd_low(ttcadr);
   itr_low(ttcadr);
   sleep(1);
   status=adc_convert(id,command,&shrt);
   volt = factor * (double)shrt;
   printf("intg_rd=0 itr=0 on volt=%f\n",volt);
   printf("card=%d\n",card);
   printf("\n\n\n");
   fflush(stdout);
   sidev(1);
   dxymwr(6,0,5);
}
cardchk(k)
int k;
  {int i,status,ok;
   unsigned short shrt;
   int trig,s1,s2,s3,s4,itr,ire,mse,tplo,tphi;
   status=0;
   ok=0;
   can_multi_low(id);
   can_bkld_low(id);
   can_tube(id,k);
   can_intg_gain(id,5);   /* s1=s3=1 s2=s4=0 */
   can_itr_high(id);           /* itr=1 */
   can_intg_rd_low(id);        /* ire=0 */
   can_mse_high(id);           /* mse=1 */
   can_small_cap_disable(id);  /* tplo=0 */
   can_large_cap_enable(id);   /* tphi=1 */
   can_trig_disable(id);       /* trig=0 */
   can_bkld_high(id);
   can_bkld_low(id);
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
   can_intg_gain(id,0xa);   /* s1=s3=0 s2=s4=1 */
   can_itr_low(id);           /* itr=1 */
   can_intg_rd_high(id);        /* ire=1 */
   can_mse_low(id);           /* mse=0 */
   can_small_cap_enable(id);  /* tplo=1 */
   can_large_cap_disable(id);   /* tphi=0 */
   can_trig_enable(id);       /* trig=1 */
   can_bkld_high(id);
   can_bkld_low(id);
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
