#include <stdio.h>
#include <math.h>
#include <time.h>
char xstring[80],ystring[80],string[80],err[80];
int gpib_adr = 3;
int ent=0;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
{ 
   int isw[6],i1,i2,ll,i,mode,s1,s2,s3,s4,itr,ire,mse,tpe,lcal;
   int sw1,dacset,max[6],del[6],k;
   short cadr,sval,dval,tval,t1val,t2val;
   double fmin,ratio,volt,ped,expr[6];
   int zone,sector,card,l,gain;
   unsigned int itst;
   unsigned short shrt;
   ttcvi_map();
   zone = 0;
   sector = 0;  
   card = 38;
   can_zone(zone);
   can_sector(sector);
   can_tube(card);
   can_multi_low();
   if(argc != 2)
    {itst = 0x1eaa;
     for(k=0;k<49;k++)
       { can_tube(k);
         i=0xaaa;
         i1 = 0xfc | ((i>>8)&3);
         i2 = i & 0xff;
         tsleep(0x80000005);
 	 can_set(i1,i2);  /* send to 3in1 card */
         tsleep(0x80000005);
 	 can_set(0x9c,0);  /* fetch from 3in1 and load can output reg */
         tsleep(0x80000005);
 	 can_shft_rd(&shrt);
	 if((int)shrt != itst) continue;
         card = k;
         goto found;
      }
    printf("no card found\n");
    return(0);
found:
    printf("card found = %d\n",card);
    can_tube(card);
   }
   if(argc == 2)
     { sscanf(argv[1],"%d",&card);
       printf("card %d requested\n",card);
       can_tube(card);
     }

   can_multi_low();

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
   { dxyset(i,(double)max[i],0.0,5.0,0.0,0,0);
     sprintf(xstring,"isw=%x xaxis=dac value",isw[i]);
     sprintf(ystring,"yaxis=volts");
     dxylbl(i,xstring,ystring);
    }
   for(gain=0;gain<6;gain++)
    {can_send_3in1(2,isw[gain]);
     can_dac(0);  /* dac=0 */
     can_send_3in1(0,0);  /* intg_rd=0 */
     can_send_3in1(1,0);  /* itr=0 */
     tsleep(0x80000010);
     go_get(&ped);
     printf("gain setting %d  ped=%f\n",gain,ped);
     dacset = max[gain];
     for(i=0;i<dacset;i=i+del[gain])       
       { sw1 = sw(1);
         can_dac(i);
         tsleep(0x80000010);
         go_get(&volt);
         fmin=volt-ped;
         if(sw1 == 0) printf("dac=%d  volt=%f  ped=%f\n",i,volt,ped);
         dxyacc(gain,(double)i,fmin,0.0,0);
       }
    }
   can_send_3in1(6,48);
   can_send_3in1(0,0);  /* intg_rd=0 */
   can_send_3in1(1,0);  /* itr=0 */
   sleep(1);
   go_get(&volt);
   printf("intg_rd=0 itr=0 on volt=%f\n",volt);

   can_send_3in1(0,0);  /* intg_rd=0 */
   can_send_3in1(1,1);  /* itr=1 */
   sleep(1);
   go_get(&volt);
   printf("intg_rd=0 itr=1 on volt=%f\n",volt);

   can_send_3in1(0,1);  /* intg_rd=1 */
   can_send_3in1(1,0);  /* itr=0 */
   sleep(1);
   go_get(&volt);
   printf("intg_rd=1 itr=0 on volt=%f\n",volt);

   can_send_3in1(0,0);  /* intg_rd=1 */
   can_send_3in1(1,0);  /* itr=0 */
   sleep(1);
   go_get(&volt);
   printf("intg_rd=0 itr=0 on volt=%f\n",volt);
   printf("card=%d\n",card);
   printf("\n\n\n");
   fflush(stdout);
   sidev(1);
   dxymwr(6,0,5);
/*   gpib_close();  */
}
    go_get(v)
      double *v;
      {double volts;
       if(ent == 0)
         {gpib_reset();
          tsleep(0x80000020);

/*          printf("setting T9\n");
          sendmessage(gpib_adr,"T9 X");
          tsleep(0x80000020);
          sendmessage(gpib_adr,"T? X");
          getmessage(gpib_adr, &string[0] );
          printf("T9 back=%s\n",string);  */

          sendmessage(gpib_adr,"A2 X");
          tsleep(0x80000020);
/*          sendmessage(gpib_adr,"A? X");
          getmessage(gpib_adr, &string[0] );
          printf("A2 back=%s\n",string);  */

          sendmessage(gpib_adr,"C1 X");
          tsleep(0x80000020);
/*          sendmessage(gpib_adr,"C? X");
          getmessage(gpib_adr, &string[0] );
          printf("C1 back=%s\n",string);  */
  
          sendmessage(gpib_adr,"R3 X");
          tsleep(0x80000020);
/*          sendmessage(gpib_adr,"R? X");
          getmessage(gpib_adr, &string[0] );
          printf("R3 back=%s\n",string);    */

          sendmessage(gpib_adr,"N1 X");
          tsleep(0x80000020);
/*          sendmessage(gpib_adr,"N? X");
          getmessage(gpib_adr, &string[0] );
          printf("N1 back=%s\n",string);   */ 

          sendmessage(gpib_adr,"G0 X");
          tsleep(0x80000020);
/*          sendmessage(gpib_adr,"G? X");
          getmessage(gpib_adr, &string[0] );
          printf("G0 back=%s\n",string);   */
  
          sendmessage(gpib_adr,"Q0 X");
          tsleep(0x80000020);
/*          sendmessage(gpib_adr,"Q? X");
          getmessage(gpib_adr, &string[0] );
          printf("Q0 back=%s\n",string);    */

          ent=1;
         }

          sendmessage(gpib_adr,"T6 X");   /* trigger voltage reading */

          tsleep(0x80000010);
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