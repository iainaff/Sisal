#include "world.h"


/**************************************************************************/
/* GLOBAL **************     MonoWriteConst    ************************/
/**************************************************************************/
/* PURPOSE: WRITE CONSTANT c WITH TYPE t TO output. IF THE CONSTANT IS    */
/*          REPRESENTED BY A NULL POINTER, THEN IT IS AN ERROR CONSTANT.  */
/*          APPROPRIATE DELIMITERS ARE DETERMINED BASED ON THE CONSTANTS  */
/*          TYPE.                                                         */
/**************************************************************************/

void MonoWriteConst( c, t )
char  *c;
PINFO  t;
{
  if ( c == NULL ) {
    FPRINTF( output, " \"%s\"", ERROR_CONSTANT );
    return;
  }

  switch ( t->type ) {
   case IF_CHAR:
    FPRINTF( output, " \"\'%s\'\"", c );
    break;

   case IF_ARRAY:
   case IF_STREAM:
    FPRINTF( output, " \"\"%s\"\"", c );
    break;

   default:
    FPRINTF( output, " \"%s\"", c );
    break;
  }
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:46  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:30:50  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
