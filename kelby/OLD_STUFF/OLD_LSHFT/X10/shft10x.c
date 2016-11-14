#include <stdio.h>
#include <math.h>
double area[175]=
  {   274.450000,59342.150000,149653.000000,175683.650000,188821.650000,
   198516.150000,206472.250000,213097.000000,218608.900000,223203.650000,
   227011.250000,230187.500000,232841.450000,235041.150000,236859.400000,
   238364.250000,239657.600000,240801.300000,241820.050000,242720.900000,
   243525.050000,244247.700000,244906.450000,245487.050000,246011.650000,
   246483.200000,246913.450000,247300.350000,247651.450000,247965.750000,
   248250.100000,248501.250000,248731.450000,248942.250000,249132.200000,
   249304.550000,249452.300000,249593.950000,249719.050000,249826.250000,
   249926.200000,250014.100000,250095.650000,250168.150000,250238.000000,
   250297.900000,250350.650000,250398.000000,250444.200000,250486.150000,
   250523.150000,250554.900000,250583.700000,250611.850000,250636.900000,
   250657.500000,250677.500000,250694.450000,250709.850000,250724.350000,
   250737.450000,250749.350000,250761.500000,250769.850000,250778.600000,
   250786.400000,250792.900000,250799.950000,250806.550000,250812.700000,
   250817.700000,250822.550000,250826.750000,250830.250000,250832.750000,
   250835.950000,250838.800000,250840.900000,250842.750000,250844.800000,
   250846.200000,250847.600000,250849.400000,250850.200000,250851.000000,
   250851.950000,250852.900000,250853.550000,250854.300000,250854.700000,
   250855.500000,250856.000000,250856.650000,250857.100000,250857.300000,
   250857.550000,250858.100000,250858.450000,250858.600000,250858.950000,
   250859.250000,250859.450000,250859.550000,250859.700000,250859.750000,
   250859.800000,250859.900000,250860.000000,250860.100000,250860.200000,
   250860.250000,250860.300000,250860.300000,250860.450000,250860.500000,
   250860.550000,250860.600000,250860.650000,250860.700000,250860.750000,
   250860.800000,250860.900000,250860.900000,250860.900000,250860.950000,
   250860.950000,250861.000000,250861.000000,250861.000000,250861.000000,
   250861.000000,250861.050000,250861.050000,250861.050000,250861.050000,
   250861.050000,250861.050000,250861.050000,250861.050000,250861.050000,
   250861.050000,250861.050000,250861.050000,250861.050000,250861.050000,
   250861.050000,250861.050000,250861.050000,250861.050000,250861.050000,
   250861.050000,250861.050000,250861.050000,250861.050000,250861.050000,
   250861.050000,250861.050000,250861.050000,250861.050000,250861.050000,
   250861.050000,250861.050000,250861.050000,250861.050000,250861.050000,
   250861.050000,250861.050000,250861.050000,250861.050000,250861.050000,
   250861.050000,250861.050000,250861.050000,250861.100000,250861.100000};
double xbin[175];
unsigned int sw_base,vme_base;
int ht[1000];
double aht1[1000],aht2[1000],bin[100];
double prob=0.01,offset=50.0,amp;
double a1,b1,a2,b2,a3,b3;
int ntotal;
main()
{ int i,l,k,ibin;
  double area,x,c1,c2,t1,t2,test,dx,pedmin,pedsum,pedavg;
  unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
  init_by_array(init, length); 
  vme_base = vme_a24_base();
  sw_base = vme_base + 0xaa0000;
  set_dist();
  x = 0.0;
  c1=(x*x)/762.84;
  t1 = 100.0*exp(-c1);
  printf("g0=%f\n",t1);
  x = 25.0;
  c1=(x*x)/762.84;
  t1 = 100.0*exp(-c1);
  printf("g25=%f\n",t1);
  x = -25.0;
  c1=(x*x)/762.84;
  t1 = 100.0*exp(-c1);
  printf("gm25=%f\n",t1);
  hstset(0,2.5,0.0,200,-7);  
  hstset(1,10.0,0.0,200,-7);  
  hstset(2,100.0,0.0,200,0);  
  hstset(3,100.0,0.0,200,-3);  
  hstset(4,100.0,-100.0,200,0);  
  hstset(5,100.0,-100.0,200,-3);  
  hstset(6,100.0,0.0,200,0);  
  ntotal=0;
again:
        /*========== fill array ================== */
        for(i=0;i<710;i++) hit(i);
        /* ============= now get sums for bit 599 =====*/
       pedmin=1000.0;
       for(ibin=0;ibin<100;ibin++)
        { area=offset;
          i=600+ibin;
          for(k=0;k<605;k++)
            { l=i-599+k;
              x = 25.0*(double)(k-599);
              if(ht[l]>0)
                { c1=(x*x)/762.84;
                  c2=(x*x)/2000000.0;
                  t1 = aht1[l]*exp(-c1);
	          t2 = aht2[l]*exp(-c2);
                  area = area + t1;
		  if(x<0.0) area = area - t2;
		  dx = x;
		  if(dx<0) dx=-dx;
                  if(dx<26.0 && sw(6)==1) 
		    printf("i=%d  k=%d  l=%d  x=%f  c1=%f  c2=%f  t1=%f  t2=%f  area=%f\n",
	             i,k,l,x,c1,c2,t1,t2,area);
		  if(area<pedmin) pedmin=area;
                }
           }
        bin[ibin]=area;
	pedsum=pedsum+area;
       }
        pedavg=pedsum/100.0;
	hstacc(6,pedavg,1.0);
	hstacc(2,pedmin,1.0);
	hstacc(3,pedmin,1.0);
	hstacc(4,pedmin,1.0);
	hstacc(5,pedmin,1.0);
	hstacc(6,pedavg,1.0);
  k=ntotal/1000;
  if(1000*k==ntotal) printf("ntotal=%d\n",ntotal);
  ntotal++;
  if(sw(8) == 1) 
   { sidev(1);
     evdisp();
     sidev(2);
     evdisp();}
  if(sw(7)==1 || ntotal==5000000) 
   { printf(" hit enter for plots\n");
     sidev(1);
     hstwrt(0);
     hstwrt(1);
     hstwrt(2);
     hstwrt(3);
     hstwrt(4);
     hstwrt(5);
     hstwrt(6);
     sidev(2);
     hstwrt(0);
     hstwrt(1);    
     hstwrt(2);
     hstwrt(3);
     hstwrt(4);
     hstwrt(5);
     hstwrt(6);
      if(ntotal==5000000) exit(0);
     printf(" continue\n");
   }
   
  goto again; 
}
  hit(int i)
   { double r,E;
     int k;
     pick(&E);
     hstacc(0,E,1.0);
     hstacc(1,E,1.0);
     amp = 36.825*E;  /* higain - convert E(GeV) to counts */ 
/*     printf(" in hit E=%f  amp=%f\n",E,amp);  */
     if(E<0.02)
      {ht[i] = 0;
       return(0);
      }
     ht[i]=1;
     aht1[i]=amp;
     aht2[i]=0.007812*amp;
     return(0);
   }
