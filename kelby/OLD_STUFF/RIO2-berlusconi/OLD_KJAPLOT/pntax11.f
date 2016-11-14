      subroutine pntax11(ix,iy)
      common/lsr/lwidth,iunit
      call movax11(ix,iy)
      call drwax11(ix+lwidth,iy+lwidth)
      return
      end
