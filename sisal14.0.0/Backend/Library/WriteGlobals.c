#include "world.h"


/**************************************************************************/
/* GLOBAL **************        WriteGlobals       ************************/
/**************************************************************************/
/* PURPOSE: WRITE GLOBAL NODES IN THE IMPORT LIST TO output.              */
/**************************************************************************/

void WriteGlobals()
{
  register PNAME i;

  for ( i = inames; i != NULL; i = i->next ) {
    if ( i->mark == '\0' )
      continue;

    FPRINTF( output, "I %2d", i->info->label );
    FPRINTF( output, " \"%s\"", i->name );

    FPRINTF( output, " %%mk=%c", i->mark );
    FPRINTF( output, "\n" );
  }
}

/* $Log$
 * Revision 1.2  1994/04/01  00:02:56  denton
 * NULL -> '\0' where appropriate
 *
 * Revision 1.1  1993/01/21  23:30:45  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
