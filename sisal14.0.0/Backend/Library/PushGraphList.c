#include "world.h"


/**************************************************************************/
/* GLOBAL **************       PushGraphList       ************************/
/**************************************************************************/
/* PURPOSE: ADD A NEW GRAPH LIST HEADER TO THE CURRENT GRAPH LIST HEADER  */
/*          STACK. WHEN DONE, glstop WILL ADDRESS THIS NEW HEADER.        */
/**************************************************************************/

void PushGraphList() 
{ 
  glstop = LinkNode( glstop, NodeAlloc( 0, 0 ) );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:06  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
