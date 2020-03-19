      subroutine angmom
c
c   This routine calculates the global average relative and 'omega'
c   angular momenta.
c   The formulae and constants are from Bourke, 1974

      include 'param_dec'
      include 'bv_dec'
      parameter ( c1 = 0.942809, c2 = .421637 )
c
c---------------------------------------------------------------------
c
      atot  = 0.
c     gmass is the globally integrated mass used for normalisation
      gmass = 4.*(zmean-h(1)) / asq
      do 100 j=jxmx,jx+1,-1
c  Sum over all the coefficients skipping the global mean term which is dealt
c  with separately. Loop runs down so that smaller terms are added first.
c  The coefficients with j > jx are complex and so to include the conjugates
c  there is a factor of 2.
      atot = atot + 2.*real( (zm(j)-h(j)) * conjg(u(j)) )
100   continue
      do 200 j=jx,2,-1
      atot = atot + real( (zm(j)-h(j)) * conjg(u(j)) )
200   continue
      atot = ( u(1)*1.4142136*(zmean-h(1)) + atot ) / gmass
      atot1 = ( u(1)*1.4142136*(zmean-h(1))  ) / gmass
c  The term in wtot due to solid body rotation of constant depth fluid can't
c  change and so isn't included. It would completely dominate everything
c  else.
      wtot = ww * ( - c2*(z(3)-h(3)) ) / gmass
c  Scale to make the numbers of comparable ZZ
      atot = atot * 1e-5
      atot1 = atot1 * 1e-5
      wtot = wtot * 1e-5
      print 50, atot, wtot, atot+wtot
      print 51, atot1, wtot, atot1+wtot
   50 format('RAM  ',e14.7,'  WAM  ',e14.7,'  TOT  ',e14.7)
   51 format('RAM1 ',e14.7,'  WAM  ',e14.7,'  TOT1 ',e14.7)
      return
      end
