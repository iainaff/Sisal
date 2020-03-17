/**************************************************************************/
/* FILE   **************      FindEnclosing.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************         FindEnclosing        *********************/
/**************************************************************************/
/* PURPOSE: FIND THE STRUCTURE ENCLOSING THIS NODE.                       */
/*          A TOP-LEVEL MODULE RETURNS glstop.                            */
/*          A SUBGRAPH RETURNS ITS GRAPH.                                 */
/*          A GENERAL NODE RETURNS ITS ENCLOSING SUBGRAPH OR GRAPH        */
/**************************************************************************/

PNODE
FindEnclosing(node)
  PNODE node;
{
  PNODE g=NULL, graph;

  if ( IsGraph(node) ) {
      for (g = node->gpred; g!=NULL; g = g->gpred)
        graph = g;
  } else {
      for (graph = node->npred; !IsGraph(graph); graph = graph->npred);
  }

  return graph;
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:15  patmiller
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
 * Revision 1.4  1994/06/07  14:40:27  solomon
 * Function was returning NULL when node was a graph.  This is now fixed.
 *
 * Revision 1.3  1994/05/04  18:10:58  denton
 * R_BODY->R_MAP; FindEnclosingCompound->FindEnclosing
 *
 * Revision 1.2  1994/03/03  17:16:02  solomon
 * Fixed a bug in line 20.  It was "for (graph = graph->gpred ...)" when
 * graph had not yet been defined.  I changed it to "grapn = node->gpred."
 *
 * Revision 1.1  1994/01/28  00:09:30  miller
 * new man pages and new helper functions for library
 *
 *
 */
