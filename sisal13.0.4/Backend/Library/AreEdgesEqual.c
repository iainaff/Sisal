#include "world.h"


/**************************************************************************/
/* GLOBAL **************       AreEdgesEqual       ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF THE EDGES e1 AND e2 ARE EQUAL.  TO BE EQUAL,  */
/*          THEY MUST EITHER BOTH DEFINE THE SAME CONSTANTS OR ADDRESS    */
/*          THE SAME SOURCE NODE AND HAVE THE SAME SOURCE PORT NUMBERS.   */
/*          IF THEY ARE NOT EQUAL, FALSE IS RETURNED.                     */
/**************************************************************************/

int AreEdgesEqual( e1, e2 )
PEDGE e1;
PEDGE e2;
{
  if ( ExpandedEqual ) {
    if ( IsConst( e1 ) ^ IsConst( e2 ) ) /* XOR */
      return( FALSE );

    if ( IsConst( e1 ) )
      return( AreConstsEqual( e1, e2 ) );

    if ( (e1->src == e2->src) && (e1->eport == e2->eport) )
      return( TRUE );

  } else {
    if ( IsConst( e1 ) ) {
      if ( IsConst( e2 ) )
	return( AreConstsEqual( e1, e2 ) );

      return( FALSE );
    }

    /* IF e2 IS A CONST, e2->src WILL BE NULL AND e2->eport WILL BE CONST_PORT */
    if ( (e1->src == e2->src) && (e1->eport == e2->eport) )
      return( TRUE );

  }

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:27:44  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
