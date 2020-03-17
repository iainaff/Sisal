/**************************************************************************/
/* FILE   **************         FindNode.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************         FindNode          ************************/
/**************************************************************************/
/* PURPOSE: FIND AND RETURN THE NODE WITH LABEL label.  IF label IS 0,    */
/*          THE GRAPH NODE OF THE GRAPH BEING CONSTRUCTED IS BEING        */
/*          REFERENCED (LAST ONE IN THE GRAPH LIST ADDRESSED BY glstop),  */
/*          ELSE SEARCH FOR IT IN THE NODE LIST ADDRESSED BY nlstop. IF   */
/*          NOT FOUND, IT IS CREATED, PLACED IN THE LIST IN label ORDER,  */
/*          AND RETURNED.                                                 */
/**************************************************************************/

PNODE FindNode( label, type )
int label;
int type;
{
  register PNODE n;

  if ( label == 0 )
    return( glstop->gpred );

  /* FIRST CHECK THE FAST NODE LOOKUP HASH TABLE */
  if ( (n = nhash[label%MAX_NHASH]) != NULL )
    if ( n->label == label ) {
      if ( n->type == IFUndefined )
        n->type = type;

      return( n );
    }

  if ( nprd != NULL )
    if ( nprd->label < label )
      goto DoThePlacement;

  for ( n = nlstop->nsucc; n != NULL; n = n->nsucc )
    if ( n->label == label ) {
      if ( n->type == IFUndefined )
        n->type = type;

      /* ENTER n INTO THE FAST LOOKUP HASH TABLE */
      nhash[label%MAX_NHASH] = n;

      return( n );
    }

 DoThePlacement:
  PlaceNode( n = NodeAlloc( label, type ) );

  /* ENTER n INTO THE FAST LOOKUP HASH TABLE */
  nhash[label%MAX_NHASH] = n;

  return( n );
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:17  patmiller
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
 * Revision 1.1  1993/01/21  23:28:36  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
