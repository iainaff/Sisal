#include "world.h"


/**************************************************************************/
/* GLOBAL **************   MonoReadAssocList   ************************/
/**************************************************************************/
/* PURPOSE: READ AND RETURN A COMPOUND NODE's ASSOCIATION LIST. WHEN DONE */
/*          token WILL CONTAIN EITHER THE '%' CHARACTER OF THE FIRST      */
/*          PRAGMA OR EOLN.                                               */
/**************************************************************************/

char *MonoReadAssocList()
{
  register int idx = 0;

  for (;;) {
    if ( token == '%' || IsEoln( token ) )
      break;

    buffer[idx++] = token;

    if ( idx >= BUFFER_SIZE )
      ReadError( "ReadAssocList BUFFER OVERFLOW" );

    NextToken;
  }

  buffer[idx] = '\0';

  return( CopyString( buffer ) );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:45  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:30:15  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
