#include "world.h"


/**************************************************************************/
/* GLOBAL **************     ChangeImportPorts     ************************/
/**************************************************************************/
/* PURPOSE: FOR ALL IMPORTS OF NODE n CHANGE IMPORT PORT NUMBERS MATCHING */
/*          oport TO nport.                                               */
/**************************************************************************/

void ChangeImportPorts( n, oport, nport )
PNODE n;
int   oport;
int   nport;
{
  register PEDGE i;

  for ( i = n->imp; i != NULL; i = i->isucc )
    if ( i->iport == oport )
      i->iport = nport;
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:02  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:07  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
