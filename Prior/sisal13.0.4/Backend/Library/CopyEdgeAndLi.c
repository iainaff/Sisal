#include "world.h"


/**************************************************************************/
/* GLOBAL **************     CopyEdgeAndLink       ************************/
/**************************************************************************/
/* PURPOSE: COPY EDGE e WITH DESTINATION NODE dst AND LINK IT INTO THE    */
/*          GRAPH. THE DESTINATION INPUT PORT NUMBER IS iport.            */
/**************************************************************************/

void CopyEdgeAndLink( e, dst, iport )
PEDGE e;
PNODE dst;
int   iport;
{
  register PEDGE i;

  i = CopyEdge( e, e->src, dst );
  i->iport = iport;

  /* PRESERVE REFERENCE COUNT OPERATIONS */
  i->sr = e->sr;
  i->pm = e->pm;

  if ( !IsConst( e ) )
    LinkExport( e->src, i );

  LinkImport( dst, i );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:05  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:28:11  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
