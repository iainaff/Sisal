#include "world.h"


/**************************************************************************/
/* GLOBAL **************        GatherOthers       ************************/
/**************************************************************************/
/* PURPOSE: REMOVE MEMBER r AND ALL ITS EQUIVALENT SUCESSORS FROM THIER   */
/*          EQUIVALENCE CLASS AND PLACE THEM IN A NEW CLASS.  NODE p IS   */
/*          THE PREDECESSOR OF r.                                         */
/**************************************************************************/

void GatherOthers( p, r )
PINFO p;
PINFO r;
{
  register PINFO pm;

  /* MARK ALL ENTRIES EQUIVALENT TO REPRESENTATIVE r */

  for ( pm = r->mnext; pm != NULL; pm = pm->mnext )
    if ( SameEquivClass( r, pm ) )
      pm->label = -(pm->label);

  /* REMOVE ALL MARKED ENTRIES */

  RemoveFromEquivClass( p, r );
  CreateNewEquivClass( r );

  while ( p->mnext != NULL ) {
    pm = p->mnext;

    if ( pm->label < 0 ) {
      pm->label = -(pm->label);

      RemoveFromEquivClass( p, pm );
      AddToEquivClass ( r, pm );
    }
    else
      p = pm;
  }
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:42  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
