#include "world.h"


/**************************************************************************/
/* GLOBAL **************         EdgeAlloc         ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN AN EDGE.                     */
/**************************************************************************/

PEDGE EdgeAlloc( src, eport, dst, iport )
PNODE src;
int   eport;
PNODE dst;
int   iport;
{
  register PEDGE   e;
  register PBBLOCK b;

  b = MyBBlockAlloc();
  e = &(b->e);

  e->eport = eport;
  e->src   = src;
  e->iport = iport;
  e->dst   = dst;
  e->dname = NULL;
  e->dope  = NULL;
  e->lvl   = 0;
  e->temp  = NULL;
  e->rc    = -1;
  e->vtemp = -1;
  e->info  = NULL;
  e->sdbx  = -1;

  e->usucc = NULL;
  e->epred = e->esucc = NULL;
  e->ipred = e->isucc = NULL;
  e->grset = e->gwset = NULL;
  e->lrset = e->lwset = NULL;

  e->uedge = NULL;
  e->if1line = 0;

  EdgeInitPragmas( e );
  InitSizeInfo( e );

  return( e );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:20  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
