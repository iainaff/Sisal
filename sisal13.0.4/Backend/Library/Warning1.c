#include "world.h"

int Warnings = TRUE;		/* TRUE iff we want to see warning msgs */

/**************************************************************************/
/* GLOBAL **************         Warning1          ************************/
/**************************************************************************/
/* PURPOSE: PRINT A WARNING MESSAGE TO stderr, THEN CONTINUE AS NORMAL.   */
/**************************************************************************/

void Warning1( msg1 )
char *msg1;
{
  if ( Warnings ) FPRINTF( stderr, "%s: W - %s\n", program, msg1 );
}

/* $Log$
 * Revision 1.2  1993/04/16  17:10:29  miller
 * Added support for warnings suppression (-w)
 *
 * Revision 1.1  1993/01/21  23:30:43  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
