#include "world.h"


/**************************************************************************/
/* GLOBAL **************         MakeConst         ************************/
/**************************************************************************/
/* PURPOSE: MAKE A CONSTANT EDGE AND INITIALIZE IT (INCLUDING FINDING ITS */
/*          TYPE INFO). THEN LINK THE EDGE INTO ITS DESTINATION NODE's    */
/*          IMPORT LIST IN label ORDER.                                   */
/**************************************************************************/

void MakeConst( dnode, iport, label, CoNsT )
int    dnode;
int    iport;
int    label;
char  *CoNsT;
{
  register PEDGE c;
  register PNODE dst;

  dst = FindNode( dnode, IFUndefined );
    
  c = EdgeAlloc( (PNODE)NULL, CONST_PORT, dst, iport );

  c->info    = FindInfo( label, IF_NONTYPE );
  c->CoNsT   = CoNsT;
  c->if1line = line;

  EdgeAssignPragmas( c );

  c->funct = NULL;		/* DON'T BOTHER FOR CONSTANTS */
  c->file  = NULL;

  LinkImport( dst, c );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:34  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
