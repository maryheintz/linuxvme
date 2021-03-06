#include <stdio.h>
#include <math.h>
#include <time.h>
char xstring[80],ystring[80],string[80],err[80];
unsigned char gpib_adr = 3;
int ent=0;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   int isw[6],i1,i2,ll,i,mode,s1,s2,s3,s4,itr,ire,mse,tpe,lcal;
   int sw1,dacset,max[6],del[6];
   short cadr,sval,dval,tval,t1val,t2val;
   double fmin,ratio,volt,ped,expr[6];
   int zone,sector,card,l,gain;
   map_3in1();
   zone = 1;
   sector = 1;
   card = 41;
   multi_low();
   i = (zone<<12) | (sector<<6) | card;
   cadr = (unsigned short)i;
   card_sel(cadr);
   tval = (short)100;
   tim1_set(tval);
   tval = (short)50;
   tim2_set(tval);
   rtime();

   max[0]=640;
   max[1]=240;
   max[2]=160;
   max[3]=120;
   max[4]=80;
   max[5]=48;

   del[0]=40;
   del[1]=20;
   del[2]=8;
   del[3]=8;
   del[4]=4;
   del[5]=4;

   expr[0]=1.0;
   expr[1]=2.7;
   expr[2]=3.7;
   expr[3]=7.2;
   expr[4]=9.7;
   expr[5]=13.2;

   isw[0]=1;
   isw[1]=2;
   isw[2]=0;
   isw[3]=4;
   isw[4]=8;
   isw[5]=0xc;

   for(i=0;i<6;i++)
   { dxyset(i,(double)max[i],0.0,10.0,0.0,0,0);
     sprintf(xstring,"isw=%x xaxis=dac value",isw[i]);
     sprintf(ystring,"yaxis=volts");
     dxylbl(i,xstring,ystring);
    }
   for(gain=0;gain<6;gain++)
    {send12_3in1(2,0,isw[gain]);
     set_dac(0);
     send4_3in1(0,0);  /* intg_rd=0 */
     send4_3in1(1,0);  /* itr=0 */
     usleep((time_t)500);
     go_get(&ped);
     printf("gain setting %d  ped=%f\n",gain,ped);
     dacset = max[gain];
     for(i=0;i<dacset;i=i+del[gain])       
       { sw1 = sw(1);
         set_dac(i);
         usleep((time_t)500);
         go_get(&volt);
         fmin=volt-ped;
         if(sw1 == 0) printf("dac=%d  volt=%f  ped=%f\n",i,volt,ped);
         dxyacc(gain,(double)i,fmin,0.0,0);
       }
    }
   set_dac(48);
   send4_3in1(0,0);  /* intg_rd=0 */
   send4_3in1(1,0);  /* itr=0 */
   usleep((time_t)5000);
   go_get(&volt);
   printf("intg_rd=0 itr=0 on volt=%f\n",volt);

   send4_3in1(0,0);  /* intg_rd=0 */
   send4_3in1(1,1);  /* itr=1 */
   usleep((time_t)5000);
   go_get(&volt);
   printf("intg_rd=0 itr=1 on volt=%f\n",volt);

   send4_3in1(0,1);  /* intg_rd=1 */
   send4_3in1(1,0);  /* itr=0 */
   usleep((time_t)5000);
   go_get(&volt);
   printf("intg_rd=1 itr=0 on volt=%f\n",volt);

   send4_3in1(0,0);  /* intg_rd=1 */
   send4_3in1(1,0);  /* itr=0 */
   usleep((time_t)5000);
   go_get(&volt);
   printf("intg_rd=0 itr=0 on volt=%f\n",volt);
   printf("card=%d\n",card);
   printf("\n\n\n");
   fflush(stdout);
   sidev(1);
   dxymwr(6,0,5);
   gpib_close();  
}
    go_get(v)
      double *v;
      {double volts;
       if(ent == 0)
         {gpib_reset();
          usleep((time_t)100000);

/*          printf("setting T9\n");
          sendmessage(gpib_adr,"T9 X");
          usleep((time_t)100000);
          sendmessage(gpib_adr,"T? X");
          getmessage(gpib_adr, &string[0] );
          printf("T9 back=%s\n",string);  */

          sendmessage(gpib_adr,"A2 X");
          usleep((time_t)100000);
/*          sendmessage(gpib_adr,"A? X");
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

      double dxyfit(l,k,x)
      int l,k;
      double x;
      { double dxyf;
        dxyf=0.0;  /* dummy routine */
        return(dxyf);
      }
sw(i)
int i;
{ int k,l,shift;
  unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)0xdeaa000c;
  k = *adr;
  shift = i-1;
  l = (k>>shift) & 1;
  return(l);
}
