#include "world.h"


/**************************************************************************/
/* GLOBAL **************       IsCInterface        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF nm IS A C INTERFACE FUNCTION.                  */
/**************************************************************************/

int IsCInterface( nm )
char *nm;
{
  register char *p;
  register int   i;
  register int   ans;

  p = LowerCase( nm, FALSE, FALSE );
  ans = FALSE;

  for ( i = 0; i <= ctop; i++ )
    if ( strcmp( p, ct[i] )  == 0 ) {
      ans = TRUE;
      break;
    }

  free( p );
  return( ans );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:02  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
