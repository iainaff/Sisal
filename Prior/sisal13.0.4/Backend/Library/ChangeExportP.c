#include "world.h"


/**************************************************************************/
/* GLOBAL **************     ChangeExportPorts    *************************/
/**************************************************************************/
/* PURPOSE: FOR ALL EXPORTS OF NODE n CHANGE EXPORT PORT NUMBERS MATCHING */
/*          oport TO nport.                                               */
/**************************************************************************/

void ChangeExportPorts( n, oport, nport )
PNODE n;
int   oport;
int   nport;
{
  register PEDGE e;

  for ( e = n->exp; e != NULL; e = e->esucc )
    if ( e->eport == oport )
      e->eport = nport;
}

/* $Log$
 * Revision 1.1  1993/04/16  18:59:59  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:05  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
