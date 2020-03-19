#include "world.h"


/**************************************************************************/
/* GLOBAL **************          Error2           ************************/
/**************************************************************************/
/* PURPOSE: PRINT TWO ERROR MEASAGES TO stderr AND ABORT EXECUTION.       */
/**************************************************************************/

void Error2( msg1, msg2 )
char *msg1;
char *msg2;
{
  FPRINTF( stderr, "%s: E - %s %s\n", program, msg1, msg2 );
  Stop( ERROR );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:25  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
