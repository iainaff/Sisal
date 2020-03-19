#include "world.h"


/**************************************************************************/
/* GLOBAL **************       PushNodeList        ************************/
/**************************************************************************/
/* PURPOSE: ADD A NEW NODE LIST HEADER TO THE CURRENT NODE LIST HEADER    */
/*          STACK. WHEN DONE, nlstop WILL ADDRESS THIS NEW HEADER.        */
/**************************************************************************/

void PushNodeList()
{
  register int idx;

  /* CLEAR THE FAST NODE LOOKUP HASH TABLE */
  for ( idx = 0; idx < MAX_NHASH; idx++ )
    nhash[idx] = NULL;

  if ( nlstop != NULL )
    nlstop->usucc = nprd;
  nprd = NULL;

  nlstop = LinkGraph( nlstop, NodeAlloc( 0, 0 ) );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:07  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
