#include <stdio.h>
#include <math.h>
       struct {int ixbas;
               int iybas;
               int linx;
               int liny;
               int nxtk;
               int nytk;
               int ncyc;
               int ncxc;
               int isize;
               double xlo;
               double xhi;
               double ylo;
               double yhi;
               double a;
               double b;
               double ax;
               double bx;}axs;
      struct {int mtxt;}axl;
      axis(k)
      int k;
       {
        int ilog,n1,n2,n3,i,nn,nabs,ix,iy,nx,i1,i2,ny,k1,k2,k3,iytop,ixtop;
        double ver,xlin,xmag,sg,dx,del,fi,x,xx,xxx,fx,ymag,dy,y,yy,yyy,fy;
        iytop=axs.iybas+axs.liny;
        ixtop=axs.ixbas+axs.linx;
        boxdrw(axs.ixbas,axs.linx,axs.iybas,axs.liny);
        ver=axs.liny;      
        xlin=axs.linx;
/*  c     label horizontal axis $$$$$$$$$$$$$$$$$$$$$$$$         */
        if(axs.ncxc>0) goto s310;
/*  c     linear scale horizontal ixis  */
        xmag=fabs(axs.xhi); 
        if(fabs(axs.xlo)>xmag) xmag=fabs(axs.xlo);
        ilog=0;        
        sg=1.0;
        if(xmag>=100.0 || xmag<0.5) ilog=(int)log10(xmag);
        dx=(axs.xhi-axs.xlo)/(double)axs.nxtk;
        del=(double)axs.linx/(double)axs.nxtk;
        nn=axs.nxtk+1;     
        n1=0;
        n2=axs.nxtk/2;     
        n3=n2*2;     
        for(i=0;i<nn;i++)
         {
          fi=(double)(i); 
          x=axs.xlo+fi*dx;
          ix=axs.ixbas+(int)(fi*del);
          iy=iytop;      
          mova(ix,iy);        
          drwr(0,-15);         
          if((i != n1) && (i != n2) && (i != n3)) goto s250;         
          drwr(0,-15);         
   s250:
          iy=axs.iybas;      
          mova(ix,iy);        
          drwr(0,15);        
          if((i != n1) && (i != n2) && (i != n3)) continue;
          drwr(0,15);        
          if(axs.isize != 2) goto s270;
          movr(-50,-110);      
          if(ilog != 0) movr(30,0);     
          if(x<0.0) movr(-60,0);     
          xx=x/pow(10.0,(double)ilog);
          laxm(xx,0,axs.isize);       
          continue;
   s270:
          movr(-36,-110);
          xx=x/pow(10.0,(double)ilog);
          if(xx<0.0) movr(-30,0);
          laxm(xx,0,axs.isize);
        }
        if(ilog != 0)
         {ix=axs.ixbas+(int)(0.75*(double)axs.linx-80.0);
          iy=axs.iybas-85;
          mova(ix,iy);
          llog(ilog);}
        goto s350;
/*  c     log scale horizontal axis  */
   s310:
        nx=(int)log10(axs.xlo)-1;
        xx=pow(10.0,(double)nx);
        fx=0.0;
        nn=(int)log10(xx);
   s320:
        fx=fx+1.0;
        if(fx<10.0) goto s330;
        xx=xx*10.0;
        fx=1.0;
        nn=nn+1;
   s330:
        xxx=xx*fx;
        ix=(int)(axs.ax*log10(xxx)+axs.bx); 
        if(ix<0) goto s320;   
        ix=ix+axs.ixbas;   
        if(ix>ixtop) goto s340;         
        iy=iytop;      
        mova(ix,iy);        
        drwr(0,-15);         
        if(fx==1.0) drwr(0,-15);        
        iy=axs.iybas;      
        mova(ix,iy);        
        drwr(0,15);        
        if(fx != 1.0) goto s320;    
        drwr(0,15);        
        movr(-15,-100);     
        if(nn<0) movr(-10,0);      
        digm(1,axs.isize);
        digm(0,axs.isize);
        movr(0,15);         
        if(axs.isize != 2) movr(0,15);
        nabs=nn;
        if(nn<0) nabs=-nn;
        if(nn<0) 
          {digm(12,1);
           movr(-15,0);}
        i1=nabs/10;
        if(i1>0) digm(i1,1);
        i2=nabs-i1*10;       
        digm(i2,1);         
        goto s320;
   s340:
/*  c     label vertical axis $$$$$$$$$$$$$$$$$$$$$$$$$$  */        
   s350:
        if(axs.ncyc>0) goto s610;        
/*  c     linear plot $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$  */
        ilog=0;        
        ymag=fabs(axs.yhi);
        if(fabs(axs.ylo)>ymag) ymag=fabs(axs.ylo);
        if(ymag>=100.0 || ymag<0.05) ilog=(int)log10(ymag);
        nn=axs.nytk+1;     
        n1=0;
        n2=axs.nytk/2;     
        n3=n2*2;     
        dy=(axs.yhi-axs.ylo)/(double)axs.nytk;
        for(i=0;i<nn;i++)
         {
          fi=(double)i; 
          ix=ixtop;      
          y=fi*dy+axs.ylo;
          yy=ver*(y-axs.ylo)/(axs.yhi-axs.ylo);
          sg=1.0;
          if(yy<0.0) sg=-1.0;
          iy=axs.iybas+(int)(yy+sg*0.5);       
          mova(ix,iy);        
          drwr(-15,0);         
          if((i != n1) && (i != n2) && (i != n3)) goto s450;         
          drwr(-15,0);         
    s450:
          ix=axs.ixbas;      
          mova(ix,iy);        
          drwr(15,0);        
          if((i != n1) && (i != n2) && (i != n3)) continue;
          drwr(15,0);        
          if(axs.isize != 2) goto s470;
          movr(-160,-15);     
          if(y<0.0) movr(-45,0);     
          yyy=y/pow(10.0,(double)ilog);
          laxm(yyy,0,axs.isize);       
          continue;
    s470:
          movr(-160,-15);
          yyy=y/pow(10.0,(double)ilog);
          if(y<0.0) movr(-30,0);
          laxm(yyy,0,axs.isize);
         }
        if(ilog != 0)
         {ix=axs.ixbas-135;
          iy=axs.iybas+(int)(0.75*(double)axs.liny);
          mova(ix,iy);
          llog(ilog);}
        goto s640;     
/*  c     log plot $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$        */
s610:
        ny=(int)log10(axs.ylo)-1;
        yy=pow(10.0,(double)ny);
        fy=0.0;
        nn=log10(yy);
s620:
        fy=fy+1.0;
        if(fy<10.0) goto s630;
        yy=yy*10.0;
        fy=1.0;
        nn=nn+1;
s630:
        yyy=yy*fy;
        iy=(int)(axs.a*log10(yyy)+axs.b);
        if(iy<.0) goto s620;
        iy=iy+axs.iybas;
        if(iy>iytop) goto s640;
        ix=ixtop;
        mova(ix,iy);
        drwr(-15,0);
        if(fy==1.0) drwr(-15,0);
        ix=axs.ixbas;
        mova(ix,iy);
        drwr(15,0);
        if(fy != 1.0) goto s620;
        drwr(15,0);
        movr(-130,-15);
        if(nn<0) movr(-10,0);
        digm(1,axs.isize);
        digm(0,axs.isize);
        movr(0,15);
         if(axs.isize != 2) movr(0,15);
        if(nn<0) 
          {digm(12,1);
           movr(-15,0);}
        nabs=nn;
        if(nn<0) nabs=-nn;
        i1=nabs/10;
        if(i1>0) digm(i1,1);
        i2=nabs-i1*10;
        digm(i2,1);
        goto s620;
s640:
        if(axs.isize == 2) goto s750;
        k1=k/100;
        k2=(k-k1*100)/10;
        k3=k-k1*100-k2*10;
        ix=axs.ixbas+10;
        iy=axs.iybas+axs.liny-45;
        mova(ix,iy);
        if(k1>0) digm(k1,1);
        if(k2>0) digm(k2,1);
        digm(k3,1);
s750:
       return(0);
      }

