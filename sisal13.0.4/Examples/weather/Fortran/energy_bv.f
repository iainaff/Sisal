      subroutine energy
c
      include 'param_dec'
      include 'bv_dec'
      real ktot
c
c---------------------------------------------------------------------
c
c   This routine calculates the global average kinetic energy, available
c   potential energy and the total energy. The spectral coefficients of 
c   the KE are in e 
c   There is a sqrt(2) factor from the normalisation  of Y(0,0)
      ptot = 0.
      ktot  = 0.
c     gmass is the globally integrated mass used for normalisation
      gmass = 4.*(zmean-h(1)) / asq

      do 100 j=jxmx,jx+1,-1
c  Sum over all the coefficients skipping the global mean term which is dealt
c  with separately. Loop runs down so that smaller terms are added first.
c  The coefficients with j > jx are complex and so to include the conjugates
c  there is a factor of 2.
      ptot = ptot + 2.*zm(j)*conjg(zm(j))
      ktot = ktot + 2.*real( (zm(j)-h(j)) * conjg(e(j)) )
100   continue
      do 200 j=jx,2,-1
      ptot = ptot + zm(j)*conjg(zm(j))
      ktot = ktot + real( (zm(j)-h(j)) * conjg(e(j)) )
200   continue

      ktot = ( e(1)*1.4142136*(zmean-h(1)) + ktot ) / gmass
      ptot = ptot / gmass
      print 50, ktot, ptot, ktot+ptot
   50 format('KE  ',e14.7,'      PE  ',e14.7,'      TOT  ',e14.7)
      return
      end
