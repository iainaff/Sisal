#include "world.h"


/**************************************************************************/
/* GLOBAL **************      NormalizeNode        ************************/
/**************************************************************************/
/* PURPOSE: IF NODE n IMPORTS A CONSTANT AND n IS A Plus OR Times NODE,   */
/*          THEN MAKE THE CONSTANT THE SECOND IMPORT.                     */
/**************************************************************************/

void NormalizeNode( n )
PNODE n;
{
  switch ( n->type ) {
   case IFPlus:
   case IFTimes:
    break;

   default:
    return;
  }

  if ( IsConst( n->imp ) )
    if ( IsInteger( n->imp->info ) )
      ImportSwap( n );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:52  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
