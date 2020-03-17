/**************************************************************************/
/* FILE   **************      FindLargestIm.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************      FindLargestImport     ***********************/
/**************************************************************************/
/* PURPOSE: RETURN THE LARGEST IMPORT TO NODE node.                       */
/**************************************************************************/

int FindLargestImport( node )
PNODE node;
{
  register PEDGE edge;
  int largest_port = 0;

  if ( IsCompound ( node ) ) {
    PNODE       subgraph;
    int         sub_largest_port = 0;
    for (subgraph = node->C_SUBS; subgraph!=NULL; subgraph = subgraph->gsucc) {
      sub_largest_port = FindLargestImport( subgraph );
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

  return largest_port;
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
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
 * Revision 1.1  1994/01/28  00:09:34  miller
 * new man pages and new helper functions for library
 *
 *
 */
