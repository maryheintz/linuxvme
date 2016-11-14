#include <math.h>
#include <time.h>
#include "vme.h"
unsigned long vmebase,piobase=0x570000;
short *caen_data,*caen_prelsb,*caen_premsb,*caen_postlsb,*caen_postmsb,*caen_trigrec,
 *caen_softtrig,*caen_trigtype,*caen_trigsource,*caen_reset,*caen_start,*caen_interrupt,
 *caen_chmask;
int ksave,msave,maxsave;
double p[10];
short cdata[4][3000],raw[4][3000],ped[4][3000],posttrig,endcell,trigrec; 
short wd1[4],wd2[4],wd3[4],wd;
char string[80];
int npnts;
double xk[300],yk[300];
double yfit[300],sigmay[300];
FILE *fp;
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { int icap,n1,m,i,event,nn,ibst;
    int dac,alarm;
    double rn1,rn2,p0b,p1b,p2b,p3b,p0strt,p1strt,p2strt,p3strt;
extern double functn();  
extern double ranf();
extern double chisqr();
    fp = fopen("test.txt","w");
    dotset(0,1500.1250,0.0,150.0,0.0,0,0);
    dotset(1,80.0,40.0,2300.0,1300.0,0,0);
    hstset(0,2300.0,1300.0,100,0);
    hstset(1,1400.0,1300.0,100,0);
    hstset(2,2200.0,2100.0,100,0);
    vmebase = vme_a24_base();
    icap = 0;
    printf(" calling caen_setup\n");
        caen_setup();
	usleep(2);  
    printf(" returned from caen_setup\n");
	sidev(1);  /* force plot to screen without asking*/
/* ========== get the pedestals ================ */
    printf("stop pulsing then hit return\n");
    i = getchar();
    *caen_trigtype = 0;
    for(m=0;m<4;m++)
      { for(i=0;i<2560;i++)
	 { ped[m][i] = 0;;
	 }
      }
    for(i=0;i<10;i++)
      { *caen_reset=0;
        usleep(10);
        *caen_start=1;
        usleep(10000);
        *caen_softtrig=0;;
       usleep(10000);
       /* check if caen got a trigger */
       wd = *caen_interrupt;
       /* printf("interrupt=%d  vernier=%d    %d\n",wd&1,wd2[0],maxsave-wd2[0]);  */
       if(wd & 1 == 0)
         { printf(" error - no caen interrupt bit set \n");
	   exit(0);
	 }
       caen_read();
       for(m=0;m<4;m++)
         { for(i=0;i<2560;i++)
	    { ped[m][i]=ped[m][i] + raw[m][i];
	    }
	 }
       }
       for(m=0;m<4;m++)
         { for(i=0;i<2560;i++)
	    { ped[m][i]=ped[m][i]/10;
	    }
	 }

    printf("start pulsing then hit return\n");
    i = getchar();
    printf(" starting event loop\n");
	*caen_trigtype = 0x2;
	event=0;
 loop:         
    p[0]= 0.47;
    p[1]= -760.0;
    p[2]= 1200.0;
    p[3]= 1800.0;
       *caen_reset=0;
       usleep(10);
       *caen_start=1;
       usleep(10000);
       inject();
       usleep(10000);
       /* check if caen got a trigger */
       wd = *caen_interrupt;
       /* printf("interrupt=%d  vernier=%d    %d\n",wd&1,wd2[0],maxsave-wd2[0]);  */
       if(wd & 1 == 0)
         { printf(" error - no caen interrupt bit set \n");
	   goto loop;
	 }
       caen_read();
       dotacc(0,(double)ksave,(double)trigrec);
       dotacc(1,(double)(ksave-1300),(double)wd2[0]);
       hstacc(0,(double)wd2[0],1.0);
       hstacc(1,(double)wd2[0],1.0);
       hstacc(2,(double)wd2[0],1.0);
       npnts=250;
       for(i=0;i<250;i++)
         { xk[i] = 1500.0 + (double)i;
	   yk[i] = cdata[0][i+1500];
	   sigmay[i] = 5.0;
	  /* if(yk[i]>25.0) sigmay[i] = sqrt(yk[i]);  */
	 }
/* =================== search for a fit ====================== */
       rn1 = chisqr();
       printf("rn1=%f\n",rn1);
          if(icap == 0) sprintf(string,"B4 LOW GAIN  peak=%d  trigrec=%d  bins=%d-%d  vernier=%d",
	     ksave,trigrec,n1,n1+250,wd2[0]);
          if(icap == 1) sprintf(string,"HIGH GAIN  input=%d    bins=%d-%d",msave,n1,n1+250);
          msave=0;
          nn=ksave/250;
          n1=nn*250;
	  printf(" ksave=%d  nn=%d  n1=%d\n",ksave,nn,n1);
          onedisp(250,string,n1,msave);
/* =============================== pass one ============================== */
  rn1 = 1.0e9;
  p[1] = -850.0;
  for(i=0;i<300;i++)
    { p[1] = -800.0 + (double)i * 0.5;
      rn2 = chisqr();
/*      printf(" i=%d p[1]=%f  rn2=%f\n",i,p[1],rn2); */
      if(rn2<rn1)
       { rn1 = rn2;
	 p1b = p[1];
	 ibst = i;
       }
    }
   p[1] = p1b;
   printf(" i=%d p[1]=%f  x2bst=%f\n",ibst,p[1],rn1);

   rn1 = 1.0e9;   
  for(i=0;i<1000;i++)
    { p[0] = 0.46 + (double)i * 0.0001;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p0b = p[0];
	 ibst = i;
       }
     }
   p[0] = p0b;
   printf(" i=%d p[0]=%f  x2bst=%f\n",ibst,p[0],rn1);

   rn1 = 1.0e9;
   for(i=0;i<1000;i++)
    { p[2] = 1000.0 + (double)i * 1.0;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p2b = p[2];
	 ibst = i;
       }
     }
   p[2] = p2b;
   printf(" i=%d p[2]=%f  x2bst=%f\n",ibst,p[2],rn1);

   rn1 = 1.0e9;   
   for(i=0;i<1000;i++)
    { p[3] = 1700.0 + (double)i * 2.0;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p3b = p[3];
       }
    }
   p[3] = p3b; 
   rn2=chisqr();                      

