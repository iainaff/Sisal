#include "world.h"


/**************************************************************************/
/* GLOBAL **************     DontCombineFill       ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF AFill NODE n CYCLES THROUGH A LOOP.            */
/**************************************************************************/

int DontCombineFill( n )
PNODE n;
{
  register PEDGE e;

  for ( e = n->exp; e != NULL; e = e->esucc ) {
    if ( IsLoop( e->dst ) ) {
      if ( IsExport( e->dst->L_INIT, e->iport ) )
	if ( !IsExport( e->dst->L_TEST, e->iport ) )
	  if ( !IsExport( e->dst->L_BODY, e->iport ) )
	    if ( !IsExport( e->dst->L_RET, e->iport ) )
	      return( TRUE );
    }
  }

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:11  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:18  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
