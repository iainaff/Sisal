/**************************************************************************/
/* FILE   **************      FindLargestPo.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************      FindLargestPort       ***********************/
/**************************************************************************/
/* PURPOSE: RETURN THE LAST IMPORT or EXPORT TO NODE n.                   */
/**************************************************************************/

int FindLargestPort( node )
PNODE node;
{
  register PEDGE edge;
  int largest_port = 0;

  /*** Check the boundaries. ***/

  if ( IsCompound ( node ) ) {
    PNODE       subgraph;
    int         sub_largest_port = 0;
    for (subgraph = node->C_SUBS; subgraph!=NULL; subgraph = subgraph->gsucc) {
      sub_largest_port = FindLargestPort( subgraph );
      if ( sub_largest_port > largest_port )
        largest_port = sub_largest_port;
    }
  }

  for ( edge = node->imp; edge != NULL; edge = edge->isucc ) {
    if ( edge->iport > largest_port )
        largest_port = edge->iport;
    if ( edge->isucc == NULL )
      break;
  }
  for ( edge = node->exp; edge != NULL; edge = edge->esucc ) {
    if ( edge->eport > largest_port )
        largest_port = edge->eport;
    if ( edge->esucc == NULL )
      break;
  }

  return largest_port;
}

/*
 * $Log$
 * Revision 1.1.1.1  2000/12/31 17:58:16  patmiller
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
 * Revision 1.1  1994/01/28  00:09:35  miller
 * new man pages and new helper functions for library
 *
 *
 */
