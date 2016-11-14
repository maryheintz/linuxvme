      subroutine plotx11       
c     update the display
      call ixupdwi
      read(5,1000) iwait
 1000 format(a1)
      return        
      end 

