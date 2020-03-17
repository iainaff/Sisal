/**************************************************************************/
/* FILE   **************         MakeEdge.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************         MakeEdge          ************************/
/**************************************************************************/
/* PURPOSE: MAKE AN EDGE AND INITIALIZE IT (INCLUDING FINDING ITS TYPE    */
/*          INFO).  THE EDGE IS LINKED TO ITS DESTINATION NODE (IN iport  */
/*          ORDER) AND ITS SOURCE NODE (IN RANDOM ORDER).                 */
/**************************************************************************/

void MakeEdge( snode, eport, dnode, iport, type )
int   snode, eport;
int   dnode, iport;
int   type;
{
  register PNODE dst;
  register PNODE src;
  register PEDGE e;

  dst  = FindNode( dnode, IFUndefined );
  src  = FindNode( snode, IFUndefined );

  e          = EdgeAlloc( src, eport, dst, iport );
  e->if1line = line;

  EdgeAssignPragmas( e );

  if ( DMarkProblem ) {
    /* FOR SOME YET KNOWN REASON, dmark EDGES ARE NOT FREED IN ALL CASES. */
    /* THIS CAN BE FORCED BY SETTING THE CM PRAGMA OF dmark EDGES TO -1.  */
    if ( e->dmark )             /* CANN 10-3 */
      e->cm = -1;               /* CANN 10-3 */
  }


  e->funct = NULL;              /* DON'T BOTHER FOR EDGE INFORMATION */
  e->file  = NULL;

  LinkImport( dst, e );
  LinkExport( src, e );

  e->info = FindInfo( type, IF_NONTYPE );
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:27  patmiller
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
 * Revision 1.1  1993/01/21  23:29:35  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
