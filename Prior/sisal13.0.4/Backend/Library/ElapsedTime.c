#include "world.h"


/**************************************************************************/
/* GLOBAL **************      ElapsedTime          ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE ELAPSED TIME BETWEEN StartT AND StopT.	  */
/**************************************************************************/

double ElapsedTime( StartT, StopT )
struct timeval *StartT;
struct timeval *StopT;
{
  double Seconds;
  double USeconds;		/* MICROSECONDS */

  Seconds  = (double) (StopT->tv_sec  - StartT->tv_sec);
  USeconds = (double) (StopT->tv_usec - StartT->tv_usec); 

  if ( USeconds < 0.0 ) {
    USeconds += 1000000.0;
    Seconds  -= 1.0;
  }

  return( Seconds + (USeconds / 1000000.0) );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:20  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
