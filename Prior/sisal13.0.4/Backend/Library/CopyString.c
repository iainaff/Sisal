#include "world.h"


/**************************************************************************/
/* GLOBAL **************        CopyString         ************************/
/**************************************************************************/
/* PURPOSE: RETURN A COPY OF THE INPUT STRING s.                          */
/**************************************************************************/

char *CopyString( s ) 
char *s;
{
  return( strcpy( MyAlloc( (int)(strlen( s ) + 1) ), s ) );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:13  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
