#include "world.h"
/**************************************************************************/
/* GLOBAL **************          MyAlloc          ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE AND RETURN A POINTER TO size BYTES OF MEMORY. IF THE */
/*          ALLOCATION FAILS, AN ERROR MESSAGE IS PRINTED AND EXECUTION   */
/*          TERMINATES.                                                   */
/**************************************************************************/

char *MyAlloc( size )
int size;
{
  char *p;

  if ( (p = (char *)malloc( size )) == NULL )
    Error1( "MALLOC FAILED" );

  return( p );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:48  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
