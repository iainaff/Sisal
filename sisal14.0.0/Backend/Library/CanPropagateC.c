#include "world.h"


/**************************************************************************/
/* GLOBAL **************     CanPropagateConst     ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF THE CONSTANT DEFINED BY EDGE i CAN BE MOVED   */
/*          INTO A COMPOUND NODES SUBGRAPHS.  ERROR CONSTANTS ARE NEVER   */
/*          MOVED AND NEGATIVE ARITHMETIC CONSTANTS ARE ONLY MOVED IF     */
/*          sgnok IS ENABLED.  NOTE THIS ROUTINE IS TO MAINTAIN GRAPH     */
/*          NORMALIZATION OF DI.                                          */
/**************************************************************************/

int CanPropagateConst( i )
PEDGE i;
{
  /* BUG FIX: 4/16/92 */  /* ERROR CONSTANT? */
  if ( IsArray( i->info ) || (i->CoNsT == NULL) )  
    return( FALSE );

  if ( IsArithmetic( i->info ) )
    if ( (i->CoNsT[0] == '-') && !sgnok )
      return( FALSE );

  return( TRUE );
}

/* $Log$
 * Revision 1.1  1993/04/16  18:59:57  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:03  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
