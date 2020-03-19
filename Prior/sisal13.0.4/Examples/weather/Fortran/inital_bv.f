      subroutine inital
c
c  This routine initialises some model variables and sets up some indexing
c  arrays.

      include 'param_dec'
      include 'bv_dec'
c
      ir = ires
      ilong = ix
      ilat = iy
      ww=7.292e-05
      tw =ww+ww

C     DONE IN main_bv CANN
C     asq=6371.22e03*6371.22e03
C     grav = 9.80616

      zmean= zmean*grav/asq
c
c     now set up indexing arrays to improve efficiency
c
      do 7 m=1,mx
      kmjx(m)=(m-1)*jx
    7 kmjxx(m)=(m-1)*jxx

      ihdelt = idelt
      irmax = ir
      ilath = ilat / 2
      irmax1=irmax+1
      irmax2=irmax+2
      do 1 j=1,ir+ir
    1 ksq(j)=j*(j+1)
      do 3 mp=1,mx
      do 3 j=1,jxx
      il=kmjxx(mp)+j
      l=j+mp-2
      m=mp-1
      t= (l+m)*(l-m)
      b= 4*l*l-1
    3 epsi(il)= sqrt(t/b)
      epsi(1)=0.0
c
      return
      end
