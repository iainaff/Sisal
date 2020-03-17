/**************************************************************************/
/* FILE   **************        DirectEdge.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"

/**************************************************************************/
/* GLOBAL **************         DirectEdge        ************************/
/**************************************************************************/
/* PURPOSE: DIRECT AN EDGE FROM src TO dst IN SIMPLE OR COMPOUND GRAPH.   */
/**************************************************************************/

PEDGE
DirectEdge(src, eport, dst, iport, info)
  PNODE src;
  int   eport;
  PNODE dst;
  int   iport;
  PINFO info;
{
  PNODE src_graph;
  PNODE dest_graph;
  PEDGE edge;
  PNODE compound_graph;
  int   new_port;


  for (src_graph=src; !IsGraph(src_graph); src_graph = src_graph->npred);
  for (dest_graph=dst; !IsGraph(dest_graph); dest_graph = dest_graph->npred);

  /**********************************************************************/
  /* If the nodes are in the same graph, just link them together.       */
  /**********************************************************************/

  if ( src_graph == dest_graph ) {
    edge = EdgeAlloc( src, eport, dst, iport );

    EdgeAssignPragmas( edge );
    StandardPragmas(&pragmas);
    edge->info = info;
    edge->funct = NULL;
    edge->file  = NULL;

    LinkImport( dst, edge );
    LinkExport( src, edge );

  /**********************************************************************/
  /* Add an edge from the graph to the destination node and             */
  /* recursively add an edge from the source to the compound.           */
  /* Find the enclosing compound and first free port.                   */
  /**********************************************************************/

  } else {
    compound_graph = FindEnclosing(dest_graph);
    new_port = FindLargestPort(compound_graph) + 1;
    (void)DirectEdge(dest_graph, new_port, dst, iport, info);
    edge = DirectEdge(src, eport, compound_graph, new_port, info);
  }

  return edge;
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
 * Revision 1.3  1994/05/04  18:10:57  denton
 * R_BODY->R_MAP; FindEnclosingCompound->FindEnclosing
 *
 * Revision 1.2  1994/04/14  20:57:13  solomon
 * Changed variable tmax to LargestTypeLabelSoFar.  Variable tmax was used
 * for a couple things.  I created the LargestTypeLabelSoFar variable for
 * type labels only.
 *
 * Revision 1.1  1994/01/28  00:09:29  miller
 * new man pages and new helper functions for library
 *
 *
 */
