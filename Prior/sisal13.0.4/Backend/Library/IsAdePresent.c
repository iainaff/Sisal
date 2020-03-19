#include "world.h"


/**************************************************************************/
/* GLOBAL **************       IsAdePresent        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF AN ADE EXIST BETWEEN NODe src (THE ORIGIN) AND */
/*          NODE dst (THE DESTINATION), ELSE RETURN FALSE.                */
/**************************************************************************/

int IsAdePresent( src, dst )
PNODE src;
PNODE dst;
{
  register PADE a;

  for ( a = src->aexp; a != NULL; a = a->esucc )
    if ( a->dst == dst )
      return( TRUE );

  return( FALSE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:00  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
