      subroutine drwrx11(idx,idy)        
      common/lsr/lwidth,iunit 
      common/lsrpos/ixx,iyy   
      ix=ixx+idx    
      iy=iyy+idy    
      call drwax11(ix,iy)        
      return        
      end 

