#include "world.h"


/**************************************************************************/
/* GLOBAL **************          AdeAlloc         ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN AN ARTIFICIAL DEPENDENCE     */
/*          EDGE.                                                         */
/**************************************************************************/

PADE AdeAlloc( src, dst, p )
PNODE src;
PNODE dst;
int   p;
{
  PADE a;
  PBBLOCK b;

  /* a = (PADE) MyAlloc( sizeof(ADE) ); */
  b = MyBBlockAlloc();
  a = &(b->a);

  a->dst = dst;
  a->src = src;

  a->epred = a->esucc = NULL;
  a->ipred = a->isucc = NULL;

  a->priority = p;

  return( a );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:27:42  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
