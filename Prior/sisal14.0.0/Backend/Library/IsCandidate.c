/**************************************************************************/
/* FILE   **************       IsCandidate.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************        IsCandidate        ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF n IS A POSSIBLE SIZE EXPRESSION NODE AND IS   */
/*          NOT DEAD (HAVING NO EXPORTS).                                 */
/**************************************************************************/

int IsCandidate( n )
PNODE n;
{
  if ( n->exp == NULL )
    return( FALSE );

  switch ( n->type ) {
   case IFPlus:
   case IFMinus:
   case IFTimes:
   case IFDiv:
   case IFMax:
   case IFNeg:
    if ( !IsInteger( n->exp->info ) )
      return( FALSE );

    return( TRUE );

   case IFALimL:
   case IFASize:
    if ( !IsArray( n->imp->info ) )
      return( FALSE );

    return( TRUE );

   default:
    return( FALSE );
  }
}

/*
 * $Log$
 * Revision 1.1.1.1  2000/12/31 17:58:25  patmiller
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
 * Revision 1.1  1993/01/21  23:29:05  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
