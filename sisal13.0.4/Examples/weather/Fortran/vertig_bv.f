          subroutine vertig
c
c         in this subroutine non-linear terms are formed at grid points
c
      include 'param_dec'
      include 'bv_dec'
c
      do 100 ik=1,ilong
      pvg(ik)=pg(ik)*vg(ik)
      pug(ik)=pg(ik)*ug(ik)
      zvg(ik)=zg(ik)*vg(ik)
      zug(ik)=zg(ik)*ug(ik)
      eg(ik)=ug(ik)*ug(ik)+vg(ik)*vg(ik)
  100 continue
      return
      end
