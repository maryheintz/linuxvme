


#include <stdio.h>
#include <math.h>
#include "axis.h"
#include "dot.h"
      FILE *fp_dot,*fp_diag;
      static int bufstat=0;
/*    bufstat=0 nothing done yet 
              1 open for write
              2 final write done
              3 open for read 
              4 error blocked     */
      int ix,iy,ntotal;
      unsigned int itmp;
      static double xlns=1536.0,ylns=1536.0,ax[10],bx[10],ay[10],by[10];
      static int ixbs=450,iybs=900,linh=1536,linv=1536,nhtk=10,nvtk=10;       
      static int jx[10][10]={450,0,0,0,0,0,0,0,0,0,        
                     600,600,0,0,0,0,0,0,0,0,    
                     750,750,750,0,0,0,0,0,0,0,
                     240,1350,240,1350,0,0,0,0,0,0,    
                     240,1350,240,1350,795,0,0,0,0,0,  
                     240,1350,240,1350,240,1350,0,0,0,0,     
                     400,1350,400,1350,400,1350,875,0,0,0, 
                     400,1350,400,1350,400,1350,400,1350,0,0,      
                     250,950,1650,250,950,1650,250,950,1650,0,  
                     400,1350,400,1350,400,1350,400,1350,400,1350};
      static int jy[10][10]={900,0,0,0,0,0,0,0,0,0,        
                     1680,240,0,0,0,0,0,0,0,0,   
                     2130,1130,130,0,0,0,0,0,0,0,        
                     1650,1650,450,450,0,0,0,0,0,0,    
                     2160,2160,1160,1160,162,0,0,0,0,0,    
                     2160,2160,1160,1160,162,162,0,0,0,0,    
                     2390,2390,1640,1640,890,890,130,0,0,0, 
                     2390,2390,1640,1640,890,890,130,130,0,0,        
                     2135,2135,2135,1140,1140,1140,145,145,145,0,  
                     2450,2450,1880,1880,1310,1310,740,740,170,170};
      static int nh[10]={1536,1024,768,768,768,768,512,512,448,512};
      static int nv[10]={1536,1024,768,768,768,768,512,512,512,448};
      double boxs[10][48][48],boxm[10];
      char title[20][80];
      int tset[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
      char text[132];
      dotset(k,xhigh,xlow,yhigh,ylow,jlogx,jlogy)
      int k,jlogx,jlogy;
      double xhigh,xlow,yhigh,ylow;
      { if(dot.nset[k] != 0) return(0);
        boxm[k]=0.0;
        for(ix=0;ix<48;ix++)
         for(iy=0;iy<48;iy++)
           {boxs[k][ix][iy]=0.0;}
        dot.nset[k]=1;
        if(bufstat==0) 
         {sct.nsbf=0;
          sct.nrec=0;
          dot.ibbd=0;
          fp_dot=fopen("dot.scratch","w+");  /* read and write open */
          fp_diag=fopen("dot.diag","w"); 
          bufstat=1;}
        if(k<0 || k>9) goto s80;
        dot.tsum[k]=0.0;
        dot.sum1[k]=0.0;
        dot.sum2[k]=0.0;
        dot.x2sum1[k]=0.0;
        dot.x2sum2[k]=0.0;
        dot.nlogx[k]=jlogx;
        dot.nlogy[k]=jlogy;
        if(yhigh == ylow) goto s50;
        dot.yhi[k]=yhigh;
        if(ylow>yhigh) dot.yhi[k]=ylow;
        dot.ylo[k]=ylow;
        if(yhigh<ylow) dot.ylo[k]=yhigh;
        if(xhigh == xlow) goto s50;
        dot.xhi[k]=xhigh;
        if(xlow>xhigh) dot.xhi[k]=xlow;
        dot.xlo[k]=xlow;
        if(xhigh<xlow) dot.xlo[k]=xhigh;
        if(jlogx>0)
         {xlow=xhigh/pow(10.0,(double)jlogx);
          ax[k]=xlns/(log10(xhigh)-log10(xlow));
          bx[k]=-ax[k]*log10(xlow);
          dot.xlo[k]=xlow;
/*          printf("k=%d xhigh=%g xlow=%g jlogx=%d\n",k,xhigh,xlow,jlogx);
          printf("  ax=%g bx=%g\n",
               ax[k],bx[k]); */
         }
        if(jlogy>0)
         {ylow=yhigh/pow(10.0,(double)jlogy);
          ay[k]=ylns/(log10(yhigh)-log10(ylow));
          by[k]=-ay[k]*log10(ylow);
          dot.ylo[k]=ylow;
         }
        return(0);
   s50:
        printf("illegal plot limits %f %f\n",xhigh,xlow);
        return(0);
   s80:
        printf("illegal  call - plot # bad %d\n",k);
        return(0);
      }

     dotacc(k,xx,yy)  
      double xx,yy;
      {
      double xline,yline;
      int nwds,ierr,kx,ky;
      if(bufstat == 4) return(0);
      if(bufstat == 3)
       {bufstat=4;
        printf("******* error dotacc called after scratch file closed ******\n");
        return(0);}
      if(k<0 || k>9) goto s90;
      if(dot.nset[k] != 1) goto s80;
      dot.sum1[k]=dot.sum1[k]+yy;        
      dot.sum2[k]=dot.sum2[k]+xx;        
      dot.tsum[k]=dot.tsum[k]+1.0;        
      dot.x2sum1[k]=dot.x2sum1[k]+yy*yy;   
      dot.x2sum2[k]=dot.x2sum2[k]+xx*xx;
      if(dot.nlogx[k]>0) goto s10;        
      xline=xlns*(xx-dot.xlo[k])/(dot.xhi[k]-dot.xlo[k]);
      ix=(int)(xline+1.0);
      goto s20;
s10:
      if(xx<=0.0) goto s70;
      ix=(int)(ax[k]*log10(xx)+bx[k]);
s20:
      if(dot.nlogy[k]>0.0) goto s30;
      yline=ylns*(yy-dot.ylo[k])/(dot.yhi[k]-dot.ylo[k]);
      iy=(int)(yline+1.0);      
      goto s40;
s30:
      if(yy<=0.0) goto s70;
      iy=(int)(ay[k]*log10(yy)+by[k]);
s40:
      if(ix<1 || ix>1536) goto s70;
      if(iy<1 || iy>1536) goto s70;      
      dot.nin[k]=dot.nin[k]+1;
      kx=ix/32;
      ky=iy/32;
      boxs[k][kx][ky]=boxs[k][kx][ky]+1.0;
      if(boxs[k][kx][ky]>boxm[k]) boxm[k]=boxs[k][kx][ky];
      return(0);
s70:
      dot.nout[k]=dot.nout[k]+1;
      return(0);
s80:
      if(dot.nset[k] == 0) 
       {printf("illegal dotacc call - plot %d not set up\n",k);}
      dot.nset[k]=-1;
      return(0);
s90:
      if(dot.ibbd == 0) printf("illegal call to dotacc %d\n",k);
      dot.ibbd=1;
      return(0);
     }


      dotwrt(k)
      int k;
      { double mean1,mean2,sig1,sig2,s2;
        int l,nn,isg;
        isg=0;
        if(k<0)
         {isg=1;
          k=-k;}
        if(k<0 || k>9) goto s90;
        if(dot.nset[k] != 1) goto s80;
        if(dot.nin[k]<=0) goto s70;
        erasm();
        if(tset[k]!=0) symb(100,3000,&title[k][0]);
        axs.isize=2;
        axs.ixbas=ixbs;
        axs.iybas=iybs;
        axs.linx=linh;
        axs.liny=linv;
        axs.nxtk=nhtk;
        axs.nytk=nvtk;
        axs.xlo=dot.xlo[k];
        axs.xhi=dot.xhi[k];
        axs.ylo=dot.ylo[k];
        axs.yhi=dot.yhi[k];
        axs.ncyc=dot.nlogy[k];
        axs.ncxc=dot.nlogx[k];
        if(axs.ncyc == 0) goto s8;
        axs.a=ay[k];
        axs.b=by[k];
    s8:
        if(axs.ncxc<=0 || axs.xlo<=0.0) goto s9;
        axs.ax=ax[k];
        axs.bx=bx[k];
    s9:
/*      draw the axis etc.   */
        axis(k);
/*      draw in the points   */
        dotdpt(k);
/*      put means and sigmas on the plot  */
        if(isg == 1)
         {sprintf(text,"dotplot %d    %d in    %d out",
                k,dot.nin[k],dot.nout[k]);
          symb(axs.ixbas,axs.iybas-250,text);
          mean1=0.0;
          mean2=0.0;
          sig1=0.0;
          sig2=0.0;
          if(dot.tsum[k]>0.0) 
           {mean1=dot.sum1[k]/dot.tsum[k];
            s2=dot.x2sum1[k]/dot.tsum[k]-mean1*mean1;
            if(s2>0.0) sig1=sqrt(s2);
            mean2=dot.sum2[k]/dot.tsum[k];
            s2=dot.x2sum2[k]/dot.tsum[k]-mean2*mean2;
            if(s2>0.0) sig2=sqrt(s2);}
          sprintf(text,"  y axis mean=%f sigma=%f",mean1,sig1);
          symb(axs.ixbas,axs.iybas-350,text);
          sprintf(text,"  x axis mean=%f sigma=%f",mean2,sig2);
          symb(axs.ixbas,axs.iybas-450,text);
         }
/*      finish up            */
        plotmx();
        axs.ncxc=0;
        return(0);
   s70:
        printf("dotplot %d    %d in    %d out\n",k,dot.nin[k],dot.nout[k]);
        mean1=0.0;
        mean2=0.0;
        sig1=0.0;
        sig2=0.0;
        if(dot.tsum[k]>0.0) 
         {mean1=dot.sum1[k]/dot.tsum[k];
          s2=dot.x2sum1[k]/dot.tsum[k]-mean1*mean1;
          if(s2>0.0) sig1=sqrt(s2);
          mean2=dot.sum2[k]/dot.tsum[k];
          s2=dot.x2sum2[k]/dot.tsum[k]-mean2*mean2;
          if(s2>0.0) sig2=sqrt(s2);}
        printf("  y axis mean=%f sigma=%f\n",mean1,sig1);
        printf("  x axis mean=%f sigma=%f\n",mean2,sig2);
        return(0);
   s80:
        printf("dotwrt called for a plot not setup %d\n",k);
        return(0);
   s90:
        printf("dotwrt called for an illegal plot # %d\n",k);
        return(0);
      }

     dotlbl(n,strg)
     int n;
     char *strg;
      { sprintf(&title[n][0],"%s",strg);
        tset[n]=1;
        return(0);
      }

  dotdpt(k)    
   int k;
   { double size,fac;
     int ixx,iyy;
      for(ixx=0;ixx<48;ixx++)
       for(iyy=0;iyy<48;iyy++)
         {if(boxs[k][ixx][iyy] != 0.0) 
           {fac=25.0/boxm[k];
            size=fac*boxs[k][ixx][iyy];
            drwbox(size,ixx,iyy);}
         }
      }
      drwbox(size,kx,ky)
      double size;
      int kx,ky;
      {int isize,hsize;
       ix=32*kx+axs.ixbas;
       iy=32*ky+axs.iybas;
       isize=(int)size;
       hsize=(int)(0.5*size);
       if(size>0.0 && size<2.0) pnta(ix,iy);
       mova(ix-hsize,iy-hsize);
       drwr(isize,0);
       drwr(0,isize);
       drwr(-isize,0);
       drwr(0,-isize);
       return(0);
     }
      dotmwr(ll,l1,l2)
      int ll,l1,l2;
      { int l,m;
        if(l1<0 && l1>9) 
         { printf(" dotmwr calling error %d %d %d\n",ll,l1,l2);
           return(0); }
        erasm();
        for (l=l1;l<l2+1;l++)
         { if(dot.nset[l] != 1) continue;
           m=l-l1;
           axs.isize=1;
           axs.xlo=dot.xlo[l];
           axs.xhi=dot.xhi[l];
           axs.ylo=dot.ylo[l];
           axs.yhi=dot.yhi[l];
           axs.ixbas=jx[ll-1][m];
           axs.iybas=jy[ll-1][m];
           axs.linx=nh[ll-1];
           axs.liny=nv[ll-1];
           axs.nxtk=nhtk;
           axs.nytk=nvtk;
           axs.ncxc=dot.nlogx[l];
           axs.ncyc=dot.nlogy[l];
           if(axs.ncyc == 0) goto s8;
           axs.a=ay[l];
           axs.b=by[l];
    s8:
           if(axs.ncxc<=0 || axs.xlo<=0.0) goto s9;
           axs.ax=ax[l];
           axs.bx=bx[l];
    s9:
           axis(l);
           dotdpt(l);
         }
         plotmx();
/*       restore default mark size  */
         axs.ncxc=0;
         return(0);
      }
      dotzro()
      {int i;
       if(bufstat != 0) i=fclose(fp_dot);
       bufstat=0;
       for(i=0;i<20;i++) tset[i]=0;
       return(0);}

