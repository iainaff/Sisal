#include "world.h"


/**************************************************************************/
/* GLOBAL **************        IsInnerLoop        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF THE LOOP WITH BODY b IS AN INNER LOOP.         */
/**************************************************************************/

int IsInnerLoop( b )
PNODE b;
{
  register PNODE n;
  register PNODE sg;

  for ( n = b->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      if ( !IsSelect( n ) )
        return( FALSE );

      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        if ( !IsInnerLoop( sg ) )
          return( FALSE );
    }
  }

  return( TRUE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:10  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
