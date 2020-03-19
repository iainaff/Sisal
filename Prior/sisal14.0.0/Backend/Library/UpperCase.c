/**************************************************************************/
/* FILE   **************        UpperCase.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************         UpperCase         ************************/
/**************************************************************************/
/* PURPOSE: CONVERT THE LOWER CASE LETTERS OF NAME n TO UPPER CASE. THIS  */
/*          ROUTINE APPEND A '_' PREFIX AND SUFFIX TO n IF underl AND     */
/*          underr ARE TRUE RESPECTIVELY.                                 */
/**************************************************************************/

char *UpperCase( n, underl, underr )
char *n;
int   underl;
int   underr;
{
  register char *p;
  char  buf[100];

  SPRINTF( buf, "%s%s%s", (underl)? "_" : "", n, (underr)? "_" : "" );

  for ( p = buf; *p != '\0'; p++ )
    if ( (*p >= 'a') && (*p <= 'z') )
      *p = 'A' + (*p - 'a'); 
    else if ( *p == '.' )
      *p = '_';

  return( CopyString( buf ) );
}

/*
 * $Log$
 * Revision 1.1.1.1  2000/12/31 17:58:35  patmiller
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
 * Revision 1.1  1993/01/21  23:30:41  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */