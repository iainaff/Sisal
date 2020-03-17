/**************************************************************************/
/* FILE   **************       UnlinkExport.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************       UnlinkExport        ************************/
/**************************************************************************/
/* PURPOSE: UNLINK EDGE e FROM ITS SOURCE NODE'S EXPORT LIST.  IF e IS A  */
/*          CONSTANT, NOTHING IS DONE.                                    */
/**************************************************************************/

void UnlinkExport( e )
PEDGE e;
{
  if ( IsConst( e ) )
    return;

  if ( e->epred == NULL ) {     /* FIRST ONE IN LIST */
    e->src->exp = e->esucc;

    if ( e->esucc != NULL )
      e->esucc->epred = NULL;
  } else {
    e->epred->esucc = e->esucc;

    if ( e->esucc != NULL )
      e->esucc->epred = e->epred;
  }
}

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
 * Revision 1.3  1994/07/21  16:33:10  denton
 * Removed NULL assignment just put in to detect bad memory access.  Will fix later
 *
 * Revision 1.1  1993/01/21  23:30:36  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
