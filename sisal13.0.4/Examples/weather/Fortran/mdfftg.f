	subroutine mdfftg(grid,four,nlev,nwave,trigs,ifax,nfax,n)
	parameter(nmax=96)
	dimension grid(n,1)
	complex four(n/2,1),f(nmax+1)
	dimension x(nmax*2+2),trigs(n),work(nmax*2+2),ifax(nfax),
     &            c(nmax*2+2)
	equivalence(c,f)
	common / fftwor / x, work, c

	if(n.gt.nmax) stop ' INCREASE NMAX IN MDFFTG '

	do 100 ilev=1,nlev
	do 4 i=1,nwave
    4	    f(i) = four(i,ilev)
	do 5 i=nwave+1,n/2
    5	    f(i)=(0.,0.)

c    start of ffft part
	la=1
	if (nfax.gt.1)go to 10
	call passg(c(1),c(2),x(1),x(2),trigs,2,2,n,ifax(1),la)
	return
   10	if(mod(nfax,2).eq.1)go to 20
	call passg(c(1),c(2),work(1),work(2),trigs,2,2,n,ifax(1),la)
	inext=50
	go to 30
   20	call passg(c(1),c(2),x(1),x(2),trigs,2,2,n,ifax(1),la)
	inext=40

   30	do 70 l=2,nfax-1
	la=la*ifax(l-1)
	if(inext.eq.50)go to 50
   40	call passg(x(1),x(2),work(1),work(2),trigs,2,2,n,ifax(l),la)
	inext=50
	go to 60
   50 	call passg(work(1),work(2),x(1),x(2),trigs,2,2,n,ifax(l),la)
	inext=40
   60 	continue
   70 	continue

c  final factor
	la=la*ifax(nfax-1)
    	call passg(work(1),work(2),grid(1,ilev),grid(2,ilev),trigs,
     &			2,1,n,ifax(nfax),la)

100	continue
	return
	end
