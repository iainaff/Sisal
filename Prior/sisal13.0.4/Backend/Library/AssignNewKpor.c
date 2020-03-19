#include "world.h"


/**************************************************************************/
/* GLOBAL **************      AssignNewKports      ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW K PORT NUMBERS, STARTING WITH 1, TO THE IMPORTS OF */
/*          COMPOUND NODE c AND ADJUST ALL REFERENCES. THE NEXT LEGAL     */
/*          PORT NUMBER IS RETURNED.                                      */
/**************************************************************************/

int AssignNewKports( c )
PNODE c;
{
  register PNODE g;
  register PEDGE i;
  register int   p = 1;

  for ( i = c->imp; i != NULL; i = i->isucc, p++ )
    if ( i->iport != p ) {
      for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
	ChangeExportPorts( g, i->iport, -p );

      i->iport = -p;
    }

  return( p );
}

/* $Log$
 * Revision 1.1  1993/04/16  18:59:43  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:49  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
