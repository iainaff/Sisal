      subroutine ordleg(sx,coa,ir)
c
      implicit double precision (a-h,o-z)
      irpp = ir + 1
      irppm = irpp - 1
      delta =dacos(coa)
      sqr2=dsqrt(2.0d0)
c
      theta=delta
      c1=sqr2
      do 20 n=1,irppm
      fn=n
      fn2=fn+fn
      fn2sq=fn2*fn2
      c1=c1*sqrt(1.0d0-1.0d0/fn2sq)
   20 continue
c
      n=irppm
      ang=fn*theta
      s1=0.0d0
      c4=1.0d0
      a=-1.0d0
      b=0.0d0
      n1=n+1
      do 27 kk=1,n1,2
      k=kk-1
      if (k.eq.n) c4=0.5d0*c4
      s1=s1+c4*dcos(ang)
      a=a+2.0d0
      b=b+1.0d0
      fk=k
      ang=theta*(fn-fk-2.0d0)
      c4=(a*(fn-b+1.0d0)/(b*(fn2-a)))*c4
   27 continue
      sx=s1*c1
c
      return
      end
