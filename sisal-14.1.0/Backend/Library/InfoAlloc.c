/**************************************************************************/
/* FILE   **************        InfoAlloc.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"

/**************************************************************************/
/* GLOBAL **************         InfoAlloc         ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN AN INFO NODE.                */
/**************************************************************************/


PINFO InfoAlloc( label, type )
int   label;
int   type;
{
  register PINFO i;
  register PBBLOCK b;

  /* Update LargestTypeLabelSoFar. */

  if (LargestTypeLabelSoFar < label)
    LargestTypeLabelSoFar = label;

  b = MyBBlockAlloc();
  i = &(b->i);

  TypeInitPragmas( i );

  i->label = label;
  i->type  = type;

  i->info1 = NULL;
  i->info2 = NULL;
  i->touch1 = FALSE;
  i->touch2 = FALSE;
  i->touch3 = FALSE;
  i->touch4 = FALSE;
  i->touch5 = FALSE;
  i->touch6 = FALSE;
  i->touchflag = FALSE;

  i->fmem   = FALSE;

  i->tname  = NULL;
  i->cname  = NULL;
  i->sname  = NULL;
  i->rname  = NULL;
  i->wname  = NULL;

  i->fname1 = NULL;
  i->fname2 = NULL;

  i->next  = NULL;
  i->print = TRUE;

  i->temp = NULL;

  return( i );
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:24  patmiller
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
 * Revision 1.4  1994/06/24  16:01:32  denton
 * Added -IF3.
 *
 * Revision 1.3  1994/05/25  19:44:35  denton
 * Added FindLargestLabel to return the value of LargestTypeLabelSoFar.
 * Shortened name of GetAggregateType.c to remove ar warnings.
 *
 * Revision 1.2  1994/04/14  21:01:09  solomon
 * Added the global variable int LargestTypeLabelSoFar = 0.  Also added
 * code to update this variable.
 *
 * Revision 1.1  1993/01/21  23:28:56  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
