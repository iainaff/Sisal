#include "world.h"


/**************************************************************************/
/* GLOBAL **************    IsReductionInterface     **********************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF nm IS A REDUCTION INTERFACE FUNCTION.          */
/**************************************************************************/

int IsReductionInterface( nm )
char *nm;
{
  register char *p;
  register int   i;
  register int   ans;

  p = LowerCase( nm, FALSE, FALSE );
  ans = FALSE;

  for ( i = 0; i <= rtop; i++ )
    if ( strcmp( p, reduct[i] )  == 0 ) {
      ans = TRUE;
      break;
    }

  free( p );
  return( ans );
}

/* $Log$
 * Revision 1.1  1994/02/17  17:24:14  denton
 * Added for reductions
 *
 * Revision 1.1  1993/09/03  23:04:19  denton
 * Initial revision
 *
 * */
