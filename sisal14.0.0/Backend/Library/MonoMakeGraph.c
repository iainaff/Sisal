#include "world.h"


/**************************************************************************/
/* GLOBAL **************     MonoMakeGraph     ************************/
/**************************************************************************/
/* PURPOSE: MAKE A NEW GRAPH NODE AND ADD IT TO THE NODE LIST. DEPENDING  */
/*          ON THE GRAPH NODE, AN ENTRY IS MADE IN THE APPROPRIATE NAME   */
/*          LIST (USED TO IDENTIFY UNDEFINED FUNCTIONS). NOTE, INTRINSICS */
/*          ARE NOT ADDED TO THE IMPORT NAME LIST AND NODE LIST; HENCE    */
/*          ARE NOT PLACED BACK INTO THE MONOLITH.                        */
/**************************************************************************/

void MonoMakeGraph( type, label, name )
int   type;
int   label;
char *name;
{
  register PNODE n;

  n = NodeAlloc( 0, type );

  if ( type != IFSGraph ) {
    sfunct   = name;
    n->info  = FindInfo( label, IF_FUNCTION );
    n->CoNsT = LowerCase( name, FALSE, FALSE );
  }

  NodeAssignPragmas( n );

  if ( type != IFIGraph && type != IFSGraph )
    AddToNameList( n );

  if ( type == IFIGraph ) {
    AddToImportList( n, n->CoNsT, n->info );
    return;
  }

  AddToNodeList( n );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:42  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:29:42  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
