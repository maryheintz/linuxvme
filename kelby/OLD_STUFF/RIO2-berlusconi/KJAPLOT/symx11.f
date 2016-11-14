      subroutine symx11(ix,iy,angle,ifont,n,str)
      character*(*) str
      dimension nsize(17)
      character*40 fonts(2,17)
      common/picscl/facx,facy,ixsize,iysize 
      data nsize/25,30,35,40,45,
     $           25,30,35,40,45,
     $           25,30,35,40,45,
     $           20,20/
      data fonts/
     $ '-Adobe-Helvetica-Medium-R-Normal--*-100-',
     $ '*-*-P-*-ISO8859-1                       ',
     $ '-Adobe-Helvetica-Medium-R-Normal--*-120-',
     $ '*-*-P-*-ISO8859-1                       ',
     $ '-Adobe-Helvetica-Medium-R-Normal--*-140-',
     $ '*-*-P-*-ISO8859-1                       ',
     $ '-Adobe-Helvetica-Medium-R-Normal--*-180-',
     $ '*-*-P-*-ISO8859-1                       ',
     $ '-Adobe-Helvetica-Medium-R-Normal--*-240-',
     $ '*-*-P-*-ISO8859-1                       ',

     $ '-Adobe-Helvetica-Bold-R-Normal--*-100-*-',
     $ '*-P-*-ISO8859-1                         ',
     $ '-Adobe-Helvetica-Bold-R-Normal--*-120-*-',
     $ '*-P-*-ISO8859-1                         ',
     $ '-Adobe-Helvetica-Bold-R-Normal--*-140-*-',
     $ '*-P-*-ISO8859-1                         ',
     $ '-Adobe-Helvetica-Bold-R-Normal--*-180-*-',
     $ '*-P-*-ISO8859-1                         ',
     $ '-Adobe-Helvetica-Bold-R-Normal--*-240-*-',
     $ '*-P-*-ISO8859-1                         ',

     $ '-Adobe-Symbol-Medium-R-Normal--*-100-*-*',
     $ '-P-*-ADOBE-FONTSPECIFIC                 ',
     $ '-Adobe-Symbol-Medium-R-Normal--*-120-*-*',
     $ '-P-*-ADOBE-FONTSPECIFIC                 ',
     $ '-Adobe-Symbol-Medium-R-Normal--*-140-*-*',
     $ '-P-*-ADOBE-FONTSPECIFIC                 ',
     $ '-Adobe-Symbol-Medium-R-Normal--*-180-*-*',
     $ '-P-*-ADOBE-FONTSPECIFIC                 ',
     $ '-Adobe-Symbol-Medium-R-Normal--*-240-*-*',
     $ '-P-*-ADOBE-FONTSPECIFIC                 ',

     $ '-Adobe-Helvetica-Medium-R-Normal--*-80-*',
     $ '-*-P-*-ISO8859-1                        ',
     $ '-Adobe-Helvetica-Bold-R-Normal--*-80-*-*',
     $ '-P-*-ISO8859-1                          '/
      data iflst/0/
      if(ifont.eq.iflst) go to 10
      iii=ixsettf(1,80,fonts(1,ifont))
c      write(7,1000) ifont,iii
c 1000 format(' font',i3,'  ifail=',i2)
      iflst=ifont
   10 continue
      ixx=ix*facx
      iyy=iysize-facy*iy
      call ixtext(0,ixx,iyy,angle,1.0,n,str)
      call movrx11(nsize(ifont)*n,0)
      if(n.eq.1.and.str.eq.'.') call movrx11(-nsize(ifont)/2,0)
      return
      end





