      subroutine frtosp
c
c     this routine calculates (accumulates) the fourier coefficients
c     into the full spectral coefficients required in the spectral
c     model.
c
      include 'param_dec'
      include 'bv_dec'
c
c     do seperate loops for symmetric and antisymmetric modes
c
c	-----------------------

      do 2000 isyasy=1,2
      if(isyasy.eq.1) ja=1
      if(isyasy.eq.2) ja=2
      if(isyasy.eq.1) jb=2
      if(isyasy.eq.2) jb=1
c
c     fourier coefficients are added or subtracted, as to whether we
c     are considering the symmetric or antisymmetric modes

      call frsymm(isyasy)

c       -----------------------

      if(isyasy.eq.2) go to 250
c
c     do mean components when F=fn + fs
c
      wa2=wocs(ihem)*alp(1)
      eri(1)=eri(1)+eg(1)*wa2
  250 continue

      do 110 m=1,mx
      mi=m+m
      mr=mi-1
      realm=m-1
      do 100 j=ja,jx,2
      if(j.eq.1.and.m.eq.1) go to 101
      jm=kmjx(m)+j
      jmi=jm+jm
      jmr=jmi-1
      jmx=kmjxx(m)+j
      realn=j+m-2
      gwplm=alp(jmx)*wocs(ihem)
      b=realm*gwplm
      ptri(jmr) = ptri(jmr) + b*pug(mi)
      ptri(jmi) = ptri(jmi) - b*pug(mr)
      ctri(jmr) = ctri(jmr) - b*pvg(mi)
      ctri(jmi) = ctri(jmi) + b*pvg(mr)
      ztri(jmr) = ztri(jmr) + b*zug(mi)
      ztri(jmi) = ztri(jmi) - b*zug(mr)
      eri(jmr)  = eri(jmr)  + gwplm*eg(mr)
      eri(jmi) =  eri(jmi)  + gwplm*eg(mi)
  101 continue
  100 continue
c
c     now do contribution from terms 'up' and 'down' (in j of 'alp')
c
      do 201 j=jb,jx,2
      if(j.eq.1.and.m.eq.1) go to 202
      jm=kmjx(m)+j
      jmi=jm+jm
      jmr=jmi-1
      jmx=kmjxx(m)+j
      realn=j+m-2
      alpm=0.0
      if(j.ne.1)alpm=alp(jmx-1)
      alpp=alp(jmx+1)
      a=   ((realn+1)*epsi(jmx)*alpm-realn*epsi(jmx+1)*alpp)*wocs(ihem)
      ptri(jmr) = ptri(jmr) + a * pvg(mr)
      ptri(jmi) = ptri(jmi) + a * pvg(mi)
      ctri(jmr) = ctri(jmr) + a * pug(mr)
      ctri(jmi) = ctri(jmi) + a * pug(mi)
      ztri(jmr) = ztri(jmr) + a * zvg(mr)
      ztri(jmi) = ztri(jmi) + a * zvg(mi)
  202 continue
  201 continue
  110 continue
 2000 continue

c	--------------------
      return
      end
