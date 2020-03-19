/**************************************************************************/
/* FILE   **************       MakeCompound.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************       MakeCompound        ************************/
/**************************************************************************/
/* PURPOSE: MAKE A COMPOUND NODE.  ALL THE SUBGRAPHS HAVE BEEN BUILD AND  */
/*          ARE ADDRESSED BY glstop (IN ORDER OF ENCOUNTER). THE CURRENT  */
/*          NODE LIST ADDRESSED BY nlstop IS REMOVED TO UNCOVER THE SCOPE */
/*          DEFINING THE COMPOUND NODE AND TO COMPLETE CONSTRUCTION OF    */
/*          THE LAST SUBGRAPH.  THEN THE SUBGRAPHS ARE LINKED TO THE      */
/*          COMPOUND NODE, COMPLETING SCOPE EXIT.                         */
/**************************************************************************/

void MakeCompound( label, type, cnt, lst )
int    label;
int    type;
int    cnt;
PALIST lst;
{
  register PNODE n;

  PopNodeList();

  n          = FindNode( label, type );
  n->if1line = line;

  NodeAssignPragmas( n );

  n->C_SUBS = glstop->gsucc;

  if ( glstop->gsucc != NULL )
    glstop->gsucc->gpred = n;

  n->C_SCNT = cnt;
  n->C_ALST = lst;

  AssignTags( lst, n );

  PopGraphList();
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
 * Revision 1.2  1994/01/28  00:09:39  miller
 * new man pages and new helper functions for library
 *
 * Revision 1.1  1993/01/21  23:29:33  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
