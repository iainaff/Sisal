
#include <stdio.h>
#include	<sys/time.h>
#include	<sys/resource.h>


/* *****                                                     */
/* ***** EXECUTION TIME AND RESOURCE UTILIZATION INFORMATION */
/* *****                                                     */

static double UserTime;
static double SysTime;

static struct rusage  StartUsage;
static struct rusage  StopUsage;

static   double Seconds;
static   double USeconds;

static double ElapsedTime( Start, Stop )
struct timeval *Start;
struct timeval *Stop;
{

  Seconds  = (double) (Stop->tv_sec  - Start->tv_sec);
  USeconds = (double) (Stop->tv_usec - Start->tv_usec); 

  if ( USeconds < 0.0 ) {
    USeconds += 1000000.0;
    Seconds  -= 1.0;
    }

  return( Seconds + (USeconds / 1000000.0) );
}

time1_()
{
  getrusage( RUSAGE_SELF, &StartUsage );
}

time2_()
{
  getrusage   ( RUSAGE_SELF, &StopUsage );

  UserTime = ElapsedTime( &StartUsage.ru_utime, &StopUsage.ru_utime );
  SysTime  = ElapsedTime( &StartUsage.ru_stime, &StopUsage.ru_stime );

  printf( "%f\n", UserTime + SysTime );
}
