#include "world.h"


/**************************************************************************/
/* GLOBAL **************        LinkExport         ************************/
/**************************************************************************/
/* PURPOSE: ADD EDGE e TO THE HEAD OF src'S EXPORT LIST. THE PEDECESSOR   */  
/*          OF THE FIRST EXPORT IS ALWAYS NULL.                           */
/**************************************************************************/


void LinkExport( src, e )
PNODE src;
PEDGE e;
{
  e->src = src;

  e->epred = NULL;
  e->esucc = src->exp;

  if ( src->exp != NULL )
    src->exp->epred = e;

  src->exp = e;
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:21  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
