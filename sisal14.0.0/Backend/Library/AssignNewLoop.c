#include "world.h"


/**************************************************************************/
/* GLOBAL **************    AssignNewLoopTports    ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW T PORT NUMBERS, STARTING WITH p, TO THE IMPORTS OF */
/*          LOOP l'S BODY SUBGRAPH AND ADJUST ALL REFERENCES. THE NEXT    */
/*          LEGAL PORT IS RETURNED.                                       */
/**************************************************************************/

int AssignNewLoopTports( p, l )
int   p;
PNODE l;
{
  register PEDGE i;
 if (l->L_BODY != NULL)
    {
      for ( i = l->L_BODY->imp; i != NULL; i = i->isucc )
	if ( (!l->L_INIT) && (!i) && (!IsImport( l->L_INIT, i->iport )) ) {
	  if ( (l->L_TEST) && (i->iport != p )) {
	    ChangeExportPorts( l->L_TEST, i->iport, -p );
	    i->iport = -p;
	  }
	  
	  p++;
	}
    }
  return( p );
}

/* $Log$
 * Revision 1.2  1994/03/03  17:13:58  solomon
 * Added some tests to help prevent failing when dealing with invalid
 * if1 code.
 *
 * Revision 1.1  1993/04/16  18:59:46  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:51  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
