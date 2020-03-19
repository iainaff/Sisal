#include "world.h"


/**************************************************************************/
/* GLOBAL **************         MakeGraph         ************************/
/**************************************************************************/
/* PURPOSE: MAKE A NEW GRAPH NODE AND INITIALIZE IT.  POP THE CURRENT NODE*/
/*          LIST HEADER OFF THE NODE LIST HEADER STACK SO TO COMPLETE     */
/*          CONSTRUCTION OF PREVIOUS GRAPH.  THEN PUSH A NEW NODE LIST    */
/*          HEADER FOR THE NODE LIST OF THE NEW GRAPH.  IF THE NEW GRAPH  */
/*          IS A FUNCTION, THEN FIND ITS TYPE INFORMATION.  THEN PLACE THE*/
/*          GRAPH NODE AT THE END OF THE CURRENT GRAPH LIST ADDRESSED BY  */
/*          glstop.                                                       */
/**************************************************************************/

void MakeGraph( type, label, name )
int   type;
int   label;
char *name;
{
  register PNODE n;

  n          = NodeAlloc( GRAPH_LABEL, type );
  n->if1line = line;

  PopNodeList();
  PushNodeList();

  if ( type != IFSGraph ) {
    n->G_INFO = FindInfo( label, IF_FUNCTION );
    n->G_NAME = name;
    cfunct    = n;
  }
  else
    n->G_TAGS = NULL;

  NodeAssignPragmas( n );

  if ( type == IFIGraph && n->smark  ) /* CANN 2/92 */
    n->ccost = DefaultFunctionCost;
  else if ( type == IFIGraph )
    n->ccost = IntrinsicCost;
  else if ( type != IFSGraph )
    n->ccost = DefaultFunctionCost;

  PlaceGraph( n );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:36  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
