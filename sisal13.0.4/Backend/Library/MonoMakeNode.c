#include "world.h"


/**************************************************************************/
/* GLOBAL **************     MonoMakeNode      ************************/
/**************************************************************************/
/* PURPOSE: MAKE A SIMPLE NODE AND ADD IT TO THE NODE LIST.               */
/**************************************************************************/

void MonoMakeNode( label, type )
int   label;
int   type;
{
  register PNODE n;

  n = NodeAlloc( label, type );

  NodeAssignPragmas( n );
  AddToNodeList( n );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:43  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:29:43  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
