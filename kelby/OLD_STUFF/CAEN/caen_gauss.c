#include <math.h>
#include <time.h>
#include "vme.h"
unsigned long vmebase,piobase=0x570000,badr;
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
    double rn1,rn2,p0b,p1b,p2b,p3b,p4b,p0strt,p1strt,p2strt,p3strt,p4strt;
extern double functn();  
extern double ranf();
extern double chisqr();
    fp = fopen("test.txt","w");
    dotset(0,1500.1250,0.0,150.0,0.0,0,0);
    dotset(1,120.0,100.0,2300.0,1300.0,0,0);
    hstset(0,2300.0,1300.0,100,0);
    hstset(1,1400.0,1300.0,100,0);
    hstset(2,2200.0,2100.0,100,0);
    vmebase = vme_a24_base();
    printf("vme_a24_base=%x\n",vme_a24_base);
    icap = 0;
    printf(" enter caen base address 0=0x46  1=0x56  2=0x66\n");
    fscanf(stdin,"%d",&i);
    badr = 0x460000;
    if(i == 0) badr = 0x460000;
    if(i == 1) badr = 0x560000;
    if(i == 2) badr = 0x660000;
    printf("badr=%x\n",badr);
    caen_setup();
    usleep(2);  
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
       if(fpsw(2)==1)
         { fscanf(stdin,"%d",&posttrig);
	   printf("posttrig=%d\n",posttrig);
           caen_postlsb = (unsigned short*)(vmebase+badr+0x1a00);
	   caen_postmsb = (unsigned short*)(vmebase+badr+0x1b00);
           *caen_postlsb = posttrig & 0xff;
           *caen_postmsb = (posttrig>>8) & 0xff;
         }
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
       nn=ksave/250;
       n1=nn*250;
       npnts=250;
       for(i=0;i<250;i++)
         { xk[i] = (double)i;
	   yk[i] = cdata[msave][i+n1];
	   sigmay[i] = 5.0;
	  /* if(yk[i]>25.0) sigmay[i] = sqrt(yk[i]);  */
	 }
/* =================== search for a fit ====================== */
      p[0]= 1285.0;
      p[1]= ksave+2;
      p[2]= 35.0;
      p[3]= 40.0;
      p[4] = 320.0;
      rn1 = chisqr();
      printf("rn1=%f\n",rn1);
      printf(" ksave=%d  nn=%d  n1=%d\n",ksave,nn,n1);
      if(fpsw(0) == 1)
        { if(icap == 0) sprintf(string,"B4 LOW GAIN  peak=%d  trigrec=%d  bins=%d-%d  vernier=%d",
	     ksave,trigrec,n1,n1+250,wd2[0]);
          if(icap == 1) sprintf(string,"B4 HIGH GAIN  peak=%d  trigrec=%d  bins=%d-%d  vernier=%d",
	     ksave,trigrec,n1,n1+250,wd2[0]);
          if(icap == 1) sprintf(string,"HIGH GAIN  input=%d    bins=%d-%d",msave,n1,n1+250);
          onedisp(250,string,n1,msave);
	} 
/* =============================== pass one ============================== */
  for(i=0;i<80;i++)
    { p[1] = (ksave-20.0) + (double)i * 0.5;
      rn2 = chisqr();
/*      printf(" i=%d p[1]=%f  rn2=%f\n",i,p[1],rn2); */
      if(rn2<rn1)
       { rn1 = rn2;
	 p1b = p[1];
	 ibst = i;
       }
    }
   p[1] = p1b;
 /*  printf(" i=%d p[1]=%f  x2bst=%f\n",ibst,p[1],rn1); */

   rn1 = 1.0e9;   
  for(i=0;i<60;i++)
    { p[0] = 1275.0 + (double)i * 0.5;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p0b = p[0];
	 ibst = i;
       }
     }
   p[0] = p0b;
  /*  printf(" i=%d p[0]=%f  x2bst=%f\n",ibst,p[0],rn1); */

   rn1 = 1.0e9;
   for(i=0;i<20;i++)
    { p[2] = 25 + (double)i * 0.5;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p2b = p[2];
	 ibst = i;
       }
     }
   p[2] = p2b;
