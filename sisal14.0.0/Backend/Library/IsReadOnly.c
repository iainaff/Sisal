#include "world.h"


/**************************************************************************/
/* GLOBAL **************        IsReadOnly         ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF THE ARRAY EXPORTED FROM NODE n AT PORT eport   */
/*          IS READ ONLY.                                                 */
/**************************************************************************/

int IsReadOnly( n, eport )
PNODE n;
int   eport;
{
  register PEDGE e;
  register PNODE sg;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != eport )
      continue;

    if ( !IsArray( e->info ) )
      continue;

    if ( IsCompound( e->dst ) ) {
      for ( sg = e->dst->C_SUBS; sg != NULL; sg = sg->gsucc )
        if ( !IsReadOnly( sg, e->iport ) )
          return( FALSE );

      continue;
    }

    switch ( e->dst->type ) {
     case IFALimL:
     case IFASize:
     case IFALimH:
      break;

     case IFAElement:
      if ( !IsReadOnly( e->dst, 1 ) )
	return( FALSE );

      break;

     default:
      return( FALSE );
    }
  }

  return( TRUE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:15  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
