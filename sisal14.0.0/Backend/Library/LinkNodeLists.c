#include "world.h"


/**************************************************************************/
/* GLOBAL **************       LinkNodeLists       ************************/
/**************************************************************************/
/* PURPOSE: LINK THE NODE LIST OF GRAPH g2 TO THAT OF GRAPH g1. THE NODE  */
/*          LIST OF g2 IS SET TO NULL.                                    */
/**************************************************************************/

void LinkNodeLists( g1, g2 )
PNODE g1;
PNODE g2;
{
  register PNODE ln;
  register PNODE n;
  register PNODE ns;

  ln = FindLastNode( g1 );

  for ( n = g2->G_NODES; n != NULL; n = ns ) {
    ns = n->nsucc;
    ln = LinkNode( ln, n );
  }

  g2->G_NODES = NULL;
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:28  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
