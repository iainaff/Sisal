#include "world.h"


/**************************************************************************/
/* GLOBAL **************          IsCaller         ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF GRAPH g CALLS FUNCTION who.                    */
/**************************************************************************/

int IsCaller( g, who )
PNODE  g;
char  *who;
{
  register PNODE  n;
  register PNODE  sg;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        if ( IsCaller( sg, who ) )
	  return( TRUE );

    if ( IsCall( n ) )
      if ( strcmp( n->imp->CoNsT, who ) == 0 )
	return( TRUE );
  }

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:04  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
