      subroutine xlin
c
c     this routine replaces nonlin in a linear version of the model.
c     It performs all the eXtra linear operations required in the
c     spectral model.  
c
      include 'param_dec'
      include 'bv_dec'
c
c------------------------------------------------------------------
c
      do 370 il=1,jxmx
      pt(il)=(0.0,0.0)
      ct(il)=(0.0,0.0)
      zt(il)=(0.0,0.0)
      e(il)=(0.0,0.0)
  370 continue
      rewind 2
      do 300 ill=1,ilath
      read(2,err=605) alp
      go to 606
  605 write(*,610)
      stop
  606 do 300 inhsh=1,2
      if(inhsh.eq.2) call ass(alp,ir,irmax2)
      ihem= ill
      if(inhsh.eq.2) ihem= iy+1-ill
  610 format(' stop : bufferin of alp failed in nonlin')
c============================================================================
      call sptofr
c============================================================================
c     In the linear model the terms puf etc, can be formed by direct mult'n
c     of the spectral coefficients. There is no need to call the fft or vertig.
c     All the multiplications are of a real by a complex number so we save
c     operations by doing them explicitly with the grid arrays.

      pug(1) = pg(1)*ug(1)
      pvg(1) = pg(1)*vg(1)
      zug(1) = zg(1)*ug(1)
      zvg(1) = zg(1)*vg(1)
      eg(1)  = ug(1)*ug(1) + vg(1)*vg(1)
      pug(2) = 0.
      pvg(2) = 0.
      zug(2) = 0.
      zvg(2) = 0.
      eg(2)  = 0.
      do 400 jm = 3,mx2
      pug(jm) = pg(1)*ug(jm) + pg(jm)*ug(1)
      pvg(jm) = pg(1)*vg(jm) + pg(jm)*vg(1)
      zug(jm) = zg(1)*ug(jm) + zg(jm)*ug(1)
      zvg(jm) = zg(1)*vg(jm) + zg(jm)*vg(1)
      eg(jm)  = 2.*( ug(1)*ug(jm) + vg(1)*vg(jm) )
  400 continue
c     if ( ihem.eq.1) then
c     write(8,1000) kount,zg(1),ug(1),vg(1)
c     write(8,1100) zg(3),zg(4),ug(3),ug(4),vg(3),vg(4)
c     write(8,1200) zug(3),zug(4),zvg(3),zvg(4)
c     end if
 1000 format(i4,3e12.4)
 1100 format(4x,6e12.4)
 1200 format(4x,4e12.4)
c
c     if in NH, save fourier coefficients for later
c     if in SH, reverse 'alp' to return to NH values and
c     call 'frtosp'
c
      if(inhsh.eq.1) call keepnh
      if(inhsh.eq.2) call ass(alp,ires,jxx)
      if(inhsh.eq.2) call frtosp
  300 continue
      return
      end
