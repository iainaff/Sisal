#include "world.h"
/**************************************************************************/
/* GLOBAL **************         CallAlloc         ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN A CALL GRAPH NODE.           */
/**************************************************************************/

PCALL CallAlloc( n )
PNODE n;
{
  register PCALL c;

  c = GetNewCallNode();

  c->callee = c->caller = NULL;

  c->graph	= n;
  c->rcnt	= 0;
  c->InLineFunction   = TRUE;
  c->expanded = FALSE;
  c->cycle    = FALSE;
  c->skipped  = FALSE;
  c->disabled = FALSE;
  c->color    = WHITE;
  c->checked  = FALSE;

  return( c );
}

/* $Log$
 * Revision 1.2  1993/04/01  23:28:53  miller
 * GCC Support to eliminate warnings.
 *
 * Revision 1.1  1993/01/21  23:28:01  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
