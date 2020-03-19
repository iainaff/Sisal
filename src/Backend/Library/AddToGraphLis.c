/**************************************************************************/
/* FILE   **************      AddToGraphLis.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************       AddToGraphList       ***********************/
/**************************************************************************/
/* PURPOSE: ADD NODE n TO THE END OF THE GLOBAL GRAPH NODE LIST.          */
/**************************************************************************/

void AddToGraphList( n )
register PNODE n;
{
  register PNODE f;
  for ( f = glstop->gsucc; f->gsucc != NULL ; f = f->gsucc)
        ;
  f->gsucc = n;
  n->gpred = f;
  n->gsucc = NULL;
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:10  patmiller
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
 * Revision 1.1  1994/02/17  17:24:13  denton
 * Added for reductions
 *
 * Revision 1.3  1993/09/21  22:31:05  denton
 * *** empty log message ***
 *
 * Revision 1.2  1993/09/21  22:29:26  denton
 * *** empty log message ***
 *
 * Revision 1.1  1993/09/21  22:19:45  denton
 * Initial revision
 *
 * Revision 1.1  1993/01/21  23:27:41  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
