#include "world.h"


/**************************************************************************/
/* GLOBAL **************        WriteTheLMap       ************************/
/**************************************************************************/
/* PURPOSE:  Traverses and writes out information for loops.		  */
/**************************************************************************/
void WriteTheLMap( lvl, plvl, g )
int   lvl;
int   plvl;
PNODE g;
{
  register PNODE n;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    switch ( n->type ) {
     case IFForall:
      PrintLInfo( lvl,plvl, "Forall", n );
      WriteTheLMap( lvl + 1, plvl+1, n->F_BODY );
      break;

     case IFLoopB:
     case IFLoopA:
      PrintLInfo( lvl, plvl, "For Initial", n );
      WriteTheLMap( lvl + 1, plvl, n->L_BODY );
      break;

     case IFFirstSum:
     case IFFirstMin:
     case IFFirstMax:
     case IFTri:
     case IFFirstAbsMin:
     case IFFirstAbsMax:
      n->vmark = TRUE;
      PrintLInfo( lvl, plvl, "For Initial", n );
      n->vmark = FALSE;
      break;

     case IFSelect:
      WriteTheLMap( lvl, plvl, n->S_CONS );
      WriteTheLMap( lvl, plvl, n->S_ALT );
      break;

     default:
      break;
    }
  }
}

/* $Log$
 * Revision 1.4  1993/06/02  23:45:30  miller
 * Discovered dead var with lint.
 *
 * Revision 1.3  1993/03/23  22:51:22  miller
 * date problem
 *
 * Revision 1.2  1994/03/18  18:34:24  miller
 * Added parallelism level to the loop map.
 *
 * Revision 1.1  1993/01/21  23:30:55  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
