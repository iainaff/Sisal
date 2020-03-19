/**************************************************************************/
/* FILE   **************         p-timer.c         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "sisalrt.h"

static double ElapsedTime( Start, Stop )
struct timeval *Start;
struct timeval *Stop;
{
  double Seconds;
  double USeconds;

  Seconds  = (double) (Stop->tv_sec  - Start->tv_sec);
  USeconds = (double) (Stop->tv_usec - Start->tv_usec); 

  if ( USeconds < 0.0 ) {
    USeconds += 1000000.0;
    Seconds  -= 1.0;
    }

  return( Seconds + (USeconds / 1000000.0) );
}

#ifdef HAVE_TSECND_INTRINSIC
extern double TSECND();
#else
/**************************************************************************/
/* GLOBAL **************          TSECND           ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE ELAPSED CPU TIME SINCE THE START OF THE PROGRAM.   */
/*          NOTE THAT TSECND IS A CRAY INTRINSIC!                         */
/**************************************************************************/

static double TSECND()
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

void StartTimer() 
{ 
  (void)gettimeofday( &(MyInfo->WallTimeBuffer), (struct timezone *) NULL );
  MyInfo->CpuTime = TSECND(); 
}

void StopTimer()  
{ 
  struct timeval StopWallTime;
  register double StopCpuTime;

  StopCpuTime = TSECND(); 
  (void)gettimeofday( &StopWallTime, (struct timezone *) NULL );

  MyInfo->WallTime = ElapsedTime( &(MyInfo->WallTimeBuffer), &StopWallTime );
  MyInfo->CpuTime  = (StopCpuTime - MyInfo->CpuTime);
}
