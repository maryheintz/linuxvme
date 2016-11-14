

#include <stdio.h>
#include <math.h>
#include <time.h>
int sw1,sw2,sw3,sw4,sw5,sw6,sw7,sw8;
int fdata[3][256],ipk[3],pk[3];
struct{unsigned int trig;
       unsigned int reset;
       unsigned int status;
       unsigned int sample;
       unsigned int delay;
       unsigned int rdall;
       unsigned int rd2;
       unsigned int rd1;
       unsigned int rd0;}fermi;
unsigned short *reg;
unsigned short *reg0,*reg1,*reg2;
unsigned int base=0xdfb40000;
unsigned int base2=0xdfb40000;
main()
  { FILE *fp;
    double asum,axsum,axxsum,ped,dped,sig2,aval;
    int timc,timf,i;
    int zone,sector,card,tube;
    int dac_fine,dac_coarse;
    double charge,fine,coarse,charge_true;
    char xstring[80],ystring[80],string[80],err[80];
    unsigned short cadr,tc,tf;
    fermi_setup();
    printf("input - zone sector tube\n");
    fscanf(stdin,"%d %d %d",&zone,&sector,&tube);
    card = 49-tube;
    i = (zone<<12) | (sector<<6) | card;
    cadr = (unsigned short)i;
    card_sel(cadr);
    send12_3in1(2,0,0);   /* s1=s2=s3=s4=0 */
    send4_3in1(1,0);   /* itr=0 */
    send4_3in1(0,0);   /* ire=0 */
    send4_3in1(3,0);   /* mse=0 */
    send4_3in1(4,1);   /* tpe=1 */
    send4_3in1(5,0);   /* lcal=0 */
    tc = (unsigned short)100;
    tim1_set(tc);   /* coarse */
    tf = 0;
    tim2_set(tf);   /* fine */
    printf("input - charge\n");
    fscanf(stdin,"%lf",&charge);
    fine = 255.0;
floop:
    coarse = (charge * 255.0 * 255.0 ) / (500.0 * fine);
    if(coarse<50.0)
     { fine = 0.5 * fine;
       goto floop;}
    printf("charge=%f  coarse=%f  fine=%f\n",charge,coarse,fine);
    dac_coarse = (int) coarse;
    dac_fine = (int) fine;
    charge_true=5.0*128.0*((double)dac_coarse/255.0)*((double)dac_fine/255.0);
    printf(" dc=%d df=%d charge_true=%f\n",dac_coarse,dac_fine,charge_true);
    send12_3in1(6,1,dac_fine);   /* set dac_fine value */
    send12_3in1(6,0,dac_coarse);   /* set dac_coarse value */
loop:
        fermi_reset();
        tp_high();
        tsleep(5);
        stime();
        tsleep(2);
        rtime();
/*     go read the fermi module ------------------------------- */
        fermi_read();
/*     display the high gain? */
         sw1 = sw(1);
         if(sw1 != 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=1;i<101;i++)
             { aval = (double)fdata[2][i];
               asum = asum+1.0;
               axsum = axsum + aval;
               axxsum = axxsum + aval*aval;
              }
            ped = axsum / asum;
            dped = 0.0;
            sig2=axxsum/asum - ped*ped;
            if(sig2>0.0) dped=sqrt(sig2);
            sprintf(string,
              "high gain ipk=%d  pk=%d  ped=%f +/- %f",
              ipk[2],pk[2],ped,dped);
            evdisp(250,string,2);}
/*     display the low gain? */
         sw2 = sw(2);
         if(sw2 != 1) 
          { asum = 0.0;
            axsum = 0.0;
            axxsum = 0.0;
            for(i=1;i<101;i++)
             { aval = (double)fdata[1][i];
               asum = asum+1.0;
               axsum = axsum + aval;
               axxsum = axxsum + aval*aval;
              }
            ped = axsum / asum;
            dped = 0.0;
            sig2=axxsum/asum - ped*ped;
            if(sig2>0.0) dped=sqrt(sig2);
            sprintf(string,
              "lo gain ipk=%d  pk=%d  ped=%f +/- %f",
              ipk[1],pk[1],ped,dped);
            evdisp(250,string,1);}
       goto loop;
}  /* end of main */
sw(i)
  int i;
  {int *adr,k,l,shft;
   adr=(int *)0xdfcf0000;
   k=*adr;
   shft=23+i;
   l=(k>>shft)&1; 
   return(l);
  }      
fermi_setup()
 { FPermit(base,0x100,3);
   FPermit(base2,0x100,3);
   fermi.trig=base+0x26;
   fermi.reset=base+0x24;
   fermi.status=base+0x22;
   fermi.sample=base+0x20;
   fermi.delay=base+0x1e;
   fermi.rdall=base2+0x18;
   fermi.rd2=base+0x14;
   fermi.rd1=base+0x12;
   fermi.rd0=base+0x10;
/* issue reset */
   reg=(unsigned short *)fermi.reset;
   *reg=0;
/* set to full 255 samples */
   reg=(unsigned short *)fermi.sample;
   *reg=255;
/* set delay */
   reg=(unsigned short *)fermi.delay;
   *reg=120;
   reg0=(unsigned short *) fermi.rd0;
   reg1=(unsigned short *) fermi.rd1;
   reg2=(unsigned short *) fermi.rd2;
 }
