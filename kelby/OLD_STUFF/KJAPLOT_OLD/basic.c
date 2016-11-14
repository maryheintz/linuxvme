#include <stdio.h>
#include <math.h>
      static int ixold,iyold,idev=-1,jxx,jyy,kxx,kyy;
      static int idg[13]={48,49,50,51,52,53,54,55,56,57,46,120,45};

      gidev(kdev)
      int *kdev;
      {*kdev=idev;}

      sidev(kdev)
      int kdev;
      {idev=kdev;}

      plot_reset()
      {idev=-1;}

      boxdrw(ixbas,linx,iybas,liny)
      int ixbas,linx,iybas,liny;
      {
        int lwl;
        lwl=1;
        mova(ixbas,iybas+liny);  
        drwr(0,-liny);      
        drwr(linx,0);   
        drwr(0,liny);       
        drwr(-linx,0);      
      }
      erasm() 
      {
      int ichr;
      static int kja=0;
      if(idev!=-1) goto s5;
      printf("postscript(0) or tek(1) or vt240(2) output\n");
      fscanf(stdin,"%d",&idev);
      ichr=getchar();
      kja = 1;
s5:
      if(kja==0) ichr=getchar();
      kja = 1;
      if(idev==0) erasps(); 
      if(idev==1) erastk();
      if(idev==2) erasvt();
      }
      mova(ix,iy)
      int ix,iy;
      {
       extern int jxx,jyy;
       jxx=ix;
       jyy=iy;
       if(idev==0) movaps(ix,iy);
       if(idev==1) movatk(ix,iy);
       if(idev==2) movavt(ix,iy);
      }
      movr(idx,idy)
      int idx,idy;
      {
       extern int jxx,jyy;
       jxx=jxx+idx;
       jyy=jyy+idy;
      if(idev==0) movaps(jxx,jyy);
      if(idev==1) movatk(jxx,jyy);
      if(idev==2) movavt(jxx,jyy);
      }
      drwa(ix,iy)
      int ix,iy;
      {
       extern int jxx,jyy;
       jxx=ix;
       jyy=iy;
      if(idev==0) drwaps(ix,iy);
      if(idev==1) drwatk(ix,iy);
      if(idev==2) drwavt(ix,iy);
      }
      drwr(idx,idy)
      int idx,idy;
      {
       extern int jxx,jyy;
      jxx=jxx+idx;
      jyy=jyy+idy;
      if(idev==0) drwaps(jxx,jyy);
      if(idev==1) drwatk(jxx,jyy);
      if(idev==2) drwavt(jxx,jyy);
                                        }
      pnta(ix,iy)
      int ix,iy;
      {
       extern int jxx,jyy;
       jxx=ix;
       jyy=iy;
      if(idev==0) pntaps(ix,iy);
      if(idev==1) pntatk(ix,iy);
      if(idev==2) pntavt(ix,iy);
      }
      getp(int *ix,int *iy)
      {
       extern int jxx,jyy;
       *ix=jxx;
       *iy=jyy;
      }
      plotmx()
      { if(idev==0) plotps();
        if(idev==1) plottk(0);
        if(idev==2) plotvt();
      }
      plotmxnw()
      { if(idev==0) plotps();
        if(idev==1) plottk(1);
        if(idev==2) plotvt();
      }
      digm(l,isize)
      int l,isize;
      { extern int jxx,jyy;
        int ixx,iyy,ixbs,iybs,ix;
        char *idigit="0\0";      
        if(l<0 || l>12) return(0);      
        if(idev==0) 
        { if(l==11) movr(-5,0);
          getp(&ixx,&iyy);
          if(l<0 || l>12) return(0);      
          if(l==0) idigit="0\0";
          if(l==1) idigit="1\0";
          if(l==2) idigit="2\0";
          if(l==3) idigit="3\0";
          if(l==4) idigit="4\0";
          if(l==5) idigit="5\0";
          if(l==6) idigit="6\0";
          if(l==7) idigit="7\0";
          if(l==8) idigit="8\0";
          if(l==9) idigit="9\0";
          if(l==10) idigit=".\0";
          if(l==11) idigit="x\0";
          if(l==12) idigit="-\0";
          symps(ixx,iyy,0.0,2*isize,idigit);
          if(l==10) movr(-5,0);
          movr(30,0);
        }
        if(idev==1) 
         { putchar(idg[l]);
           movr(30,0);
         } 
        if(idev==2) 
         { getp(&ixx,&iyy);
           ixbs=ixx;      
           iybs=iyy;      
           mova(ixx,iyy+50);
           printf("+T");
           putchar(34);
           putchar(idg[l]);
           putchar(34);
           ix=ixbs+25;    
           mova(ix,iybs);    
         }
        return(0);
      }
      laxm(xx,ilog,isz) 
      double xx;
      int ilog,isz;
      { double x,sg;
        int i1,i2,i3,i4,idx,idy,iszz,jlog;
        x=xx/pow(10.0,(double)ilog);
        sg=1.0;
        if(x<0) sg=-1.0;
        x=x+0.005*sg;
        if(fabs(x)<0.0001) x=0.0;
        if(ilog==0) goto s5;
        idx=40*isz;
        movr(-idx,0);
  s5:
        if(x<0.0) digm(12,isz);
        x=fabs(x);
        i1=(int)(x/10.0);
        if(i1>0) digm(i1,isz);
        i2=x-(double)(i1*10);
        digm(i2,isz);
        digm(10,isz);
        i3=(int)(x*10.0-(double)(i1*100+i2*10));
        digm(i3,isz);
        if(i1>0) goto s10;
        i4=(int)(x*100.0-(double)(i2*100+i3*10));
        digm(i4,isz);
  s10:
        if(ilog==0) return(0);
        digm(11,isz);
        digm(1,isz);
        digm(0,isz);
        iszz=isz-1;
        if(iszz<1) iszz=1;
        idy=8+(isz-1)*8;
        movr(0,idy);
        if(ilog<0) digm(12,iszz);
        jlog=ilog;
        if(ilog<0) jlog=-ilog;
        i1=jlog/10;
        if(i1!=0) digm(i1,iszz);
        i2=jlog-i1*10;
        digm(i2,iszz);
        return(0);
      }
      llog(ilog)
      int ilog;
      { int kxx,kyy,i1,i2,jlog;
        getp(&kxx,&kyy);
        digm(11,1);
        digm(1,1);
        digm(0,1);
        movr(0,8);
        if(ilog<0) digm(12,1);
        jlog=ilog;
        if(ilog<0) jlog=-ilog;
        i1=jlog/10;
        if(i1!=0) digm(i1,1);
        i2=jlog-i1*10;
        digm(i2,1);
      }   
      erastk()
      {
/*      switch from vt240 to tek4010 mode  */
        putchar(27);
        putchar(91);
        putchar(63);
        putchar(51);
        putchar(56);
        putchar(104);
/*      set to graphic,erase screen,set to alpha  */
        putchar(29);
        putchar(27);
        putchar(12);
        putchar(31);
  /*      putchar(0);
        putchar(0);     */ 
      }
      pttk(ix,iy)
      int ix,iy;
      { matk(ix,iy);
        datk(ix+1,iy);
      }
      mrtk(idx,idy)
      int idx,idy;
      { int ix,iy;
        ix=ixold+idx;
        iy=iyold+idy;
        matk(ix,iy);
      }
      drtk(idx,idy)
      int idx,idy;
      { int ix,iy;
        ix=ixold+idx;
        iy=iyold+idy;
        datk(ix,iy);
      }
      matk(ix,iy)
      int ix,iy;
      { int iyh,iyhi,iylo,ixh,ixhi,ixlo;
        ixold=ix;
        iyold=iy;
        putchar(29);
        iyh=iyold/32;
        iyhi=iyh+32;
        iylo=iyold-32*iyh+96;
        ixh=ixold/32;
        ixhi=ixh+32;
        ixlo=ixold-32*ixh+64;
        putchar(iyhi);
        putchar(iylo);
        putchar(ixhi);
        putchar(ixlo);
        putchar(31);
        putchar(0);
        putchar(0);
        putchar(0);
        putchar(0);
      }
      datk(ix,iy)
      int ix,iy;
      { int iyh,iyhi,iylo,ixh,ixhi,ixlo;
        putchar(29);
        iyh=iyold/32;
        iyhi=iyh+32;
        iylo=iyold-32*iyh+96;
        ixh=ixold/32;
        ixhi=ixh+32;
        ixlo=ixold-32*ixh+64;
        putchar(iyhi);
        putchar(iylo);
        putchar(ixhi);
        putchar(ixlo);
        ixold=ix;
        iyold=iy;
        iyh=iyold/32;
        iyhi=iyh+32;
        iylo=iyold-32*iyh+96;
        ixh=ixold/32;
        ixhi=ixh+32;
        ixlo=ixold-32*ixh+64;
        putchar(iyhi);
        putchar(iylo);
        putchar(ixhi);
        putchar(ixlo);
        putchar(0);
        putchar(0);
        putchar(0);
      }
      plottk(kkk)
      int kkk;
      { int wait;
        movatk(10,10);
        if(kkk == 0 ) 
	{ printf(" <cr> for next plot");
          fflush(stdout);
          wait=getchar();
	}
/*      return to vt240 mode      */
        putchar(27); 
        putchar(3);   /* for xterm sessions */
        putchar(27); 
        putchar(50);  /* for mac versatermpro sessions */
        fflush(stdout);
      }
      movatk(ix,iy)
      int ix,iy;
      {
      int jx,jy;
      jx=(ix*1024)/2200;
      jy=(iy*750)/3100;
      matk(jx,jy);
      }
      drwatk(ix,iy)
      int ix,iy;
      {
      int jx,jy;
      jx=(ix*1024)/2200;
      jy=(iy*750)/3100;
      datk(jx,jy);
      }
      pntatk(ix,iy)
      int ix,iy;
      {
      int jx,jy;
      jx=(ix*1024)/2200;
      jy=(iy*750)/3100;
      pttk(jx,jy);
      }
      symb(ix,iy,ang,text)
      int ix,iy;
      double ang;
      char *text;
      { int iysave;
        if(idev==1) 
        { mova(ix,iy);
          printf(text); }
          fflush(stdout);
        if(idev==0)
        { symps(ix,iy,ang,4,text); }
        if(idev==2)
        { iysave=iy;
          mova(ix,iy+60);
          printf("+T");
          putchar(34);
          printf("%s",text);
          putchar(34);
          mova(ix,iy);
        }
      }

      erasvt()
      { printf("%c%c%c",(char)27,'P','p');
        printf("+S(E)");         
        printf("+S(A[0,0][799,479])");
      }
      movavt(ix,iy)
      int ix,iy;
      {
      int jx,jy;
      jx=(ix*800)/2200;
      jy=(iy*480)/3100;
      mavt(jx,jy);
      }
      drwavt(ix,iy)
      int ix,iy;
      {
      int jx,jy;
      jx=(ix*800)/2200;
      jy=(iy*480)/3100;
      davt(jx,jy);
      }
      pntavt(ix,iy)
      int ix,iy;
      {
      int jx,jy;
      jx=(ix*800)/2200;
      jy=(iy*480)/3100;
      ptvt(jx,jy);
      }
      mavt(ix,iy)
      int ix,iy;
      { int ky;
        ky=479-iy;     
        printf("+P[%3d,%3d]",ix,ky);   
      }
      mrvt(idx,idy)        
      int idx,idy;
      { extern int kxx,kyy;
        int ky;
        kxx=kxx+idx;   
        kyy=kyy+idy;   
        ky=479-kyy;    
        printf("+P[%3d,%3d]",kxx,ky);     
      }
      ptvt(ix,iy)
      int ix,iy;
      { extern int kxx,kyy;
        int ky;
        kxx=ix;        
        kyy=iy;        
        ky=479-kyy;    
        printf("+P[%3d,%3d],V[]",kxx,ky);     
      }
      drvt(idx,idy)        
      int idx,idy;
      { extern int kxx,kyy;
        int ky;
        kxx=kxx+idx;   
        kyy=kyy+idy;   
        ky=479-kyy;    
        printf("+V[%3d,%3d]",kxx,ky);     
      }
      davt(ix,iy)
      int ix,iy;
      { extern int kxx,kyy;
        int ky;
        kxx=ix;        
        kyy=iy;
        ky=479-kyy;    
        printf("+V[%3d,%3d]",kxx,ky);     
        return(0);        
      }
      plotvt()
      { int wait;
        mova(10,3000);
/*        printf("+T");
        putchar(34);
        putchar(34); */
/*      return to vt240 mode      */
        putchar(36);
        putchar(27);
        putchar(47);
        putchar(27);
        putchar(47);
        printf(" <cr> for next plot");
        wait=getchar();
/*        fscanf(stdin,"%d",&wait); */
      }
hstfill(k)
int k;
 {if(idev==0) fillps(k);
 }
