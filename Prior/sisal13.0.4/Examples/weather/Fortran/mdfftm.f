c   Routine for fourier analysis of n real data points in array grid.
c   Fourier coefficients for waves 0 to (n/2)-1 are returned in array four.
c   Note that these are n times too large and must be corrected elsewhere.
c   For details on use see FFT_model.help
	subroutine mdfftm(grid,four,nlev,nwave,trigs,ifax,nfax,n)
	parameter(nmax=96)
	dimension grid(n,1)
	complex four(n/2,1),c(nmax+1)
	dimension x(nmax*2+2),trigs(n),work(nmax*2+2),ifax(nfax)
	equivalence(x,c)
	common / fftwor / x, work, dummy(nmax*2+2)
c   This comoon block is just to save a little space in the FFT routines
	if(n.gt.nmax) stop ' INCREASE NMAX IN MDFFTG '
c	------------------
	do 100 ilev=1,nlev
c  the ffft replacement section
	la=n/ifax(1)
	if (nfax.gt.1)go to 10
	call passm(grid(1,ilev),grid(2,ilev),x(1),x(2),trigs,
     &			1,2,n,ifax(1),la)
	return
   10	if(mod(nfax,2).eq.1)go to 20
	call passm(grid(1,ilev),grid(2,ilev),work(1),work(2),trigs,
     &			1,2,n,ifax(1),la)
	inext=50
	go to 30
   20	call passm(grid(1,ilev),grid(2,ilev),x(1),x(2),trigs,
     &			1,2,n,ifax(1),la)
	inext=40
c	------------------------------------
   30	do 70 l=2,nfax
	la=la/ifax(l)
c	------------------
	if(inext.eq.50)go to 50
   40	call passm(x(1),x(2),work(1),work(2),trigs,2,2,n,ifax(l),la)
	inext=50
c	----------------------
	go to 60
   50 	call passm(work(1),work(2),x(1),x(2),trigs,2,2,n,ifax(l),la)
	inext=40
c	------------------------
   60 	continue
   70 	continue
c  end of the ffft section
	do 80 i=1,nwave
   80	    four(i,ilev) = c(i)
	do 90 i=nwave+1,n/2
   90	    four(i,ilev) = (0.,0.)
  100	continue
	return
	end
