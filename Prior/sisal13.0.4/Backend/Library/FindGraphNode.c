#include "world.h"


/**************************************************************************/
/* GLOBAL **************      FindGraphNode        ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE GRAPH NODE OWNING NODE n.                          */
/**************************************************************************/

PNODE FindGraphNode( n )
PNODE n;
{
  register PNODE nn;

  for ( nn = n; !IsGraph(nn); nn = nn->npred ) 
    /* DO NOTHING */;

  return( nn );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:30  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