/* =============================== pass two ============================== */
  rn1 = 1.0e9;
  p1strt = p1b-20.0;
  for(i=0;i<80;i++)
    { p[1] = p1strt + (double)i * 0.5;
      rn2 = chisqr();
/*      printf(" i=%d p[1]=%f  rn2=%f\n",i,p[1],rn2); */
      if(rn2<rn1)
       { rn1 = rn2;
	 p1b = p[1];
	 ibst = i;
       }
    }
   p[1] = p1b;
   printf(" i=%d p[1]=%f  x2bst=%f\n",ibst,p[1],rn1);

   rn1 = 1.0e9;   
  p0strt = p0b - 0.01;
  for(i=0;i<200;i++)
    { p[0] = p0strt + (double)i * 0.0001;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p0b = p[0];
	 ibst = i;
       }
    }
   p[0] = p0b;
   printf(" i=%d p[0]=%f  x2bst=%f\n",ibst,p[0],rn1);

   rn1 = 1.0e9;
   p2strt = p2b - 100.0;
   for(i=0;i<200;i++)
    { p[2] = p2strt + (double)i * 1.0;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p2b = p[2];
	 ibst = i;
       }
    }
   p[2] = p2b;
   printf(" i=%d p[2]=%f  x2bst=%f\n",ibst,p[2],rn1);

   rn1 = 1.0e9;
   p3strt = p3b - 100.0;   
   for(i=0;i<200;i++)
    { p[3] = p3strt + (double)i * 1.0;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p3b = p[3];
       }
    }
   p[3] = p3b; 
   rn2=chisqr();                      

       printf("fit return rn2=%f   p=%f  %f  %f %f\n",rn2,p[0],p[1],p[2],p[3]);
          if(icap == 0) sprintf(string,"LOW GAIN  peak=%d  trigrec=%d  bins=%d-%d  vernier=%d",
	     ksave,trigrec,n1,n1+250,wd2[0]);
          if(icap == 1) sprintf(string,"HIGH GAIN  input=%d    bins=%d-%d",msave,n1,n1+250);
          msave=0;
          nn=ksave/250;
          n1=nn*250;
	  printf(" ksave=%d  nn=%d  n1=%d\n",ksave,nn,n1);
          onedisp(250,string,n1,msave);
	usleep(1);
	if(fpsw(5)==1) dotwrt(0);
	if(fpsw(5)==1) dotwrt(1);
	if(fpsw(4)==1) hstwrt(0);
	if(fpsw(4)==1) hstwrt(1);
	if(fpsw(4)==1) hstwrt(2);
        event++;
	wtstreg(event);
          goto loop;
   }  /* end of main */
   
    caen_setup()
      { short k1,k2,j1,j2,pre;
        /* test write-read to caen v1729 */
	caen_prelsb = (short*)(vmebase+0x661800);  
        *caen_prelsb = 0xaa;
	usleep(1);
	k1 = *caen_prelsb;
	k2 = k1 & 0xff;
	printf("sent=0xaa  read back=%x %x\n",k1,k2);
	if(k2 != 0xaa) exit(0);
 
         /* ========== issue reset ========== */
        caen_reset = (short*)(vmebase+0x660800);
        *caen_reset=0;
 
 	/* ========== setup pretrig ========== */
	pre = 10;
	caen_prelsb = (short*)(vmebase+0x661800);  
	caen_premsb = (short*)(vmebase+0x661900); 
	k1 = (pre & 0xff);
	k2 = ((pre>>8) & 0xff);

        *caen_prelsb = k1;
        *caen_premsb = k2;
	j1 = *caen_prelsb & 0xff;
	j2 = *caen_premsb & 0xff;

        /* ========== setup posttrig ========== */
	posttrig = 0;
	caen_postlsb = (short*)(vmebase+0x661a00);
	caen_postmsb = (short*)(vmebase+0x661b00);
        *caen_postlsb = posttrig & 0xff;
        *caen_postmsb = (posttrig>>8) & 0xff;

        /* ========== setup the trigger source ========== */
	caen_trigsource = (short*)(vmebase+0x661e00);
	*caen_trigsource = 0;

        /* ========== channel mask - store data for all 4 channels ========== */
	caen_chmask = (short*)(vmebase+0x662300);
	*caen_chmask = 0xf;

        /* ========== setup triger type ========== */
	caen_trigtype = (short*)(vmebase+0x661d00);
	*caen_trigtype = 0x2;

        caen_start = (short*)(vmebase+0x661700);
	caen_trigrec = (short*)(vmebase+0x662000);
	caen_data = (short*)(vmebase+0x660d00);
	caen_softtrig = (short*)(vmebase+0x661c00);
	caen_interrupt = (short*)(vmebase+0x668000);

      }
      
    caen_read()
        { int i,kk,k,m,shift;
          unsigned short wd;
	  short tr;
	  msave=0;
	  ksave=0;
	  maxsave=0;
 	  tr = *caen_trigrec;
	  trigrec = tr & 0xff;
          shift = 2560*(128-trigrec)/128;
	  /* printf(" tr=%x  trigrec=%d  endcell=%d\n",tr,trigrec,endcell);   */
	  endcell = (20*(posttrig+trigrec))%128;
	     wd1[3]=*caen_data;
	     wd1[2]=*caen_data;
	     wd1[1]=*caen_data;
	     wd1[0]=*caen_data;
	     wd2[3]=*caen_data;
	     wd2[2]=*caen_data;
	     wd2[1]=*caen_data;
	     wd2[0]=*caen_data;
	     wd3[3]=*caen_data;
	     wd3[2]=*caen_data;
	     wd3[1]=*caen_data;
	     wd3[0]=*caen_data;
	     for(i=0;i<2560;i++) 
	      { wd=*caen_data;
	        raw[3][i] = wd & 0xfff;
	        wd=*caen_data;
	        raw[2][i] = wd & 0xfff;
	        wd=*caen_data;
	        raw[1][i] = wd & 0xfff;
	        wd=*caen_data;
	        raw[0][i] = wd & 0xfff;
	      }
          for(m=0;m<4;m++)
	   { for(i=0;i<2560;i++)
	       {  kk = 2560+i-endcell;
	         k = kk%2560;
                 k=i-shift;
		 if(k<0) k=k+2560;
		 if(k>2560) k=k-2560;
	         cdata[m][k]=raw[m][i]-ped[m][i];  
		 /* if(m==0) printf(" i=%d  raw=%d  k=%d  cdata=%d\n",i,raw[0][i],k,cdata[0][k]); */
	         /* cdata[m][i]=raw[m][i]; */ /* just copy through for now */
		 if(cdata[m][k]>maxsave)
		  { msave=m;
		    ksave=k;
		    maxsave=cdata[m][k];
		    /* printf("m=%d  k=%d  max=%d\n",msave,ksave,maxsave);  */
		  }
	       } 
           }
	 }	

     inject()
       { unsigned long *adr;
 /* write to PIO vme module (programable io) */
 /* let one trigger pulse through to the CAEN v1729 */
        (unsigned long *)adr = (unsigned long *)(vmebase+0x570000+0x4);
        *adr = 0;
       }
  
