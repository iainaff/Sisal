#include "world.h"


/**************************************************************************/
/* GLOBAL  **************        WriteStamps        ************************/
/**************************************************************************/
/* PURPOSE: WRITE THE IF1 STAMPS IN THE STAMP TABLE TO output.            */
/**************************************************************************/

void WriteStamps()
{
  register int i;

  for ( i = 0; i < 127; i++ )
    if ( stamps[i] != NULL )
      FPRINTF( output, "C$  %c %s\n", i, stamps[i] );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:54  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
