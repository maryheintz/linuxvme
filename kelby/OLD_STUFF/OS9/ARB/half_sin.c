#include <stdio.h>#include <math.h>#include <time.h>int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;int fdata[512];struct{unsigned int trig;       unsigned int reset;       unsigned int status;       unsigned int sample;       unsigned int delay;       unsigned int rdall;       unsigned int rd3;       unsigned int rd2;       unsigned int rd1;}fermi;unsigned short *reg;unsigned int base=0xf0b40000;unsigned int base2=0x80b40000;double fermi_val[1200],fermi_volt[1200];double scut;double vsave[1000],count[1000];main()  { FILE *fp;    unsigned long ext,b,c,n,a,f,q,off;    double fval;    int nevnt,nevmax,nstrtev,k,l,nn,i;    int ipeak,peak_val,ipd;    double xhist,yhist;    double vpulse,db,dbstp,volt;    int nloop,ncia=16,kkk,kkkcnt;    char string[80],err[80];    unsigned short data,cia;    double sum,sumx,sumy,sumxy,sumx2;    double aa,bb,y,diff;    dbstp=0.1;    vpulse=2.0;    nevmax=5;    scut=5.0;        hstset(1,5.0,-5.0,50,1);        sprintf(string,"residual distribution f(volt)-count");        hstlbl(1,string);        dotset(0,2.0,0.0,1000.0,0.0,0,0);        sprintf(string,         "x-voltage      y-fermi peak volts ");        dotlbl(0,string);        dotset(1,2.0,0.0,5.0,-5.0,0,0);        sprintf(string,         "x-voltage      y-linear fit-data ");        dotlbl(1,string);    fp=fopen("fermi.dat","w");    gpib_reset();/*  initialize the iotech controller - programable attenuators */    sendmessage(3, "D4 X" );  /* lf+eoi terminate */    sendmessage(3, "P1 X" );  /* select master and execute */    sendmessage(3, "F3 X" );  /* select hex *//*  set up the AWG2040 arbitrary waveform generator */    curve();/* --------------- INITIALIZE CAMAC --------------------------------*/    printf("initialize camac\n");    b=0;    c=1;    n=25;    a=0;/*  set up control word ext */    cdreg(&ext,&b,&c,&n,&a);    off=0;    cccz(&ext);          /* initialize */     cccc(&ext);          /* clear */    ccci(&ext,&off);     /* remove crate inhibit *//*  send pulse to reset visual scaler */    f=16;    a=7;    n=2;    data=1;    cdreg(&ext,&b,&c,&n,&a);    cssa(&f,&ext,&data,&q);/*  read switches */    c_latch();    sw1 = sw(1);    sw2 = sw(2);    sw3 = sw(3);    sw4 = sw(4);    sw5 = sw(5);    sw6 = sw(6);    sw7 = sw(7);    sw8 = sw(8);    printf("switches  1-8= %d %d %d %d %d %d %d %d\n",      sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8);    f_reset();    f_ctrig();    k=-1;/* ################ start - throw away one event to get going ########## */     s_trig();    k=-1;    i=0;   for (db=0.0; db<64.01; db=db+dbstp)  /* start l loop */     {if(sw7 == 1) break;      db_set(vpulse,db,&volt);      if(db>=8.0) dbstp=0.2;      if(db>=16.0) dbstp=0.4;      if(db>=32.0) dbstp=0.8;      if(db>=48.0) dbstp=1.6;      k++;      if(sw3 == 1) printf("k=%d db=%f volt=%f\n",k,db,volt);   nstrtev=-1;   for(nevnt=0;nevnt<nevmax;nevnt++)    {/*   reset last event ------------------------------ */rstrt:       f_ctrig();       c_latch();       sw2 = sw(2);       sw3 = sw(3);       sw4 = sw(4);       sw5 = sw(5);       sw6 = sw(6);       sw7 = sw(7);       sw8 = sw(8);/*     send the laser trigger ------------------------------- */       s_trig();       tsleep(0x80000020);/*     look for latch bit set ------------------------------- */       kkkcnt=0;loop:       kkk=r_latch();       kkkcnt++;       if(kkkcnt>1000)         {if(sw6 == 1) printf("no latch bit found\n");         goto rstrt;}       if(kkk != 1) goto loop;/*     go read the fermi module ------------------------------- */       f_read(&ipeak,&peak_val);       if(sw8 == 1) goto rstrt;       l++;/*     histogram the fermi readout ? ------------------------------- */       if(sw1 == 1 )        {hstrst();         hstset(0,250.0,0.0,250,1000);         sprintf(string,"ipeak=%d peak_val=%d",ipeak,peak_val);         hstlbl(0,string);         for(nn=0; nn<256; nn++)          {xhist = (double)nn + 0.5;           yhist = (double)fdata[nn];           hstacc(0,xhist,yhist);          }         hstwrt(0);         }         fval=(double)fdata[ipeak];         if(sw2 == 1) printf("ipeak=%d peak_val=%d fval=%f\n",           ipeak,peak_val,fval);         if(fval>10.0) dotacc(0,volt,fval);         if(fval>10.0 && fval<1023.0)           {vsave[i]=volt;            count[i]=fval;            i++;}     } /* end of nevnt loop */    }  /* end of attenuator angle loop *//*   now do linear fit */     sum=0.0;     sumx=0.0;          sumy=0.0;          sumxy=0.0;          sumx2=0.0;     for(nn=0;nn<i;nn++)       {sum=sum+1.0;        sumx=sumx+vsave[nn];        sumy=sumy+count[nn];        sumxy=sumxy+vsave[nn]*count[nn];        sumx2=sumx2+vsave[nn]*vsave[nn];       }     plot_reset();      aa=(sumx*sumxy-sumx2*sumy)/(sumx*sumx-sum*sumx2);     bb=(sumx*sumy-sum*sumxy)/(sumx*sumx-sum*sumx2);             printf("aa=%f  bb=%f  y=aa+bb*x fit\n",aa,bb);     for(nn=0;nn<i;nn++)      {y=aa+bb*vsave[nn];       diff=count[nn]-y;       dotacc(1,vsave[nn],diff);       if(diff>-5.0 && diff<5.0) hstacc(1,diff,1.0);       printf("nn=%d  v=%f  y=%f  yfit=%f\n",         nn,vsave[nn],count[nn],y);      }     dotwrt(0);     dotwrt(1);     hstwrt(1);     plot_reset();      printf("second chance to hard copy plots\n");      dotwrt(0);      dotwrt(1);     hstwrt(1);}  /* end of main */    double hstfit(i,l,x)    int i,l;    double x;    {double y;     y=0.0;     return(y);     }       sw(i)        int i;        {int *adr,k,l,shft;         adr=(int *)0x80cf0000;         k=*adr;         shft=23+i;         l=(k>>shft)&1;         return(l);         }           r_latch()        {int *adr,k,l;         adr=(int *)0x80cf0000;         k=*adr;         l=k&1;         return(l);         }           c_latch()        {int *adr,k,l;         adr=(int *)0x80cf0008;         *adr=NULL;         return(0);         }           s_trig()        {int *adr,k,l;         adr=(int *)0x80cf0004;         *adr=NULL;         return(0);         }      f_reset(){ FPermit(base,0x100,3);  FPermit(base2,0x100,3);  fermi.trig=base+0x26;  fermi.reset=base+0x24;  fermi.status=base+0x22;  fermi.sample=base+0x20;  fermi.delay=base+0x1e;  fermi.rdall=base2+0x18;  fermi.rd3=base+0x14;  fermi.rd2=base+0x12;  fermi.rd1=base+0x10;  /* issue reset */  reg=(unsigned short *)fermi.reset;  *reg=0;  /* set to full 255 samples */  reg=(unsigned short *)fermi.sample;  *reg=255;  /* set delay */  reg=(unsigned short *)fermi.delay;  *reg=10;  /* set to start mode */  reg=(unsigned short *)fermi.status;  *reg=0;  return(0);}f_ctrig(){ reg=(unsigned short *)fermi.trig;  *reg=0;  return(0);}f_read(peak,peak_val)int *peak,*peak_val;{int stat,data_ready,busy,lcnt,i,dd,k,kmax; /* wait for a trigger */  lcnt=0;loop:  lcnt++;  reg = (unsigned short *)fermi.status;  stat = *reg;  data_ready = stat & 4;  busy = stat & 2;  if(lcnt > 1000 ) goto error;  if ( data_ready == 0 ) goto loop;  /* read the data and find peak channel */  k=0;  kmax=0;  for (i=0;i<255;i++)   {reg=(unsigned short *) fermi.rd3;    dd = *reg & 0x3ff;    fdata[i] = 0x3ff - dd;    if(i == 0) fdata[i]=0;    if(fdata[i]>kmax)     {kmax=fdata[i];      k=i;}   }   *peak=k;   *peak_val=kmax;   return(0);error:   *peak=-1;   return(0);   }      db_set(vpulse,db,volt)      double vpulse,db,*volt;      { double dbset,afac;        unsigned long lout;        char string[80];        dbset=0.0;        lout=0;        if(db>64)          { dbset=dbset+64.0;            lout=lout+0x4000;            db=db-64.0;}        if(db>32)          { dbset=dbset+32.0;            lout=lout+0x2000;            db=db-32.0;}        if(db>16)          { dbset=dbset+16.0;            lout=lout+0x1000;            db=db-16.0;}        if(db>8)          { dbset=dbset+8.0;            lout=lout+0x800;            db=db-8.0;}        if(db>4)          { dbset=dbset+4.0;            lout=lout+0x400;            db=db-4.0;}        if(db>2)          { dbset=dbset+2.0;            lout=lout+0x200;            db=db-2.0;}        if(db>1)          { dbset=dbset+1.0;            lout=lout+0x100;            db=db-1.0;}        if(db>0.8)          { dbset=dbset+0.8;            lout=lout+0x8;            db=db-0.8;}        if(db>0.4)          { dbset=dbset+0.4;            lout=lout+0x4;            db=db-0.4;}        if(db>0.2)          { dbset=dbset+0.2;            lout=lout+0x2;            db=db-0.2;}        if(db>0.1)          { dbset=dbset+0.1;            lout=lout+0x1;            db=db-0.1;}        afac=-dbset/20.0;        *volt=vpulse*pow(10.0,afac);        sprintf(string,"C%x X",lout);        sendmessage(3, string );        tsleep(0x80000025);         return(0);        }unsigned char string[8000],back[8000];curve()  { int i,ii,iy,ipnt,i1,i2;    double x,y,dx;    unsigned char gpib_device=13;    sprintf(string,":DEBUG:SNOOP:STATE OFF \0");     printf(":DEBUG:SNOOP:STATE OFF\n");    sendmessage(gpib_device, string );    sprintf(string,"DATA:DESTINATION 'ABC.WFM' \0");     printf("DATA:DESTINATION 'TEST.WFM'\n");    sendmessage(gpib_device, string );     sprintf(string,"DATA:WIDTH2:ENCDG SRPBINARY\0");    printf("DATA:WIDTH2:ENCDG SRPBINARY\n");    fflush(stdout);    sendmessage(gpib_device, string );     sprintf(string,"WFMPRE:YZERO 0.0 \0");    printf("WFMPRE:YZERO 0.0\n");    sendmessage(gpib_device, string );     sprintf(string,"WFMPRE:YMULT 1.0e-03 \0");    printf("WFMPRE:YMULT 1.0e-03\n");    sendmessage(gpib_device, string );     sprintf(string,"WFMPRE:XINCR 9.7646e-10 \0");    printf("WFMPRE:XINCR 9.7646e-10\n");    sendmessage(gpib_device, string );     sprintf(string,":CURVE #42048");    ipnt=13;    dx=(2.0*3.1415927)/1023.0;    for(i=0;i<1024;i++)     {x=dx*(double)i;      y=4095.0*sin(x);      if(y<0.0) y=0.0;      iy=(int)y;/*      printf("i=%d y=%f iy=%d\n",i,y,iy); */      i1=iy&0xff;      i2=(iy>>8)&0xff;      string[ipnt++]=(unsigned char)i2;      string[ipnt++]=(unsigned char)i1;     }    sendsize(gpib_device, string,ipnt );    printf("CURVE #42048<bytes of data>\n");    sprintf(string,"CH1:WAVEFORM 'ABC.WFM' \0");     printf("CH1:WAV 'ABC.WFM' \n");     sendmessage(gpib_device, string );     sprintf(string,"CH1:OFFSET 1.0V \0");     printf("CH1:OFFSET 1.0V \n");     sendmessage(gpib_device, string );     sprintf(string,"CH1:AMPL 2.0V \0");     printf("CH1:AMPL 2.0V \n");     sendmessage(gpib_device, string );     sprintf(string,":MODE TRIGGERED \0");     printf("MODE TRIGGERED \n");     sendmessage(gpib_device, string );     sprintf(string,":CH1:MARKERLEVEL1:LOW -0.5V \0");     printf(":CH1:MARKERLEVEL1:LOW -0.5V \n");     sendmessage(gpib_device, string );     sprintf(string,":CH1:MARKERLEVEL1:HIGH 0.0V \0");     printf(":CH1:MARKERLEVEL1:LOW -0.5V \n");     sendmessage(gpib_device, string );     sprintf(string,":DATA:DESTINATION 'ABC.WFM' \0");     printf(":DATA:DESTINATION 'ABC.WFM' \n");     sendmessage(gpib_device, string );     sprintf(string,":MARKER:DATA #41024");    ipnt=19;    for(i=0;i<1024;i++)     {      if(i<256) string[ipnt++]=(unsigned char)3;      if(i>255) string[ipnt++]=(unsigned char)0;     }    sendsize(gpib_device, string,ipnt );    printf("MARKER:DATA #41024<bytes of data>\n");    sprintf(string,"OUTPUT:CH1:NORMAL:STATE 1 \0");     printf("OUTPUT:CH1:NORMAL:STATE 1 \n");     sendmessage(gpib_device, string );        return(0);     }