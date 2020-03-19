C ALLIANT VERSION OF THE SPECIAL VECTORIZATION LIBRARY
C INDEX OF FIRST MIN: REAL
       function rifmin(x,l,h,v1)
       real x(*)
       integer min24,l,h,v1,rifmin
       min24 = 1
       do 100 i = 2,h-l+2
         if ( x(i) .lt. x(min24) ) min24 = i
 100   continue
       rifmin = min24 + v1 - 1
       return
       end
C INDEX OF FIRST MAX: REAL
       function rifmax(x,l,h,v1)
       real x(*)
       integer max24,l,h,v1,rifmax
       max24 = 1
       do 100 i = 2,h-l+2
         if ( x(i) .gt. x(max24) ) max24 = i
 100   continue
       rifmax = max24 + v1 - 1
       return
       end
C INDEX OF FIRST MIN: INTEGER
       function iifmin(x,l,h,v1)
       integer x(*)
       integer min24,l,h,v1,iifmin
       min24 = 1
       do 100 i = 2,h-l+2
         if ( x(i) .lt. x(min24) ) min24 = i
 100   continue
       iifmin = min24 + v1 - 1
       return
       end
C INDEX OF FIRST MAX: INTEGER
       function iifmax(x,l,h,v1)
       integer x(*)
       integer max24,l,h,v1,iifmax
       max24 = 1
       do 100 i = 2,h-l+2
         if ( x(i) .gt. x(max24) ) max24 = i
 100   continue
       iifmax = max24 + v1 - 1
       return
       end
C INDEX OF FIRST MIN: DOUBLE
       function difmin(x,l,h,v1)
       real*8 x(*)
       integer min24,l,h,v1,difmin
       min24 = 1
       do 100 i = 2,h-l+2
         if ( x(i) .lt. x(min24) ) min24 = i
 100   continue
       difmin = min24 + v1 - 1
       return
       end
C INDEX OF FIRST MAX: DOUBLE
       function difmax(x,l,h,v1)
       real*8 x(*)
       integer max24,l,h,v1,difmax
       max24 = 1
       do 100 i = 2,h-l+2
         if ( x(i) .gt. x(max24) ) max24 = i
 100   continue
       difmax = max24 + v1 - 1
       return
       end
C FIRST SUM: REAL
       subroutine rfsum(x,y,l,h,v1)
       real x(*),y(*),v1
       integer l,h,i
       x(1) = v1
       do 100 i = 2,h-l+2
	 x(i) = x(i-1) + y(i)
  100  continue
       return
       end
C FIRST SUM: INTEGER
       subroutine ifsum(x,y,l,h,v1)
       integer x(*),y(*),v1
       integer l,h,i
       x(1) = v1
       do 100 i = 2,h-l+2
	 x(i) = x(i-1) + y(i)
  100  continue
       return
       end
C FIRST SUM: DOUBLE
       subroutine dfsum(x,y,l,h,v1)
       real*8 x(*),y(*),v1
       integer l,h,i
       x(1) = v1
       do 100 i = 2,h-l+2
	 x(i) = x(i-1) + y(i)
  100  continue
       return
       end
C TRIDIAGONAL: DOUBLE
       subroutine dtri(x,y,z,l,h,v1)
       real*8 z(*),x(*),y(*),v1
       integer l,h,i
       x(1) = v1
       do 100 i = 2,h-l+2
	 x(i) = z(i) * (y(i) - x(i-1))
  100  continue
       return
       end 
C TRIDIAGONAL: REAL
       subroutine rtri(x,y,z,l,h,v1)
       real z(*),x(*),y(*),v1
       integer l,h,i
       x(1) = v1
       do 100 i = 2,h-l+2
	 x(i) = z(i) * (y(i) - x(i-1))
  100  continue
       return
       end 
C TRIDIAGONAL: INTEGER
       subroutine itri(x,y,z,l,h,v1)
       integer z(*),x(*),y(*),v1
       integer l,h,i
       x(1) = v1
       do 100 i = 2,h-l+2
	 x(i) = z(i) * (y(i) - x(i-1))
  100  continue
       return
       end 
