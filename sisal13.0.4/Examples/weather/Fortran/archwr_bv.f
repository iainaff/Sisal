      subroutine  archwr
c
c  This routine writes the current model time step to disk as a history file.
c  This is done at intervals specified by the idumpt variable.

      include 'param_dec'
      include 'bv_dec'
      character*4 runno
      character*20 ofname,oldnam
      character*6 dayno
c
c Test unit odcb to see what file it is connected to.  If it is not 
c connected to a file, or if it is connected to a file with a name 
c starting with "bv", then open a new "bv" file with an appropriate
c name.  Otherwise, assume that the right file has already been opened,
c and just rewind it and write to it.
c
      odcb=4
      inquire(unit=odcb,name=oldnam)
      if(oldnam(1:2).ne.'  '.and.oldnam(1:2).ne.'bv') go to 303
      write(runno,310) ihir
 310  format(i4)
      fdayno = (ihkont*idelt)/3600.0/24.0
      write(dayno,320) fdayno
 320  format(f6.2)
      ofname = 'bv'//runno//'.'//dayno
      do 301 ib=3,10
	  if(ofname(ib:ib).eq.' ') ofname(ib:ib) = '0'
 301  continue
      if(ofname(11:13).eq.'.00') ofname(11:13) = '   '
      open(unit=odcb,file=ofname,status='unknown',form='unformatted')

c	Pau added
c      open(unit=odcb,file=ofname,status='unknown',form='formatted')
c	-----------------------------
 303  rewind(odcb)
      idata( 9)=ihkont
      write(odcb,err=1000)idata
      write(odcb,err=1000) p
      write(odcb,err=1000) c
      write(odcb,err=1000) z
c      write(odcb,*,err=1000)idata
c      write(odcb,*,err=1000) p
c      write(odcb,*,err=1000) c
c      write(odcb,*,err=1000) z
      endfile odcb
      close(unit=odcb)
      return
 1000 print *,' stop: error on writing in archwr.f'
      stop
      end
