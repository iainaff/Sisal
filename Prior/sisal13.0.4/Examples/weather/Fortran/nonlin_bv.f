	subroutine nonlin
c
c
c     this routine performs all the nonlinear operations required in the
c     spectral model.  
c
      include 'param_dec'
      include 'bv_dec'
c
c------------------------------------------------------------------
c

      do 370 il=1,jxmx
      pt(il)=(0.0,0.0)
      ct(il)=(0.0,0.0)
      zt(il)=(0.0,0.0)
      e(il)=(0.0,0.0)
  370 continue
      rewind 2
      do 300 ill=1,ilath
      read(2,err=605) alp
c      read(2,*,err=605) alp
      go to 606
  605 print *,' stop : bufferin of alp failed in nonlin'
      stop

  606 do 300 inhsh=1,2
      if(inhsh.eq.2) call ass(alp,ir,irmax2)

      ihem= ill
      if(inhsh.eq.2) ihem= iy+1-ill
  
      prec=0.0
c============================================================================
      call sptofr
c============================================================================
      call mdfftg(pg,pf,1,mx,trigb,ifax,nfax,ix)
      call mdfftg(zg,zf,1,mx,trigb,ifax,nfax,ix)
      call mdfftg(ug,uf,1,mx,trigb,ifax,nfax,ix)
      call mdfftg(vg,vf,1,mx,trigb,ifax,nfax,ix)
c	Pau added==========------
c	if (ill.ne.1) goto 111
c	print *,' ]'
c	print *,' [1:'
c	print *,'------ vg'
c	print *,vg
c  111	continue
c============================================================================
      call vertig
c============================================================================
c
      call mdfftm(pug,puf,1,mx,trigf,ifax,nfax,ix)
      call mdfftm(pvg,pvf,1,mx,trigf,ifax,nfax,ix)
      call mdfftm(zug,zuf,1,mx,trigf,ifax,nfax,ix)
      call mdfftm(zvg,zvf,1,mx,trigf,ifax,nfax,ix)
      call mdfftm(eg,ef,1,mx,trigf,ifax,nfax,ix)
c
c     if in NH, save fourier coefficients for later
c     if in SH, reverse 'alp' to return to NH values and
c     call 'frtosp'
c
      if(inhsh.eq.1) call keepnh
      if(inhsh.eq.2) call ass(alp,ires,jxx)
      if(inhsh.eq.2) call frtosp
  300 continue
      return
      end
