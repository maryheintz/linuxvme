#include <stdio.h>#include <math.h>#include "dxy.h"double xx[100],yy[100],dyy[100];double c1,c2,c3,c4,c5,aa,bb,fit,dif,sig2;int ncnt;main()  { FILE *fp,*fp_gpib;    unsigned long ext,b,c,n,a,f,q,off;    int s1,s2,s3,s4,s5,s6,s7,s8,s9,s10,s11,s12,s13,s14,s15,s16;    int i,ngpib,k,l,ierr,nevnt,channel,lq;    int dbg=1;    unsigned short data;    char string[80],err[80];    unsigned char gpib_device = 2;    char ESC=27;    double volt,vstrt,vstep,vmax,sm,xsm,xxsm,mean,sqsm,sigma,dat,ch;    char label[10][80];/*  -------------------- INITIALIZE GPIB PULSER ---------------------- */    fp=fopen("qvt.dat","w");    gpib_reset();    fp_gpib=fopen("gpib_qvt.dat","r");    fscanf(fp_gpib,"%lf %lf %lf %d %d %d",        &vstrt,&vstep,&vmax,&ngpib,&nevnt);/*    string[0]=getc(fp_gpib); */    printf("ngpib=%d vstrt=%f vstep=%f vmax=%f nevnt=%d\n",      ngpib,vstrt,vstep,vmax,nevnt);    for (k=0; k<ngpib; k++)     {fgetline(fp_gpib, &string[0]);        sendmessage(gpib_device, string );        sendmessage(gpib_device, "ERR?" );        getmessage(gpib_device, err );        ierr=err_parse(err);        printf("k=%d  string=%s  err=%d\n",k,string,ierr);     }    /* --------------- INITIALIZE CAMAC --------------------------------*/    dxyzro();    dxyset(0,0.0,vmax,1000.0,0.0,0,0);    sprintf(label[0],"Mean Channel  Number vs Voltage");    dxylbl(0,label[0]);    dxyset(1,0.0,vmax,50.0,-50.0,0,0);    sprintf(label[1],"Residuals to Straight Line");    dxylbl(1,label[1]);    dxyset(2,0.0,vmax,5.0,0.0,0,0);    sprintf(label[2],"Sigma vs Voltage");    dxylbl(2,label[2]);    dxyset(3,0.0,vmax,200.0,0.0,0,0);    sprintf(label[3],"Point Sum vs Voltage");    dxylbl(3,label[3]);    b=0;    c=1;    n=7;    a=0;    /*  set up control word ext */    cdreg(&ext,&b,&c,&n,&a);    off=0;    cccz(&ext);          /* initialize */     cccc(&ext);          /* clear */    ccci(&ext,&off);     /* remove crate inhibit */    /* -------------- SET UP PLOTS --------------------------------------*/    l=-1;    for (volt=vstrt; volt>vmax; volt=volt+vstep)  /* start l loop */    {l++;     sprintf(string,"A:VLOW %f",volt);     sendmessage(gpib_device, string );     sendmessage(gpib_device, "ERR?" );     getmessage(gpib_device, err );     ierr=err_parse(err);/*     sendmessage(gpib_device, "A:VLO?" );     getmessage(gpib_device, err );     printf("volt=%f  vback=%s\n",volt,err); */     if(ierr != 0) printf(" error setting vlow=%f\n",volt);     /* stop the QVT */     f=24;     a=0;     n=10;     cdreg(&ext,&b,&c,&n,&a);     cssa(&f,&ext,&data,&q);/*   clear the QVT */     f=9;     cssa(&f,&ext,&data,&q);     tsleep(0x80000050);     /* start the QVT */     f=26;     cssa(&f,&ext,&data,&q);/*   now take data at this voltage */     f=16;     a=7;     n=2;     data=0x1;     cdreg(&ext,&b,&c,&n,&a);/*  send nevnt pulses */     for (k=0; k<nevnt; k++)         {cssa(&f,&ext,&data,&q);         tsleep(0x80000008);        }     /* stop the QVT */     f=24;     a=0;     n=10;     cdreg(&ext,&b,&c,&n,&a);     cssa(&f,&ext,&data,&q);     /* read the QVT channel counts and calculate mean */     sm=0.0;     xsm=0.0;     xxsm=0.0;     channel=0;loopq1:     f=17;     data=channel;     cssa(&f,&ext,&data,&q);     tsleep(0x80000050);loopq2:     f=2;     cssa(&f,&ext,&data,&q);     if(q != -1)         { /* printf("read back trouble from QVT channel=%d  q=%d\n",channel,q); */          goto loopq1;        }     if(q == -1)        { if(data != 0)              { dat = (double)data;               ch = (double)channel;               sm = sm + dat;               xsm = xsm + ( ch*dat );               xxsm= xxsm + ( dat * ch * ch );               if(dbg == 1)                   {printf("channel=%d  data=%d  sm=%f  xsm=%f  xxsm=%f\n",                     channel,data,sm,xsm,xxsm);}                fprintf(fp,"channel=%d  data=%d  sm=%f  xsm=%f  xxsm=%f\n",                  channel,data,sm,xsm,xxsm);             }        channel = channel + 1;        if( channel<1024 ) goto loopq2;        }       dxyacc(3,volt,sm,0.0,0);       mean=0.0;       if(sm > 0.0) mean=xsm/sm;       xx[l]=volt;       yy[l]=mean;       sqsm = (xxsm/sm - mean*mean);       sigma=sqrt(sqsm);       dyy[l]=sigma;       ncnt=l;       printf("l=%d  volt=%f  mean=%f  sigma=%f\n",l,volt,mean,sigma);       }  /*  end volt loop */    for(lq=0;lq<=ncnt;lq++)       {dxyacc(0,xx[lq],yy[lq],dyy[lq],0);       dxyacc(2,xx[lq],dyy[lq],0.0,0);}/* calculate the linear fit to the low data */   c1=0.0;   c2=0.0;   c3=0.0;   c4=0.0;   c5=0.0;   for(i=0;i<ncnt;i++)    { sig2=dyy[i]*dyy[i];      if(sig2<1.0) sig2=1.0;      c1=c1+(xx[i]*xx[i])/sig2;      c2=c2+(xx[i])/sig2;      c3=c3+(yy[i]*xx[i])/sig2;      c4=c4+(1.0)/sig2;      c5=c5+(yy[i])/sig2;    }    aa=(c3*c4-c2*c5)/(c1*c4-c2*c2);    bb=(c5-aa*c2)/c4;   for(i=0;i<ncnt;i++)    { fit=aa*xx[i]+bb;      dif=yy[i]-fit;      dxyacc(1,xx[i],dif,dyy[i],0);    }    dxywrt(0);    dxywrt(1);    dxywrt(2);    dxywrt(3);    i=fclose(fp);    }    double dxyfit(iplot,ifit,x)    int iplot,ifit;    double x;    { double fit;      fit=0.0;      return(fit);    }fgetline(fp,s)  FILE *fp;  char s[];  {int c,i,lim;   lim=80;   i=0;   while(--lim>0&&(c=getc(fp)) != EOF && c != '\n')    s[i++] =c;   s[i]='\0';   if(i<1)     i=EOF;   return(i);  }  int err_parse(s)      char s[];      { int i,i1,i2,k;        i1=0;        i=0;        while(s[i] != ',')        i++;        i2=i;        k=0;        for (i=i1; i<i2; i++)        { k=k*10+(int)s[i]-48;}        /*        printf("err string=%s  i1=%d i2=%d s[i1]=%c s[i2]=%c k=%d\n",          s,i1,i2,s[i1],s[i2],k); */        return(k);        }