fermi_reset()
 { 
/* issue reset */
   reg=(unsigned short *)fermi.reset;
   *reg=0;
 }
fermi_read()
 { int stat,data_ready,busy,lcnt,i,k0,k1,k2,kmax0,kmax1,kmax2;
   int dd0,dd1,dd2;
   unsigned short sd0,sd1,sd2;
   /* wait for a trigger */
   lcnt=0;
loop:
   lcnt++;
   reg = (unsigned short *)fermi.status;
   stat = *reg;
   data_ready = stat & 4;
   busy = stat & 2;
   if(lcnt > 1000 ) goto error;
   if ( data_ready == 0 ) goto loop;
   /* read the data and find peak channel */
   k0=0;
   kmax0=0;
   k1=0;
   kmax1=0;
   k2=0;
   kmax2=0;
   for (i=0;i<255;i++)
    { sd0 = *reg0;
      dd0 = (int)sd0 & 0xfff;
      fdata[0][i] = 0xfff - dd1;
      if(fdata[0][i]>kmax0)
       { kmax0=fdata[0][i];
         k0=i;}
      sd1 = *reg1;
      dd1 = (int)sd1 & 0xfff;
      fdata[1][i] = 0xfff - dd1;
      if(fdata[1][i]>kmax1)
       { kmax1=fdata[1][i];
         k1=i;}
      sd2 = *reg2;
      dd2 = (int)sd2 & 0xfff;
      fdata[2][i] = 0xfff - dd2;
      if(fdata[2][i]>kmax2)
       { kmax2=fdata[2][i];
         k2=i;}
      sw4 = sw(4);
      if(sw4 == 1) printf("i=%d  dd0=%d  dd1=%d  dd2=$d\n",i,dd0,dd1,dd2);
     }
   ipk[0]=k0;
   pk[0]=kmax0;
   ipk[1]=k1;
   pk[1]=kmax1;
   ipk[2]=k2;
   pk[2]=kmax2;
   return(0);
error:
   ipk[0]=-1;
   ipk[1]=-1;
   ipk[2]=-1;
}
evdisp(nn,string,l)
 int nn,l;
 char *string;
 { int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii;
   int idel;
   double dely,y,dy;
   char str[5];
   erasm();
   max = pk[l];
   mmax=5000;
   if(max<4000) mmax=4000;
   if(max<3000) mmax=3000;
   if(max<2000) mmax=2000;
   if(max<1000) mmax=1000;
   if(max<800) mmax=800;
   if(max<600) mmax=600;
   if(max<500) mmax=500;
   if(max<400) mmax=400;
   if(max<200) mmax=200;
   if(max<100) mmax=100;
   if(max<50) mmax=50;
   idx = 1600/nn;
   linx = idx*nn;
   ixbas = 100+(2200-linx)/2;
   dy = 2400.0/(double)mmax;
   iybas = 400;
   mova(ixbas,iybas);
   drwr(linx,0);
   mova(ixbas,iybas);
   iylst = 0;
   for(i=0;i<nn;i++)
    { ii = fdata[l][i];
      y = (double)ii;
      y = y*dy;
      iy = (int)y;
      idy = iy-iylst;
      drwr(0,idy);
      drwr(idx,0);
      iylst=iy;
    }
      drwr(0,-iylst);

/*  label horizontal axis */
    idel=10;
    if(nn>100) idel=25;
    if(nn>200) idel=50;
    for(i=0;i<nn+1;i=i+idel)
     { ix=ixbas+idx*i;
       mova(ix,iybas);
       drwr(0,-30);
       k=i/2;
       if(k*2 == i)
         { sprintf(str,"%3d",i);
           kx = ix-45;
           if(i<100) kx = ix-55;
           if(i<10) kx=ix-75;
           symb(kx,iybas-100,0.0,str);
         }
     }
/*  label vertical axis  */
    ny=mmax/100;
    if(ny>10) ny=10;
    if(mmax == 200) ny=4;
    if(mmax == 100) ny=4;
    if(mmax == 50) ny=5;
    y=(double)mmax*dy;
    iy = (int)y;
    mova(ixbas,iybas);
    drwr(0,iy);
    for(i=0;i<=ny;i++)
     { ky = i*mmax/ny;
       y = (double)ky * dy;
       iy = (int)y + iybas;
       mova(ixbas,iy);
       drwr(-20,0);
       sprintf(str,"%4d",ky);
       kx = ixbas-150;
       symb(kx,iy-20,0.0,str);
     }
    /*  print label */
    symb(200,3000,0.0,string);
    plotmx();
 }



