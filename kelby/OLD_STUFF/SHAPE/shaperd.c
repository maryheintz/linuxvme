#include <stdio.h>#include <math.h>#include <time.h>#include "vme.h"unsigned int sw_base,vme_base,pio_base;unsigned long vmebase,piobase,badr;double crg[12],avg[12][300],fmx[12]; double fmax,scale;char xstring[80],ystring[80],string[80],err[80];main(argc,argv,envs)int argc;char *argv[];char *envs[];  { FILE *fp;    int i,k;    double charge;   vmebase = vme_a24_base();   pio_base = vmebase + 0x570000;   sw_base = vme_base + 0xaa0000;    printf("opening shape.data\n");    fp = fopen("shape.data","r");    printf("after opening shape.data\n");    for(k=0;k<12;k++)     { fscanf(fp,"%lf%",&crg[k]);       printf(" k=%d charge=%5.1f\n",k,charge);       for(i=0;i<250;i++)         { fscanf(fp,"%lf",&avg[k][i]);	  printf(" i=%d k=%d avt=%7.2f\n",i,k,avg[k][i]);	}     }    for(k=0;k<12;k++)     { sprintf(string,"charge=%5.1f",crg[k]);       onedisp(250,string,0,k);       fmx[k] = fmax;     }    for(k=1;k<12;k++)     { for(i=0;i<250;i++) avg[k][i] = avg[k][i]-avg[0][i]; }    for(k=1;k<12;k++)     { sprintf(string,"charge=%5.1f ped subtracted",crg[k]);       onedisp(250,string,0,k);       fmx[k] = fmax;     }    for(k=1;k<11;k++)     { scale = fmx[11]/fmx[k];       sprintf(string,"charge=%5.1f  vs charge=800.0",crg[k]);       printf("k=%d fmx[11]=%f  fmx[k]=%f  scale=%f\n",k,fmx[11],fmx[k],scale);       onescale(250,string,0,k);     }  }  /* end of main */fpsw(int i)  { int ent=0,l;    unsigned long *adr,kk;    adr = (unsigned long *)pio_base;    kk = *adr;    l = (kk>>i) & 1;    return(l);  }sw(i)int i;{ int k,l,shift;  unsigned short *adr;  (unsigned short *)adr = (unsigned short *)(sw_base + 0x0c);  k = *adr;  shift = i-1;  l = (k>>shift) & 1;  return(l);}onedisp(nn,string,n1,km)  /* nn=bins to display  starting with bin n1 */ int nn,n1,km; char *string; { int mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii,iii,n2;   int idel,ifix;   double dely,y,dy;   char str[5];/* ======================= first channel plot kk1 ================== */       fmax = 0.0;   n2 = n1 + nn;   for(iii=n1;iii<n2;iii++) if(fmax < avg[km][iii]) fmax = avg[km][iii];   printf(" n1=%d  n2=%d  km=%d  fmax=%f\n",n1,n2,km,fmax);   erasm();   mmax=15000;   if(fmax<14000.0) mmax=14000;   if(fmax<13000.0) mmax=13000;   if(fmax<12000.0) mmax=12000;   if(fmax<11000.0) mmax=11000;   if(fmax<10000.0) mmax=10000;   if(fmax<9000.0) mmax=9000;   if(fmax<8000.0) mmax=8000;   if(fmax<7000.0) mmax=7000;   if(fmax<6000.0) mmax=6000;   if(fmax<5000.0) mmax=5000;   if(fmax<4000.0) mmax=4000;   if(fmax<3000.0) mmax=3000;   if(fmax<2000.0) mmax=2000;   if(fmax<1000.0) mmax=1000;   if(fmax<800.0) mmax=800;   if(fmax<600.0) mmax=600;   if(fmax<500.0) mmax=500;   if(fmax<400.0) mmax=400;   if(fmax<200.0) mmax=200;   if(fmax<100.0) mmax=100;   if(fmax<50.0) mmax=50;   idx = 1600/nn;   linx = idx*nn;   ixbas = 100+(2200-linx)/2;   dy = 2400.0/(double)mmax;   iybas = 250;   mova(ixbas,iybas);   drwr(linx,0);   mova(ixbas,iybas);   iylst = 0;   for(i=n1;i<=n2;i++)    { if(i<0) ifix=i+2560;      if(i>=0) ifix=i%2560;      ii = avg[km][ifix];      y = (double)ii;      y = y*dy;      iy = (int)y;      idy = iy-iylst;      drwr(0,idy);      drwr(idx,0);      iylst=iy;    }      drwr(0,-iylst);/*  label horizontal axis */    idel=2;    if(nn>100) idel=25;    if(nn>200) idel=50;    for(i=0;i<nn+2;i=i+idel)     { ix=ixbas+idx*i;       mova(ix,iybas);       drwr(0,-30);       k=i/2;       if(k*2 == i)         { sprintf(str,"%3d",i);           kx = ix-45;           if(i<100) kx = ix-55;           if(i<10) kx=ix-75;           symb(kx,iybas-110,0.0,str);         }     }/*  label vertical axis  */    ny=mmax/100;    if(ny>10) ny=10;    if(mmax == 1000) ny=5;    if(mmax == 800) ny=4;    if(mmax == 600) ny=3;    if(mmax == 200) ny=4;    if(mmax == 100) ny=4;    if(mmax == 50) ny=5;    y=(double)mmax*dy;    iy = (int)y;    mova(ixbas,iybas);    drwr(0,iy);    for(i=0;i<=ny;i++)     { ky = i*mmax/ny;       y = (double)ky * dy;       iy = (int)y + iybas;       mova(ixbas,iy);       drwr(-20,0);       sprintf(str,"%4d",ky);       kx = ixbas-150;       symb(kx,iy-20,0.0,str);     }    /*  print label */    symb(200,3000,0.0,string);    plotmx(); }onescale(nn,string,n1,k2)  /* nn=bins to display  starting with bin n1 */ int nn,n1,k2; char *string; { int mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii,iii,n2;   int idel,ifix,km;   double dely,y,dy;   char str[5];/* ======================= first channel plot kk1 ================== */       km=11;   fmax = 0.0;   n2 = n1 + nn;   for(iii=n1;iii<n2;iii++) if(fmax < avg[km][iii]) fmax = avg[km][iii];   printf(" n1=%d  n2=%d  km=%d  fmax=%f\n",n1,n2,km,fmax);   erasm();   mmax=15000;   if(fmax<14000.0) mmax=14000;   if(fmax<13000.0) mmax=13000;   if(fmax<12000.0) mmax=12000;   if(fmax<11000.0) mmax=11000;   if(fmax<10000.0) mmax=10000;   if(fmax<9000.0) mmax=9000;   if(fmax<8000.0) mmax=8000;   if(fmax<7000.0) mmax=7000;   if(fmax<6000.0) mmax=6000;   if(fmax<5000.0) mmax=5000;   if(fmax<4000.0) mmax=4000;   if(fmax<3000.0) mmax=3000;   if(fmax<2000.0) mmax=2000;   if(fmax<1000.0) mmax=1000;   if(fmax<800.0) mmax=800;   if(fmax<600.0) mmax=600;   if(fmax<500.0) mmax=500;   if(fmax<400.0) mmax=400;   if(fmax<200.0) mmax=200;   if(fmax<100.0) mmax=100;   if(fmax<50.0) mmax=50;   idx = 1600/nn;   linx = idx*nn;   ixbas = 100+(2200-linx)/2;   dy = 2400.0/(double)mmax;   iybas = 250;   mova(ixbas,iybas);   drwr(linx,0);   mova(ixbas,iybas);   iylst = 0;   for(i=n1;i<=n2;i++)    { if(i<0) ifix=i+2560;      if(i>=0) ifix=i%2560;      ii = avg[km][ifix];      y = (double)ii;      y = y*dy;      iy = (int)y;      idy = iy-iylst;      drwr(0,idy);      drwr(idx,0);      iylst=iy;    }      drwr(0,-iylst);/*  label horizontal axis */    idel=2;    if(nn>100) idel=25;    if(nn>200) idel=50;    for(i=0;i<nn+2;i=i+idel)     { ix=ixbas+idx*i;       mova(ix,iybas);       drwr(0,-30);       k=i/2;       if(k*2 == i)         { sprintf(str,"%3d",i);           kx = ix-45;           if(i<100) kx = ix-55;           if(i<10) kx=ix-75;           symb(kx,iybas-110,0.0,str);         }     }/*  label vertical axis  */    ny=mmax/100;    if(ny>10) ny=10;    if(mmax == 1000) ny=5;    if(mmax == 800) ny=4;    if(mmax == 600) ny=3;    if(mmax == 200) ny=4;    if(mmax == 100) ny=4;    if(mmax == 50) ny=5;    y=(double)mmax*dy;    iy = (int)y;    mova(ixbas,iybas);    drwr(0,iy);    for(i=0;i<=ny;i++)     { ky = i*mmax/ny;       y = (double)ky * dy;       iy = (int)y + iybas;       mova(ixbas,iy);       drwr(-20,0);       sprintf(str,"%4d",ky);       kx = ixbas-150;       symb(kx,iy-20,0.0,str);     }   km=k2;   mova(ixbas,iybas);   iylst = 0;   for(i=n1;i<=n2;i++)    { if(i<0) ifix=i+2560;      if(i>=0) ifix=i%2560;      ii = scale*avg[km][ifix];      y = (double)ii;      y = y*dy;      iy = (int)y;      idy = iy-iylst;      drwr(0,idy);      drwr(idx,0);      iylst=iy;    }      drwr(0,-iylst);          km=k2;    /*  print label */    symb(200,3000,0.0,string);    plotmx(); }wtstreg(int i)/* write to test register on PIO (programable IO) vme module */  { unsigned long *adr;    adr = (unsigned long *)(piobase + 0x14);    *adr = (unsigned long)i;  }    double hstfit(i,l,x)    int i,l;    double x;    {double y;     y=0.0;     return(y);     }  