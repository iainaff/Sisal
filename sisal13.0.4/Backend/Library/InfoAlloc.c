#include "world.h"

/**************************************************************************/
/* GLOBAL **************         InfoAlloc         ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN AN INFO NODE.                */
/**************************************************************************/


PINFO InfoAlloc( label, type )
int   label;
int   type;
{
  register PINFO i;
  register PBBLOCK b;

  /* Update LargestTypeLabelSoFar. */

  if (LargestTypeLabelSoFar < label)
    LargestTypeLabelSoFar = label;

  b = MyBBlockAlloc();
  i = &(b->i);

  TypeInitPragmas( i );

  i->label = label;
  i->type  = type;

  i->info1 = NULL;
  i->info2 = NULL;
  i->touch1 = FALSE;
  i->touch2 = FALSE;
  i->touch3 = FALSE;
  i->touch4 = FALSE;
  i->touch5 = FALSE;
  i->touch6 = FALSE;
  i->touchflag = FALSE;

  i->fmem   = FALSE;

  i->tname  = NULL;
  i->cname  = NULL;
  i->sname  = NULL;
  i->rname  = NULL;
  i->wname  = NULL;

  i->fname1 = NULL;
  i->fname2 = NULL;

  i->next  = NULL;
  i->print = TRUE;

  i->temp = NULL;

  return( i );
}

/* $Log$
 * Revision 1.4  1994/06/24  16:01:32  denton
 * Added -IF3.
 *
 * Revision 1.3  1994/05/25  19:44:35  denton
 * Added FindLargestLabel to return the value of LargestTypeLabelSoFar.
 * Shortened name of GetAggregateType.c to remove ar warnings.
 *
 * Revision 1.2  1994/04/14  21:01:09  solomon
 * Added the global variable int LargestTypeLabelSoFar = 0.  Also added
 * code to update this variable.
 *
 * Revision 1.1  1993/01/21  23:28:56  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
