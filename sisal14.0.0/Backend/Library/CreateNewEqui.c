#include "world.h"


/**************************************************************************/
/* GLOBAL **************    CreateNewEquivClass    ************************/
/**************************************************************************/
/* PURPOSE: CREATE A NEW EQUIVALENCE CLASS WHOSE FIRST ENTRY IS r.        */
/**************************************************************************/

void CreateNewEquivClass( r )
PINFO r;
{
  if ( lclass++ >= MaxClass )
    Error1( "CreateNewEquivClass: OUT OF EQUIVALENCE CLASSES" );

  r->mnext = NULL;
  r->eid   = lclass;

  htable[ lclass ] = ttable[ lclass ] = r;
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:10  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:17  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
