#include "world.h"


/**************************************************************************/
/* GLOBAL **************        ThreadToUse        ************************/
/**************************************************************************/
/* PURPOSE: THREAD A REFERENCE FROM NODE src EXPORT PORT eport TO NODE    */
/*          dst IMPORT PORT iport; THE THREAD TYPE IS edgeinfo.		  */
/**************************************************************************/

void ThreadToUse( src, eport, dst, iport, edgeinfo )
PNODE src;
PNODE dst;
int   eport;
int   iport;
PINFO edgeinfo;
{
  register PNODE sg;

  while ( src->level != dst->level ) {
    sg = dst->lstack[src->level + 1];

    AttachEdge( src, eport, sg->G_DAD, ++maxint, edgeinfo, (char*)NULL );

    eport = maxint;
    src   = sg;
  }

  AttachEdge( src, eport, dst, iport, edgeinfo, (char*)NULL );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:31  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
