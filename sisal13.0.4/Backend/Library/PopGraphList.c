#include "world.h"


/**************************************************************************/
/* GLOBAL **************       PopGraphList        ************************/
/**************************************************************************/
/* PURPOSE: REMOVE THE TOP GRAPH LIST HEADER FROM GRAPH LIST HEADER STACK */
/*          IF IT IS NOT ALREADY EMPTY. WHEN DONE, glstop WILL ADDRESS THE*/
/*          PREVIOUS GRAPH LIST HEADER.  THE REMOVED HEADER NODE IS FREED.*/
/**************************************************************************/

void PopGraphList() 
{ 
  if ( glstop == NULL ) return;
  glstop = UnlinkNode( glstop );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:59  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
