      subroutine ass(alp,ir,irmax2)
      dimension alp(1)
      irp1=ir+1
      lpfin =irp1
      if(mod(ir,2).eq.0)lpfin=ir+2
      do 50 mp=1,irp1
      m= mp-1
      ipm =m*irmax2
      do 50 lp=2,lpfin,2
      ilm = ipm +lp
   50 alp(ilm) = -alp(ilm)
      return
      end
