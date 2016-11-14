#include <stdio.h>
#include <math.h>
#include <time.h>
char xstring[80],ystring[80],string[80],err[80];
unsigned int ttcadr,id;
int gpib_adr = 4;
unsigned int card;
int cnt[6],ped[48][6];
double v1[6],v2[6],v3[6],v4[6];
double vts[48][50][6],d[48][50][6];
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{  
   int isw[6],i1,i2,ll,i,mode,s1,s2,s3,s4,itr,ire,mse,tpe,lcal;
   int sw1,dacset,max[6],del[6],k,kkk,icnt;
   short cadr,sval,dval,tval,t1val,t2val;
   double fmin,ratio,volt,expr[6];
   double a,b,ped0,ped100,dac,factor;
   int l,gain,pause,start;
   unsigned int itst;
   unsigned short shrt;
   long status;
   ttcadr = 0x1234;
   ttcvi_map(ttcadr);
   multi_low(ttcadr);
   rxw_low(ttcadr);
   back_low(ttcadr);
   start=0;
   if(argc == 2)
     { sscanf(argv[1],"%d",&start);
       printf("start at %d requested\n",start);
     }
   for(k=start;k<49;k++) 
       { set_tube(ttcadr,card);
         mse_low(ttcadr);
         intg_rd_low(ttcadr);
         itr_low(ttcadr);
         set_dac(ttcadr,0);  /* dac=0 */
         mse_high();
       }
   multi_high();
   max[0]=1000;
   max[1]=240;
   max[2]=160;
   max[3]=80;
   max[4]=72;
   max[5]=50;

   del[0]=100;
   del[1]=24;
   del[2]=16;
   del[3]=8;
   del[4]=6;
   del[5]=4;

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
   dxyft_set(0,1,5);
   for(gain=0;gain<6;gain++)
    {printf("start gain=%d\n",gain);
    intg_rd_low(ttcadr);
     itr_low(ttcadr);
     set_dac(ttcadr,0);  /* dac=0 */
     set_intg_gain(ttcadr,isw[gain]);
     for(k=1;k<49;k++)
       { set_tube(ttcadr,card);
         usleep(10);
         go_get(&volt);
	 ped[k][gain]=volt;
	 printf(" k=%d gain=%d ped=%f\n",
	     k,gain,ped[k][gain]);
       }
     dacset = max[gain];
     icnt=0;
     for(i=0;i<dacset;i=i+del[gain])       
       { set_dac(ttcadr,i);
         for(k=1;k<49;k++)
           { set_tube(ttcadr,card);
             usleep(10);
             go_get(&volt);
             vts[k][icnt][gain] = volt-ped[k][gain];
             d[k][icnt][gain] = (double)i;
             printf(" gain=%d  kard=%d  icnt=%d  volt=%f  ped=%f\n",
	         gain,k,icnt,volt,ped[k][gain]);
           }
	 icnt++;
       }
     }
   cnt[gain]=icnt;
   set_dac(ttcadr,48);
   usleep(20);
   intg_rd_low(ttcadr);
   itr_low(ttcadr);
     for(k=1;k<49;k++)
       { set_tube(ttcadr,card);
         usleep(1);
         go_get(&v1[k]);
       }

   intg_rd_low(ttcadr);
   itr_high(ttcadr);
     for(k=1;k<49;k++)
       { set_tube(ttcadr,card);
         usleep(1);
         go_get(&v2[k]);
       }

   intg_rd_high(ttcadr);
   itr_low(ttcadr);
     for(k=1;k<49;k++)
       { set_tube(ttcadr,card);
         usleep(1);
         go_get(&v3[k]);
       }

   intg_rd_low(ttcadr);
   itr_low(ttcadr);
     for(k=1;k<49;k++)
       { set_tube(ttcadr,card);
         usleep(1);
         go_get(&v4[k]);
       }
   dxyzro();
   for(k=1;k<49;k++)
    { for(i=0;i<6;i++)
      { if(i == 0) dxyset(i,(double)max[i],0.0,5.0,0.0,0,0);
        if(i != 0) dxyset(i,(double)max[i],0.0,10.0,0.0,0,0);
        sprintf(xstring,"isw=%x xaxis=dac value",isw[i]);
        sprintf(ystring,"yaxis=volts");
        dxylbl(i,xstring,ystring);
        for(l=1;l<=icnt;l++)
	   for(gain=0;gain<6;gain++)
	     dxyacc(i,d[k][l][gain],vts[k][l][gain],0.0,0);
      }
      sidev(1);
      dxymwr(6,0,5);
   }
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
	if(ent == 0)
	  {  getp(&ix,&iy);
             sprintf(text,"card %d  %5.1f  %5.1f  %5.1f  %5.1f",
	       card,v1,v2,v3,v4);
	     symb(500,3100,0.0,text);
             mova(ix,iy);  
             ent=1;
          }  
        return(dxyf);
      }
    go_get(v)
      double *v;
      {double volts;
       int static ent = 0;
       if(ent == 0)
         { /* printf(" go_get entry gpib_adr=%d\n",gpib_adr); */
	  gpib_reset();  
/*          printf("gpib_reset returned\n");
          printf("setting T9\n");
          sendmessage(gpib_adr,"T9 X");
          usleep((time_t)100000);
          sendmessage(gpib_adr,"T? X");
          getmessage(gpib_adr, &string[0] );
          printf("T9 back=%s\n",string);  */

          sendmessage(gpib_adr,"A2 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"A? X");
          usleep((time_t)100000);
          getmessage(gpib_adr, &string[0] );
          printf("A2 back=%s\n",string);  */  

          sendmessage(gpib_adr,"C1 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"C? X");
          getmessage(gpib_adr, &string[0] );
          printf("C1 back=%s\n",string);  */
  
          sendmessage(gpib_adr,"R3 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"R? X");
          getmessage(gpib_adr, &string[0] );
          printf("R3 back=%s\n",string);    */

          sendmessage(gpib_adr,"N1 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"N? X");
          getmessage(gpib_adr, &string[0] );
          printf("N1 back=%s\n",string);   */ 

          sendmessage(gpib_adr,"G0 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"G? X");
          getmessage(gpib_adr, &string[0] );
          printf("G0 back=%s\n",string);   */
  
          sendmessage(gpib_adr,"Q0 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"Q? X");
          getmessage(gpib_adr, &string[0] );
          printf("Q0 back=%s\n",string);    */

          ent=1;
         }

          sendmessage(gpib_adr,"T6 X");   /* trigger voltage reading */

          usleep((time_t)5000);
          getmessage(gpib_adr, &string[0] );
/*          printf("first readback=%c%c%c%c%c%c%c%c%c%c\n",
              string[0],string[1],string[2],string[3],string[4],
	      string[5],string[6],string[7],string[8],string[9]);   */    
/*          printf("first readback=%s\n",string);  */
          sscanf(string,"%le",&volts);
/*          printf("volts=%e\n",volts);   */ 

          *v=volts;
          return(0);
}
