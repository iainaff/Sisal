#include "world.h"


/**************************************************************************/
/* GLOBAL **************         FindGraph         ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE nTH GRAPH IN THE GRAPH LIST ADDRESSED BY glstop.   */
/**************************************************************************/

PNODE FindGraph( n )
int n;
{
  register PNODE g;

  for ( g = glstop->gsucc; g != NULL; g = g->gsucc, n-- )
    if ( n == 0 )
      break;

  return( g );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:29  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
