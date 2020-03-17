/**************************************************************************/
/* FILE   **************      PropagateCons.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************       PropagateConst      ************************/
/**************************************************************************/
/* PURPOSE: PROPAGATE ALL CONSTANT IMPORTS OF COMPOUND NODE c INTO ITS    */
/*          SUBGRAPHS. TO CONFOM TO DI EXPECTATIONS, NEGATIVE ARITHMETIC  */
/*          CONSTANTS ARE NOT MOVED INTO SUBGRAPHS---OVERRIDDEN BY sgnok. */
/**************************************************************************/

void PropagateConst( c )
PNODE c;
{
  register PNODE g;
  register PEDGE i;
  register PEDGE si;

  for ( i = c->imp; i != NULL; i = si ) {
    si = i->isucc;

    if ( !IsConst( i ) )
      continue;

    if ( !CanPropagateConst( i ) )
      continue;

    pcnt++;

    for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
      ChangeExportsToConst( g, i->iport, i );

    /* NEW CANN 12/6/90 */
    UnlinkImport( i );
  }
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:31  patmiller
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
 * Revision 1.1  1993/04/16  19:00:53  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:30:04  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
