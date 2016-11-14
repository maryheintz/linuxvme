#include <stdio.h>
#include <math.h>
#include "axis.h"
#include "dxy.h"
struct {int mtxt;}axl;
      int ixbs=450,iybs=900,linh=1536,linv=1536,nhtk=10,nvtk=10;       
      int jx[10][10]={450,0,0,0,0,0,0,0,0,0,        
                     600,600,0,0,0,0,0,0,0,0,    
                     750,750,750,0,0,0,0,0,0,0,
                     240,1350,240,1350,0,0,0,0,0,0,    
                     240,1350,240,1350,795,0,0,0,0,0,  
                     240,1350,240,1350,240,1350,0,0,0,0,     
                     400,1350,400,1350,400,1350,875,0,0,0, 
                     400,1350,400,1350,400,1350,400,1350,0,0,      
                     250,950,1650,250,950,1650,250,950,1650,0,  
                     400,1350,400,1350,400,1350,400,1350,400,1350};
      int jy[10][10]={900,0,0,0,0,0,0,0,0,0,        
                     1680,240,0,0,0,0,0,0,0,0,   
                     2130,1130,130,0,0,0,0,0,0,0,        
                     1650,1650,450,450,0,0,0,0,0,0,    
                     2160,2160,1160,1160,162,0,0,0,0,0,    
                     2160,2160,1160,1160,162,162,0,0,0,0,    
                     2390,2390,1640,1640,890,890,130,0,0,0, 
                     2390,2390,1640,1640,890,890,130,130,0,0,        
                     2135,2135,2135,1140,1140,1140,145,145,145,0,  
                     2450,2450,1880,1880,1310,1310,740,740,170,170};
      int nh[10]={1536,1024,768,768,768,768,512,512,448,512};
      int nv[10]={1536,1024,768,768,768,768,512,512,512,448};
      int msize=4;           
      char xtitxy[20][40],ytitxy[20][40],mtitle[80];
      int tsetxy[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
      char text[132];
     double dxyfit(int,int,double);

     dxylbl(n,xstrg,ystrg)
     int n;
     char *xstrg,*ystrg;
      { sprintf(&xtitxy[n][0],"%s",xstrg);
        sprintf(&ytitxy[n][0],"%s",ystrg);
        tsetxy[n]=1;
        return(0);
      }

     dxymlbl(strg)
     char *strg;
      { sprintf(&mtitle[0],"%s",strg);
        axl.mtxt=1;
        return(0);
      }

     dxyacc(k,xx,yy,dy,ik)  
      int k,ik;
      double xx,yy,dy;
      {
      if(k<0 || k>9) goto s90;
      if(dxy.nlogy[k]<0) goto s80;
      if(dxy.nbn==499) goto s50;
      dxy.sum[k]=dxy.sum[k]+yy;        
      dxy.xsum[k]=dxy.xsum[k]+xx*yy;   
      dxy.x2sum[k]=dxy.x2sum[k]+xx*xx*yy;        
      if(xx<dxy.xbot[k]) dxy.sl[k]=dxy.sl[k]+yy;  
      if(xx>dxy.xtop[k]) dxy.sr[k]=dxy.sr[k]+yy;  
      if(xx<dxy.xbot[k] || xx>dxy.xtop[k]) goto s30; 
      dxy.nbn=dxy.nbn+1;     
      dxy.xb[dxy.nbn]=xx;    
      dxy.yb[dxy.nbn]=yy;    
      dxy.dyb[dxy.nbn]=dy;   
      dxy.mrk[dxy.nbn]=ik;   
      dxy.nplt[dxy.nbn]=k;   
  s30:
      return(0);        
  s50:
      if(dxy.iov==0) printf(" too many data points - dxy array overflow\n");  
      dxy.iov=1;
      return(0);        
  s80:
      if(dxy.ibd==0) 
        {printf("dxyacc called with bad plot number %d\n",k);
         printf(" - this message is issued only once/job to save paper\n");}
      dxy.ibd=1;
      return(0);
  s90:
      if(dxy.nlogy[k]==-1) 
        {printf("dxyacc called for plot not set up %d\n",k);
         printf(" - this message is issued only once/job to save paper\n");}
      dxy.nlogy[k]=-2;
      return(0);
  }

  dxydbn(k)    
   int k;
   { double hor,ver,dx,dy,dely,yy,dl,yup,ydn,xx,xxx;
     int i,iyup,iydn,iy,iytop,ixtop,ix,idxy,jdx;
      iytop=axs.iybas+axs.liny;
      ixtop=axs.ixbas+axs.linx;
      hor=(double)axs.linx;      
      ver=(double)axs.liny;      
      dx=axs.xhi-axs.xlo;
      dy=axs.yhi-axs.ylo;
      for(i=0;i<dxy.nbn+1;i++)
        {if(dxy.nplt[i] != k) continue;
         if(axs.ncyc>0) goto s130;        
         dely=dxy.yb[i]-axs.ylo;
         yy=ver*dely/dy;
         dl=ver*dxy.dyb[i]/dy;        
         yup=yy+dl;
         ydn=yy-dl;
         if(ydn>ver) continue;
         if(yup<0.0) continue;
         iyup=(int)(yup+0.5)+axs.iybas;  
         iydn=(int)(ydn+0.5)+axs.iybas;
         iy=(int)(yy+0.5)+axs.iybas;
         goto s160;     
  s130:
         yup=dxy.yb[i]+dxy.dyb[i];
         ydn=dxy.yb[i]-dxy.dyb[i];
         iyup=axs.iybas;
         if(yup>0.0) iyup=(int)(axs.a*log10(yup)+axs.b)+axs.iybas;
         iydn=axs.iybas;
         if(ydn>0.0) iydn=(int)(axs.a*log10(ydn)+axs.b)+axs.iybas;
         if(iyup<axs.iybas) continue;
         if(iydn>iytop) continue;
         iy=(int)(axs.a*log10(dxy.yb[i])+axs.b)+axs.iybas;
  s160:
         if(iyup>iytop) iyup=iytop;
         if(iydn<axs.iybas) iydn=axs.iybas;
         xx=hor*(dxy.xb[i]-axs.xlo)/dx;         
         ix=axs.ixbas+(int)(xx+0.5);   
/*         if(iy.le.axs.iybas.or.iy.gt.iytop) go to 200         */
         if(axs.ncxc<=0) goto s180;
         if(dxy.xb[i]<=0.0) continue;
         xxx=axs.ax*log10(dxy.xb[i])+axs.bx;
         ix=axs.ixbas+(int)xxx;
  s180:
         if(ix<axs.ixbas || ix>ixtop) continue;
         if(iy>=axs.iybas && iy<=iytop) dxymrk(ix,iy,dxy.mrk[i]);
         mova(ix,iyup);
         drwa(ix,iydn);
      } 
      return(0);        
      }

      dxyfdr(k,l)  
      int l,k;
      {
      double dx,dy,hor,ver,xlol,xhil,dxl,xx,x1,xlg,yy,y1,dely;
      int istr,nn,ix,iy,i;
      istr=0;        
      dx=(axs.xhi-axs.xlo)/(double)xyft.ivcts[k];
      dy=axs.yhi-axs.ylo;  
      nn=xyft.ivcts[k]+1; 
      hor=(double)axs.linx;      
      ver=(double)axs.liny;      
      if(axs.ncxc<=0) goto s300;
      xlol=log10(dxy.xbot[k]);
      xhil=xlol+(double)axs.ncxc;
      dxl=(xhil-xlol)/(double)xyft.ivcts[k];
  s300:
      for(i=0;i<nn;i++)
       { xx=(double)i*dx+axs.xlo;
         if(axs.ncxc>0) goto s500;
         x1=hor*(xx-axs.xlo)/(axs.xhi-axs.xlo);
         ix=(int)axs.ixbas+(int)(x1+0.5);   
         goto s600;
  s500:
         xlg=(double)(i-1)*dxl+xlol;
         xx=pow(10.0,xlg);
         if(xx<=0.0) continue;
         ix=(int)(axs.ax*log10(xx)+axs.bx)+axs.ixbas;
/*c     yy equals function value at xx    */
  s600:
         yy=dxyfit(k,l,xx);       
         if(dxy.nlogy[k]>0) goto s700;        
         dely=yy-axs.ylo;         
         y1=ver*dely/dy;         
         iy=(int)(y1+0.5)+(int)axs.iybas;   
         goto s750;     
  s700:
         iy=0;
         if(yy<=0.0) goto s750;
         iy=(int)(axs.a*log10(yy)+axs.b)+(int)axs.iybas;
  s750:
         if(iy<=axs.iybas+axs.liny && iy>=axs.iybas) goto s850;   
         istr=0;        
         continue;
  s850:
         if(istr==0) mova(ix,iy);    
         if(istr!=0) drwa(ix,iy);    
         istr=1;        
        }
      return(0);        
      }

      dxymrk(ix,iy,jk)       
      int ix,iy,jk;
      { int ik,i,j,k,l,k5,m,n;
        ik=jk;
/* if jk > 10 do filled symbol (see 90)  */
        if(jk>=10) ik=jk-10;
        i=axs.isize;
        l=2*axs.isize;
        j=3*axs.isize;
        k=4*axs.isize;
        k5=5*axs.isize;
        n=6*axs.isize;
        m=8*axs.isize;
        if(ik != 0) goto s10;    
/*     box   */
        mova(ix-j,iy+j);
        drwr(n,0);
        drwr(0,-n);         
        drwr(-n,0);
        drwr(0,n);
        goto s90;
   s10:
        if(ik != 1) goto s20;
/*     circle  */
        mova(ix-j,iy-i);
        drwr(0,l);
        drwr(l,l);
        drwr(l,0);
        drwr(l,-l);
        drwr(0,-l);
        drwr(-l,-l);
        drwr(-l,0);
        drwr(-l,l);
        goto s90;
   s20:
        if(ik != 2) goto s30;
/*     up triangle  */
        mova(ix-k,iy-l);
        drwr(k,n);
        drwr(k,-n);
        drwr(-m,0);
        goto s90;
   s30:
        if(ik != 3) goto s40;
/*    +  */
        mova(ix,iy+l);
/*     dont remove the vertical line or zero error bar plots look bad!  */
        drwr(0,-k);
        mova(ix-j,iy);
        drwr(n,0);
        goto s90;
   s40:
        if(ik != 4) goto s50;
/*     x  */
        mova(ix-j,iy-j);
        drwr(n,n);
        mova(ix-j,iy+j);
        drwr(n,-n);
        goto s90;
   s50:
        if(ik != 5) goto s60;
/*     diamond   */
        mova(ix,iy+k);
        drwr(k,-k);
        drwr(-k,-k);
        drwr(-k,k);
        drwr(k,k);
        goto s90;
   s60:
        if(ik != 6) goto s70;
/*     down triangle   */
        mova(ix-k,iy+l);
        drwr(m,0);
        drwr(-k,-n);
        drwr(-k,n);
        goto s90;
   s70:
        if(ik != 7) goto s80;
/*     hour glass   */
        mova(ix-j,iy-j);
        drwr(n,n);
        drwr(-n,0);
        drwr(n,-n);
        drwr(-n,0);
        goto s90;
   s80:
        if(ik != 8) goto s90;
/*     star   */
        mova(ix-k,iy);
        drwr(m,0);
        mova(ix-j,iy+j);
        drwr(n,-n);
        mova(ix,iy+k);
        drwr(0,-m);
        mova(ix-j,iy-j);
        drwr(n,n);
   s90:
        return(0);
      }

      dxymwr(ll,l1,l2)
      int ll,l1,l2;
      { int jsize,l,m,n1,ixtop,iytop,ipl;
        double yl,yh,sumin,ax,bx,xlin,ver;
        if(l1<0 && l1>9) 
         { printf(" dxymwr calling error %d %d %d\n",ll,l1,l2);
           return(0); }
        erasm();
/*      save default mark size  */
        jsize=msize;
/*      for small plots use small mark   */
        if(ll != 1) msize=3;
        for (l=l1;l<l2+1;l++)
         { if(dxy.nlogy[l]<0) continue;
           m=l-l1;
           yl=dxy.ybot[l];
           yh=dxy.ytop[l];
           dxyscl(l);
           sumin=dxy.sum[l]-dxy.sl[l]-dxy.sr[l];
           axs.isize=1;
           axs.xlo=dxy.xbot[l];
           axs.xhi=dxy.xtop[l];
           axs.ylo=dxy.ybot[l];
           axs.yhi=dxy.ytop[l];
           axs.ixbas=jx[ll-1][m];
           axs.iybas=jy[ll-1][m];
           axs.linx=nh[ll-1];
           axs.liny=nv[ll-1];
           axs.nxtk=nhtk;
           axs.nytk=nvtk;
           axs.ncxc=dxy.nlogx[l];
           axs.ncyc=dxy.nlogy[l];
           if(axs.ncyc==0) goto s8;
           axs.yhi=dxy.bscl;
           axs.ylo=axs.yhi/pow(10.0,(double)axs.ncyc);
           ver=(double)axs.liny;
           axs.a=ver/(log10(axs.yhi)-log10(axs.ylo));
           axs.b=-axs.a*log10(axs.ylo);
    s8:
           if(axs.ncxc<=0 || axs.xlo<=0.0) goto s9;
           xlin=(double)axs.linx;
           axs.xhi=axs.xlo*pow(10.0,(double)axs.ncxc);
           axs.ax=xlin/(log10(axs.xhi)-log10(axs.xlo));
           axs.bx=-axs.ax*log10(axs.xlo);
    s9:
           axis(l);
           dxydbn(l);
           if(xyft.nplts[l]<=0) goto s27;
           for(n1=1;n1<xyft.nplts[l]+1;n1++) dxyfdr(l,n1);
   s27:
           dxy.ybot[l]=yl;
           dxy.ytop[l]=yh;
         }
         ipl = 0x20000 + 100*l1 + l2;
	 plotmx(ipl);
/*       restore default mark size  */
         msize=jsize;
         axs.ncxc=0;
         return(0);
      }

      dxyscl(k)
      int k;
      { int i,ifst,npow,n;
        double fmean,sigma,bmax,bmin,botm,dcyc,fmax,fac,sig2,delc;
        fmean=0.0;
        sigma=0.0;
        axs.ncyc=dxy.nlogy[k];
        ifst=0;
        bmax=0.0;
        bmin=0.0;
        if(dxy.nbn<0) goto s220;
        for (i=0;i<dxy.nbn+1;i++)
         { if(dxy.nplt[i] != k) continue;
           if(ifst != 0) goto s110;
           bmax=dxy.yb[i]+dxy.dyb[i];
           bmin=dxy.yb[i]-dxy.dyb[i];
           ifst=1;
  s110:
           axs.yhi=dxy.yb[i]+dxy.dyb[i];
           axs.ylo=dxy.yb[i]-dxy.dyb[i];
           if(axs.yhi>bmax) bmax=axs.yhi;
           if(axs.ylo<bmin) bmin=axs.ylo;
      }
  s220:
        if(bmax==0.0 && bmin==0.0)
          { bmax=1.0;
            bmin=1.0;}
          if(axs.ncyc>0) goto s250;
          if(dxy.ytop[k] != dxy.ybot[k]) goto s300;
          npow=(int)(log10(bmax)-1.0);
          n=(int)(bmax/pow(10.0,(double)npow));
          dxy.ytop[k]=(double)(n+2)*pow(10.0,(double)npow);
          goto s300;
/*     log plot requested  */
  s250:
          botm=bmax/pow(10.0,(double)dxy.nlogy[k]);
          if(botm>bmin) goto s270;
          if(botm<bmin) botm=bmin;
          dcyc=log10(bmax)-log10(botm);
          delc=0.5*((double)dxy.nlogy[k]-dcyc);
          fmax=log10(bmax)+delc;
          dxy.bscl=pow(10.0,fmax);
          goto s290;
  s270:
         fac=1.0+0.1*(double)axs.ncyc;
         if(fac>2.0) fac=2.0;
         dxy.bscl=bmax*fac;
         if(dxy.ytop[k] != dxy.ybot[k]) dxy.bscl=dxy.ytop[k];
  s290:
         dxy.ytop[k]=dxy.bscl;
         dxy.ybot[k]=dxy.bscl/pow(10.0,(double)axs.ncyc);
  s300:
         if(dxy.sum[k]==0.0) goto s375;
         fmean=dxy.xsum[k]/dxy.sum[k];
         sigma=0.0;
         sig2=dxy.x2sum[k]/dxy.sum[k]-fmean*fmean;
         if(sig2>0.0) sigma=sqrt(sig2);
  s375:
        return(0);
        }

      dxyset(k,xhigh,xlow,yhigh,ylow,jlogx,jlogy)
      int k,jlogx,jlogy;
      double xhigh,xlow,yhigh,ylow;
      { static int ient=0;
        if(ient==0) 
         {dxy.nbn=-1;
          ient=1;
	  axl.mtxt=0;
	 }
        if(k<0 || k>9) goto s80;
        dxy.sum[k]=0.0;
        dxy.xsum[k]=0.0;
        dxy.x2sum[k]=0.0;
        dxy.sl[k]=0.0;
        dxy.sr[k]=0.0;
        dxy.nlogx[k]=jlogx;
        dxy.nlogy[k]=jlogy;
        if(yhigh == ylow) goto s50;
        dxy.ytop[k]=yhigh;
        if(ylow>yhigh) dxy.ytop[k]=ylow;
        dxy.ybot[k]=ylow;
        if(yhigh<ylow) dxy.ybot[k]=yhigh;
        if(xhigh == xlow) goto s50;
        dxy.xtop[k]=xhigh;
        if(xlow>xhigh) dxy.xtop[k]=xlow;
        dxy.xbot[k]=xlow;
        if(xhigh<xlow) dxy.xbot[k]=xhigh;
        return(0);
   s50:
        printf("illegal plot limits %f %f\n",xhigh,xlow);
        return(0);
   s80:
        printf("illegal dxyset call - plot # bad %d\n",k);
        return(0);
      }

      dxywrt(k)
      int k;
      { double yl,yh,sumin,xlin,ax,bx,ver;
        int l,nn,kdev,ixx,iyy,kk,ipl;
        if(k<0 || k>9) goto s90;
        if(dxy.nlogy[k]<0) goto s80;
	/* printf("k=%d  xyft.nplts[k]=%d\n",k,xyft.nplts[1]); */
        erasm();
        gidev(&kdev);
        if(kdev == -1) erasm();
        gidev(&kdev);
        if(kdev == 1)
          { sprintf(text,"DXYPLT %d X-%s  Y-%s",k,&xtitxy[k][0],&ytitxy[k][0]);
            if(tsetxy[k]!=0) symb(100,3000,0.0,text);}
        if(kdev == 0 && tsetxy[k]!=0)
          { sprintf(text,"DXYPLT %d",k);
            symb(100,3000,0.0,text);
            sprintf(text,"%s",&xtitxy[k][0]);
            kk=strlen(text);
            ixx=ixbs+linh/2-25*kk/2;
            iyy=iybs-150;
            symb(ixx,iyy,0.0,text);
            sprintf(text,"%s",&ytitxy[k][0]);
            kk=strlen(text);
            ixx=ixbs-300;
            iyy=iybs+linv/2-25*kk/2;
            symb(ixx,iyy,90.0,text);}
        yl=dxy.ybot[k];
        yh=dxy.ytop[k];
        dxyscl(k);
        sumin=dxy.sum[k]-dxy.sl[k]-dxy.sr[k];
        axs.isize=2;
        axs.ixbas=ixbs;
        axs.iybas=iybs;
        axs.linx=linh;
        axs.liny=linv;
        axs.nxtk=nhtk;
        axs.nytk=nvtk;
        axs.xlo=dxy.xbot[k];
        axs.xhi=dxy.xtop[k];
        axs.ylo=dxy.ybot[k];
        axs.yhi=dxy.ytop[k];
        axs.ncyc=dxy.nlogy[k];
        axs.ncxc=dxy.nlogx[k];
        if(axs.ncyc == 0) goto s8;
        ver=(double)axs.liny;
        axs.a=ver/(log10(axs.yhi)-log10(axs.ylo));
        axs.b=-axs.a*log10(axs.ylo);
    s8:
        if(axs.ncxc<=0 || axs.xlo<=0.0) goto s9;
        xlin=(double)axs.linx;
        axs.xhi=axs.xlo*pow(10.0,(double)axs.ncxc);
        axs.ax=xlin/(log10(axs.xhi)-log10(axs.xlo));
        axs.bx=-axs.ax*log10(axs.xlo);
    s9:
        axis(k);
        dxydbn(k);
        if(xyft.nplts[k] == 0) goto s20;
        nn=xyft.nplts[k];
        for (l=1;l<nn+1;l++) dxyfdr(k,l);
   s20:
        dxy.ybot[k]=yl;
        dxy.ytop[k]=yh;
        ipl = 0x20000+k;
	plotmx(ipl);
        axs.ncxc=0;
        return(0);
   s80:
        printf("dxywrt called for a plot not setup %d\n",k);
        return(0);
   s90:
        printf("dxywrt called for an illegal plot # %d\n",k);
        return(0);
      }

      dxyzro()
      { int i;
        dxy.nbn=-1;   
        dxy.iov=0;
        dxy.ibd=0;
        for(i=0;i<10;i++) dxy.nlogy[i]=-1;
        return(0);    
      }
      dxyft_set(int l,int nplt,int ivct)
        {xyft.nplts[l]=nplt;
	 xyft.ivcts[l]=ivct;
	}