/*   printf(" i=%d p[2]=%f  x2bst=%f\n",ibst,p[2],rn1); */

   rn1 = 1.0e9;   
   for(i=0;i<40;i++)
    { p[3] = 30.0 + (double)i * 0.5;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p3b = p[3];
       }
    }
   p[3] = p3b; 
   rn2=chisqr();                      

   rn1 = 1.0e9;   
   for(i=0;i<80;i++)
    { p[4] = 300.0 + (double)i * 0.5;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p4b = p[4];
       }
    }
   p[4] = p4b; 
   rn2=chisqr();                      



/* =============================== pass two ============================== */
  rn1 = 1.0e9;
  for(i=0;i<80;i++)
    { p[1] = (ksave-20.0) + (double)i * 0.5;
      rn2 = chisqr();
/*      printf(" i=%d p[1]=%f  rn2=%f\n",i,p[1],rn2); */
      if(rn2<rn1)
       { rn1 = rn2;
	 p1b = p[1];
	 ibst = i;
       }
    }
   p[1] = p1b;
/*   printf(" i=%d p[1]=%f  x2bst=%f\n",ibst,p[1],rn1); */

   rn1 = 1.0e9;   
  for(i=0;i<60;i++)
    { p[0] = 1280.0 + (double)i * 0.5;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p0b = p[0];
	 ibst = i;
       }
     }
   p[0] = p0b;
/*   printf(" i=%d p[0]=%f  x2bst=%f\n",ibst,p[0],rn1); */

   rn1 = 1.0e9;
   for(i=0;i<20;i++)
    { p[2] = 25 + (double)i * 0.5;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p2b = p[2];
	 ibst = i;
       }
     }
   p[2] = p2b;
