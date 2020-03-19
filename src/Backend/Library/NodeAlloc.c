/**************************************************************************/
/* FILE   **************        NodeAlloc.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************         NodeAlloc         ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN A NODE.                      */
/**************************************************************************/

PNODE NodeAlloc( label, type )
int   label;
int   type;
{
  register PNODE n;
  register PBBLOCK b;

  b = MyBBlockAlloc();
  n = &(b->n);

  NodeInitPragmas( n );

  n->label = label;
  n->type  = type;
  n->next  = NULL;
  n->size  = -1;

  n->imp   = NULL;
  n->exp   = NULL;

  n->aimp  = NULL;
  n->aexp  = NULL;
  n->temp  = NULL;
  n->info  = NULL;
  n->usucc = NULL;
  n->color = WHITE;
  n->sdbx  = -1;

  n->flps  = NULL;

  n->gpred = n->gsucc = NULL;
  n->npred = n->nsucc = NULL;

  n->level  = InitialNodeLevel;
  n->lstack = NULL;
  n->visited  = FALSE;
  n->sorted   = FALSE;
  n->cnum   = 0;
  n->pbusy  = 1;

  n->executed = FALSE;
  n->checked  = FALSE;

  InitSizeInfo( n );

  return( n );
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:30  patmiller
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
 * Revision 1.1  1993/01/21  23:29:51  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
