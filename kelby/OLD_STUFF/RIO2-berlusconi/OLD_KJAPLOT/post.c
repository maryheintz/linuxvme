#include <stdio.h>
#include <math.h>
#include <strings.h>
      FILE *fpost;
      static double xlast=-99999.0,ylast=-99999.0,wlast=-999.0,xfac,yfac,width;
      static int ixx,iyy,iset=0,iflst=0,istrt=0;
      pfset(string)
      char string[80];
      { char percent='%';
        fpost=fopen(string,"w");
        fprintf(fpost," %c!PS-Adobe-2.0\n",percent);
        fprintf(fpost," statusdict begin false setduplexmode\n");
        fprintf(fpost," /st {stroke} def /lt {lineto} def\n");
        fprintf(fpost," /mt {moveto} def /rlt {rlineto} def 0 setlinejoin\n");
        fprintf(fpost,"0.0 0.0 0.0 setrgbcolor\n"); /* red green blue */
        istrt=1; }
      erasps()
      {
         char percent='%';
         if(istrt==0) 
           { fpost=fopen("kja.ps","w");
             fprintf(fpost," %c!PS-Adobe-2.0\n",percent);
             fprintf(fpost," statusdict begin false setduplexmode\n");
             fprintf(fpost," /st {stroke} def /lt {lineto} def\n");
             fprintf(fpost," /mt {moveto} def /rlt {rlineto} def 0 setlinejoin\n");
             fprintf(fpost,"0.0 0.0 0.0 setrgbcolor\n"); /* red green blue */
             istrt=1; }
         xfac=576.0/2400.0;
         yfac=792.0/3200.0;
         width=0.5;
         fprintf(fpost,"  0.5 setlinewidth\n");
         fflush(fpost);
         wlast=0.5;
         return(0);        
      }
      movaps(ix,iy)  
      int ix,iy;
      { ixx=ix;
        iyy=iy;
        return(0);
      }
      movrps(idx,idy)
      int idx,idy;
      { ixx=ixx+idx;
        iyy=iyy+idy;
        return(0);
      }
      drwaps(ix,iy)  
      int ix,iy;
      { char string[80];
        double x1,y1,x2,y2;
        x1=(double)ixx*xfac;
        y1=(double)iyy*yfac;
        ixx=ix;
        iyy=iy;
        x2=(double)ixx*xfac;
        y2=(double)iyy*yfac;
        if(wlast != 0.5)
          { fprintf(fpost,"  0.5 setlinewidth\n");
            wlast=0.5;}
        if(iset==0)
          { fprintf(fpost," newpath");
            fflush(fpost);
            iset=1; }
        if(x1 != xlast || y1 != ylast) 
         { if(iset == 2) { fprintf(fpost," stroke newpath");
                           fflush(fpost);}
           fprintf(fpost,"%8.3f %8.3f mt\n",x1,y1);
           fflush(fpost);
           fprintf(fpost,"%8.3f %8.3f lt\n",x2,y2);
           fflush(fpost);
         }
        else
         { fprintf(fpost,"%8.3f %8.3f lt\n",x2,y2);
           fflush(fpost);}
        xlast=x2;
        ylast=y2;
        iset=2;
        return(0);       
        }
      drwrps(idx,idy)
      int idx,idy;
      { int ix,iy;
        ix=ixx+idx;
        iy=iyy+idy;
        drwaps(ix,iy);
        return(0);
      }
      pntaps(ix,iy)  
      int ix,iy;
      { movaps(ix,iy);
        drwrps(1,0);
        return(0);
      }
      plotps()
      { int ipnt;
        if(iset == 2) 
        { fprintf(fpost," stroke\n");
          fflush(fpost);
          iset=0;
          iflst=0;
          wlast=-9999.0;
          xlast=-9999.0;
          ylast=-9999.0;
        }
        fprintf(fpost," showpage\n");
        fflush(fpost);
        iset=0;
        iflst=0;
        return(0);        
      }
      symps(ix,iy,ang,ifont,ichar)
      int ix,iy,ifont;
      double ang;
      char *ichar;
      { double x1,y1;
        char *font1="/Helvetica findfont 8 scalefont setfont\0";
        char *font2="/Helvetica findfont 9 scalefont setfont\0 ";
        char *font3="/Helvetica findfont 10 scalefont setfont\0";
        char *font4="/Helvetica findfont 12 scalefont setfont\0";
        char *font5="/Helvetica findfont 14 scalefont setfont\0";
        char *font6="/Helvetica findfont 18 scalefont setfont\0";
        char *font7="/Helvetica findfont 24 scalefont setfont\0";
        char *font8="/Symbol findfont 8 scalefont setfont\0";
        char *font9="/Symbol findfont 9 scalefont setfont\0";
        char *font10="/Symbol findfont 10 scalefont setfont\0";
        char *font11="/Symbol findfont 12 scalefont setfont\0";
        char *font12="/Symbol findfont 14 scalefont setfont\0";
        char *font13="/Symbol findfont 18 scalefont setfont\0";
        char *font14="/Symbol findfont 24 scalefont setfont\0";
        static double w[14]={0.8,0.9,1.0,1.2,1.4,1.8,2.4,0.8,0.9,1.0,1.2,
               1.4,1.8,2.4};
           if(ifont<1 || ifont>14) return(0);
           if(iset == 2) 
            { fprintf(fpost," stroke\n");
              fflush(fpost);
              iset=0; }
           if(1.5*w[ifont-1] != wlast) 
            { fprintf(fpost,"%8.2f setlinewidth\n",1.5*w[ifont-1]);
              fflush(fpost);
              wlast=1.5*w[ifont-1];
            }
           if(iflst != ifont) 
            { if(ifont==1)fprintf(fpost," %s\n",font1);
              if(ifont==2)fprintf(fpost," %s\n",font2);
              if(ifont==3)fprintf(fpost," %s\n",font3);
              if(ifont==4)fprintf(fpost," %s\n",font4);
              if(ifont==5)fprintf(fpost," %s\n",font5);
              if(ifont==6)fprintf(fpost," %s\n",font6);
              if(ifont==7)fprintf(fpost," %s\n",font7);
              if(ifont==8)fprintf(fpost," %s\n",font8);
              if(ifont==9)fprintf(fpost," %s\n",font9);
              if(ifont==10)fprintf(fpost," %s\n",font10);
              if(ifont==11)fprintf(fpost," %s\n",font11);
              if(ifont==12)fprintf(fpost," %s\n",font12);
              if(ifont==13)fprintf(fpost," %s\n",font13);
              if(ifont==14)fprintf(fpost," %s\n",font14);
              iflst=ifont;
              fflush(fpost);
            }
           ixx=ix;
           iyy=iy;
           x1=(double)ixx*xfac;
           y1=(double)iyy*yfac;
           if(ang == 90.0)
            {fprintf(fpost," gsave\n");
             fprintf(fpost,"%f %f translate\n",x1,y1);
             fprintf(fpost," 0.0 0.0 mt\n");
             fprintf(fpost," 90.0 rotate\n");
             fprintf(fpost,"( %s ) show\n",ichar);
             fprintf(fpost," grestore\n");
            }
           if(ang != 90.0)
            {fprintf(fpost,"%9.3f %9.3f mt\n",x1,y1);
             fflush(fpost);
             fprintf(fpost," (%s) show\n",ichar);
             fflush(fpost);
            }
           ixx=ixx+(int)(w[ifont-1]*20.0);
           xlast=-9999.0;
           ylast=-9999.0;
           return(0);
      }
fillps(k)
int k;  
 { if(k==1)
     {fprintf(fpost," closepath\n");
      fprintf(fpost," 1.0 1.0 0.0 setrgbcolor\n");
      fprintf(fpost," fill\n");
      fprintf(fpost," 0.0 0.0 0.0 setrgbcolor\n");
     }
   if(k==0) fprintf(fpost," newpath\n");
   return(0);
 }
