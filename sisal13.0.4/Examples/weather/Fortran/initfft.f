	subroutine initfft(n,trigf,trigb,ifax,nfax)
c  Routine does the initialisation necessary before the fft's can be used.
c  It factorises the number of points (note n/2 is used because of the 
c  real to complex conversion later) and sets up the trig and cw arrays.
c  trigf and cwf are for forward transforms and trigb and cwb for reverse
c  transforms.
c  NOTE on dimensions.
c  If n is the number of real points then the dimensions set in the main
c  program must be at least  trigf(b)(n/2), cwf(b)(0:n/4)

	dimension ifax(1)
	complex trigf(1),trigb(1),cexp,carg

c	Check that n is a multiple of 2 and not too large
	if( (mod(n,2) .ne. 0) .or. (n.gt.200) ) then
	    print*,' STOP in initfft '
	    stop
	end if
	call factr4(n,ifax,nfax)

c    Set up the array trigs
c    trigs(k) = exp(2*pi*i*(k-1)/N) for N points. Here N =n/2.
	do 100 k=1,n/2
	    carg =  cmplx(0., -2*3.14159265*(k-1)/n) 
	    trigf(k) = cexp(carg)
	    trigb(k) = conjg(trigf(k))
100	continue
 

	return
	end
