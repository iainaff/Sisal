#include "world.h"


/**************************************************************************/
/* GLOBAL **************     CreateAndInsertAde    ************************/
/**************************************************************************/
/* PURPOSE: CREATE AN ARTIFICIAL DEPENDENCE EDGE WITH SOURCE NODE src,    */
/*          DESTINATION NODE dst, AND PRIORITY p; MAKING THE APPROPRIATE  */
/*          LINK LIST INSERTIONS. IF THE ADE WOULD INTRODUCE A CYCLE, IT  */
/*          IS NOT INSERTED. NOTE: BOUND EDGES NEVER INTRODUCE CYCLES.    */
/**************************************************************************/

void CreateAndInsertAde( src, dst, p )
PNODE src;
PNODE dst;
int   p;
{
  register PADE a;

  if ( p != BOUND ) {
    IsPathInit( src );
    if ( IsPath( dst, src ) ) {
      cycle++;
      return;
    }
  }

  if ( IsAdePresent(src,dst) ) return; /* Don't make two dependencies -- PJM */

  a = AdeAlloc( src, dst, p );

  LinkAdeExport( src, a );
  LinkAdeImport( dst, a );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:09  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.2  1993/04/16  17:07:42  miller
 * Added on advice of Steve Fitzger to eliminate duplicate ADE's
 *
 * Revision 1.1  1993/01/21  23:28:16  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
