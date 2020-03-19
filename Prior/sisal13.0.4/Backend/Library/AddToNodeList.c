#include "world.h"


/**************************************************************************/
/* GLOBAL **************       AddToNodeList       ************************/
/**************************************************************************/
/* PURPOSE: ADD NODE n TO THE END OF THE NODE LIST.                       */
/**************************************************************************/

void AddToNodeList( n )
register PNODE n;
{
  n->next = NULL;

  if ( ntail != NULL )
    ntail->next = n;
  else
    nhead = n;

  ntail = n;
}

/* $Log$
 * Revision 1.1  1993/01/21  23:27:41  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