onedisp(nn,string,n1,m)  /* nn=bins to display  starting with bin n1 */
 int nn,n1,m;
 char *string;
 { int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii,iii,n2;
   int idel,ifix;
   double dely,y,dy;
   char str[5];
   erasm();
/* ======================= first channel plot kk1 ================== */    
   max = 0;
   n2 = n1 + nn;
   for(iii=n1; iii<n2; iii++) 
     {  if(iii<0) ifix=iii+2560;
        if(iii>=0) ifix=iii%2560;
        if (max < cdata[m][ifix]) max = cdata[m][ifix];
     };
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
   iybas = 250;
   mova(ixbas,iybas);
   drwr(linx,0);
   mova(ixbas,iybas);
   iylst = 0;
   for(i=n1;i<=n2;i++)
    { if(i<0) ifix=i+2560;
      if(i>=0) ifix=i%2560;
      ii = cdata[m][ifix];
      y = (double)ii;
      y = y*dy;
      iy = (int)y;
      idy = iy-iylst;
      drwr(0,idy);
      drwr(idx,0);
      iylst=iy;
    }
      drwr(0,-iylst);
/* draw the fit */
   for(i=n1;i<n2;i++)
     { ii=i-n1;
       y = yfit[ii]*dy;
       iy = (int)y + iybas;
       ix = ixbas + idx*(ii) + idx/2;
       if(i==n1) mova(ix,iy);
       if(i!=n1) drwa(ix,iy);
        fprintf(fp,"i=%d ii=%d ix=%d  iy=%d  yfit=%f  cdata=%d\n",
	   i,ii,ix,iy,yfit[ii],cdata[0][i]);
     }      

/*  label horizontal axis */
    idel=2;
    if(nn>100) idel=25;
    if(nn>200) idel=50;
    for(i=0;i<nn+2;i=i+idel)
     { ix=ixbas+idx*i;
       mova(ix,iybas);
       drwr(0,-30);
       k=i/2;
       if(k*2 == i)
         { sprintf(str,"%3d",i);
           kx = ix-45;
           if(i<100) kx = ix-55;
           if(i<10) kx=ix-75;
           symb(kx,iybas-110,0.0,str);
         }
     }
/*  label vertical axis  */
    ny=mmax/100;
    if(ny>10) ny=10;
    if(mmax == 1000) ny=5;
    if(mmax == 800) ny=4;
    if(mmax == 600) ny=3;
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
fpsw(int i)
/* read front panel w=switches on PIO (programable IO) vme module */
  { int l;
    unsigned long swadr=0x570000,*adr,kk;
    adr = (unsigned long *)(vmebase + swadr);
    kk = *adr;
    l = (kk>>i) & 1;
    return(l);
  }

wtstreg(int i)
/* write to test register on PIO (programable IO) vme module */
  { unsigned long swadr=0x570000,*adr;
    adr = (unsigned long *)(vmebase + swadr + 0x14);
    *adr = (unsigned long)i;
  }

    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=0.0;
     return(y); 
    }  
 double chisqr()
   { int i;
     double chisq1,sdy,dy;
     extern double functn();  
        for(i=0;i<npnts;i++) 
	  { yfit[i] = functn(xk[i]);
	    if(fpsw(1)==1) 
	      { printf("i=%d  xk=%f  yk=%f  yfit=%f\n",
	             i,xk[i],yk[i],yfit[i]);
	      }
	  }     
           chisq1=0.0;
	   for(i=0;i<npnts;i++) 
	     { dy = yk[i]-yfit[i];
	       sdy = sigmay[i]; 
	       chisq1 = chisq1 + ((dy*dy)/(sdy*sdy));
	       if(fpsw(1)==1) printf("i=%d  yk[i]=%f  yfit=%f  dy=%f  sdy=%f chisq1=%f\n",
	         i,yk[i],yfit[i],dy,sdy,chisq1);
	     }
	  /* printf(" chisqr=%f  p=%f  %f  %f\n",chisq1,p[0],p[1],p[2]); */
		if(i==fpsw(3)==1) exit(0); 
	  return(chisq1);
   }
  double functn(double xx)
    { int ix;
      double yy,dx,dy,delx,dely,ximai,yimai,xkx;
      double xbin[100];				       
      double ybin[100] = { 0.000147, 0.000589, 0.000368, 0.001105, 0.000663,
                           0.000589, 0.000663,-0.000295,-0.000884,-0.000074,
                            0.002137, 0.001695,-0.000074,-0.002358, 0.000663,
                            0.000221, 0.001474,-0.000295,-0.001326, 0.000663,
                            0.002874, 0.001916, 0.000589, 0.002579, 0.008253,
                            0.015695, 0.028369, 0.047307, 0.074792, 0.108246,
                            0.147963, 0.191217, 0.248988, 0.330559, 0.420381,
                            0.505638, 0.603419, 0.690295, 0.772015, 0.845996,
                            0.907671, 0.953577, 0.984453, 1.000000, 0.996905,
                            0.978336, 0.941347, 0.892860, 0.833542, 0.763836,
                            0.691180, 0.616168, 0.535554, 0.456930, 0.382875,
                            0.317884, 0.250461, 0.195859, 0.147152, 0.105298,
                            0.069339, 0.040749, 0.014885, 0.001253,-0.001179,
                           -0.001253,-0.005748,-0.008695,-0.008990,-0.008990,
                           -0.007884,-0.006484,-0.005453,-0.005084,-0.005379,
                           -0.005895,-0.006558,-0.008400,-0.010611,-0.011348,
                           -0.011421,-0.012085,-0.013337,-0.013853,-0.014516,
                           -0.014443,-0.013411,-0.013558,-0.012600,-0.011716,
                           -0.010758,-0.010685,-0.009653,-0.009211,-0.008990,
                           -0.007590,-0.007442,-0.006705,-0.006853,-0.007811};
	ximai = p[0]*xx + p[1];
	if(ximai<-107.0) 
	    { yimai = (ybin[0] + ybin[1] + ybin[2] + ybin[3] + ybin[4])/5.0;
	      yy = p[2]*yimai + p[3];
	      if(fpsw(2)==1) fprintf(fp," xx=%f  yy=%f  ximai=%f  yimai=%f  %f  %f\n",
	          xx,yy,ximai,yimai,p[2]*yimai,p[3]); 
	      return(yy);
	    }
	if(ximai>139.0) 
	    { yimai = (ybin[95] + ybin[96] + ybin[97] + ybin[98] + ybin[99])/5.0;
	      yy = p[2]*yimai + p[3];
	      if(fpsw(2)==1) fprintf(fp," xx=%f  yy=%f  ximai=%f  yimai=%f  %f  %f\n",
	          xx,yy,ximai,yimai,p[2]*yimai,p[3]); 
	      return(yy);
	    }
        ix = (int)(ximai/2.5) + 43;
	dx = fmod(ximai,2.5);
	delx = 2.5;
	dely = ybin[ix+1] - ybin[ix];
	dy = dx*dely/delx;
	yimai = ybin[ix]+dy;
	yy = p[2]*yimai + p[3];
	if(fpsw(2)==1) fprintf(fp," xx=%f  yy=%f  ximai=%f  yimai=%f  %f  %f\n",
	     xx,yy,ximai,yimai,p[2]*yimai,p[3]); 
	return(yy);
   }	
