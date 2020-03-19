	subroutine passm(a,b,c,d,trigs,inc1,inc2,n,ifac,la)
	dimension a(1),b(1),c(1),d(1),trigs(1)
	data sin36/0.587785252292473/,cos36/0.8090116994374947/,
     *	sin72/0.951056516295154/,cos72/0.309016994374947/,
     *	sin60/0.866025403784437/, sin45/0.7071067812/
	m=n/ifac
	jink=m*inc2
	iink=la*inc1
	jump=(ifac-1)*iink
	ia=1
	ja=1
	igo=ifac-1
	if(igo.gt.5)igo=5
	go to (5,100,200),igo

c   factor of 2
    5	continue
	do 10 k=0,m/2,la

c  	k=0 case
	if(k.eq.0) then
	do 20 l=1,la
	ib=ia+iink
	jb=ja+jink
	c(ja)=a(ia)+a(ib)
	c(jb)=a(ia)-a(ib)
	ia=ia+inc1
   20	ja=ja+inc2
   	ia=ia+jump
	go to 10
	end if

c   0 < k < m/2 case
	if(2*k.lt.m) then
	kb=k+k
	lower=jink+2
	lupper=jink+iink+iink-2
	do 40 l=lower,lupper,4
	ib=ia+iink
	jbxx=l-ja
	tempr=a(ib)*trigs(kb+1)-b(ib)*trigs(kb+2)
	tempi=a(ib)*trigs(kb+2)+b(ib)*trigs(kb+1)
	c(ja)=a(ia)+tempr
	d(ja)=b(ia)+tempi
	c(jbxx)=a(ia)-tempr
	d(jbxx)=-b(ia)+tempi
	ia=ia+inc1
   40   ja=ja+inc2
   	ia=ia+jump
	go to 10
	end if

c  k=m/2 case
	if(2*k.eq.m) then
	do 50 l=1,la
	ib=ia+iink
	c(ja)=a(ia)
	d(ja)=-a(ib)
	ia=ia+inc1
   50	ja=ja+inc2
	end if

10	continue
	return

c  factor of 3
  100   do 110 k=0,m/2,la

	if(k.eq.0) then
	do 120 l=1,la
	ib=ia+iink
	ic=ib+iink
	jb=ja+jink
	a1=a(ib)+a(ic)
	a2=a(ia)-0.5*a1
	a3=sin60*(a(ic)-a(ib))
	c(ja)=a(ia)+a1
	c(jb)=a2
	d(jb)=a3
	ia=ia+inc1
  120	ja=ja+inc2
   	ia=ia+jump
	go to 110
	end if

	if(2*k.lt.m) then
	kb=k+k
	kc=kb+kb
	lower=jink+2
	lupper=jink+iink+iink-2
	do 140 l=lower,lupper,4
	ib=ia+iink
	ic=ib+iink
	jb=ja+jink
	jcxx=l-ja
	tempr1=a(ib)*trigs(kb+1)-b(ib)*trigs(kb+2)
	tempi1=a(ib)*trigs(kb+2)+b(ib)*trigs(kb+1)
	tempr2=a(ic)*trigs(kc+1)-b(ic)*trigs(kc+2)
	tempi2=a(ic)*trigs(kc+2)+b(ic)*trigs(kc+1)
	a1=tempr1+tempr2
	b1=tempi1+tempi2
	a2=a(ia)-0.5*a1
	b2=b(ia)-0.5*b1
	a3=sin60*(tempr2-tempr1)
	b3=sin60*(tempi2-tempi1)
	c(ja)=a(ia)+a1
	c(jb)=a2-b3
	c(jcxx)=a2+b3
	d(ja)=b(ia)+b1
	d(jb)=b2+a3
	d(jcxx)=a3-b2
	ia=ia+inc1
  140	ja=ja+inc2
   	ia=ia+jump
	go to 110
	end if

	if(2*k.eq.m) then
	do 150 l=1,la
	ib=ia+iink
	ic=ib+iink
	jb=ja+jink
	a1=a(ib)-a(ic)
	c(ja)=a(ia)+0.5*a1
	c(jb)=a(ia)-a1
	d(ja)=-sin60*(a(ib)+a(ic))
	ia=ia+inc1
  150 	ja=ja+inc2
	end if

  110	continue
	return


c  factor of 4
200	continue

	do 210 k=0,m/2,la

c  k=0 case
	if(k.eq.0) then
	do 220 l=1,la
	ib=ia+iink
	ic=ib+iink
	id=ic+iink
	jb=ja+jink
	jc=jb+jink
	a1=a(ia)+a(ic)
	a2=a(ib)+a(id)
	a3=a(ia)-a(ic)
	a4=a(id)-a(ib)
	c(ja)=a1+a2
	c(jb)=a3
	d(jb)=a4
	c(jc)=a1-a2
	ia=ia+inc1
220	ja=ja+inc2
	ia=ia+jump
	go to 210
	end if

c  0 < k < m/2  case
	if(2*k.lt.m) then
	lower=jink+2
	lupper=jink+iink+iink-2
	do 240 l=lower,lupper,4
	ib=ia+iink
	ic=ib+iink
	id=ic+iink
	jb=ja+jink
	jdxx=l-ja
	jcxx=jdxx+jink
	kb=k+k
	kc=kb+kb
	kd=kc+kb
	tempr1=a(ib)*trigs(kb+1)-b(ib)*trigs(kb+2)
	tempi1=a(ib)*trigs(kb+2)+b(ib)*trigs(kb+1)
	tempr2=a(ic)*trigs(kc+1)-b(ic)*trigs(kc+2)
	tempi2=a(ic)*trigs(kc+2)+b(ic)*trigs(kc+1)
	tempr3=a(id)*trigs(kd+1)-b(id)*trigs(kd+2)
	tempi3=a(id)*trigs(kd+2)+b(id)*trigs(kd+1)
	a0=a(ia)+tempr2
	a1=tempr1+tempr3
	a2=a(ia)-tempr2
	a3=tempr1-tempr3
	b0=b(ia)+tempi2
	b1=tempi1+tempi3
	b2=b(ia)-tempi2
	b3=tempi1-tempi3
	c(ja)=a0+a1
	d(ja)=b0+b1
	c(jb)=a2+b3
	d(jb)=b2-a3
	c(jcxx)=a0-a1
	d(jcxx)=b1-b0
	c(jdxx)=a2-b3
	d(jdxx)=-b2-a3
	ia=ia+inc1
  240	ja=ja+inc2
   	ia=ia+jump
	go to 210
	end if

	if(2*k.eq.m) then
	do 260 l=1,la
	ib=ia+iink
	ic=ib+iink
	id=ic+iink
	jb=ja+jink
	a1=sin45*(a(ib)-a(id))
	a2=sin45*(a(ib)+a(id))
	c(ja)=a(ia)+a1
	c(jb)=a(ia)-a1
	d(ja)=-a(ic)-a2
	d(jb)=a(ic)-a2
	ia=ia+inc1
  260	ja=ja+inc2
	end if

  210	continue
	return
	end
