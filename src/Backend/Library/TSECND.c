/**************************************************************************/
/* FILE   **************          TSECND.c         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


#ifndef HAVE_TSECND_INTRINSIC
/**************************************************************************/
/* GLOBAL **************          TSECND           ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE ELAPSED CPU TIME SINCE THE START OF THE PROGRAM.   */
/*          NOTE THAT TSECND IS A CRAY INTRINSIC!                         */
/**************************************************************************/

double TSECND()
{
  register double CurrentCpuTime;

#ifdef HAVE_RUSAGE
  struct rusage StartTimerInfo;
  struct rusage StopTimerInfo;

  (void) getrusage( RUSAGE_SELF, &StopTimerInfo );

  StartTimerInfo.ru_utime.tv_sec  = 0;
  StartTimerInfo.ru_utime.tv_usec = 0;
  StartTimerInfo.ru_stime.tv_sec  = 0;
  StartTimerInfo.ru_stime.tv_usec = 0;

  CurrentCpuTime = 
    ElapsedTime( &StartTimerInfo.ru_utime, &StopTimerInfo.ru_utime ) +
      ElapsedTime( &StartTimerInfo.ru_stime, &StopTimerInfo.ru_stime );
#else
#ifdef HAVE_TIMES
  struct tms         StopTimeNow;

  (void)times(&StopTimeNow);
  CurrentCpuTime  = ((double) (StopTimeNow.tms_utime + StopTimeNow.tms_stime));
  CurrentCpuTime /= CLK_TCK;
#else
  /* Provide one with no functionality */
  CurrentCpuTime = 0.0;
#endif
#endif

  return( CurrentCpuTime );
}
#endif

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:46  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:34  patmiller
 * Well, here is the first set of big changes in the distribution
 * in 5 years!  Right now, I did a lot of work on configuration/
 * setup (now all autoconf), breaking out the machine dependent
 * #ifdef's (with a central acconfig.h driven config file), changed
 * the installation directories to be more gnu style /usr/local
 * (putting data in the /share/sisal14 dir for instance), and
 * reduced the footprint in the top level /usr/local/xxx hierarchy.
 *
 * I also wrote a new compiler tool (sisalc) to replace osc.  I
 * found that the old logic was too convoluted.  This does NOT
 * replace the full functionality, but then again, it doesn't have
 * 300 options on it either.
 *
 * Big change is making the code more portably correct.  It now
 * compiles under gcc -ansi -Wall mostly.  Some functions are
 * not prototyped yet.
 *
 * Next up: Full prototypes (little) checking out the old FLI (medium)
 * and a new Frontend for simpler extension and a new FLI (with clean
 * C, C++, F77, and Python! support).
 *
 * Pat
 *
 *
 * Revision 1.2  1994/06/20  21:50:52  denton
 * Added CLK_TCK.
 *
 * Revision 1.1  1993/01/21  23:30:28  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