/*   printf(" i=%d p[2]=%f  x2bst=%f\n",ibst,p[2],rn1); */

   rn1 = 1.0e9;   
   for(i=0;i<40;i++)
    { p[3] = 30.0 + (double)i * 0.5;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p3b = p[3];
       }
    }
   p[3] = p3b; 
   rn2=chisqr();                      

   rn1 = 1.0e9;   
   for(i=0;i<80;i++)
    { p[4] = 300.0 + (double)i * 0.5;
      rn2 = chisqr();
      if(rn2<rn1)
       { rn1 = rn2;
	 p4b = p[4];
       }
    }
   p[4] = p4b;
   rn2=chisqr();                      

       dotacc(0,(double)ksave,(double)trigrec);
       dotacc(1,p[1],(double)wd2[0]);
       hstacc(0,(double)wd2[0],1.0);
       hstacc(1,(double)wd2[0],1.0);
       hstacc(2,(double)wd2[0],1.0);

       printf("fit return rn2=%f   p=%f  %f  %f %f %f\n",rn2,p[0],p[1],p[2],p[3],p[4]);
       if(fpsw(0)==1)
        { if(icap == 0) sprintf(string,"LOW GAIN  p[0]-p[4]= %f %f %f %f %f\n",
	     p[0],p[1],p[2],p[3],p[4]);
          if(icap == 1) sprintf(string,"HIGH GAIN  input=%d    bins=%d-%d",msave,n1,n1+250);
          nn=ksave/250;
          n1=nn*250;
	  /* printf(" ksave=%d  nn=%d  n1=%d\n",ksave,nn,n1); */
          onedisp(250,string,n1,msave);
	 }
	usleep(1);
	if(fpsw(4)==1) 
	  { dotwrt(0);
	    dotwrt(1);
	    hstwrt(0);
	    hstwrt(1);
	    hstwrt(2);
	  }
	if(fpsw(5)==1) 
	  { sidev(0);
	    dotwrt(0);
	    dotwrt(1);
	    hstwrt(0);
	    hstwrt(1);
	    hstwrt(2);
	    exit(0);
	  }
        event++;
	wtstreg(event);
          goto loop;
   }  /* end of main */
   
    caen_setup()
      { unsigned short k1,k2,j1,j2,pre;
        /* test write-read to caen v1729 */
	caen_prelsb = (unsigned short*)(vmebase+badr+0x1800);  
        *caen_prelsb = 0xaa;
	usleep(1);
	k1 = *caen_prelsb;
	k2 = k1 & 0xff;
	printf("sent=0xaa  read back=%x %x\n",k1,k2);
	if(k2 != 0xaa) exit(0);
 
         /* ========== issue reset ========== */
        caen_reset = (unsigned short*)(vmebase+badr+0x0800);
        *caen_reset=0;
 
 	/* ========== setup pretrig ========== */
	pre = 10;
	caen_prelsb = (unsigned short*)(vmebase+badr+0x1800);  
	caen_premsb = (unsigned short*)(vmebase+badr+0x1900); 
	k1 = (pre & 0xff);
	k2 = ((pre>>8) & 0xff);

        *caen_prelsb = k1;
        *caen_premsb = k2;
	j1 = *caen_prelsb & 0xff;
	j2 = *caen_premsb & 0xff;

        /* ========== setup posttrig ========== */
	posttrig = 0;
	caen_postlsb = (unsigned short*)(vmebase+badr+0x1a00);
	caen_postmsb = (unsigned short*)(vmebase+badr+0x1b00);
        *caen_postlsb = posttrig & 0xff;
        *caen_postmsb = (posttrig>>8) & 0xff;

        /* ========== setup the trigger source ========== */
	caen_trigsource = (unsigned short*)(vmebase+badr+0x1e00);
	*caen_trigsource = 0;

        /* ========== channel mask - store data for all 4 channels ========== */
	caen_chmask = (unsigned short*)(vmebase+badr+0x2300);
	*caen_chmask = 0xf;

        /* ========== setup triger type ========== */
	caen_trigtype = (unsigned short*)(vmebase+badr+0x1d00);
	*caen_trigtype = 0x2;

        caen_start = (unsigned short*)(vmebase+badr+0x1700);
	caen_trigrec = (unsigned short*)(vmebase+badr+0x2000);
	caen_data = (unsigned short*)(vmebase+badr+0x0d00);
	caen_softtrig = (unsigned short*)(vmebase+badr+0x1c00);
	caen_interrupt = (unsigned short*)(vmebase+badr+0x8000);

      }
      
    caen_read()
        { int i,kk,k,m,shift;
          unsigned short wd;
	  unsigned short tr;
	  double dshift;
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
	     /* calculate the vernier time shift */
	     dshift = -0.02222*(double)wd2[0] +148.88;
	     shift = shift + (int)dshift;
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
        /* ===========  for(m=0;m<4;m++) =========== */
          for(m=0;m<4;m++)
	   { for(i=0;i<2560;i++)
	       {  kk = 2560+i-endcell;
	         k = kk%2560;
                 k=i-shift;
		 if(k<0) k=k+2560;
		 if(k>2560) k=k-2560;
		 if(k<0) k=k+2560;
		 if(k<0 || k>2559) printf(" error ======== k=%d  i=%d  endcell=%d  kk=%d\n",
		    k,i,endcell,kk); 
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
       /* fprintf(fp,"i=%d ii=%d ix=%d  iy=%d  yfit=%f  cdata=%d\n",
	   i,ii,ix,iy,yfit[ii],cdata[0][i]); */
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
     double chisq1,sdy,dy,t1,t2;
        for(i=0;i<npnts;i++) 
	  { t1 = xk[i] - p[1];
            if(t1<=0.0) t2 = 2.0 * p[2]*p[2];
	    if(t1>0.0) t2 = 2.0 * p[3]*p[3];
            yfit[i] = p[4] + p[0]*exp(-(t1*t1)/t2);
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
