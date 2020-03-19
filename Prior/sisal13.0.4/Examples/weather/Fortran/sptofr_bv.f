      subroutine sptofr
c
c     this routine calculates the fourier coefficients of certain
c     variables from the full spectral arrays, as required in the
c     spectral model
c     The variables considered are p,u,v and h
c     The latitude at which the calculation is being carried out is
c     that relevant to the 'alp'
c     Note that we reverse the direction of summation over 'j' to
c     consider the smaller contributions first (for accuracy)
c
c
      include 'param_dec'
      include 'bv_dec'
c
c------------------------------------------------------------------------
c
      do 20 m=1,mx
      mi=m+m
      mr=mi-1
      pg(mr)=0.0
      pg(mi)=0.0
      zg(mr)=0.0
      zg(mi)=0.0
      do 30 j=jx,1,-1
      if(j.eq.1.and.m.eq.1) go to 30
      jm= kmjx(m) +j
      jmi=jm+jm
      jmr=jmi-1
      jmx=kmjxx(m)+j
      pg(mr)=pg(mr) + alp(jmx)*pri(jmr)
      pg(mi)=pg(mi) + alp(jmx)*pri(jmi)
      zg(mr)=zg(mr) + alp(jmx)*zri(jmr)
      zg(mi)=zg(mi) + alp(jmx)*zri(jmi)
   30 continue
   20 continue
c
c     now calculate the Fourier coefficients of 'u' and 'v'.
c     Note that these have an extra 'j' value
c
      do 120 m=1,mx
      mi=m+m
      mr=mi-1
      ug(mr)=0.0
      ug(mi)=0.0
      vg(mr)=0.0
      vg(mi)=0.0
      do 130 j=jxx,1,-1
      jmx= kmjxx(m) +j
      jmi=jmx+jmx
      jmr=jmi-1
      ug(mr)=ug(mr) + alp(jmx)*uri(jmr)
      ug(mi)=ug(mi) + alp(jmx)*uri(jmi)
      vg(mr)=vg(mr) + alp(jmx)*vri(jmr)
      vg(mi)=vg(mi) + alp(jmx)*vri(jmi)
  130 continue
  120 continue
      return
      end
