/**************************************************************************/
/* FILE   **************        LinkImport.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************        LinkImport         ************************/
/**************************************************************************/
/* PURPOSE: ADD IMPORT e TO THE IMPORT LIST of NODE dst IN iport ORDER.   */
/*          THE PREDECESSOR OF THE FIRST IMPORT IS ALWAYS NULL.           */
/**************************************************************************/

void LinkImport( dst, e )
register PNODE dst;
register PEDGE e;
{
  register PEDGE i;

  echange = TRUE;

  e->ipred = NULL;
  e->isucc = NULL;
  e->dst   = dst;

  if ( dst->imp == NULL ) {     /* IMPORT LIST EMPTY */
    dst->imp = e;
    return;
  }

  if ( dst->imp->iport > e->iport ) { /* BEFORE FIRST IN LIST */
    e->isucc        = dst->imp;
    dst->imp->ipred = e;
    dst->imp        = e;
    return;
  }

  for( i = dst->imp; i->isucc != NULL; i = i->isucc ) /* WHERE? */
    if ( i->isucc->iport > e->iport )
      break;

  e->isucc  = i->isucc;         /* LINK AFTER i */
  e->ipred  = i;

  if ( i->isucc != NULL )
    i->isucc->ipred = e;

  i->isucc = e;
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:26  patmiller
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
 * Revision 1.1  1993/01/21  23:29:25  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
