	subroutine passg(a,b,c,d,trigs,inc1,inc2,n,ifac,la)
	dimension a(1),b(1),c(1),d(1),trigs(1)
	data sin36/0.587785252292473/,cos36/0.8090116994374947/,
     *	sin72/0.951056516295154/,cos72/0.309016994374947/,
     *	sin60/0.866025403784437/, sin45/0.7071067812/
	m=n/ifac
	iink=m*inc1
	jink=la*inc2
	jump=(ifac-1)*jink
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
   	ja=ja+jump
	go to 10
	end if

c   0 < k < m/2 case
	if(2*k.lt.m) then
	kb=k+k
	lower=iink+2
	lupper=iink+jink+jink-2
	do 40 l=lower,lupper,4
	jb=ja+jink
	ibxx=l-ia
	c(ja)=a(ia)+a(ibxx)
	d(ja)=b(ia)-b(ibxx)
	c(jb)=a(ia)-a(ibxx)
	d(jb)=b(ia)+b(ibxx)
	tempr=c(jb)*trigs(kb+1)-d(jb)*trigs(kb+2)
	tempi=c(jb)*trigs(kb+2)+d(jb)*trigs(kb+1)
	c(jb)=tempr
	d(jb)=tempi
	ia=ia+inc1
   40   ja=ja+inc2
   	ja=ja+jump
	go to 10
	end if

c  k=m/2 case
	if(2*k.eq.m) then
	do 50 l=1,la
	jb=ja+jink
	c(ja)=2*a(ia)
	c(jb)=-2*b(ia)
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
	jb=ja+jink
	jc=jb+jink
	a1=a(ia)-a(ib)
	b1=2*sin60*b(ib)
	c(ja)=a(ia)+2*a(ib)
	c(jb)=a1-b1
	c(jc)=a1+b1
	ia=ia+inc1
  120	ja=ja+inc2
   	ja=ja+jump
	go to 110
	end if

	if(2*k.lt.m) then
	kb=k+k
	kc=kb+kb
	lower=iink+2
	lupper=iink+jink+jink-2
	do 140 l=lower,lupper,4
	ib=ia+iink
	icxx=l-ia
	jb=ja+jink
	jc=jb+jink
	a1=a(ib)+a(icxx)
	b1=b(ib)-b(icxx)
	a2=a(ia)-0.5*a1
	b2=b(ia)-0.5*b1
	a3=sin60*(a(ib)-a(icxx))
	b3=sin60*(b(ib)+b(icxx))
	c(ja)=a(ia)+a1
	d(ja)=b(ia)+b1
	c(jb)=a2-b3
	d(jb)=b2+a3
	c(jc)=a2+b3
	d(jc)=b2-a3
	tempr=c(jb)*trigs(kb+1)-d(jb)*trigs(kb+2)
	tempi=c(jb)*trigs(kb+2)+d(jb)*trigs(kb+1)
	c(jb)=tempr
	d(jb)=tempi
	tempr=c(jc)*trigs(kc+1)-d(jc)*trigs(kc+2)
	tempi=c(jc)*trigs(kc+2)+d(jc)*trigs(kc+1)
	c(jc)=tempr
	d(jc)=tempi
	ia=ia+inc1
  140	ja=ja+inc2
   	ja=ja+jump
	go to 110
	end if

	if(2*k.eq.m) then
	do 150 l=1,la
	ib=ia+iink
	jb=ja+jink
	jc=jb+jink
	a1=0.5*a(ia)-a(ib)
	b1=sin60*b(ia)
	c(ja)=a(ia)+a(ib)
	c(jb)=a1-b1
	c(jc)=-a1-b1
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
	jb=ja+jink
	jc=jb+jink
	jd=jc+jink
	a1=a(ia)+a(ic)
	a2=a(ib)+a(ib)
	a3=a(ia)-a(ic)
	a4=b(ib)+b(ib)
	c(ja)=a1+a2
	c(jb)=a3-a4
	c(jc)=a1-a2
	c(jd)=a3+a4
	ia=ia+inc1
220	ja=ja+inc2
	ja=ja+jump
	go to 210
	end if

c  0 < k < m/2  case
	if(2*k.lt.m) then
	lower=iink+2
	lupper=iink+jink+jink-2
	do 240 l=lower,lupper,4
	ib=ia+iink
	idxx=l-ia
	icxx=iink+idxx
	jb=ja+jink
	jc=jb+jink
	jd=jc+jink
	kb=k+k
	kc=kb+kb
	kd=kc+kb
	a0=a(ia)+a(icxx)
	a1=a(ib)+a(idxx)
	a2=a(ia)-a(icxx)
	a3=a(ib)-a(idxx)
	b0=b(ia)-b(icxx)
	b1=b(ib)-b(idxx)
	b2=b(ia)+b(icxx)
	b3=b(ib)+b(idxx)
	c(ja)=a0+a1
	d(ja)=b0+b1
	c(jb)=a2-b3
	d(jb)=b2+a3
	c(jc)=a0-a1
	d(jc)=b0-b1
	c(jd)=a2+b3
	d(jd)=b2-a3
	tempr=c(jb)*trigs(kb+1)-d(jb)*trigs(kb+2)
	tempi=c(jb)*trigs(kb+2)+d(jb)*trigs(kb+1)
	c(jb)=tempr
	d(jb)=tempi
	tempr=c(jc)*trigs(kc+1)-d(jc)*trigs(kc+2)
	tempi=c(jc)*trigs(kc+2)+d(jc)*trigs(kc+1)
	c(jc)=tempr
	d(jc)=tempi
	tempr=c(jd)*trigs(kd+1)-d(jd)*trigs(kd+2)
	tempi=c(jd)*trigs(kd+2)+d(jd)*trigs(kd+1)
	c(jd)=tempr
	d(jd)=tempi
	ia=ia+inc1
  240	ja=ja+inc2
   	ja=ja+jump
	go to 210
	end if

	if(2*k.eq.m) then
	do 260 l=1,la
	ib=ia+iink
	jb=ja+jink
	jc=jb+jink
	jd=jc+jink
	a1=sin45*(b(ia)+b(ib))
	a2=sin45*(a(ia)-a(ib))
	c(ja)=2*(a(ia)+a(ib))
	c(jb)=2*(a2-a1)
	c(jc)=2*(b(ib)-b(ia))
	c(jd)=2*(-(a1+a2))
	ia=ia+inc1
  260	ja=ja+inc2
	end if

  210	continue
	return
	end
