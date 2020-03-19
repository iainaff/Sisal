#include "world.h"


/**************************************************************************/
/* GLOBAL **************         MakeEdge          ************************/
/**************************************************************************/
/* PURPOSE: MAKE AN EDGE AND INITIALIZE IT (INCLUDING FINDING ITS TYPE    */
/*          INFO).  THE EDGE IS LINKED TO ITS DESTINATION NODE (IN iport  */
/*          ORDER) AND ITS SOURCE NODE (IN RANDOM ORDER).                 */
/**************************************************************************/

void MakeEdge( snode, eport, dnode, iport, type )
int   snode, eport;
int   dnode, iport;
int   type;
{
  register PNODE dst;
  register PNODE src;
  register PEDGE e;

  dst  = FindNode( dnode, IFUndefined );
  src  = FindNode( snode, IFUndefined );

  e          = EdgeAlloc( src, eport, dst, iport );
  e->if1line = line;

  EdgeAssignPragmas( e );

  if ( DMarkProblem ) {
    /* FOR SOME YET KNOWN REASON, dmark EDGES ARE NOT FREED IN ALL CASES. */
    /* THIS CAN BE FORCED BY SETTING THE CM PRAGMA OF dmark EDGES TO -1.  */
    if ( e->dmark )		/* CANN 10-3 */
      e->cm = -1;		/* CANN 10-3 */
  }


  e->funct = NULL;		/* DON'T BOTHER FOR EDGE INFORMATION */
  e->file  = NULL;

  LinkImport( dst, e );
  LinkExport( src, e );

  e->info = FindInfo( type, IF_NONTYPE );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:35  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
