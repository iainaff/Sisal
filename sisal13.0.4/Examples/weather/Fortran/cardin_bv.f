      subroutine  cardin
 
c  This routine reads in the model control variables from the standard input

      include 'param_dec'
      include 'bv_dec'
c
      ifcann = 2
      open(unit=ifcann,file='datacards',status='old',form='formatted')
      rewind ifcann

      read(ifcann,1009) ktotal
      read(ifcann,1009) idelt
      read(ifcann,1009) idumpt
      read(ifcann,1009) ihirmx

      read(ifcann,1009) imp
      read(ifcann,1009) istart
      read(ifcann,1009) izon
      read(ifcann,1009) ilin

      read(ifcann,1010) zmean, hdiff, hdrag, vnu

 1009 format(I12)
 1010 format(e15.7)
      return
      end
