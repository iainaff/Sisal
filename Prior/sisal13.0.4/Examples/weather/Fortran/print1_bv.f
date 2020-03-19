      subroutine  print1
c
c   This routine prints out the values of some of the switches set on
c   model startup.

      include 'param_dec'
      include 'bv_dec'

      print*, 'Parameters for this run'
      print*, 'Run label is ',ihirmx
      if ( ilin.eq.1 ) then
      print*, 'MODEL IS LINEARISED ABOUT THE ZONAL MEAN FLOW '
      end if
      if ( imp .eq. 1 ) then
          print*, 'SEMI-IMPLICIT TIME STEP'
      else
          print*, 'EXPLICIT TIME STEP'
      end if
      print*, 'Time Step =  ', idelt
      print*, 'Total number of steps ', ktotal
      if ( izon .eq. 1 ) then
          print*,'ZONAL MEAN FLOW WILL BE HELD CONSTANT'
      end if
      if ( istart .eq. 1 ) then
          print*, 'MODEL RESTARTED FROM FULL HISTORY FILE'
      else
          print*, 'MODEL COLD STARTED, LINEAR BALANCE PERFORMED'
      end if
      print*, '  Free surface mean height = ', zmean
      print *,'  Linear drag coeff        = ', hdrag
      print *,'  Diffusion coeff          = ', hdiff
      print *,'  Time Filter coeff        = ', vnu
      return
      end
