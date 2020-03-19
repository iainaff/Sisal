      subroutine gaussg(nzero,fs,wts,sias,rads,coangs)
c
c     this routine calculates the cosine of the colatitudes "f" and
c     the weights "wt" , for the gaussian quadrature with "nzero" point
c     between pole and equator
c     this version works in double precision, but communicates
c     outside in single
c
c
      include 'param_dec'
c
      implicit double precision (a-h,o-z)
      real fs,wts,sias,rads,coangs
c      common / alpwor / f(iyh),wt(iyh),sia(iyh),rad(iyh),coang(iyh)
c	Pau added
      common / alpwor / f(iyh),wt(iyh),sia(iyh),rad(iyh),coang(iyh),
     $                  dummy(jxxmx-5*iyh)
      dimension fs(iyh),wts(iyh),sias(iyh),rads(iyh),coangs(iyh)
c
c----------------------------------------------------------------------
c
c
      xlim=1.e-12
      ir = nzero+nzero
      fi=ir
      fi1=fi+1.0d0
      pi=3.141592653589793d0
      piov2 = pi * 0.5d0
      fn=piov2/float(nzero)
      do 10 i=1,nzero
   10 wt(i)=i-0.5d0
      do 20 i=1,nzero
   20 f(i)=dsin(wt(i)*fn+piov2)
      dn = fi/dsqrt(4.0d0*fi*fi-1.0d0)
      dn1=fi1/dsqrt(4.0d0*fi1*fi1-1.0d0)
      a = dn1*fi
      b = dn*fi1
      irp = ir + 1
      irm = ir -1
      do 2 i=1,nzero
    5 call ordleg(g,f(i),ir)
      call ordleg(gm,f(i),irm)
      call ordleg(gp,f(i),irp)
      gt = (f(i)*f(i)-1.0d0) / (a*gp-b*gm)
      ftemp = f(i) - g*gt
      gtemp = f(i) - ftemp
      f(i) = ftemp
      if(dabs(gtemp).gt.xlim) go to 5
    2 continue
      do 6 i=1,nzero
      a=2.0  *(1.0  -f(i)*f(i))
      call ordleg(b,f(i),irm)
      b = b*b*fi*fi
      wt(i)=a*(fi-0.5d0)/b
      rad(i) =dacos(f(i))
      coang(i)=rad(i)*180.0d0 /pi
    6 sia(i) =dsin(rad(i))
c
c     now convert to single precision
c
      do 30 i=1,nzero
      fs    (i)=sngl(f(i))
      wts   (i)=sngl(wt(i))
      rads  (i)=sngl(rad(i))
      coangs(i)=sngl(coang(i))
   30 sias  (i)=sngl(sia(i))
      return
      end
