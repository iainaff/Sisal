#include "world.h"


/**************************************************************************/
/* GLOBAL **************        AttachEdge         ************************/
/**************************************************************************/
/* PURPOSE: ATTACHES AN EDGE OF TYPE info FROM NODE src TO NODE dst. THE  */
/*          SOURCE PORT IS eport AND THE DESTINATION PORT IS iport.       */
/**************************************************************************/

void AttachEdge( src, eport, dst, iport, edgeinfo, c )
PNODE  src;
PNODE  dst;
int    eport;
int    iport;
PINFO  edgeinfo;
char  *c;
{
  register PEDGE e;

  e = EdgeAlloc( src, eport, dst, iport );

  e->info  = edgeinfo;
  e->CoNsT = c;

  LinkImport( dst, e );

  if ( c == NULL )
    LinkExport( src, e );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:27:59  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
