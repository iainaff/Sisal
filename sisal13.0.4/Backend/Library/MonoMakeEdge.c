#include "world.h"


/**************************************************************************/
/* GLOBAL **************     MonoMakeEdge      ************************/
/**************************************************************************/
/* PURPOSE: MAKE AN EDGE AND ADD IT TO THE NODE LIST.                     */
/**************************************************************************/

void MonoMakeEdge( snode, eport, dnode, iport, label )
int   snode, eport;
int   dnode, iport;
int   label;
{
  register PNODE n;

  n = NodeAlloc( 0, IFEdge );

  NodeAssignPragmas( n );
  n->file = NULL; n->funct = NULL; /* JUST EXTRA BAGGAGE */

  n->snode = snode;
  n->eport = eport;
  n->dnode = dnode;
  n->iport = iport;
  n->info  = FindInfo( label, IF_NONTYPE );

  AddToNodeList( n );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:41  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:29:41  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
