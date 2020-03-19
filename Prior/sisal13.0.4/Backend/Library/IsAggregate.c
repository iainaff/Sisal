#include "world.h"


/**************************************************************************/
/* GLOBAL **************        IsAggregate        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF SYMBOL TABLE ENTRY i DEFINES AN AGGREGATE:     */
/*          UNION, RECORD, ARRAY, STREAM, OR MULTIPLE THERE OF.           */
/**************************************************************************/

int IsAggregate( i )
PINFO i;
{
  if ( IsMultiple( i ) )
    i = i->A_ELEM;

  switch( i->type ) {
   case IF_ARRAY:
   case IF_UNION:
   case IF_STREAM:
   case IF_RECORD:
    return( TRUE );

   default:
    break;
  }

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:01  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
