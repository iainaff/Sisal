#include "world.h"


/**************************************************************************/
/* GLOBAL **************   MonoMakeCompound    ************************/
/**************************************************************************/
/* PURPOSE: MAKE A COMPOUND NODE AND ADD IT TO THE NODE LIST.             */
/**************************************************************************/

void MonoMakeCompound( type, alst )
int   type;
char *alst;
{
  register PNODE n;

  n = NodeAlloc( 0, type );

  n->CoNsT = alst;

  NodeAssignPragmas( n );
  AddToNodeList( n );
}

/* $Log$
 * Revision 1.1  1993/04/16  19:00:38  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:29:38  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
