#include "world.h"


/**************************************************************************/
/* GLOBAL **************       LinkAdeImport       ************************/
/**************************************************************************/
/* PURPOSE: ADD ADE IMPORT a TO THE HEAD OF dst'S ADE IMPORT LIST.  THE   */
/*          PREDECESSOR OF THE FIRST IMPORT IS ALWAYS NULL.               */
/**************************************************************************/

void LinkAdeImport( dst, a )
register PNODE dst;
register PADE  a;
{
  a->dst = dst;

  a->ipred = NULL;
  a->isucc = dst->aimp;

  if ( dst->aimp != NULL )
    dst->aimp->ipred = a;

  dst->aimp = a;
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:19  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
