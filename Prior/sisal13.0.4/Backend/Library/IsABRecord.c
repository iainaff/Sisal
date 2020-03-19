#include "world.h"
/**************************************************************************/
/* GLOBAL **************        IsABRecord         ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF INFO i IS A BASIC RECORD CANDIDATE.            */
/**************************************************************************/

#define MAX_BRECORD 4 /* MAXIMUM NUMBER OF FIELDS ALLOWED IN A BASIC RECORD */

int IsABRecord( i )
PINFO i;
{
  register PINFO ii;
  register int   c;

  if ( i->type != IF_RECORD )
    return( FALSE );

  for ( c = 0, ii = i->R_FIRST; ii != NULL; ii = ii->L_NEXT, c++ ) {
    if ( !IsBasic( ii->L_SUB ) )
      return( FALSE );
  }

  if ( c <= MAX_BRECORD )
    return( TRUE );

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:59  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
