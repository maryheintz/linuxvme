#include <math.h>
#include <time.h>
#include "vme.h"
unsigned long vmebase,badr;
unsigned int sw_base,vme_base,pio_base;
unsigned short *caen_data,*caen_prelsb,*caen_premsb,*caen_postlsb,*caen_postmsb,*caen_trigrec,
 *caen_softtrig,*caen_trigtype,*caen_trigsource,*caen_reset,*caen_start,*caen_interrupt,
 *caen_chmask;
int dum1[100],ksave,msave,maxsave,dum2[100];
unsigned short cdata[4][3000],raw[4][3000],ped[4][3000],posttrig,endcell,trigrec; 
unsigned short wd1[4],wd2[4],wd3[4],wd;
char string[80];
main(argc,argv,envs)
int argc;
char *argv[];
char *envs[];
  { int icap,n1,m,event,nn;
    int dac,alarm,vmin,vmax;
    unsigned short i,l;
    int zone,sector,card,time1,time2,time3,time4;
    double capacitor,dacval;
    double charge,charge_true;
    unsigned short cadr,tc,tf;
    unsigned short sval,dval;
    dotset(0,1500.1250,0.0,150.0,0.0,0,0);
    dotset(1,80.0,40.0,2300.0,1300.0,0,0);
    hstset(0,2300.0,1300.0,100,0);
    hstset(1,1400.0,1300.0,100,0);
    hstset(2,2200.0,2100.0,100,0);
   vmebase = vme_a24_base();
   vme_base = (unsigned int)vmebase;
   pio_base = vmebase + 0x570000;
   sw_base = vme_base + 0xaa0000;
   set_3in1_base(vme_base);
   badr = 0x460000;

    zone = 1;
    sector = 1;
    time1 = 0;
    time2 = 0;
    time3 = 0;
    time4 = 0;
    tc=200;
    tf=0;
    tim1_set(tc);
    tim2_set(tf);
    set_drawer_timer(zone,sector,1,time1);
    set_drawer_timer(zone,sector,2,time2);
    set_drawer_timer(zone,sector,3,time3);
    set_drawer_timer(zone,sector,4,time4);
    caen_setup();
    usleep(2);  
    printf(" returned from caen_setup\n");
    sidev(1);  /* force plot to screen without asking*/
/* ========== get the pedestals ================ */
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
    icap = 0;
    charge = 500.0;
    capacitor = 100.0;
    if(icap==1) capacitor = 5.2;
    printf(" starting event loop\n");
         dacval = (charge * 1023.0 ) / (2.0 * 4.096* capacitor);
         if(dacval > 1023.0) dacval = 1023.0;
         dac = (int) dacval;
        /* charge = ( 2.0 * 4.096 * capacitor *(double)dac ) / 1023.0;
         printf(" charge=%f  dacval=%f  dac=%x(hex)  cap=%f\n",
           charge,dacval,dac,capacitor); */
    multi_low();
    for(i=0;i<49;i++)
       { card = i;
         l = (zone<<12) | (sector<<6) | card;
         cadr = (unsigned short)l;
         card_sel(cadr);
         sval = (unsigned short)0x555;
         wrt_shift(13,sval);
         read_shift(&dval);
         if(sval != (dval&0x1fff) ) continue;
         sval = (unsigned short)0xaaa;
         wrt_shift(13,sval);
         read_shift(&dval);
         if(sval != (dval&0x1fff) ) continue;
         printf("card %d found\n",card);
         send13_3in1(2,0);   /* s1=s2=s3=s4=0 */
         send4_3in1(1,0);   /* itr=0 */
         send4_3in1(0,1);   /* ire=0 */
         send4_3in1(3,0);   /* mse=0 */
         send4_3in1(4,0);   /* small C disable */
         send4_3in1(5,0);   /* large C disable */
         send4_3in1(7,1);   /* trig enable */
         send13_3in1(6,dac);
         if(i==38 || i==40)
           { if(icap==1) send4_3in1(4,1);   /* small C enable */
             if(icap==0) send4_3in1(5,1);   /* large C enable */
           }
      }
	*caen_trigtype = 0x2;
	event=0;
 loop:         
       *caen_reset=0;
       usleep(1);
       *caen_start=1;
	rtime();
        tp_high();
        setime();
       usleep(1);
       /* *caen_start=1; */
       /* usleep(1); */
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
       /* printf(" m=%d  k=%d  max=%d  %d\n",msave,ksave,maxsave,cdata[0][1600]); */
       /* read the orbit count */
       if(fpsw(1)==1)  
	{ for(msave=0;msave<4;msave++)
	   for(n1=0;n1<2500;n1=n1+250)
	     { if(icap == 0) sprintf(string,"LOW GAIN  input=%d pk=%d 1st bin=%d",msave,ksave,n1);
               if(icap == 1) sprintf(string,"HIGH GAIN  input=%d  1st bin=%d",msave,n1);
               onedisp(250,string,n1,msave);
	     }
        }  
       if(fpsw(0)==1)
        { n1=ksave/250;
	  n1=n1*250;
          if(icap == 0) sprintf(string,"LOW  peak bin=%d  amp=%d trec=%d  bins=%d-%d  vnr=%d",
	     ksave,cdata[msave][ksave],trigrec,n1,n1+250,wd2[0]);
          if(icap == 1) sprintf(string,"HIGH  peak bin=%d  amp=%d trec=%d  bins=%d-%d  vnr=%d",
	     ksave,cdata[msave][ksave],trigrec,n1,n1+250,wd2[0]);
          onedisp(250,string,n1,msave);
        }  
	usleep(1);
	if(fpsw(5)==1) dotwrt(0);
	if(fpsw(5)==1) dotwrt(1);
	if(fpsw(4)==1) hstwrt(0);
	if(fpsw(4)==1) hstwrt(1);
	if(fpsw(4)==1) hstwrt(2);
        event++;
	wtstreg(event);
	if(event==1) 
	  { vmin=wd2[0];
	    vmax = wd2[0];
	  }
	if(wd2[0]<vmin) vmin = wd2[0];
	if(wd2[0]>vmax) vmax = wd2[0];
	if(fpsw(2)==1) printf("vmin=%d  vmax=%d\n",vmin,vmax);
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
	  msave = 0;
	  ksave = 0;
	  maxsave = 0;
/* ==============          for(m=0;m<4;m++) =============== */
          for(m=0;m<4;m++)
	   { for(i=0;i<2560;i++)
	       {  kk = 2560+i-endcell;
	         k = kk%2560;
                 k=i-shift;
		 if(k<0) k=k+2560;
		 if(k>2560) k=k-2560;
	         cdata[m][k]=raw[m][i]-ped[m][i];  
		 if(cdata[m][k]>maxsave)
		  { msave = m;
		    ksave = k;
		    maxsave = cdata[m][k];
		    /* printf("m=%d  k=%d  max=%d\n",msave,ksave,maxsave); */
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
