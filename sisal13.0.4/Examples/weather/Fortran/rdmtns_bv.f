      subroutine rdmtns(zs,jx,mx,jxx,const,var,nfile)
c
c     this subroutine reads topography off a formatted file, and 
c     packs it into the (complex) array 'zs'.
c     notice that we read only the elements jx x mx.
c     for purposes of the model and analysis packages, we may pad
c     out with zeros
c     to coincide with the normalization used in model, or analysis
c     programs, we multiply all waves by 'const'
c     we use 'nfile' as the temporary file number to 'open' onto the
c     mountains.  if a particular value clashes with external usage
c     'nfile' may be reset to a more convenient value
c     we also calculate the variance in the mountains 'var'
c
      complex zs(jxx,mx),zt
      character filnam*100
      if(jxx.ge.jx) go to 10
      write(*,1010) jx,jxx
      stop
   10 continue
c
c     check that unit 'nfile' number is not 'open' ed onto any file
c
c     inquire(unit=nfile,name=filnam)
c     if(filnam.ne.'') go to 1070
      open(unit=nfile,file='mountains',status='old',form='formatted')
      rewind nfile
      do 20 m=1,mx
      do 20 j=1,jx
      do 30 nread=1,200
      read(nfile,1020,err=1030,end=1030) jt,mt,zt

      if(jt.ne.j.or.mt.ne.m) go to 30
      zs(j,m)=zt
c	Pau added
c	print *,zt
c	-------
      go to 20
   30 continue
      write(*,1050) j,m
      stop
   20 continue


      if(jxx.le.jx) go to 80
c
c     now pad out with zeros,if required
c
      jxp=jx+1
      do 40 m=1,mx
      do 40 j=jxp,jxx
   40 zs(j,m)=(0.0,0.0)
   80 var=0.0
      do 60 m=1,mx
      do 60 j=1,jxx
      if(j.eq.1.and.m.eq.1) go to 60
      var=var+cabs(zs(j,m))**2
   60 continue
      do 50 m=1,mx
      do 50 j=1,jxx
   50 zs(j,m)=const*zs(j,m)
      close(unit=nfile)
c	I add this:
c	print *,'final zs='
c	print *,zs

      return
 1030 write(*,1031)
      stop
 1070 write(*,1080)
      stop
c
c *** formats
c
 1010 format(//,' stop: ambiguity in calling parameters in (rdmtns)',
     1          2i4)
 1020 format(2i2,2e15.7)
 1031 format(//,' stop: eoi or err on (mountains) file',//)
 1050 format(//,' stop: (rdmtns) has failed to find a wave on',/,
     1          ' (mountains) ; wave is ',2i5,//)
 1080 format(//,' stop: unit (nfile) in rdmtns.f already open,'/,
     1          ' choose another unit number',//)
      end
