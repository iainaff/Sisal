#include "world.h"


/**************************************************************************/
/* GLOBAL **************        UsageCount         ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE NUMBER OF EXPORT EDGES ATTACHED TO NODE n WITH     */
/*          EXPORT PORT eport.                                            */
/**************************************************************************/

int UsageCount( n, eport )
PNODE n;
int   eport;
{
  register PEDGE e;
  register int   cnt = 0;

  for ( e = n->exp; e != NULL; e = e->esucc )
    if ( e->eport == eport )
      cnt++;

  return( cnt );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:42  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
