#include <stdio.h>#include <math.h>#include <time.h>int year,month,day,hour,min,sec;struct sgtbuf timebuf;main(argc,argv,envs)int argc;char *argv[];char *envs[];{  FILE *fp;   int card,zone,sector,l,wait,i,ok,count,error;   unsigned short cadr,sval,dval;   int s1,s2,s3,s4,itr,ire,mse,tpS,tpL,trg;   double t0,tnow,dt;   char string[80];    multi_low();    zone = 1;    sector = 1;start:       card = 37;       l = (zone<<12) | (sector<<6) | card;       cadr = (unsigned short)l;       card_sel(cadr);       if(sw(2) == 1) printf("  \n");       ok=0;       card = 47;       l = (zone<<12) | (sector<<6) | card;       cadr = (unsigned short)l;       card_sel(cadr);       send13_3in1(2,5);  /* s1=s3=1 s2=s4=0 */       send4_3in1(1,1);   /* itr=1 */       send4_3in1(0,0);   /* ire=0 */       send4_3in1(3,1);   /* mse=1 */       send4_3in1(4,0);   /* tpS=0 */       send4_3in1(5,1);   /* tpL=1 */       send4_3in1(7,0);   /* trig=0 *//*       sleep(1);  */       read_status(&sval);   /* read back the controller shift register */       i = (int)sval;       trg = (i>>3) & 1;       s1 = (i>>4) & 1;       s2 = (i>>5) & 1;       s3 = (i>>6) & 1;       s4 = (i>>7) & 1;       itr = (i>>8) & 1;       ire = (i>>9) & 1;       mse = (i>>10) & 1;       tpS = (i>>11) & 1;       tpL = (i>>12) & 1;       if( (s1==1) && (s2==0) && (s3==1) && (s4==0) && (itr==1) && (ire==0)         && (mse==1) && (tpS==0) && (tpL==1) && (trg==0) )ok++;       if(ok!=1 || sw(2) == 1)         { if(sw(2)==1)          {printf("s1..4=1 0 1 0 itr=1 ire=0 mse=1 tpS=0 tpL=1 trg=0 sent  card=%d\n",card);           printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tpS=%d tpL=%d trg=%d %x\n"            ,s1,s2,s3,s4,itr,ire,mse,tpS,tpL,trg,i,card);}        }       send13_3in1(2,0xa);  /* s1=s3=0 s2=s4=1 */       send4_3in1(1,0);   /* itr=0 */       send4_3in1(0,1);   /* ire=1 */       send4_3in1(3,0);   /* mse=0 */       send4_3in1(4,1);   /* tpS=1 */       send4_3in1(5,0);   /* tpL=0 */       send4_3in1(7,1);   /* trig=1 */       sleep(1);       read_status(&sval);   /* read back the controller shift register */       i = (int)sval;       trg = (i>>3) & 1;       s1 = (i>>4) & 1;       s2 = (i>>5) & 1;       s3 = (i>>6) & 1;       s4 = (i>>7) & 1;       itr = (i>>8) & 1;       ire = (i>>9) & 1;       mse = (i>>10) & 1;       tpS = (i>>11) & 1;       tpL = (i>>12) & 1;       if( (s1==0) && (s2==1) && (s3==0) && (s4==1) && (itr==0) && (ire==1)         && (mse==0) && (tpS==1) && (tpL==0) && (trg==1) )ok++;       if(ok!=2 || sw(2) == 1)         { if(sw(2)==1)          {printf("s1..4=0 1 0 1 itr=0 ire=1 mse=0 tpS=1 tpL=0 trg=1 sent\n");           printf("s1..4=%d %d %d %d itr=%d ire=%d mse=%d tpS=%d tpL=%d trg=%d %x\n"            ,s1,s2,s3,s4,itr,ire,mse,tpS,tpL,trg,i);}        }      if(ok != 2) printf("card %d not found\n",card);      if(ok == 2) printf("card %d found\n",card);    if(sw(8)==0) goto start; }sw(i)   /* new switch timer module */int i;{ int k,l,shift;  unsigned short *adr;  adr = (unsigned short *)( 0xf0aa000c);  k = *adr;  shift = i-1;  l = (k>>shift) & 1;  return(l);}