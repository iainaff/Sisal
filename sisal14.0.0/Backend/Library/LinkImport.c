#include "world.h"


/**************************************************************************/
/* GLOBAL **************        LinkImport         ************************/
/**************************************************************************/
/* PURPOSE: ADD IMPORT e TO THE IMPORT LIST of NODE dst IN iport ORDER.   */
/*          THE PREDECESSOR OF THE FIRST IMPORT IS ALWAYS NULL.           */
/**************************************************************************/

void LinkImport( dst, e )
register PNODE dst;
register PEDGE e;
{
  register PEDGE i;

  echange = TRUE;

  e->ipred = NULL;
  e->isucc = NULL;
  e->dst   = dst;

  if ( dst->imp == NULL ) {	/* IMPORT LIST EMPTY */
    dst->imp = e;
    return;
  }

  if ( dst->imp->iport > e->iport ) { /* BEFORE FIRST IN LIST */
    e->isucc        = dst->imp;
    dst->imp->ipred = e;
    dst->imp        = e;
    return;
  }

  for( i = dst->imp; i->isucc != NULL; i = i->isucc ) /* WHERE? */
    if ( i->isucc->iport > e->iport )
      break;

  e->isucc  = i->isucc;		/* LINK AFTER i */
  e->ipred  = i;

  if ( i->isucc != NULL )
    i->isucc->ipred = e;

  i->isucc = e;
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:25  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
