#include "world.h"


/**************************************************************************/
/* GLOBAL **************       PlaceGraph          ************************/
/**************************************************************************/
/* PURPOSE: PLACE A NEW GRAPH NODE AT END OF GRAPH LIST ADDRESSED BY      */ 
/*          glstop. NOTE THAT glstop->gpred ADDRESSES THE LAST GRAPH NODE */
/*          IN THE LIST, BUT IT IS NOT ADJUSTED BY UnlinkGraph.           */
/**************************************************************************/

void PlaceGraph( g )
PNODE g;
{
  if ( glstop->gsucc == NULL ) {
    glstop->gsucc = g;
    glstop->gpred = g;
    g->gpred      = glstop;
  }
  else
    glstop->gpred = LinkGraph( glstop->gpred, g );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:55  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
