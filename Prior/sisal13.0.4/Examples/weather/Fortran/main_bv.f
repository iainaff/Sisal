c
c        p(256) denotes vorticity spectral amplitudes of 'kx' levels
c
c        pt denotes tendencies of p
c
c        pm denotes previous time step value of p
c
c        pf,uf,vf denote intermediate complex fourier
c                  amplitudes at the current latitude circle. these
c                  are derived knowing p,u,v in mhexp.
c
c        pg,ug,vg are grid point values for the
c                  current latitude circle and are fourier transforms
c                  of the intermediate complex amplitudes.
c
c        the quantities pug, pvg 
c                  denote non-linear terms evaluated at grid points in
c                  vertig.
c        puf, pvf are the
c                  fourier amplitudes derived from transforms of
c                  the above non-linear products.
c

      real tm
      real tmarray(2)
      include 'param_dec'
      include 'bv_dec'
      complex ct1
c     complex hinit(jx,mx)
c
c-------------------------------------------------------------------
c        The following input variables are read at startup.
c        ktotal   -     number of time steps
c        idelt    -     time step in seconds
c        kountp   -     number of time steps after which data is printed out
c        nrun     -     identification number of the run
c        imp      -     0 denotes explicit time step
c        istart   -     0 denotes 'cold start' using only the initial p
c    			field and linear balance.
c                       1 denotes 'hot start' using a ful datastart file
c        izon     -     0 for normal model run
c      			1 for run in which the zonal mean flow is held
c			constant.
c        ilin     -     1 denotes a version of the model linearised about
c			the zonal flow.
c
c        in this version we perform a forward time step to start
c        the model
c        ifirst - 1 if forward time step
c               - 0 if regular time step
c----------------------------------------------------------------------
c
c     tm=dtime(tmarray)
c     activate timing

      kount=0
      ifirst=1
      itflag=1
      call cardin
      call print1
      call datain

C CANN first two lines used to be in inital_bv.f: moved for TIMINGS
      asq=6371.22e03*6371.22e03
      grav = 9.80616
      call rdmtns(h,jx,mx,jx,grav/asq,var,4)

C CANN RECORD START TIME
      call time1

c
      if (idumpt.eq.0) idumpt = 1000
      if (kountp.eq.0) kountp = 1000
      if (iglobe.eq.0) iglobe = 2
      delt = idelt


c
c     inital sets up some constants and indexing arrays.
      call inital

      call initfft(ix,trigf,trigb,ifax,nfax)
      call gaussg(ilath,coa,w,sia,delta,wocs)
c
c     in mdfftm.f we avoid some computation by omitting to
c     divide the complex wave amplitudes by ix
c     The only place in the model analysed fields are used is
c     in the north-south qaudrature. We here divide the Gaussian
c     weights by the appropriate factors.
c     Note that in the linear model ( ilin=1 ) the fft is not used and
c     so this division should not be done.
c
      if ( ilin.ne.1) then
          do 430 i=1,ilath
  430     w(i)=w(i) / ix
      end if
      do 410 i=1,ilath
      inv=iy+1-i
      coa(inv)=-coa(i)
      w(inv)  = w(i)
  410 continue
      do 420 i=1,iy
      delta(i)=acos(coa(i))
      sia(i)=sin(delta(i))
      wocs(i)=w(i) / (sia(i)*sia(i))
  420 continue
c
c
c     precompute legendre functions and put on disk
c
      open(unit=2,file='alf',form='unformatted',status='unknown')
c      open(unit=2,file='alf',form='formatted',status='unknown')
      rewind 2
      do 600 i=1,ilath
      call lgndre(alp,coa(i),sia(i),delta(i),ir,irmax2)
c	Pau added
c	print *,' ]'
c	print *,' [ 1:'
c	print *,alp
c	-----------------
c      write(2,*,err=601) alp
      write(2,err=601) alp
      go to 602
  601 print 610
  602 continue
  610 format(' bufferout of alp failed in main')
  600 continue
c	print *,'Pau end of lgndre'
c	stop
      if (iglobe.eq.2) go to 7
      do 6 i = 1,ilath
      wocs(i) = wocs(i) + wocs(i)
    6 w(i) = w(i) + w(i)
    7 continue
c	---------------------
c     Read in the mountains file 
C     call rdmtns(h,jx,mx,jx,grav/asq,var,4)
c      call rdmtns(h,31,31,31,grav/asq,var,4)

      print *,' mountains used have variance ', var
 
c
c     In the linear version of the model the meridional derivative of the 
c     topography must be zero. Partly achieve this by removing the zonal
c     mean which is the largest part. The rest is done by using xlin rather
c     than nonlin.
      if ( ilin.eq.1 ) then
      do 150 j=1,jx
  150    h(j) = (0.,0.)
      end if

      if ( istart.eq.0 ) call linbal
c
c     write current time into 'previous' time to use in forward
c     time step to start up. Also write the initial vorticty to p1
c     to allow the relaxation to the initial state.
      call initpv
c===============================================================
      print*, ' ' 
      if ( istart .eq. 1 ) then
          print*, 'MODEL STARTED FROM STEP ', ihkont,' OF RUN ',ihir
      end if

      ihir=ihirmx
      if(istart.eq.0) ihkont = 0
c  Start of time step loop
c      tm=dtime(tmarray)
c     deactivate timing
c      print *,tm,tmarray
c      tm=dtime(tmarray)

      do 1 kount=1,ktotal

      do 250 m=1,mx
      do 250 j=1,jxx
      jm=kmjxx(m)+j
      call vduvjm(u(jm),v(jm),j,m)
  250 continue
c
c     Calculate the height of the free surface above the topography.
      do 260 jm=1,jxmx
	  zdiff(jm) = z(jm)-h(jm)
  260 continue
c     if(kount.le.240) call mtscal(kount,hinit,h,240)

      if (ilin.eq.1) then
	  call xlin
      else
          call nonlin
      end if
      ct1 = ct(23)

      call linear

      call tstep

c     write(7,1100) kount,c(23),ct(23),z(23),zt(23)
c1100 format(i4,2e15.6,2x,2e15.6,/,4x,2e15.6,2x,2e15.6)
c
      ihkont = ihkont + 1
c     if (mod(ihkont,idumpt).eq.0.and.ihkont.gt.0) call archwr
      print 500,ihkont
  500 format('number of time steps completed = ',i5)
      call energy
      call angmom
      if (mod(ihkont,idumpt).eq.0.and.ihkont.gt.0) then
      call specam
      print*, ' ' 
c     print*,' '
      end if
c
   1  continue


C CANN RECORD AND PRINT STOP TIME
      call time2

c     tm=dtime(tmarray)
c     deactivate timing
c     print *,tm,tmarray
      stop
      end
