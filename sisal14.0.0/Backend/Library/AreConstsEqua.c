#include "world.h"


/**************************************************************************/
/* GLOBAL **************       AreConstsEqual      ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF CONSTANT c1 IS THE SAME AS CONSTANT c2, ELSE   */
/*          RETURN FALSE.  TWO ERROR CONSTANTS ARE NOT CONSIDERED EQUAL.  */
/**************************************************************************/

int AreConstsEqual( c1, c2 )
PEDGE c1;
PEDGE c2;
{
  if ( c1->CoNsT == NULL || c2->CoNsT == NULL )
    return( FALSE );

  if ( c1->info->type == c2->info->type )
    if ( strcmp( c1->CoNsT, c2->CoNsT ) == 0 )
      return( TRUE );

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/04/16  18:59:39  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:43  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
