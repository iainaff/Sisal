#include "world.h"


/**************************************************************************/
/* GLOBAL **************       AddToGraphList       ***********************/
/**************************************************************************/
/* PURPOSE: ADD NODE n TO THE END OF THE GLOBAL GRAPH NODE LIST.          */
/**************************************************************************/

void AddToGraphList( n )
register PNODE n;
{
  register PNODE f;
  for ( f = glstop->gsucc; f->gsucc != NULL ; f = f->gsucc)
	;
  f->gsucc = n;
  n->gpred = f;
  n->gsucc = NULL;
}

/* $Log$
 * Revision 1.1  1994/02/17  17:24:13  denton
 * Added for reductions
 *
 * Revision 1.3  1993/09/21  22:31:05  denton
 * *** empty log message ***
 *
 * Revision 1.2  1993/09/21  22:29:26  denton
 * *** empty log message ***
 *
 * Revision 1.1  1993/09/21  22:19:45  denton
 * Initial revision
 *
 * Revision 1.1  1993/01/21  23:27:41  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
