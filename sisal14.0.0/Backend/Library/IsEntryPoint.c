#include "world.h"


/**************************************************************************/
/* GLOBAL **************        IsEntryPoint       ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE INDEX OF nm IF IT IS IN THE ENTRY POINT TABLE,     */
/*          ELSE RETURN -1.                                               */
/**************************************************************************/

int IsEntryPoint( nm )
char *nm;
{
  register int   i;
  register char *p;
  register int   ans;

  p = LowerCase( nm, FALSE, FALSE );
  ans = -1;

  for ( i = 0; i <= etop; i++ )
    if ( strcmp( p, entryt[i] )  == 0 ) {
      ans = i;
      break;
    }

  free( p );
  return( ans );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:08  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
