#include "world.h"


/**************************************************************************/
/* GLOBAL **************       AreNodesEqual       ************************/
/**************************************************************************/
/* PURPOSE: CHECK IF TWO NODES ARE EQUIVALENT, RETURNING TRUE IF THEY ARE */
/*          AND FALSE IF THEY ARE NOT.                                    */
/**************************************************************************/

int AreNodesEqual( n1, n2 )
PNODE n1;
PNODE n2;
{
  register PEDGE i1;
  register PEDGE i2;

  if ( !IsSimple( n1 ) )
    return( FALSE );

  if ( n1->type != n2->type )
    return( FALSE );

  if ( glue && IsCall( n1 ) )
    return( FALSE );

  switch ( n1->type ) {
   case IFABuild:case IFSBuild:/*cy--streams should have same semantics*/
    /* BUG FIX FOR EMPTY ARRAY BUILDS --- 7/14/89 CANN */
    if ( n1->imp->isucc == NULL ) return( FALSE );
    /* END BUG FIX */

   case IFAFill:
   case IFRBuild:
   case IFAGather:
   case IFSGather:
    if ( n1->exp->info->type != n2->exp->info->type )
      return( FALSE );

   default:
    break;
  }

  /* CANN 6/20/90 */
  if ( n1->type == IFAFill ) {
    if ( DontCombineFill( n1 ) ) return ( FALSE );
    if ( DontCombineFill( n2 ) ) return ( FALSE );
  }

  /* TAKE ADVANTAGE OF COMMUTATIVITY */
  switch ( n1->type ) {
   case IFPlus:
   case IFTimes:
    if ( noassoc ) break;

    /* IF A = C THEN B MUST = D */
    if ( AreValuesEqual( n1->imp, n2->imp ) )
      if ( AreValuesEqual( n1->imp->isucc, n2->imp->isucc ) )
	return( TRUE );

    /* IF A = D THEN B MUST = C */
    if ( AreValuesEqual( n1->imp, n2->imp->isucc ) )
      if ( AreValuesEqual( n1->imp->isucc, n2->imp ) )
	return( TRUE );

    return( FALSE );

   default:
    break;
  }

  i1 = n1->imp;
  i2 = n2->imp; 
    
  while ( (i1 != NULL) && (i2 != NULL) ) {
    if ( !AreValuesEqual( i1, i2 ) )
      return( FALSE );

    i1 = i1->isucc;
    i2 = i2->isucc;
  }

  if ( (i1 == NULL) && (i2 == NULL) )
    return( TRUE );

  /* UNEQUAL NUMBER OF IMPORTS */
  return( FALSE );  
}

/* $Log$
 * Revision 1.2  1994/08/29  08:01:33  chad
 * New STREAM node numbers (333 to ...) have been added.
 * Predicates for identifying STREAM nodes also added.
 *
 * Revision 1.1  1993/01/21  23:27:46  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
