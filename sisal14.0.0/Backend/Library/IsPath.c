#include "world.h"


/**************************************************************************/
/* GLOBAL **************          IsPath           ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF A PATH EXISTS BETWEEN NODE src AND dst.  THIS  */
/*          ROUTINE ASSUMES THE GRAPH IS ACYCLIC.                         */
/**************************************************************************/

int IsPath( src, dst )
PNODE src;
PNODE dst; /* FIXED */
{
  register PEDGE e;
  register PADE  a;

  if ( src == dst ) 
    return( TRUE );

  if ( src->checked ) 
    return( FALSE );

  src->checked = TRUE;

  for ( e = src->exp; e != NULL; e = e->esucc )
    if ( !IsGraph( e->dst ) )
      if ( IsPath( e->dst, dst ) )
	return( TRUE );

  for ( a = src->aexp; a != NULL; a = a->esucc )
    if ( !IsGraph( a->dst ) )
      if ( IsPath( a->dst, dst ) )
	return( TRUE );

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:13  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
