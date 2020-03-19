      subroutine tstep
c
c   This routine performs a model timestep in the spectral model.
c   Vorticity is treated explicitly and divergence and height semi-implicitly.
c   There is an option to make all time steps explicit.
c   The time filtering, linear drag and diffusion are all done here.
c   Note that the drag is applied only to the momentum terms ( p & c).
c   On the first time step a correction is made to the linear balance with
c   the divergence again set to zero and the height found
c   from the linear balance equation.
c
      include 'param_dec'
      include 'bv_dec'
      complex ppv,ccv,zzv
      real kl
c
c------------------------------------------------------------------
 
      if(ifirst.eq.0) then
c     Do a normal centered difference time step
      deltt2=delt*2.0
      else
c     Do a forward time step for the first one. Note that the same code can
c     be used for the forward and centered steps because at the start the
c     current and previous step values are the same.
      deltt2=delt
      end if
      deltt = deltt2 * 0.5

      do 1000 m=1,mx
      if(m.eq.1 .and. izon.eq.1) go to 1000
c     If izon then don't change the zonal terms
      do 1001 j=1,jx
      jm=kmjx(m)+j
      if(jm.eq.1) go to 1001

      kl = float(ksq(j+m-2))
      dkl = kl-2.
c     Apply the linear drag and the horizontal diffusion 
c     Note that for the vorticity the drag relaxes back to the initial
c     state, not necessarily to zero. The divergence is relaxed towards
c     zero.
      pt(jm) = pt(jm) - dkl*hdiff*pm(jm) - hdrag*(pm(jm)-p1(jm))
      ct(jm) = ct(jm) - ( hdrag + dkl*hdiff )*cm(jm)
      zt(jm) = zt(jm) - ( dkl*hdiff )*zm(jm)


      ppv=pm(jm)+deltt2*pt(jm)
      if ( imp.eq.1 ) then
c         Do a semi-implicit time step
      ccv = ( cm(jm) + 
     &deltt2*( ct(jm) + kl*( zm(jm) + deltt*(zt(jm)-zmean*cm(jm)*.5))))/
     & 	        ( 1. + deltt*deltt*kl*zmean )
      zzv = zm(jm) + deltt2*( zt(jm) - zmean*(cm(jm)+ccv)*.5 )
      else
c         Do an explicit time step
          ccv=cm(jm)+deltt2*(ct(jm)+kl*z(jm))
          zzv = zm(jm) + deltt2*( zt(jm) - zmean*c(jm) )
      end if
 
      if (ifirst.eq.0) then
c     Here we do the Asselin time filtering. Note we filter AND update
c     ( [alpha]m=[alpha] ), so the '...m' appears on lhs rather than the
c     current values. nb that ppv is the future p value at this stage.

      pm(jm)=p(jm) + vnu*(pm(jm)-2.*p(jm)+ppv)
      cm(jm)=c(jm) + vnu*(cm(jm)-2.*c(jm)+ccv)
      zm(jm)=z(jm) + vnu*(zm(jm)-2.*z(jm)+zzv)
  200 continue
      p(jm)=ppv
      c(jm)=ccv
      z(jm)=zzv

      else
c     Do a forward time step w/o updating the previous step values or time
c     filtering

      p(jm)=ppv
c     We apply non-linear balance at the first time step, thus c1=0
c     z0 = ct/kl and z1= z0. Setting z1 = z0 is an error but in simple
c     tests it didn't seem to matter much.
      if (istart.eq.0) then
        c(jm) = (0.,0.)
        zm(jm) = -ct(jm)/kl
        z(jm) = zm(jm)
      else
        c(jm) = ccv
        z(jm) = zzv
      end if
      end if
 1001 continue
 1000 continue
c
c     turn off forward timestep flag (may already be off)
      ifirst=0
      return
      end
