      subroutine initpv
c
c  This routine copies the current time step values into the previous step
c  ones at model startup.

      include 'param_dec'
      include 'bv_dec'
c
      do 310 jm=1,jxmx
      pm(jm)=p(jm)
      p1(jm)=p(jm)
      cm(jm)=c(jm)
      zm(jm)=z(jm)
  310 continue
      return
      end
