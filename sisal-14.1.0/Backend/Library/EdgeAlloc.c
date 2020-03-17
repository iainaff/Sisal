/**************************************************************************/
/* FILE   **************        EdgeAlloc.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************         EdgeAlloc         ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN AN EDGE.                     */
/**************************************************************************/

PEDGE EdgeAlloc( src, eport, dst, iport )
PNODE src;
int   eport;
PNODE dst;
int   iport;
{
  register PEDGE   e;
  register PBBLOCK b;

  b = MyBBlockAlloc();
  e = &(b->e);

  e->eport = eport;
  e->src   = src;
  e->iport = iport;
  e->dst   = dst;
  e->dname = NULL;
  e->dope  = NULL;
  e->lvl   = 0;
  e->temp  = NULL;
  e->rc    = -1;
  e->vtemp = -1;
  e->info  = NULL;
  e->sdbx  = -1;

  e->usucc = NULL;
  e->epred = e->esucc = NULL;
  e->ipred = e->isucc = NULL;
  e->grset = e->gwset = NULL;
  e->lrset = e->lwset = NULL;

  e->uedge = NULL;
  e->if1line = 0;

  EdgeInitPragmas( e );
  InitSizeInfo( e );

  return( e );
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:14  patmiller
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
 * Revision 1.1  1993/01/21  23:28:20  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
