#include "world.h"


/**************************************************************************/
/* GLOBAL **************    IsFortranInterface     ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF nm IS A FORTRAN INTERFACE FUNCTION.            */
/**************************************************************************/

int IsFortranInterface( nm )
char *nm;
{
  register char *p;
  register int   i;
  register int   ans;

  p = LowerCase( nm, FALSE, FALSE );
  ans = FALSE;

  for ( i = 0; i <= ftop; i++ )
    if ( strcmp( p, fortt[i] )  == 0 ) {
      ans = TRUE;
      break;
    }

  free( p );
  return( ans );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:26  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:29:09  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
