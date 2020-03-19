      subroutine factr4 (n,ifact,nfact)
c     factor n into its prime factors, nfact in number.  for example,
c     for n = 1960, nfact = 6 and ifact(if) = 2, 2, 2, 5, 7 and 7.
c     
c     This version modified to give the actors of 4 as well. These are used
c     by the new fft. The above example would now resuly in 4,2,5,7,7
c     M.Dix 23/2/85
      dimension ifact(1)
      if=0
      npart=n
      do 50 id=1,n,2
      idiv=id
      if (id-1) 10,10,20
 10   idiv=2
 20   iquot=npart/idiv
      if (npart-idiv*iquot) 40,30,40
 30   if=if+1
      ifact(if)=idiv
      npart=iquot
      go to 20
 40   if (iquot-idiv) 60,60,50
 50   continue
 60   if (npart-1) 80,80,70
 70   if=if+1
      ifact(if)=npart
 80   nfact=if
c     
c  Count the factors of 2
      n2=0
      do 100 i=1,nfact
100	   if( ifact(i).eq.2 ) n2=n2+1
      n4 = n2/2
c  Replace the first n4 factors of 2 by 4
      do 200 i=1,n4
200	   ifact(i) = 4
c  Move the rest of the factors down the array. This overwrites the replaced
c  2's
      do 300 i=n4+1,nfact-n4
300	   ifact(i) = ifact(n4+i)
      do 400 i=nfact-n4+1,nfact
400	   ifact(i) = 0
      nfact=nfact-n4
      return
      end
