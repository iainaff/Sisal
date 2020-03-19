#include "world.h"


/**************************************************************************/
/* GLOBAL **************         NodeAlloc         ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN A NODE.                      */
/**************************************************************************/

PNODE NodeAlloc( label, type )
int   label;
int   type;
{
  register PNODE n;
  register PBBLOCK b;

  b = MyBBlockAlloc();
  n = &(b->n);

  NodeInitPragmas( n );

  n->label = label;
  n->type  = type;
  n->next  = NULL;
  n->size  = -1;

  n->imp   = NULL;
  n->exp   = NULL;

  n->aimp  = NULL;
  n->aexp  = NULL;
  n->temp  = NULL;
  n->info  = NULL;
  n->usucc = NULL;
  n->color = WHITE;
  n->sdbx  = -1;

  n->flps  = NULL;

  n->gpred = n->gsucc = NULL;
  n->npred = n->nsucc = NULL;

  n->level  = InitialNodeLevel;
  n->lstack = NULL;
  n->visited  = FALSE;
  n->sorted   = FALSE;
  n->cnum   = 0;
  n->pbusy  = 1;

  n->executed = FALSE;
  n->checked  = FALSE;

  InitSizeInfo( n );

  return( n );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:51  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
