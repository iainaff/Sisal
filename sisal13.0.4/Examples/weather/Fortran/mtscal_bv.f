      subroutine mtscal(kount,hinit,h,ns)
c     This routine smoothly introduces the mountains
c     ns is the number of steps over which the mountains are introduced.
      include 'param_dec'
      parameter(vfact=1.714e25)
      complex hinit(jx,mx),h(jx,mx)

      var=0.0
      scal = float(kount)/float(ns)
      do 60 m=1,mx
      do 60 j=1,jx
      if(j.eq.1.and.m.eq.1) go to 60
      h(j,m)=scal*hinit(j,m)
      var=var+ cabs(h(j,m))**2
   60 continue
      write(*,1000) kount,var*vfact
 1000 format(' Step = ',i4,' Mountain var = ',e10.4)
      end
