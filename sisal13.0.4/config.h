/*****************************************************************************\
 * File:	config.h
 * Description:	standard header files
 * Note:	Tools/osc.c and several files in Runtime (especially p-ppp.c)
 *		may need upgraded for a new code port.
\*****************************************************************************/

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#if !defined(SUN)
#include <stdlib.h>
#endif
#include <signal.h>
#include <sys/wait.h>

#ifdef SGI
#include <ulocks.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <sys/sysmp.h>
#include <sys/schedctl.h>
#endif

#ifdef PTHREADS
#include <pthread.h>
#endif

#ifdef STHREADS
#include <thread.h>
#include <synch.h>
#endif

#if defined(DJGPP)
#include <process.h>
#else
#include <sys/file.h>
#endif

#ifdef USE_TIMES
#  include <sys/types.h>
#  include <sys/times.h>
#  ifdef SGI
#    include <sys/param.h>
#  endif
#endif

#ifdef SUN
   extern void exit();
   extern double atof();
   extern int atoi();
#  define CTIME_DEFINED
#  include <sys/time.h>
#  include <sys/resource.h>
#  define CLK_TCK 60
#  define const
#  define time_t long
#endif

#ifdef SUNOS
#  define memmove(dest, src, size) bcopy(src, dest, size)
#endif

#ifdef SGI
#  define CTIME_DEFINED
#  include <sys/time.h>
#  include <sys/resource.h>
#endif

#ifdef CRAY
#  define CTIME_DEFINED
#  include <time.h>
#  include <sys/time.h>
#endif

#if defined(HPUXPA) || defined(RS6000)
#  define CTIME_DEFINED
#  include <time.h>
#  include <sys/time.h>
#  include <sys/resource.h>
#endif

#ifndef CTIME_DEFINED
#  include <sys/time.h>
#  include <sys/resource.h>
#endif

#ifndef CLK_TCK
#  define CLK_TCK sysconf(_SC_CLK_TCK)
#endif

#define PRINTF		(void)printf
#define FPRINTF		(void)fprintf
#define SPRINTF		(void)sprintf
#define FCLOSE		(void)fclose
#define FPUTC		(void)fputc
#define UNGETC		(void)ungetc
#define SIZEOF(x)	((int)sizeof(x))

#define StreamError UNEXPECTED("stream node")

#if !defined(NO_ASSERTIONS) && !defined(ERROR_ASSERTIONS)
#define ASSERT(w, x)	{if (!(w)) { char msg[1000]; SPRINTF(msg, \
			"assert <%s> failed in %s line %d", \
			x, __FILE__, __LINE__); Warning1( msg ); }}
#else
#ifdef ERROR_ASSERTIONS
#define ASSERT(w, x)	{if (!(w)) { char msg[1000]; SPRINTF(msg, \
			"assert <%s> failed in %s line %d", \
			x, __FILE__, __LINE__); Error1( msg ); }}
#else
#define ASSERT(w, x)	{}	/* w is assertion logical, x is text */
#endif
#endif

#define UNEXPECTED(x)	{char msg[1000]; SPRINTF(msg, \
			"unexpected <%s> in %s line %d", \
			x, __FILE__, __LINE__); Warning1( msg );}
#define UNIMPLEMENTED(x) {char msg[1000]; SPRINTF(msg, \
			"unimplemented <%s> in %s line %d", \
			x, __FILE__, __LINE__); Warning1( msg );}
#define ERRORINFO(y, z)	{char msg[1000]; SPRINTF(msg, "  info "); \
			SPRINTF(&msg[7], y, z); Warning1( msg );}

#if !defined(TRUE)
#define TRUE             1
#define FALSE            0
#endif
