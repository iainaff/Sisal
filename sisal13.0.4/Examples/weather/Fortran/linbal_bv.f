      subroutine linbal
c
c     this routine calculates the initial gepotential field using the linear
c     balance equation. (i.e. requiring D=0 and dD/dt=0 ).
c
      include 'param_dec'
      include 'bv_dec'
c
c------------------------------------------------------------------
c
      do 20 m=1,mx
      do 30 j=1,jx
      if(j.eq.1.and.m.eq.1) go to 30
      jm= kmjx(m) +j
      jmx=kmjxx(m)+j
      realn=m+j-2
      z(jm) = -tw * ((realn+1)/realn * epsi(jmx)*p(jm-1)  +  
     &	                realn/(realn+1) * epsi(jmx+1)*p(jm+1)) /
     &			(realn*(realn+1))
c     if(m.gt.1) z(jm) = z(jm)*sqrt(2.)
   30 continue
   20 continue
       
      return
      end
