#include "world.h"


/**************************************************************************/
/* GLOBAL **************        EnterInSet         ************************/
/**************************************************************************/
/* PURPOSE: ENTER EDGE e IN READ/WRITE SET s.  OVERFLOW RESULTS IN AN     */
/*          ERROR MESSAGE.                                                */
/**************************************************************************/

void EnterInSet( s, e )
PSET  s;
PEDGE e;
{
  register PEDGE *p;
  register int    i;

  if ( s == NULL )
    return;


  if ( s->last == s->ssize ) {
    p = s->set;
    s->set = (PEDGE*) MyAlloc((int)sizeof(PEDGE)*(s->ssize + SET_INC + 1));

    for ( i = 0; i <= s->last; i++ )
      s->set[i] = p[i];

    s->ssize += SET_INC;

    /* if ( p != NULL ) */
    /* free( p ); */
  }

  s->last++;
  s->set[s->last] = e;
}

/* $Log$
 * Revision 1.1  1993/01/21  23:28:21  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
