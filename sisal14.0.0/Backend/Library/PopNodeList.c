#include "world.h"


/**************************************************************************/
/* GLOBAL **************        PopNodeList        ************************/
/**************************************************************************/
/* PURPOSE: REMOVE THE TOP NODE LIST HEADER FROM NODE LIST HEADER STACK   */
/*          IF IT IS NOT ALREADY EMPTY. WHEN DONE, nlstop WILL ADDRESS    */
/*          THE PREVIOUS NODE LIST HEADER ON THE STACK. THE NODE LIST OF  */
/*          THE HEADER IS ASSIGNED TO THE GRAPH NODE OF THE GRAPH         */
/*          CURRENTLY BEING COMPLETED (ADDRESSED BY glstop->gpred ). THEN */
/*          THE REMOVED HEADER IS FREED.                                  */
/**************************************************************************/

void  PopNodeList() 
{ 
  register int   idx;

  /* CLEAR THE FAST NODE LOOKUP HASH TABLE */
  for ( idx = 0; idx < MAX_NHASH; idx++ )
    nhash[idx] = NULL;
  nprd = NULL;

  if ( nlstop == NULL )
    return;

  if ( glstop->gpred != NULL ) {
    glstop->gpred->G_NODES = nlstop->nsucc;

    if ( nlstop->nsucc != NULL )
      nlstop->nsucc->npred = glstop->gpred;
  }

  nlstop = UnlinkGraph( nlstop );

  /* RESTORE OLD NODE LIST TAIL */
  if ( nlstop != NULL )
    nprd = nlstop->usucc;
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:00  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
