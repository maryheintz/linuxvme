      subroutine erasx11
      character*7 display
      character*30 dval
      data display/'DISPLAY'/
      common/picscl/facx,facy,ixsize,iysize
      common/lsr/lwidth,iunit
      common/lsrpos/ixx,iyy
      common/mas11/im11
      common/cornr/icdig
      data im11,icdig/0,0/ 
      data istrt/0/                                                 
c      write(7,*) istrt
      if(istrt.ne.0) go to 10
c      call errset
      ixsize=700
      iysize=700
      call getenv(display,dval)
      if(dval(1:8).eq.'hepterm1') then
       ixsize=1000
       iysize=1000
      elseif(dval(1:8).eq.'hepterm2') then
       ixsize=1000
       iysize=1000
      elseif(dval(1:6).eq.'uchepp') then
       ixsize=800
       iysize=800
      elseif(dval(1:6).eq.'uchepo') then
       ixsize=800
       iysize=800
      elseif(dval(1:5).eq.'opal1') then
       ixsize=700
       iysize=700
      elseif(dval(1:3).eq.'hep'.and.dval(4:7).ne.'term') then
       ixsize=800
       iysize=800
      endif
c      write(6,1000) dval,ixsize,iysize,dval(1:6)
c 1000 format(' dval=',a30,2i10,2x,a6)
      istrt=1
      lwidth=2
      facx=float(ixsize)/2200.0
      facy=float(iysize)/3000.0
c     close any old display
      call ixclsds
c     open the display
      istat=ixopnds(0,display)
c     if(istat.eq.-1) go to 99
c     open the window
      iwn=ixopnwi(150,0,ixsize,iysize,7,'KJAPLOT')
c     select the window
      call ixselwi(iwn)
c     clear the window
      call ixclrwi
c     set clipping region
      call ixclip(iwn,0,0,ixsize,iysize)
c     set color intensities
      call ixsetco(1,0.0,0.0,0.0)
      call ixsetfc(1)
      call ixsetlc(1)
      call ixsetmc(1)
      call ixsettc(1)
      call sleep(1)
c      go to 11
 10   continue
c     clear the window
      call ixclrwi
c     select the window
 11   call ixselwi(iwn)
c     set the line width
      call ixsetln(lwidth)
c     take some time by fetching back window position and size
c      otherwise first part of draw gets lost
      call ixgetge(iwn,ixget,iyget,iwget,ihget)
c      call ixupdwi
      return
 99   write(6,2000) istat
 2000 format(' erasx11 trouble istst=',i5)
      stop
      end



