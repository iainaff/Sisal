#include "world.h"


/**************************************************************************/
/* GLOBAL **************   CopyEdgeAndThreadToUse  ************************/
/**************************************************************************/
/* PURPOSE: CREATE A REFERENCE TO EDGE e AND THREAD IT TO NODE dst USING  */
/*          IMPORT PORT iport.  EDGE e MAY BE A CONSTANT!                 */
/**************************************************************************/

void CopyEdgeAndThreadToUse( e, dst, iport )
PEDGE e;
PNODE dst;
int   iport;
{
  register PEDGE ee;
  register PNODE sg;

  ee = CopyEdge( e, e->src, dst );

  if ( !IsConst( e ) )
    LinkExport( e->src, ee );

  if ( e->dst->level < dst->level ) {
    sg = dst->lstack[e->dst->level + 1];

    ee->iport = ++maxint;
    ee->dst   = sg->G_DAD;

    LinkImport( sg->G_DAD, ee );
    ThreadToUse( sg, maxint, dst,iport, ee->info );

    return;
  }

  ee->iport = iport;

  LinkImport( dst, ee );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:07  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:12  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
