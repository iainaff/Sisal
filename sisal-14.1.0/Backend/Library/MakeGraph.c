/**************************************************************************/
/* FILE   **************        MakeGraph.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************         MakeGraph         ************************/
/**************************************************************************/
/* PURPOSE: MAKE A NEW GRAPH NODE AND INITIALIZE IT.  POP THE CURRENT NODE*/
/*          LIST HEADER OFF THE NODE LIST HEADER STACK SO TO COMPLETE     */
/*          CONSTRUCTION OF PREVIOUS GRAPH.  THEN PUSH A NEW NODE LIST    */
/*          HEADER FOR THE NODE LIST OF THE NEW GRAPH.  IF THE NEW GRAPH  */
/*          IS A FUNCTION, THEN FIND ITS TYPE INFORMATION.  THEN PLACE THE*/
/*          GRAPH NODE AT THE END OF THE CURRENT GRAPH LIST ADDRESSED BY  */
/*          glstop.                                                       */
/**************************************************************************/

void MakeGraph( type, label, name )
int   type;
int   label;
char *name;
{
  register PNODE n;

  n          = NodeAlloc( GRAPH_LABEL, type );
  n->if1line = line;

  PopNodeList();
  PushNodeList();

  if ( type != IFSGraph ) {
    n->G_INFO = FindInfo( label, IF_FUNCTION );
    n->G_NAME = name;
    cfunct    = n;
  }
  else
    n->G_TAGS = NULL;

  NodeAssignPragmas( n );

  if ( type == IFIGraph && n->smark  ) /* CANN 2/92 */
    n->ccost = DefaultFunctionCost;
  else if ( type == IFIGraph )
    n->ccost = IntrinsicCost;
  else if ( type != IFSGraph )
    n->ccost = DefaultFunctionCost;

  PlaceGraph( n );
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
 * Revision 1.1  1993/01/21  23:29:36  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
