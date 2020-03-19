      subroutine vduvjm(ujm,vjm,j,m,k)
c
c     this routine calculates the (j,m) coefficient of U and V, at
c     the k th. level, from spectral feilds of vorticity (p) and
c     divergence (c).
c     The routine can consider values of index j up to jxx
c
      include 'param_dec'
      include 'bv_dec'
      complex ujm,vjm
      complex pd,pc,pu,cd,cc,cu,coeffd,coeffc,coeffu,zero,iz
      data iz,zero / (0.0,1.0), (0.0,0.0)/

      realm=m-1
      nreal=j+m-2
      realn=float(nreal)
      jm=(m-1)*jx+j
      jmx=(m-1)*jxx+j
      if(j.ne.1) go to 10
      coeffd=zero
      if(nreal.eq.0) coeffc=zero
      if(nreal.ne.0) coeffc=iz*realm / (realn*(realn+1.0))
      coeffu=epsi(jmx+1) / (realn + 1.0)
      pd=zero
      pc=p(jm)
      pu=p(jm+1)
      cd=zero
      cc=c(jm)
      cu=c(jm+1)
      go to 100
   10 if(j.ne.jx) go to 20
      coeffd=epsi(jmx) / realn
      coeffc=iz*realm / (realn*(realn+1.0))
      coeffu=epsi(jmx+1) / (realn+1.0)
      pd=p(jm-1)
      pc=p(jm)
      pu=zero
      cd=c(jm-1)
      cc=c(jm)
      cu=zero
      go to 100
   20 if(j.ne.jxx) go to 30
      coeffd=epsi(jmx) / realn
      coeffc=zero
      coeffu=zero
      pd=p(jm-1)
      pc=zero
      pu=zero
      cd=c(jm-1)
      cc=zero
      cu=zero
      go to 100
   30 coeffd=epsi(jmx) / realn
      coeffc=iz*realm / (realn*(realn+1.0))
      coeffu=epsi(jmx+1) / (realn+1.0)
      pd=p(jm-1)
      pc=p(jm  )
      pu=p(jm+1)
      cd=c(jm-1)
      cc=c(jm  )
      cu=c(jm+1)
  100 continue
      ujm= -coeffd*pd + coeffu*pu - coeffc*cc
      vjm=  coeffd*cd - coeffu*cu - coeffc*pc
      return
      end
