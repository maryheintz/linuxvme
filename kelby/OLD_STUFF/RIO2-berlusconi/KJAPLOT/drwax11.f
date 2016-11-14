      subroutine drwax11(ix,iy)
      integer*2 xy(2,2)
      common/picscl/facx,facy,ixsize,iysize 
      common/lsr/lwidth,iunit 
      common/lsrpos/ixx,iyy   
      data lwlast/0/
      if(lwidth.eq.lwlast) go to 10     
      lwlast=lwidth 
      call ixsetln(lwidth)
   10 continue      
      xy(1,1)=float(ixx)*facx
      xy(2,1)=float(iysize)-float(iyy)*facy
      xy(1,2)=float(ix)*facx
      xy(2,2)=float(iysize)-float(iy)*facy
      call ixline(2,xy)
      ixx=ix        
      iyy=iy        
      return        
      end 
