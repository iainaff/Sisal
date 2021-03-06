#include "world.h"


/**************************************************************************/
/* GLOBAL **************      AssignNewRports      ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW R PORT NUMBERS, STARTING WITH 1, TO THE IMPORTS OF */
/*          SUBGRAPH g AND ADJUST ALL REFERENCES IN THE EXPORT LIST OF    */
/*          THE COMPOUND NODE TO WHICH g BELONGS.                         */
/**************************************************************************/

void AssignNewRports( g )
PNODE g;
{
  register PEDGE i;
  register int   p = 1;

  if (g != NULL)
    {
      for ( i = g->imp; i != NULL; i = i->isucc, p++ )
	if ( i->iport != p ) {
	  ChangeExportPorts( g->G_DAD, i->iport, -p );
	  i->iport = -p;
	}
    }
}

/* $Log$
 * Revision 1.2  1994/03/03  17:14:01  solomon
 * Added some tests to help prevent failing when dealing with invalid
 * if1 code.
 *
 * Revision 1.1  1993/04/16  18:59:51  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:54  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
