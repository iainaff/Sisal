      subroutine frsymm(isyasy)
c
c     this routine calculates the symmetric or antisymmetric part
c     of the fourier coefficients, depending on the value
c     of 'isyasy'. This routtine is called from a version of 'frtosp'
c     which considers the NH and SH fourier coefficients at a given
c     latitude at the same time, to make use of the (anti)symmetries
c     in the legendre functions.
c
      include 'param_dec'
      include 'bv_dec'
c
c--------------------------------------------------------------
c
      if(isyasy.eq.2) go to 1000
      do 10 mri=1,mx2
      pug (mri)=pug (mri) + pugn(mri)
      pvg (mri)=pvg (mri) + pvgn(mri)
      zug (mri)=zug (mri) + zugn(mri)
      zvg (mri)=zvg (mri) + zvgn(mri)
      eg (mri)=eg (mri) + egn(mri)
   10 continue
      return
c
c     now we do antisymmetic part.
c     what we wish to take is (NH) - (SH). Because on the second
c     pass here pug etc. is (NH) + (SH), the difference has
c     the funny form given below
c
 1000 do 110 mri=1,mx2
      pug (mri)=2.0*pugn(mri) - pug(mri)
      pvg (mri)=2.0*pvgn(mri) - pvg(mri)
      zug (mri)=2.0*zugn(mri) - zug(mri)
      zvg (mri)=2.0*zvgn(mri) - zvg(mri)
      eg (mri)=2.0*egn(mri) - eg(mri)
  110 continue
      return
      end
