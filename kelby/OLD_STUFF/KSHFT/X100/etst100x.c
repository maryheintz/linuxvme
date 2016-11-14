#include <stdio.h>
#include <math.h>
double area[175]=
  {   0.000000,   0.000000,   0.000000,   0.100000,   1.700000,
      8.500000,  27.850000,  63.600000, 119.900000, 193.400000,
    284.350000, 391.900000 ,514.750000, 649.550000, 795.500000,
    958.600000,1124.600000,1299.200000,1477.000000,1662.250000,
   1850.700000,2040.400000,2228.250000,2415.400000,2599.350000,
   2786.900000,2965.450000,3142.850000,3318.900000,3493.000000,
   3658.750000,3820.250000,3974.250000,4119.300000,4264.200000,
   4407.000000,4541.000000,4676.100000,4801.850000,4923.650000,
   5039.000000,5146.450000,5254.350000,5354.400000,5451.500000,
   5548.150000,5633.550000,5716.650000,5796.700000,5871.250000,
   5945.150000,6014.500000,6078.150000,6144.550000,6205.850000,
   6260.850000,6317.150000,6365.100000,6411.800000,6455.400000,
   6497.750000,6538.450000,6578.050000,6612.000000,6646.500000,
   6677.800000,6707.200000,6736.200000,6762.100000,6788.500000,
   6813.100000,6835.050000,6856.800000,6875.150000,6895.300000,
   6912.550000,6928.100000,6943.800000,6958.550000,6972.100000,
   6983.950000,6996.500000,7008.200000,7018.050000,7028.150000,
   7037.950000,7046.150000,7053.150000,7061.250000,7069.200000,
   7076.400000,7083.100000,7088.700000,7094.000000,7099.350000,
   7105.000000,7110.050000,7113.950000,7117.400000,7120.950000,
   7124.150000,7127.200000,7130.350000,7132.950000,7135.100000,
   7137.500000,7140.350000,7143.000000,7144.850000,7146.850000,
   7148.800000,7150.050000,7151.900000,7153.900000,7155.050000,
   7156.100000,7156.950000,7157.800000,7158.750000,7159.650000,
   7160.300000,7161.000000,7161.500000,7161.950000,7162.500000,
   7163.100000,7163.650000,7164.150000,7164.450000,7164.900000,
   7165.400000,7165.800000,7165.950000,7166.100000,7166.400000,
   7166.700000,7166.800000,7167.050000,7167.200000,7167.450000,
   7167.600000,7167.900000,7167.950000,7168.050000,7168.200000,
   7168.450000,7168.450000,7168.500000,7168.600000,7168.750000,
   7168.900000,7168.900000,7168.900000,7168.950000,7169.000000,
   7169.050000,7169.150000,7169.250000,7169.300000,7169.400000,
   7169.550000,7169.550000,7169.600000,7169.600000,7169.650000,
   7169.650000,7169.650000,7169.700000,7169.700000,7169.700000,
   7169.700000,7169.700000,7169.700000,7169.700000,7169.700000};
double xbins[175];
unsigned int sw_base,vme_base;
double a1,b1,a2,b2,a3,b3;
main()
{ int i,l,k,ibin;
  FILE *fp;
  double E,xbin,ybin;
  unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
  init_by_array(init, length); 
  vme_base = vme_a24_base();
  sw_base = vme_base + 0xaa0000;
  hstset(0,5.0,0.0,200,-4);  
  hstset(1,10.0,0.0,200,-4);  
loop:
     pick(&E);
     hstacc(0,E,1.0);
     hstacc(1,E,1.0);
  if(sw(7)==1) 
   { sidev(1);
     hstwrt(0);
     hstwrt(1);
     sidev(2);
     hstwrt(0);
     hstwrt(1);
     fp=fopen("bins.txt","w");
     for(k=0;k<200;k++)
      { hstbin(1,k,&xbin,&ybin);
        fprintf(fp,"%d %f %f\n",k,xbin,ybin);
      }
     for(k=0;k<200;k++)
      { hstbin(1,k,&xbin,&ybin);
        fprintf(fp,"%f ",ybin);
      }
     l=fclose(fp);
   }
  goto loop; 
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
pick(double *E)
{ int i,k;
  static int ent=0;
  double r,a;
  FILE *fp;
  double sum;
  sum=0.0;
  if(ent==0)
   { for(i=0;i<175;i++) xbins[i]=0.725 + (double)i * 0.05;
     ent=1;
   }
   rmt(&r);
   a = 7169.7*r;
   k=0;
   for(i=0;i<175;i++)
    { if(area[i]<a) continue;
      *E=xbins[i];
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
       
