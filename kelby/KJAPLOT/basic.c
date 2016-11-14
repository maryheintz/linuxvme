#include <stdio.h>
#include <math.h>
#include "gd.h"
#include "gdfontg.h"
      static int ixold,iyold,idev=-1,jxx,jyy,kxx,kyy;
      static int idg[13]={48,49,50,51,52,53,54,55,56,57,46,120,45};
      gdImagePtr im;
      gdFontPtr ft;
      FILE *pngout, *jpgout;
      int ixpj,iypj;
      int black;
      int white;
          
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
      printf("postscript(0) or tek(1) or png(2) of jpg(3) output\n");
      fscanf(stdin,"%d",&idev);
      ichr=getchar();
      kja = 1;
s5:
      if(kja==0) ichr=getchar();
      kja = 1;
      if(idev==0) erasps(); 
      if(idev==1) erastk();
      if(idev==2) eraspj();
      if(idev==3) eraspj();
      }

      mova(ix,iy)
      int ix,iy;
      {
       extern int jxx,jyy;
       jxx=ix;
       jyy=iy;
       if(idev==0) movaps(ix,iy);
       if(idev==1) movatk(ix,iy);
       if(idev==2) movapj(ix,iy);
       if(idev==3) movapj(ix,iy);
      }
      movr(idx,idy)
      int idx,idy;
      { extern int jxx,jyy;
        jxx=jxx+idx;
        jyy=jyy+idy;
        if(idev==0) movaps(jxx,jyy);
        if(idev==1) movatk(jxx,jyy);
        if(idev==2) movapj(jxx,jyy);
        if(idev==3) movapj(jxx,jyy);
      }

      drwa(ix,iy)
      int ix,iy;
      {
       extern int jxx,jyy;
       jxx=ix;
       jyy=iy;
      if(idev==0) drwaps(ix,iy);
      if(idev==1) drwatk(ix,iy);
      if(idev==2) drwapj(ix,iy);
      if(idev==3) drwapj(ix,iy);
      }
      drwr(idx,idy)
      int idx,idy;
      {
       extern int jxx,jyy;
      jxx=jxx+idx;
      jyy=jyy+idy;
      if(idev==0) drwaps(jxx,jyy);
      if(idev==1) drwatk(jxx,jyy);
      if(idev==2) drwapj(jxx,jyy);
      if(idev==3) drwapj(jxx,jyy);
                                        }
      pnta(ix,iy)
      int ix,iy;
      {
       extern int jxx,jyy;
       jxx=ix;
       jyy=iy;
      if(idev==0) pntaps(ix,iy);
      if(idev==1) pntatk(ix,iy);
      if(idev==2) pntapj(ix,iy);
      if(idev==3) pntapj(ix,iy);
      }
      getp(int *ix,int *iy)
      {
       extern int jxx,jyy;
       *ix=jxx;
       *iy=jyy;
      }
      plotmx(int k)
      { if(idev==0) plotps(k);
        if(idev==1) plottk(0);
        if(idev==2) plotpng(k);
        if(idev==3) plotjpg(k);
      }
      plotmxnw(k)
      { if(idev==0) plotps(k);
        if(idev==1) plottk(1);
        if(idev==2) plotpng(k);
        if(idev==3) plotjpg(k);
      }
      digm(l,isize)
      int l,isize;
      { extern int jxx,jyy;
        int ixx,iyy,ixbs,iybs,ix;
        char *idigit="0\0";
	char c;      
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
        if(idev==2 || idev==3) 
         {if(l<0 || l>12) return(0);      
          if(l==0) gdImageString(im,gdFontGiant,ixpj,iypj-10,"0",black);
          if(l==1) gdImageString(im,gdFontGiant,ixpj,iypj-10,"1",black);
          if(l==2) gdImageString(im,gdFontGiant,ixpj,iypj-10,"2",black);
          if(l==3) gdImageString(im,gdFontGiant,ixpj,iypj-10,"3",black);
          if(l==4) gdImageString(im,gdFontGiant,ixpj,iypj-10,"4",black);
          if(l==5) gdImageString(im,gdFontGiant,ixpj,iypj-10,"5",black);
          if(l==6) gdImageString(im,gdFontGiant,ixpj,iypj-10,"6",black);
          if(l==7) gdImageString(im,gdFontGiant,ixpj,iypj-10,"7",black);
          if(l==8) gdImageString(im,gdFontGiant,ixpj,iypj-10,"8",black);
          if(l==9) gdImageString(im,gdFontGiant,ixpj,iypj-10,"9",black);
          if(l==10) gdImageString(im,gdFontGiant,ixpj,iypj-10,".",black);
          if(l==11) gdImageString(im,gdFontGiant,ixpj,iypj-10,"x",black);
          if(l==12) gdImageString(im,gdFontGiant,ixpj,iypj-10,"-",black);
	  movr(9*11/4,0);
/*	  ixpj = ixpj + 9;
       { ixpj=50+ix*800/2200; 
	  jxx = ((ixpj-50)*11)/8;  
	  jxx = jxx+200;  */
         }
        return(0);
      }
      laxm(xx,ilog,isz) 
      double xx;
      int ilog,isz;
      { double x,sg,xtst;
        int i0,i1,i2,i3,i4,idx,idy,iszz,jlog;
        x=xx/pow(10.0,(double)ilog);
        sg=1.0;
        if(x<0) sg=-1.0;
        xtst=x+0.005*sg;
        if(fabs(xtst)<0.0001) x=0.0;
        if(ilog==0) goto s5;
        idx=40*isz;
        movr(-idx,0);
  s5:
        if(x<0.0) digm(12,isz);
        x=fabs(x);
        i0=(int)(x/100.0);
        if(i0>0) 
	  { movr(-50,0);
	    digm(i0,isz);
	  }
        i1=(int)((x-100*i0)/10.0);
        if(i1>0 || i0>0) digm(i1,isz);
        i2=x-(double)(i0*100+i1*10);
        digm(i2,isz); 
        digm(10,isz);
        i3=(int)(x*10.0-(double)(i0*1000+i1*100+i2*10));
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
        putchar(0);
        putchar(0);      
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
        if(kkk == 0) 
	{ printf(" <cr> for next plot");
          fflush(stdout);
          wait=getchar();
	}
/*      return to vt240 mode      */
        putchar(27);
        putchar(3);   /* for xterm sessions */
        putchar(27);
        putchar(50);  /* for mac versatermpro sessions */
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
      { char ltext;
        int iysave;
        if(idev==1) 
        { mova(ix,iy);
          printf(text); }
          fflush(stdout);
        if(idev==0)
        { symps(ix,iy,ang,4,text); }
        if(idev==2 || idev==3)
        { ltext = *text;
	  mova(ix,iy);
          gdImageString(im,gdFontGiant,ixpj,iypj-10,text,black);
        }
      }

      eraspj()
        { im = gdImageCreate(900, 900);
          black = gdImageColorAllocate(im, 0, 0, 0);
          white = gdImageColorAllocate(im, 255, 255, 255);
          gdImageFilledRectangle(im,1,1,1798,1798,white);  
        }

      movapj(int ix,int iy)
       { ixpj=50+ix*800/2200;
         iypj=850 - (iy*800/3100);
       }

      drwapj(int ix,int iy)
       { int ixnext,iynext;
         ixnext = 50+ix*800/2200;
         iynext = 850 - (iy*800/3100);
         gdImageLine(im,ixpj,iypj,ixnext,iynext,black);
	 ixpj=ixnext;
	 iypj=iynext;
       }

      pntapj(int ix,int iy)
       { ixpj=50+ix*800/2200;
         iypj=850 - (iy*800/3100);
         gdImageLine(im,ixpj,iypj,ixpj+1,iypj+1,black);
       }
      plotpng(int k)
       { char string[20];
         int n;
	 n = k>>16;
         if(n==0)sprintf(string,"hist%d.png",0xffff&k);
         if(n==1)sprintf(string,"dot%d.png",0xffff&k);
         if(n==2)sprintf(string,"dxy%d.png",0xffff&k);
         if(n==3)sprintf(string,"plot%d.png",0xffff&k);
	 pngout = fopen(string, "wb");
         gdImagePng(im, pngout); 
         fclose(pngout);
       }

      plotjpg(int k)
       { char string[20];
         int n;
	 n = k>>16;
         if(n==0)sprintf(string,"hist%d.png",0xffff&k);
         if(n==1)sprintf(string,"dot%d.png",0xffff&k);
         if(n==2)sprintf(string,"dxy%d.png",0xffff&k);
         if(n==3)sprintf(string,"plot%d.png",0xffff&k);
        jpgout = fopen(string, "wb");
         gdImageJpeg(im, jpgout, -1);  
         fclose(jpgout);
         gdImageDestroy(im);
       }

      hstfill(int k)
       {if(idev==0) fillps(k);
       }
