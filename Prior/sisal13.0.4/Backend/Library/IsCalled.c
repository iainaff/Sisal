#include "world.h"


/**************************************************************************/
/* GLOBAL **************         IsCalled          ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF who IS CALLED BY A GRAPH IN THE GRAPH LIST.   */
/**************************************************************************/

int IsCalled( who )
char *who;
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    if ( IsCaller( f, who ) )
      return( TRUE );

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:03  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
