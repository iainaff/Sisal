c
c SISAL Fortran foreign language interface
c
      integer SCOLMAJOR, SROWMAJOR
      parameter (SCOLMAJOR =	 0)
      parameter (SROWMAJOR =	 1)
c
      integer SPRESERVE, SREVERSE
      parameter (SPRESERVE =	 0)
      parameter (SREVERSE =	 1)
c
      integer SCOPY, SNOCOPY
      parameter (SCOPY =	 0)
      parameter (SNOCOPY =	 1)
c
c     Array descriptors for calls to Sisal. Each descriptor has at
c     least eight fields, with five extra fields for each extra dimension:
c       1 SMAJOR)     subscript memory order (SROWMAJOR/SCOLMAJOR),
c       2 STRANSPOSE) transpose (SPRESERVE/SREVERSE),
c       3 SMUTABLE)   mutable (SREADONLY/SREADWRITE),
c       4 SPLO1)      lower bound of Fortran array,
c       5 SPHI1)      upper bound of Fortran array,
c       6 SLLO1)      index of first value in Fortran array to be passed
c       7 SLHI1)      index of last value in Fortran array to be passed
c       8 SSLO1)      lower bound of Sisal array.
c       9-13) repeat 4-8 for the second dimension with 1 replaced by 2
c
      integer SMAJOR, STRANSPOSE, SMUTABLE
      parameter (SMAJOR =	 1)
      parameter (STRANSPOSE =	 2)
      parameter (SMUTABLE =	 3)
c
      integer SPLO1, SPHI1, SLLO1, SLHI1, SSLO1
      parameter (SPLO1 =	 4)
      parameter (SPHI1 =	 5)
      parameter (SLLO1 =	 6)
      parameter (SLHI1 =	 7)
      parameter (SSLO1 =	 8)
c
      integer SPLO2, SPHI2, SLLO2, SLHI2, SSLO2
      parameter (SPLO2 =	 9)
      parameter (SPHI2 =	10)
      parameter (SLLO2 =	11)
      parameter (SLHI2 =	12)
      parameter (SSLO2 =	13)
c
      integer SPLO3, SPHI3, SLLO3, SLHI3, SSLO3
      parameter (SPLO3 =	14)
      parameter (SPHI3 =	15)
      parameter (SLLO3 =	16)
      parameter (SLHI3 =	17)
      parameter (SSLO3 =	18)
c
      integer SPLO4, SPHI4, SLLO4, SLHI4, SSLO4
      parameter (SPLO4 =	19)
      parameter (SPHI4 =	20)
      parameter (SLLO4 =	21)
      parameter (SLHI4 =	22)
      parameter (SSLO4 =	23)
c
      integer SPLO5, SPHI5, SLLO5, SLHI5, SSLO5
      parameter (SPLO5 =	24)
      parameter (SPHI5 =	25)
      parameter (SLLO5 =	26)
      parameter (SLHI5 =	27)
      parameter (SSLO5 =	28)
c
      integer SPLO6, SPHI6, SLLO6, SLHI6, SSLO6
      parameter (SPLO6 =	29)
      parameter (SPHI6 =	30)
      parameter (SLLO6 =	31)
      parameter (SLHI6 =	32)
      parameter (SSLO6 =	33)
c
      integer SPLO7, SPHI7, SLLO7, SLHI7, SSLO7
      parameter (SPLO7 =	34)
      parameter (SPHI7 =	35)
      parameter (SLLO7 =	36)
      parameter (SLHI7 =	37)
      parameter (SSLO7 =	38)
c
c     Size of array descriptors (used for declaration).
c
      integer SARRAY1, SARRAY2, SARRAY3, SARRAY4, SARRAY5,
     &        SARRAY6, SARRAY7
      parameter (SARRAY1 = SSLO1)
      parameter (SARRAY2 = SSLO2)
      parameter (SARRAY3 = SSLO3)
      parameter (SARRAY4 = SSLO4)
      parameter (SARRAY5 = SSLO5)
      parameter (SARRAY6 = SSLO6)
      parameter (SARRAY7 = SSLO7)
