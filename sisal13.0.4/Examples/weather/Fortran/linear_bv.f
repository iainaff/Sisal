      subroutine linear
c
c  This routine adds the linear terms to the time derivatives
      include 'param_dec'
      include 'bv_dec'
      complex cilm,cilp,pilm,pilp

      do 200 m=1,mx
      do 200 j=1,jx
      l = j+m-2
      kl=ksq(l)
      il=kmjx(m)+j
      ilp=il+1
      ilm=il-1
      ialp=kmjxx(m)+j
c	--------------------------
      if(j.eq.jx) then
      pilp=(0.0)
      cilp=(0.0)
      else
      cilp= c(ilp)
      pilp= p(ilp)
      end if
      if(j.eq.1) then
      pilm=(0.0)
      cilm=(0.0)
      else
      cilm= c(ilm)
      pilm= p(ilm)
      end if
      pt(il)=pt(il) - tw*(epsi(ialp)*cilm+epsi(ialp+1)*cilp+v(ialp))

c	------------------------------
      ct(il)=ct(il)  + tw*(epsi(ialp)*pilm+epsi(ialp+1)*pilp-u(ialp))
     & 	             + kl*e(il)*0.5 

c  NB other linear terms are included in the implicit time step.
  200 continue
      return
      end
