#include "world.h"


/**************************************************************************/
/* GLOBAL **************      TouchTheUnions       ************************/
/**************************************************************************/
/* PURPOSE: TOUCH ALL UNION TYPES (touch2) REACHED THROUGH TYPE i.        */
/**************************************************************************/

void TouchTheUnions( i )
PINFO i;
{
  register PINFO ii;

  /* WATCH OUT FOR RECURSIVE UNIONS! */
  if ( i->touch2 )
    return;

  switch( i->type ) {
   case IF_UNION:
    i->touch2 = TRUE;

   case IF_RECORD:
    for ( ii = i->R_FIRST; ii != NULL; ii = ii->L_NEXT )
      TouchTheUnions( ii->L_SUB );

    break;

   case IF_ARRAY:
    TouchTheUnions( i->A_ELEM );
    break;

   default:
    break;
  }
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:58  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:30:33  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