C INDEX OF FIRST ABS MIN: REAL
       function rifamin(x,l,h,v1)
       real x(*)
       integer min24,l,h,v1,rifamin
       min24 = 1
       do 100 i = 2,h-l+2
         if ( abs(x(i)) .lt. abs(x(min24)) ) min24 = i
 100   continue
       rifamin = min24 + v1 - 1
       return
       end
C INDEX OF FIRST ABS MAX: REAL
       function rifamax(x,l,h,v1)
       real x(*)
       integer max24,l,h,v1,rifamax
       max24 = 1
       do 100 i = 2,h-l+2
         if ( abs(x(i)) .gt. abs(x(max24)) ) max24 = i
 100   continue
       rifamax = max24 + v1 - 1
       return
       end
C INDEX OF FIRST ABS MIN: INTEGER
       function iifamin(x,l,h,v1)
       integer x(*)
       integer min24,l,h,v1,iifamin
       min24 = 1
       do 100 i = 2,h-l+2
         if ( abs(x(i)) .lt. abs(x(min24)) ) min24 = i
 100   continue
       iifamin = min24 + v1 - 1
       return
       end
C INDEX OF FIRST ABS MAX: INTEGER
       function iifamax(x,l,h,v1)
       integer x(*)
       integer max24,l,h,v1,iifamax
       max24 = 1
       do 100 i = 2,h-l+2
         if ( abs(x(i)) .gt. abs(x(max24)) ) max24 = i
 100   continue
       iifamax = max24 + v1 - 1
       return
       end
C INDEX OF FIRST ABS MIN: DOUBLE
       function difamin(x,l,h,v1)
       real*8 x(*)
       integer min24,l,h,v1,difamin
       min24 = 1
       do 100 i = 2,h-l+2
         if ( abs(x(i)) .lt. abs(x(min24)) ) min24 = i
 100   continue
       difamin = min24 + v1 - 1
       return
       end
C INDEX OF FIRST ABS MAX: DOUBLE
       function difamax(x,l,h,v1)
       real*8 x(*)
       integer max24,l,h,v1,difamax
       max24 = 1
       do 100 i = 2,h-l+2
         if ( abs(x(i)) .gt. abs(x(max24)) ) max24 = i
 100   continue
       difamax = max24 + v1 - 1
       return
       end
c
c     SISAL Fortran foreign language interface help routines
c
      subroutine sarray1d(idescriptor, imutable, ilower1, ihigher1)
      include 'sisalfli.inc'
      integer idescriptor(SARRAY1), imutable, ilower1, ihigher1
c
      idescriptor(SMAJOR) = SCOLMAJOR
      idescriptor(STRANSPOSE) = SPRESERVE
      idescriptor(SMUTABLE) = imutable
c
      idescriptor(SPLO1) = ilower1
      idescriptor(SPHI1) = ihigher1
      idescriptor(SLLO1) = ilower1
      idescriptor(SLHI1) = ihigher1
      idescriptor(SSLO1) = ilower1
      return
      end
c
      subroutine sarray2d(idescriptor, imutable, ilower1, ihigher1,
     & ilower2, ihigher2)
      include 'sisalfli.inc'
      integer idescriptor(SARRAY2), imutable, ilower1, ihigher1,
     & ilower2, ihigher2
c
      idescriptor(SMAJOR) = SCOLMAJOR
      idescriptor(STRANSPOSE) = SPRESERVE
      idescriptor(SMUTABLE) = imutable
c
      idescriptor(SPLO1) = ilower1
      idescriptor(SPHI1) = ihigher1
      idescriptor(SLLO1) = ilower1
      idescriptor(SLHI1) = ihigher1
      idescriptor(SSLO1) = ilower1
c
      idescriptor(SPLO2) = ilower2
      idescriptor(SPHI2) = ihigher2
      idescriptor(SLLO2) = ilower2
      idescriptor(SLHI2) = ihigher2
      idescriptor(SSLO2) = ilower2
      return
      end
