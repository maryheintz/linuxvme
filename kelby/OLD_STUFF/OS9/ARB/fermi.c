#include <stdio.h>#include <math.h>#include <time.h>int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;int fdata[512],ncmprs;double unc[512];struct{unsigned int trig;       unsigned int reset;       unsigned int status;       unsigned int sample;       unsigned int delay;       unsigned int rdall;       unsigned int rd3;       unsigned int rd2;       unsigned int rd1;}fermi;unsigned short *reg;unsigned int base=0xf0b40000;unsigned int base2=0x80b40000;double fermi_val[1200],fermi_volt[1200];double scut;main()  { FILE *fp;    FILE *fp_gpib;    int nstrtev;    int i,k,nn,l,ip,lgood,lbad,lbpk,nevnt,nevmax,target,nrl,nip,large,iplot;    unsigned long ext,b,c,n,a,f,q,off;    int ibn,pl=155,ph=167;    int ipeak,peak_val,ipd;    double fval,bn,sec,ang,astrt,sum5,sm,pdsm,ped,uped,ped_save,pk_save;    double dpeak,dped;    double sum,xsum,xxsum,psum,pxsum,pxxsum;    double pksum,pkxsum,pkxxsum,wdxsum,wdxxsum,arxsum,arxxsum,cgxsum,cgxxsum;    double cg2xsum,cg2xxsum;    double sig2,sigcg1,sigpk,sigwd,sigar,sigcg2;    double xhist,yhist,pmn,psig,scped,sigsc;    double y,x,x2,x3,x4,x5,ytst;    double fpk_cnt,fpk_wid,fpk_cntr,fpk_area;    double vpulse,vset,vstrt,vstep,vmax,volt;    int ngpib,ierr;    unsigned short data,cia;    int nloop,ncia=16,kkk,kkkcnt;    char string[80],err[80];    double kmax,max;    unsigned char gpib_device=2;    vstrt=2.0;    vmax=0.0;    vstep=0.02;    vpulse=2.0;    nevmax=5;    scut=5.0;        dotset(0,2.0,0.0,1000.0,0.0,0,0);        sprintf(string,         "x-voltage      y-fermi peak volts ");        dotlbl(0,string);    fp=fopen("fermi.dat","w");    gpib_reset();/*  initialize the iotech controller - programable attenuators */    sendmessage(3, "D4 X" );  /* lf+eoi terminate */    sendmessage(3, "P1 X" );  /* select master and execute */    sendmessage(3, "F3 X" );  /* select hex */    /* --------------- INITIALIZE CAMAC --------------------------------*/    printf("initialize camac\n");    b=0;    c=1;    n=25;    a=0;/*  set up control word ext */    cdreg(&ext,&b,&c,&n,&a);    off=0;    cccz(&ext);          /* initialize */     cccc(&ext);          /* clear */    ccci(&ext,&off);     /* remove crate inhibit *//*  send pulse to reset visual scaler */    f=16;    a=7;    n=2;    data=1;    cdreg(&ext,&b,&c,&n,&a);    cssa(&f,&ext,&data,&q);/*  read switches */    c_latch();    sw1 = sw(1);    sw2 = sw(2);    sw3 = sw(3);    sw4 = sw(4);    sw5 = sw(5);    sw6 = sw(6);    sw7 = sw(7);    sw8 = sw(8);    printf("switches  1-8= %d %d %d %d %d %d %d %d\n",      sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8);    fsetup();    f_reset();    f_ctrig();    k=-1;    lgood=0;    lbad=0;    lbpk=0;    nip=0;    nrl=0;    large=0;/* ################ start - throw away one event to get going ########## */     s_trig();    k=-1;   for (vset=vstrt; vset>vmax; vset=vset-vstep)  /* start l loop */    {at_set(vpulse,vset,&volt);      k++;      printf("k=%d vset=%f volt=%f\n",k,vset,volt);   nstrtev=-1;   for(nevnt=0;nevnt<nevmax;nevnt++)    {/*   reset last event ------------------------------ */rstrt:       f_ctrig();       c_latch();       sw2 = sw(2);       sw3 = sw(3);       sw4 = sw(4);       sw5 = sw(5);       sw6 = sw(6);       sw7 = sw(7);       sw8 = sw(8);       if(sw7 == 1) break;/*     send the laser trigger ------------------------------- */       s_trig();       tsleep(0x80000020);/*     look for latch bit set ------------------------------- */       kkkcnt=0;loop:       kkk=r_latch();       kkkcnt++;       if(kkkcnt>1000)         {nrl++;         if(sw6 == 1) printf("no latch bit found\n");         goto rstrt;}       if(kkk != 1) goto loop;/*     go read the fermi module ------------------------------- */       f_read(&ipeak,&peak_val);       if(ipeak == -1)         {nip++;         if(sw6 == 1) printf("fread ipeak=-1 error\n");         goto rstrt;}       if( ipeak == -2 && sw1 == 0)         {lbad++;         if(sw6 == 1) printf("ipeak=%d error\n",ipeak);         goto rstrt;}       if(sw8 == 1) goto rstrt;       if( (ipeak<pl || ipeak>ph) && sw1 == 0)         {lbpk++;         if(sw6 == 1) printf("ipeak=%d error\n",ipeak);         goto rstrt;}       lgood++;       l++;/*     histogram the fermi readout ? ------------------------------- */       if(sw1 == 1 )        {hstrst();         hstset(0,250.0,0.0,250,1000);         sprintf(string,"ipeak=%d peak_val=%d",ipeak,peak_val);         hstlbl(0,string);         for(nn=0; nn<256; nn++)          {xhist = (double)nn + 0.5;           yhist = (double)fdata[nn];           hstacc(0,xhist,yhist);          }         hstwrt(0);         }         dotacc(0,volt,(double)peak_val);     } /* end of nevnt loop */    }  /* end of attenuator angle loop */     printf("r_latch trouble nrl=%d\n",nrl);     printf("trouble ipeak=-1 nip=%d\n",nip);     printf("ipeak not found lbad=%d\n",lbad);     printf("ipeak outside pl ph limits lbpk=%d\n",lbpk);     printf("cia 4090 cut large=%d\n",large);     printf("lgood=%d\n",lgood);     dotwrt(0);}  /* end of main */    double dxyfit(iplot,ifit,x)    int iplot,ifit;    double x;    { double fit;      fit=0.0;      return(fit);    }    double hstfit(i,l,x)    int i,l;    double x;    {double y;     y=0.0;     return(y);     }       sw(i)        int i;        {int *adr,k,l,shft;         adr=(int *)0x80cf0000;         k=*adr;         shft=23+i;         l=(k>>shft)&1;         return(l);         }           r_latch()        {int *adr,k,l;         adr=(int *)0x80cf0000;         k=*adr;         l=k&1;         return(l);         }           c_latch()        {int *adr,k,l;         adr=(int *)0x80cf0008;         *adr=NULL;         return(0);         }           s_trig()        {int *adr,k,l;         adr=(int *)0x80cf0004;         *adr=NULL;         return(0);         }          fsetup()    { FILE *fpm;      int k;      double r1,r2;      fpm=fopen("double_ended.dat","r");      k=0;loop:      fscanf(fpm,"%lf %lf",&r1,&r2);      if(r2>0.0)        {fermi_val[k] = r1;        fermi_volt[k] = r2;/*        printf("k=%d  val=%f  volts=%f\n",k,r1,r2); */        k++;        goto loop;}      ncmprs=k-1;      printf("ncmprs=%d\n",ncmprs);      return(0);    }    cmprs( fpeak,x )    double *fpeak,*x;    { int i,k,k1,k2;      double a1,a2,a3;      k=0;      for (i=0;i<ncmprs;i++) {k=i; if( fermi_val[i] < *fpeak ) goto out;}      k=ncmprs-1;out:      k2=k;      if(k2>ncmprs) k2=ncmprs;      if(k2<1) k2=1;      k1=k2-1;      a1=fermi_val[k2] - fermi_val[k1];      a2=fermi_volt[k2] - fermi_volt[k1];      a3 = fermi_val[k2] - *fpeak;      *x = fermi_volt[k2] - (a2*a3)/a1;/*      printf("fpeak=%f k1=%d k2=%d a1=%f a2=%f a3=%f x=%f\n",        *fpeak,k1,k2,a1,a2,a3,*x);  */      return(0);      }f_reset(){ FPermit(base,0x100,3);  FPermit(base2,0x100,3);  fermi.trig=base+0x26;  fermi.reset=base+0x24;  fermi.status=base+0x22;  fermi.sample=base+0x20;  fermi.delay=base+0x1e;  fermi.rdall=base2+0x18;  fermi.rd3=base+0x14;  fermi.rd2=base+0x12;  fermi.rd1=base+0x10;  /* issue reset */  reg=(unsigned short *)fermi.reset;  *reg=0;  /* set to full 255 samples */  reg=(unsigned short *)fermi.sample;  *reg=255;  /* set delay */  reg=(unsigned short *)fermi.delay;  *reg=10;  /* set to start mode */  reg=(unsigned short *)fermi.status;  *reg=0;  return(0);}f_ctrig(){ reg=(unsigned short *)fermi.trig;  *reg=0;  return(0);}f_read(peak,peak_val)int *peak,*peak_val;{int stat,data_ready,busy,lcnt,i,ii,dd,k; double sum,xsum,xxsum,mean,sigma,sig2,up,fval; /* wait for a trigger */  lcnt=0;loop:  lcnt++;  reg = (unsigned short *)fermi.status;  stat = *reg;  data_ready = stat & 4;  busy = stat & 2;  if(lcnt > 1000 ) goto error;  if ( data_ready == 0 ) goto loop;  /* read the data and find peak channel */  for (i=0;i<255;i++)   {reg=(unsigned short *) fermi.rd3;    dd = *reg & 0x3ff;    fdata[i] = 0x3ff - dd;    if(i == 0) fdata[i]=0;   }  ii=-1;  sum=0.0;  xsum=0.0;  xxsum=0.0;  for (i=1;i<255;i++)   {    if(i>10)     {mean=xsum/sum;      sigma=0.0;      sig2=xxsum/sum-mean*mean;      if(sig2>0.0) sigma=sqrt(sig2);      up=mean+scut*sigma;      if( (double)fdata[i] > up)        { if( (double)fdata[i+1] > up)           { if( (double)fdata[i+2] > up)              {ii=i;               for(k=i;k<i+10;k++)                 {if(fdata[k]>fdata[ii]) ii=k;                 }               }            }         }     }    sum=sum+1;    fval=(double)fdata[i];    xsum=xsum+fval;    xxsum=xxsum+fval*fval;    if(ii>0)      {*peak=ii;      *peak_val=fdata[ii];      return(0);     }   }   *peak=-2;   return(0);error:   *peak=-1;   return(0);   }        prabola(ipeak,fpk_cnt,fpk_wid,fpk_cntr,fpk_area)        int ipeak;        double *fpk_cnt,*fpk_wid,*fpk_cntr,*fpk_area;        { double x1,x2,x3,y1,y2,y3,a1,a2,a3,a4,a5,a6,a7,a8;          double c1,c2,c3,xhigh,xlow,term1,term2;/*        fit to y=c1-c2(x-c3)**2  */          x2=(double)ipeak;          x1=x2-1.0;          x3=x2+1.0;          y1=unc[ipeak-1];                   y2=unc[ipeak];                   y3=unc[ipeak+1];                   a1=y1-y2;          a2=y1-y3;          a3=pow(x2,2.0)-pow(x1,2.0);          a4=pow(x3,2.0)-pow(x1,2.0);          a5=x1-x2;          a6=x1-x3;          a7=a2*a3-a1*a4;          a8=a1*a6-a5*a2;          c3=a7/(2.0*a8);          c2=(y1-y2)/(pow((x2-c3),2.0)-pow((x1-c3),2.0));          c1=y3+c2*pow((x3-c3),2.0);          *fpk_cntr=c3;          *fpk_cnt=c1;          *fpk_wid=2.0*sqrt(0.5*c1/c2);          xhigh=c3+sqrt(c1/c2);          xlow=c3-sqrt(c1/c2);          term1=c1*(xhigh-c3)-2.0*c2*pow((xhigh-c3),3.0)/3.0;          term2=c1*(xlow-c3)-2.0*c2*pow((xlow-c3),3.0)/3.0;          *fpk_area=term1-term2;/*          if(sw4 == 1) printf("ipeak=%d peak=%f cnt=%f wid=%f ar=%f\n",             ipeak,unc[ipeak],*fpk_cnt,*fpk_wid,*fpk_area);         */          return(0);        }      at_set(vpulse,vset,volt)      double vpulse,vset,*volt;      { double dbset,db,afac;        unsigned long lout;        char string[80];        afac=vset/vpulse;        db= -20.0*log10(afac);        dbset=0.0;        lout=0;        if(db>64)          { dbset=dbset+64.0;            lout=lout+0x4000;            db=db-64.0;}        if(db>32)          { dbset=dbset+32.0;            lout=lout+0x2000;            db=db-32.0;}        if(db>16)          { dbset=dbset+16.0;            lout=lout+0x1000;            db=db-16.0;}        if(db>8)          { dbset=dbset+8.0;            lout=lout+0x800;            db=db-8.0;}        if(db>4)          { dbset=dbset+4.0;            lout=lout+0x400;            db=db-4.0;}        if(db>2)          { dbset=dbset+2.0;            lout=lout+0x200;            db=db-2.0;}        if(db>1)          { dbset=dbset+1.0;            lout=lout+0x100;            db=db-1.0;}        if(db>0.8)          { dbset=dbset+0.8;            lout=lout+0x8;            db=db-0.8;}        if(db>0.4)          { dbset=dbset+0.4;            lout=lout+0x4;            db=db-0.4;}        if(db>0.2)          { dbset=dbset+0.2;            lout=lout+0x2;            db=db-0.2;}        if(db>0.1)          { dbset=dbset+0.1;            lout=lout+0x1;            db=db-0.1;}        afac=-dbset/20.0;        *volt=vpulse*pow(10.0,afac);        sprintf(string,"C%x X",lout);        sendmessage(3, string );        tsleep(0x80000025);         return(0);        }fgetline(fp,s)  FILE *fp;  char s[];  {int c,i,lim;   lim=80;   i=0;   while(--lim>0&&(c=getc(fp)) != EOF && c != '\n')    s[i++] =c;   s[i]='\0';   if(i<1)     i=EOF;   return(i);  }  int err_parse(s)      char s[];      { int i,i1,i2,k;        i1=0;        i=0;        while(s[i] != ',')        i++;        i2=i;        k=0;        for (i=i1; i<i2; i++)        { k=k*10+(int)s[i]-48;}        /*        printf("err string=%s  i1=%d i2=%d s[i1]=%c s[i2]=%c k=%d\n",          s,i1,i2,s[i1],s[i2],k); */        return(k);        }