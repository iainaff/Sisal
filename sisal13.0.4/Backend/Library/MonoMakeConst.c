#include "world.h"


/**************************************************************************/
/* GLOBAL **************     MonoMakeConst     ************************/
/**************************************************************************/
/* PURPOSE: MAKE A CONSTANT EDGE AND ADD IT TO THE NODE LIST.             */
/**************************************************************************/

void MonoMakeConst( dnode, iport, label, CoNsT )
int    dnode;
int    iport;
int    label;
char  *CoNsT;
{
  register PNODE n;

  n = NodeAlloc( 0, IFLiteral );

  NodeAssignPragmas( n );
  n->file = NULL; n->funct = NULL; /* JUST EXTRA BAGGAGE */

  n->dnode = dnode;
  n->iport = iport;
  n->info  = FindInfo( label, IF_NONTYPE );
  n->CoNsT = CoNsT;

  AddToNodeList( n );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:40  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:29:40  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
