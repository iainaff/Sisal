#include "world.h"


int IsDivByZero( n )
PNODE n;
{
  if ( n->type != IFDiv )
    return( FALSE );

  if ( !IsConst( n->imp->isucc ) )
    return( FALSE );

  switch ( n->imp->isucc->info->type ) {
   case IF_DOUBLE:
   case IF_REAL:
    if ( atof( n->imp->isucc->CoNsT ) == 0.0 )
      return( TRUE );

    break;

   case IF_INTEGER:
    if ( atoi( n->imp->isucc->CoNsT ) == 0 )
      return( TRUE );

    break;

   default:
    break;
  }

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:06  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
