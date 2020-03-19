      subroutine keepnh
c
c     this routine stores '*' arrays into '*n' arrays, to hold the NH
c     fourier coefficients for later manipulation.
c     The hemispheres are later combined by frsymm.
c     This is used in 'frtosp'.
c
      include 'param_dec'
      include 'bv_dec'
c
c--------------------------------------------------------------
c
c  pug is equivalent to pf which actually contains mx complex coefficients.
c  here they are all treated as reals.
      do 10 mri=1,mx2
      pugn(mri)=pug(mri)
      pvgn(mri)=pvg(mri)
      zugn(mri)=zug(mri)
      zvgn(mri)=zvg(mri)
      ugn(mri)=ug(mri)
      vgn(mri)=vg(mri)
      egn(mri)=eg(mri)
   10 continue
      return
      end
