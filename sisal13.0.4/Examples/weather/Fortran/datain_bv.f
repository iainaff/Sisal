      subroutine  datain
c
c     This routine reads in the model initail conditions from the file
c     'datastart'.
c
      include 'param_dec'
      include 'bv_dec'
      complex pau(256)
      indcb=2
c     open(unit=indcb,file='fdatastart',status='old',
c    $     form='unformatted')
      open(unit=indcb,file='fdatastart',status='old',form='formatted')

      nrun=ihirmx
      rewind indcb

c      read(indcb,err=1000,end=1000) idata
c 	Pau
      do 1 i = 1,28
1     read(indcb,1009) idata(i)

      ihirmx=nrun
c	Pau added *
c       Here I add in pau as a complex array of size 256 as given by the input
c 	and leave p having the size jxmx, hence all elements of indices > 256
c 	are filled up with complex zero
c      read(indcb,*,err=1000,end=1000) p
      read(indcb,1020,err=1000,end=1000) pau
      do 899 i=1,256
  899 p(i)=pau(i)

c If it's a cold start the c and z fields need not be present.
      if(istart.eq.1) then
c      read(indcb,*,err=1000,end=1000) c
      read(indcb,1020,err=1000,end=1000) c
c      read(indcb,*,err=1000,end=1000) z
      read(indcb,1020,err=1000,end=1000) z
      end if
c     do 100 j=1,jxmx
c     p(j) = (0.,0.)
c     c(j) = (0.,0.)
c100  continue
c     print*, '*******************************************'
c     print*, 'DATAIN MODIFIED, P AND C SET TO ZERO  '
c     print*, '*******************************************'
      return
 1000 print *, ' stop: err or eoi on read in datain.f'
c 1020 format(5e15.7)
c	Pau
 1009 format(I12)
 1020 format(5e15.7)
      stop
      end
