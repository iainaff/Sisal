      subroutine lgndre(ps,coas,sias,deltas,ir,irmax2)
c
c     in this version of lgndre all internal calculations are done in
c     double precision but communication outside is single
c
      include 'param_dec'
c
      implicit  double precision(a-h,o-z)
      real ps,coas,sias,deltas
      dimension ps(1)
      common / alpwor / p(jxxmx)
c
c
      coa  =coas
      sia  =sias
      delta=deltas
      irpp=ir+2
      pi=3.141592653589793
      sqr2=dsqrt(2.0d0)
c
      theta=delta
      c1=sqr2
      p(1)=1.0d0/c1
      do 20 n=1,irpp
      fn=n
      fn2=2.0d0*fn
      fn2sq=fn2*fn2
      c1=c1*dsqrt(1.0d0-1.0d0/fn2sq)
      c3=c1/dsqrt(fn*(fn+1.0d0))
      ang=fn*theta
      s1=0.0d0
      s2=0.0d0
      c4=1.0d0
      c5=fn
      a=-1.0d0
      b=0.0d0
      n1=n+1
      do 27 kk=1,n1,2
      k=kk-1
      s2=s2+c5*dsin(ang)*c4
      if (k.eq.n) c4=0.5d0*c4
      s1=s1+c4*dcos(ang)
      a=a+2.0d0
      b=b+1.0d0
      fk=k
      ang=theta*(fn-fk-2.0d0)
      c4=(a*(fn-b+1.0d0)/(b*(fn2-a)))*c4
      c5=c5-2.0d0
   27 continue
      if(n-irpp)23,21,20
   23 p(n+1)=s1*c1
   21 p(n+irmax2)=s2*c3
   20 continue
   22 continue
c
c     ***** 20 has set up legendre polynomials for m=0 and m=1
c
      if(ir   .eq.2)go to 100
      do 40 m=2,ir
   39 fm=m
      fm1=fm-1.0d0
      fm2=fm-2.0d0
      fm3=fm-3.0d0
      mm1=m-1
      m1=m+1
      c6=dsqrt((2.0d0*fm+1.0d0)/(2.0d0*fm))
      p(irmax2*m+1)=c6*sia*p(irmax2*mm1+1)
   41 continue
      mpir=m+ir   +1
      do 40 l=m1,mpir
      fn=l
      c7=(fn*2.0d0+1.0d0)/(fn*2.0d0-1.0d0)
      c8=(fm1+fn)/((fm+fn)*(fm2+fn))
      c=dsqrt((fn*2.0d0+1.0d0)/(fn*2.0d0-3.0d0)*c8*(fm3+fn))
      d=dsqrt(c7*c8*(fn-fm1))
      e=dsqrt(c7*(fn-fm)/(fn+fm))
      lm=irmax2*m+l-m+1
      lmm2=irmax2*(m-2)+l-m+3
      lm1mm2=lmm2-1
      lm2mm2=lm1mm2-1
      lm1m=lm-1
      if(l-mpir)43,42,40
   43 p(lm)=c*p(lm2mm2)-d*p(lm1mm2)*coa+e*p(lm1m) *coa
      go to 40
c
c     ***** belousov equation 11
c
   42 a=dsqrt((fn*fn-0.25d0)/(fn*fn-fm*fm))
      b=dsqrt((2.0d0*fn+1.0d0)*(fn-fm-1.0d0)*(fn+fm1)/
     1((2.0d0*fn-3.0d0)*(fn-fm)*(fn+fm)))
      lm2m=lm1m-1
      p(lm)=2.0d0*a*coa*p(lm1m)-b*p(lm2m)
   40 continue
  100 continue
c
c     finally put into single precision array
c
      do 110 j=1,jxxmx
  110 ps(j)=sngl (p(j))
      return
      end
