#include "world.h"


/**************************************************************************/
/* GLOBAL **************    ChangeExportsToConst   ************************/
/**************************************************************************/
/* PURPOSE: CHANGE ALL EXPORTS OF NODE n WITH EXPORT PORT NUMBER MATCHING */
/*          eport INTO CONSTANT c.                                        */
/**************************************************************************/

void ChangeExportsToConst( n, eport, c )
PNODE n;
int   eport;
PEDGE c;
{
  register PEDGE e;
  register PEDGE se;

  for ( e = n->exp; e != NULL; e = se ) {
    se = e->esucc;

    if ( e->eport == eport )
      ChangeEdgeToConst( e, c );
  }
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:01  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:06  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
