#include "world.h"


/**************************************************************************/
/* GLOBAL **************        IsCandidate        ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF n IS A POSSIBLE SIZE EXPRESSION NODE AND IS   */
/*          NOT DEAD (HAVING NO EXPORTS).                                 */
/**************************************************************************/

int IsCandidate( n )
PNODE n;
{
  if ( n->exp == NULL )
    return( FALSE );

  switch ( n->type ) {
   case IFPlus:
   case IFMinus:
   case IFTimes:
   case IFDiv:
   case IFMax:
   case IFNeg:
    if ( !IsInteger( n->exp->info ) )
      return( FALSE );

    return( TRUE );

   case IFALimL:
   case IFASize:
    if ( !IsArray( n->imp->info ) )
      return( FALSE );

    return( TRUE );

   default:
    return( FALSE );
  }
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:05  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
