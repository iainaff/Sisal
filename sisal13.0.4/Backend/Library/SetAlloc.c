#include "world.h"


/**************************************************************************/
/* GLOBAL **************          SetAlloc         ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN A READ/WRITE SET.            */
/**************************************************************************/

PSET SetAlloc( ssucc, scope )
PSET  ssucc;
PNODE scope;
{
  PSET s;
  PBBLOCK b;

  /* s = (PSET) MyAlloc( sizeof(SET) ); */
  b = MyBBlockAlloc();
  s = &(b->s);

  s->ssucc = ssucc;
  s->graph = scope;

  s->set   = NULL;
  s->last  = -1;
  s->ssize = -1;

  s->gen   = NULL;

  return( s );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:23  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
