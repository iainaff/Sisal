#include "world.h"


/**************************************************************************/
/* GLOBAL **************        IntToAscii         ************************/
/**************************************************************************/
/* PURPOSE: BUILD AND RETURN THE ASCII REPRESENTATION OF INTEGER val.     */
/**************************************************************************/

char *IntToAscii( val )
int val;
{
  char buf[BUFFER_SIZE];

  SPRINTF( buf, "%d", val );

  return( CopyString( buf ) );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:58  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
