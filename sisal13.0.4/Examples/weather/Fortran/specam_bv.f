      subroutine specam
c
c  This routine prints out the zonal spectrum of the vorticity, divergence 
c  and height.

      include 'param_dec'
      include 'bv_dec'
      dimension ampvor(mx), ampk(mx),ampz(mx)

      do 440 m=1,mx
      ampk(m)=0.0
      ampvor(m)=0.0
      ampz(m)=0.0
      do 420 j=1,jx
      vorsq = 0.
      divsq =0.0
      zsq = 0.
      l=j+m-2
      il=kmjx(m)+j
      divsq = c(il) *conjg(c(il))
      vorsq = p(il) *conjg(p(il))
      zsq   = z(il) *conjg(z(il))
      if(m.gt.1) then
      vorsq = vorsq+vorsq
      divsq = divsq + divsq
      zsq =   zsq   + zsq
      end if
      ampvor(m) = ampvor(m) + vorsq
      ampk(m) =ampk(m)+divsq
      ampz(m) = ampz(m) + zsq
  420 continue
  440 continue
      do 650 m=1,mx
      ampvor(m)= sqrt(ampvor(m))/ww
      ampk(m)  = sqrt(ampk(m))/ww *10.0
      ampz(m) = sqrt(ampz(m))*asq/grav
  650 continue
      print 500
  500 format('Spectral decomposition -summed l amplitudes for each',
     *' m ')
  502 format(' ', 8(f8.3 ,'(mp=',i2,')',1x),/,
     *' ', 8(f8.3,'(mp=',i2,')',1x))
      print 504
  504 format('Vorticity amplitudes (relative to omega) ')
      write(*,502) (ampvor(mp),mp,mp=1,mx)
      print 505
  505 format('Divergence amplitudes (relative to omega/10)')
      write(*,502) (ampk(mp),mp,mp=1,mx)
      print 506
  506 format('Free surface height amplitudes (m) ')
      write(*,502) (ampz(mp),mp,mp=1,mx)
      return
      end
