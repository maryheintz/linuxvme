#include <stdio.h>#include <math.h>#include <time.h>int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;main(argc,argv,envs)int argc;char *argv[];char *envs[];  { FILE *fp;    unsigned short i,l;    int zone,sector,card,time1,time2,time3,time4;    int dac,icap;    double capacitor,dacval;    double charge,charge_true;    char xstring[80],ystring[80],string[80],err[80];    unsigned short cadr,tc,tf;    unsigned short sval,dval;    int kk;    kk=0;    zone = 1;    sector = 1;    time1 = 0;    time2 = 0;    time3 = 0;    time4 = 0;    tc=25;    tf=0;    tim1_set(tc);    tim2_set(tf);    set_drawer_timer(zone,sector,1,time1);    set_drawer_timer(zone,sector,2,time2);    set_drawer_timer(zone,sector,3,time3);    set_drawer_timer(zone,sector,4,time4);    multi_low();crg:    printf(" input dac value\n");    fscanf(stdin,"%d",&dac);    for(i=0;i<49;i++)       { card = i;         l = (zone<<12) | (sector<<6) | card;         cadr = (unsigned short)l;         card_sel(cadr);         sval = (unsigned short)0x555;         wrt_shift(13,sval);         read_shift(&dval);/*         printf("card=%d  sval=%x  dval=%x\n",card,(int)sval,(int)dval); */         if(sval != (dval&0x1fff) ) continue;         sval = (unsigned short)0xaaa;         wrt_shift(13,sval);         read_shift(&dval);         if(sval != (dval&0x1fff) ) continue;         printf("card %d found\n",card);         send13_3in1(2,0);   /* s1=s2=s3=s4=0 */         send4_3in1(1,0);   /* itr=0 */         send4_3in1(0,1);   /* ire=0 */         send4_3in1(3,1);   /* mse=0 */         send4_3in1(4,1);   /* small C enable */         send4_3in1(5,0);   /* large C disable */         send4_3in1(7,1);   /* trig enable */         icap = 2;         capacitor = 5.2;         charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;         printf(" charge=%f  dac=%x(hex)  cap=%f\n",           charge,dac,capacitor);         send13_3in1(6,dac);      }loop:        rtime();        tp_high();        stime();        if(sw(1) == 1) goto crg;         goto loop;}  /* end of main */sw(i)   /* new switch timer module */int i;{ int k,l,shift;  unsigned short *adr;  adr = (unsigned short *)( 0xf0aa000c);  k = *adr;  shift = i-1;  l = (k>>shift) & 1;  return(l);}