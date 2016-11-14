#include <stdio.h>
#include <math.h>
#include "axis.h"
      int ixbsh=450,iybsh=900,linhh=1536,linvh=1536,nhtkh=10,nvtkh=10;       
      int jxh[10][10]={450,0,0,0,0,0,0,0,0,0,        
                     600,600,0,0,0,0,0,0,0,0,    
                     750,750,750,0,0,0,0,0,0,0,
                     240,1350,240,1350,0,0,0,0,0,0,    
                     240,1350,240,1350,795,0,0,0,0,0,  
                     240,1350,240,1350,240,1350,0,0,0,0,     
                     400,1350,400,1350,400,1350,875,0,0,0, 
                     400,1350,400,1350,400,1350,400,1350,0,0,      
                     250,950,1650,250,950,1650,250,950,1650,0,  
                     400,1350,400,1350,400,1350,400,1350,400,1350};
      int jyh[10][10]={900,0,0,0,0,0,0,0,0,0,        
                     1680,240,0,0,0,0,0,0,0,0,   
                     2130,1130,130,0,0,0,0,0,0,0,        
                     1650,1650,450,450,0,0,0,0,0,0,    
                     2160,2160,1160,1160,162,0,0,0,0,0,    
                     2160,2160,1160,1160,162,162,0,0,0,0,    
                     2390,2390,1640,1640,890,890,130,0,0,0, 
                     2390,2390,1640,1640,890,890,130,130,0,0,        
                     2135,2135,2135,1140,1140,1140,145,145,145,0,  
                     2450,2450,1880,1880,1310,1310,740,740,170,170};
      int nhh[10]={1536,1024,768,768,768,768,512,512,448,512};
      int nvh[10]={1536,1024,768,768,768,768,512,512,512,448};



      struct {int legal;
               int itop[50];
               int ibot[50];
               int idef[50];
               int jtop[50];
               int ntot;
               int nbn;
               double xint[50];
               double xbot[50];
               double xsum[50];
               double x2sum[50];
               double bcnt[2600];}histo;
      struct {int nplts[50];
               int ivcts[50];}hft;
      int lsigma,ldigit;
      int ixtop,iytop;
      int ib,it;
      double sumin,bscl,sumh,fmean,sigmah;
      int lsg=0,ldg=0;
      char htext[132];
      char htitle[20][80];
      int htset[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
     hstmod(l1,l2)
     int l1,l2;
      { lsg=l1;
        ldg=l2;
        return(0);
      }
     hstlbl(n,text)
     int n;
     char *text;
      { sprintf(&htitle[n][0],"%s",text);
        htset[n]=1;
        return(0);
      }
      hstacc(k,x,weight)
      int k;
      double x,weight;
      {
        int ix;
        double dx,bin,del;
        if(k<0 || k>49) goto s90;
        if(histo.idef[k]!=1) goto s80;
        histo.xsum[k]=histo.xsum[k]+x*weight;
        histo.x2sum[k]=histo.x2sum[k]+pow(x,2.0)*weight;     
/*c     find bin      */
        dx=x-histo.xbot[k];  
        bin=(double)(histo.itop[k]-histo.ibot[k]-1);
        del=((bin*dx)/histo.xint[k])+1.0;         
/*c     underflow in bin histo.ibot(k)-1        */
        if(del<0.0) del=0.0;      
/*c     overflow in bin histo.itop[k]+1         */
        if(del>(bin+1.0)) del=bin+1.0;        
/*c     make sure point is within bounds  */
        ix=histo.ibot[k]+(int)del;
/*c     increment counter for this bin    */
        histo.bcnt[ix]=histo.bcnt[ix]+weight;      
        return(0);        
  s80:
        if(histo.idef[k]<0) return(0);
        histo.idef[k]=-1;
        printf("hstacc called for plot %d which was not set up\n",k);
        return(0);
  s90:
        if(histo.legal!=0) return(0);
        histo.legal=1;
        printf("hstacc called with a plot number out of the legal range %d\n",
         k);
        return(0);
      }
      hstbin(nplot,nbin,bcntr,val)     
      int nplot,nbin;
      double *bcntr,*val;
      {
        int ix,nbins;
        double dx;
        nbins=histo.itop[nplot]-histo.ibot[nplot]-1;   
        dx=histo.xint[nplot]/(double)nbins;    
        *bcntr=((double)nbin-0.5)*dx+histo.xbot[nplot];   
        ix=histo.ibot[nplot]+nbin;     
        *val=histo.bcnt[ix];
        return(0);        
      }
      hstdbn(ib_bn,it_bn)
      int ib_bn,it_bn;
        {
        int lwl,ifd,iymax,iylst,i,iy,ixnew,ixmax,nbins,l,iydel;
        double dx,dy,xfac,dely,yy,xnew,ver,hor;
        ver=(double)axs.liny;      
        hor=(double)axs.linx;
        ixtop=axs.ixbas+axs.linx;
        iytop=axs.iybas+axs.liny;
        nbins=it_bn-ib_bn+1; 
/*      lwl=(lwidth-1)/2         */
        lwl=1;
/*     draw bins  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$        */
        dx=(axs.xhi-axs.xlo)/(double)nbins;
        dy=axs.yhi-axs.ylo;
        xfac=(double)axs.linx/(double)nbins;
        hstfill(0);
        mova(axs.ixbas,axs.iybas);  
        ifd=0;
        iymax=axs.iybas;
        iylst=axs.iybas;       
        for(i=ib_bn;i<it_bn+1;i++)
         {  l=i-ib_bn+1;
            if(axs.ncyc>0) goto s130;
            dely=histo.bcnt[i]-axs.ylo;
            yy=ver*dely/dy;
            if(yy<0.0) yy=0.0;    
            iy=(int)(yy+0.5)+axs.iybas;   
            goto s160;
 s130:
            iy=0;
            if(histo.bcnt[i]>0.0) iy=(int)(axs.a*log10(histo.bcnt[i])+axs.b
                 +(double)axs.iybas);
 s160:
            if(iy<axs.iybas) iy=axs.iybas;        
            if(iy>iytop) iy=iytop;
            iydel=iy-iylst;
            if(iydel>0) ifd=1;
            drwr(0,iydel);
            xnew=(double)l*xfac;
            ixnew=(int)(xnew+0.5)+axs.ixbas;
            drwa(ixnew,iy);
/*            movr(-lwl,0);      */
            iylst=iy;      
            if(iy>iymax) continue;
            iymax=iy;
            ixmax=ixnew;
         } 
        iy=axs.iybas-iylst;
        drwr(0,iy);     
        drwa(axs.ixbas,axs.iybas);  
        hstfill(1);
/* now redraw black border around fill area */
        mova(axs.ixbas+axs.linx,axs.iybas);  
        drwr(-axs.linx,0);
        ifd=0;
        iymax=axs.iybas;
        iylst=axs.iybas;       
        for(i=ib_bn;i<it_bn+1;i++)
         {  l=i-ib_bn+1;
            if(axs.ncyc>0) goto s230;
            dely=histo.bcnt[i]-axs.ylo;
            yy=ver*dely/dy;
            if(yy<0.0) yy=0.0;    
            iy=(int)(yy+0.5)+axs.iybas;   
            goto s260;
 s230:
            iy=0;
            if(histo.bcnt[i]>0.0) iy=(int)(axs.a*log10(histo.bcnt[i])+axs.b
                 +(double)axs.iybas);
 s260:
            if(iy<axs.iybas) iy=axs.iybas;        
            if(iy>iytop) iy=iytop;
            iydel=iy-iylst;
            if(iydel>0) ifd=1;
            drwr(0,iydel);
            xnew=(double)l*xfac;
            ixnew=(int)(xnew+0.5)+axs.ixbas;
            drwa(ixnew,iy);
/*            movr(-lwl,0);      */
            iylst=iy;      
            if(iy>iymax) continue;
            iymax=iy;
            ixmax=ixnew;
         } 
        iy=axs.iybas-iylst;
        drwr(0,iy);     
        drwa(axs.ixbas,axs.iybas);  
        return(0);        
      }
      hstfdr(i,l)  
       int i,l;
       {
        extern double hstfit();
        int lplt,istr,nn,k,ix,iy,nvect;
        double xx,x1,yy,y1,dx;
/*c     draw the fit  */
        nvect=hft.ivcts[i];
        lplt=0;
        if(histo.jtop[i]<0) lplt=1; 
        istr=0;        
        dx=(axs.xhi-axs.xlo)/(double)nvect;         
        nn=nvect+1;    
/*      do 900 k=1,nn  */
        for(k=0;k<nn;k++)
         {
          xx=(double)(k)*dx+axs.xlo;
          x1=(double)axs.linx*(xx-axs.xlo)/(axs.xhi-axs.xlo);      
          ix=axs.ixbas+(int)x1;   
/*c     yy equals function value at xx  */
          yy=hstfit(i,l,xx);       
          y1=(double)axs.liny*yy/axs.yhi;  
          if(y1<0.0) y1=0.0;    
          if(axs.ncyc>0) goto s760;
          iy=(int)y1+axs.iybas;   
          goto s770;
 s760:    iy=axs.iybas;
          if(yy<0.0) goto s770;
          y1=axs.a*log10(yy)+axs.b;
          iy=(int)y1+axs.iybas;
 s770:
          if(iy<(axs.iybas+axs.liny)&&iy>axs.iybas) goto s850;   
          istr=0;        
          continue;
 s850:
          if(istr==0) mova(ix,iy);    
          if(istr!=0) drwa(ix,iy);    
          istr=1;        
        }
      return(0);        
      }
      hstrst()       
      {      
        int i;
        histo.ntot=-1;        
        for(i=0;i<50;i++) histo.idef[i]=0;
        return(0);        
      }
      hstmwr(ll,l1,l2)
      int ll,l1,l2;
      {       
         int l,m,mm;
         double ver;
         if(l1>=0 && l1<50) goto s10;
         printf("hstmwr calling error ll=%d l1=%d l2=%d\n",ll,l1,l2);
         return(0);        
  s10:
         erasm();    
        for(l=l1;l<l2+1;l++)
         { if(histo.idef[l]!=1) continue;
           m=l-l1;
           hstscl(l);
           axs.ixbas=jxh[ll-1][m];
           axs.iybas=jyh[ll-1][m];
           axs.linx=nhh[ll];   
           axs.liny=nvh[ll];   
           axs.nxtk=nhtkh;
           axs.nytk=nvtkh;
           axs.xlo=histo.xbot[l];
           axs.xhi=axs.xlo+histo.xint[l];
           axs.ylo=0.0;
           axs.yhi=bscl;
           axs.ncyc=0;
           axs.ncxc=0;
           if(histo.jtop[l]<0) axs.ncyc=-histo.jtop[l];
           if(axs.ncyc<=0) goto s22;
           ver=(double)axs.liny;
           axs.ylo=axs.yhi/pow(10.0,(double)axs.ncyc);
           axs.a=ver/(log10(axs.yhi)-log10(axs.ylo));
           axs.b=-axs.a*log10(axs.ylo);
   s22:
           axs.isize=1;
           axis(l);
           hstdbn(ib,it);
           if(hft.nplts[l]>0) {for(mm=1;mm<hft.nplts[l];mm++) hstfdr(l,mm);}
       }
          plotmx();   
          return(0);        
      }
      hstscl(k)    
      int k;
      {
        int i,npow,n,nbins;
        double bmax,fac,sqr;
        ib=histo.ibot[k]+1;  
        it=histo.itop[k]-1;  
        bmax=0.0;      
        sumin=0.0;     
/*      do 210 i=ib,it */
        for(i=ib;i<(it+1);i++)
         {
/*  c     find bin with greatest number of points     
    c     total number of points in bounds  */
          sumin=sumin+histo.bcnt[i];   
          if(histo.bcnt[i]>bmax) bmax=histo.bcnt[i];        
         }
/*  c     count total points      */
          sumh=sumin+histo.bcnt[ib-1]+histo.bcnt[it+1];         
          bscl=(double)histo.jtop[k];     
/*  c     protect against empty plot  */
          if(sumin==0.0) bscl=1.0;
          if(bmax<=0.0 && bscl<=0.0) bscl=1.0;    
          if(histo.itop[k]<0) goto s220;
          if(bmax<bscl) goto s300;        
          npow=(int)log10(bmax)-1;
          n=(int)(bmax/pow(10.0,(double)npow));
          bscl=(double)(n+2)*pow(10.0,(double)npow);        
          goto s300;
  s220:
          fac=1.2;
          bscl=fac*bmax;
          if(bscl<=0.0) bscl=1.0;
 s300:
         nbins=it-ib+1; 
         fmean=0.0;     
         sigmah=0.0;     
         if(sumh==0.0) goto s375;
         fmean=histo.xsum[k]/sumh;       
         sqr=histo.x2sum[k]/sumh-pow(fmean,2.0);         
         if(sqr<=0.0) goto s375;
         sigmah=sqrt(sqr);         
 s375:
         return(0);        
      }
      hstset(k,xmax,xmin,nbins,nlog)  
      int k,nbins,nlog;
      double xmax,xmin;
      {
        static int ient=0;      
        int i;
        if(ient==0)
          {histo.legal=0;
           histo.nbn=2600;
           histo.ntot=-1;}
        ient=1;
        if(k==-1) goto s200;    
        if(k<0 || k>50) goto s80;   
        if(histo.ntot!=-1) goto s20;  
        for(i=0;i<50;i++) histo.idef[i]=0;
   s20:
        if(histo.idef[k]==1) goto s82;         
        histo.idef[k]=1;     
        if(nbins<=0) nbins=50; 
        ib=histo.ntot+1;     
        histo.ibot[k]=ib;    
        it=histo.ntot+nbins+2;         
        histo.itop[k]=it;    
        histo.ntot=it;       
        if(histo.ntot<=histo.nbn) goto s90;
        goto s84;
  s80:
        printf("histo number out of bounds  (0 to 50 allowed) %d\n",k);
        goto s86;
  s82:
        printf(" histo %d already set up\n",k);       
        goto s86;      
  s84:
        printf("too many total histo bins used (2600 allowed)\n");
        printf(" nbin+2 used by a plot of nbin bins\n");
        printf(" error occured in setup of plot %d histo.ntot=%d\n",k,histo.ntot);
        goto s86;      
  s88:
        printf("bad histo bounds %f %f for histo %d\n",xmax,xmin,k);
  s86:
        return(0);
  s90:
        histo.jtop[k]=nlog;
/*c     zero arrays   */
        for(i=ib;i<(it+1);i++) histo.bcnt[i]=0.0;
        if(xmin==xmax) goto s88;
/*c     set up coefficients for quick binning       */
        histo.xint[k]=xmax-xmin;       
        histo.xbot[k]=xmin;  
/*c     zero mean and standard deviation  */
        histo.xsum[k]=0.0;   
        histo.x2sum[k]=0.0;  
        return(0);        
 s200:
        histo.ntot=0;        
        return(0);        
      }
      hstzro()
       { histo.legal=0;
         histo.nbn=2600;
         histo.ntot=-1;
       }

      hstwrt(k)    
      int k;
      {
        char text[132];
        int l,i,ix,iy,ixbs,iybs,ixtp,ixctr;
        double ver;
        if(k<0 || k>49) goto s80;
        if(histo.idef[k]!=1) goto s90;
        lsigma=lsg;    
        ldigit=ldg;    
        hstscl(k);
        if(sumin<=0.0) goto s95;         
        axs.isize=2;
        axs.ixbas=ixbsh;
        axs.iybas=iybsh;
        axs.linx=linhh;
        axs.liny=linvh;
        axs.nxtk=nhtkh;
        axs.nytk=nvtkh;
        axs.xlo=histo.xbot[k];
        axs.xhi=axs.xlo+histo.xint[k];
        axs.ylo=0.0;
        axs.yhi=bscl;
        axs.ncyc=0;
        axs.ncxc=0;
        if(histo.jtop[k]<0) axs.ncyc=-histo.jtop[k];
        if(axs.ncyc<=0) goto s5; 
        ver=(double)axs.liny;
        axs.ylo=axs.yhi/pow(10.0,(double)axs.ncyc);
        axs.a=ver/(log10(axs.yhi)-log10(axs.ylo));
        axs.b=-axs.a*log10(axs.ylo);
   s5:
        erasm();
        axis(k);
        hstdbn(ib,it);
        if(htset[k]!=0) 
         {sprintf(htext,"%s",&htitle[k][0]);
          symb(100,3000,0.0,htext);}
        if(hft.nplts[k]<=0) goto s10;
        for(l=1;l<hft.nplts[k]+1;l++) hstfdr(k,l); 
  s10:
/* c     print statistics $$$$$$$$$$$$$$$$$$$$$$$$$$$$$        */
        if(lsigma!=0 && sumin>0.0) goto s40;   
        ixbs=axs.ixbas+200;
        iybs=axs.iybas-600;
        ixtp=ixbs+axs.linx-400;      
        mova(ixbs,iybs);
        ix=axs.linx-400;
        drwr(ix,0);
        drwr(0,300);
        drwr(-ix,0);
        drwr(0,-300);
        movr(0,100);
        drwr(ix,0);
        movr(0,100);
        drwr(-ix,0);
        ixctr=ixbs+(ixtp-ixbs)/2;
        iy=iybs;  
        mova(ixctr,iy);
        drwr(0,300);
        sprintf(text,"plot %d\n",k);       
        symb(ixbs+30,iybs+230,0.0,text);
        sprintf(text,"in %d\n",(int)sumin);       
        if(sumin>100000) sprintf(text,"in %f\n",sumin);       
        symb(ixctr+30,iybs+230,0.0,text);
        ix=(int)histo.bcnt[ib-1];
        sprintf(text,"left %d\n",ix);
        if(ix>100000) sprintf(text,"left %f\n",histo.bcnt[ib-1]);
        symb(ixbs+30,iybs+130,0.0,text);
        ix=(int)histo.bcnt[it+1];
        sprintf(text,"right %d\n",ix);
        if(ix>100000) sprintf(text,"right %f\n",histo.bcnt[it+1]);
        symb(ixctr+30,iybs+130,0.0,text);
        sprintf(text,"mean %g\n",fmean);
        symb(ixbs+30,iybs+30,0.0,text);
        sprintf(text,"sigma %g\n",sigmah);
        symb(ixctr+30,iybs+30,0.0,text);
        plotmx();   
        return(0);        
  s40:
        plotmx(0);   
        return(0);        
  s80:
        printf("hstwrt called with illegal plot number %d\n",k);
        return(0);
  s90:
        printf("hstwrt called for plot not set up %d\n",k);
        return(0);
  s95:
        printf("plot %d   mean=%11.4e     sigma=%11.4e\n",k,fmean,sigmah);       
        printf("%11.4e  points        %11.4e points in plot\n",sumh,sumin);
        printf("%11.4e out on left        %11.4e out on right\n",
             histo.bcnt[ib-1],histo.bcnt[it+1]);     
        return(0);
      }
       hstmsg(k,mean,sigma)
       int k;
       double *mean,*sigma;
         {hstscl(k);
	  *mean = fmean;
	  *sigma = sigmah;
	 }