sw(i)
int i;
{ int k,l,shift;
  unsigned short *adr;
  (unsigned short *)adr = (unsigned short *)(sw_base + 0x0c);
  k = *adr;
  shift = i-1;
  l = (k>>shift) & 1;
  return(l);
}
set_dist()
 { /* -------------- zone 1  0.02<E<0.12 -------------- */
   b1 = -log(0.07)/0.12;
   a1 = 100000.0;
   printf("a1=%f  b1=%f\n",a1,b1);
          /* y = a1*pow(2.71828183,-b1*E) */
   /* -------------- zone 2  0.12<E<0.7 ------------- */ 
   b2 = (log(70.0)-log(5.0))/0.58;
   a2 = 500.0*pow(2.71828183,0.7*b2);
   printf("a2=%f  b2=%f\n",a2,b2);
          /* y = a2*pow(2.71828183,-b2*E) */
   /* -------------- zone 3  E<0.7 ------------------ */ 
   b3 = log(500.0)/2.8;
   a3 = pow(2.71828183,3.5*b3);
   printf("a3=%f  b3=%f\n",a3,b3);
          /* y = a3*pow(2.71828183,-b3*E) */
   /* -------------- zone 2  0.0<E<0.01------------- */ 
     /* lowest bin y = pow(10,7.0)  */

   return(0);
 }
pick(double *E)
{ int i,k;
  static int ent=0;
  double r,a,dx,da;
  FILE *fp;
  double sum;
  sum=0.0;
  if(ent==0)
   { for(i=0;i<175;i++) xbin[i]=0.025 + (double)i * 0.05;
     ent=1;
   }
   rmt(&r);
   a = 250861.1*r;
   k=0;
   for(i=0;i<175;i++)
    { if(area[i]<a) continue;
      dx=xbin[i]-xbin[i-1];
      da=area[i]-area[i-1];
      *E=xbin[i-1]+(a-area[i-1])*dx/da;
      return(0);
    }
 }
    double hstfit(i,l,x)
    int i,l;
    double x;
    {double y;
     y=90.0*exp(-0.1*x);
     return(y); 
    }  
   evdisp()  
 { int max,mmax,idx,idy,ny,linx,ixbas,iybas,ix,iy,iylst,kx,ky,i,k,ii,iii;
   int idel,kja;
   double dely,y,dy;
   char str[5];
   erasm();
/* ======================= first channel plot kk1 ================== */    
   max = 0;
   for(iii=1; iii<100; iii++) {
   if (max < bin[iii]) max = bin[iii];
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
   idx = 1600/100;
   linx = idx*100;
   ixbas = 100+(2200-linx)/2;
   dy = 1200.0/(double)mmax;
   iybas = 750;
   mova(ixbas,iybas);
   drwr(linx,0);
   mova(ixbas,iybas);
   iylst = 0;
   for(i=1;i<=100;i++)
    { ii = bin[i];
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
    idel=25;
    for(i=0;i<100+2;i=i+idel)
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
    plotmx(ntotal);
 }

     
      
