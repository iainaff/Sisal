#include "world.h"

PNODE SortedTree;

static PNODE FindNewFunctionGraph( nm )
     char           *nm;
{
  register PNODE g;

  for ( g = SortedTree; g != NULL; g = g->gsucc ) {
    if ( strcmp( g->G_NAME, nm ) == 0 ) return( g );
  }

  return( NULL );
}

static PNODE FindTheFunction( nm )
     char           *nm;
{
  register PNODE g;

  for ( g = SortedTree; g != NULL; g = g->gsucc ) {
    if ( strcmp( g->G_NAME, nm ) == 0 ) return( g );
  }

  for ( g = glstop->gsucc; g != NULL; g = g->gsucc ) {
    if ( strcmp( g->G_NAME, nm ) == 0 ) return( g );
  }

  return( NULL );
}

static int InSortedTree( f,g )
     PNODE f;
     PNODE g;
{
  register PNODE n;
  register PNODE sg;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc ) {
	if ( !InSortedTree( f,sg ) ) return FALSE;
      }
    }

    if ( IsCall( n ) && strcmp(n->imp->CoNsT,f->G_NAME) != 0 ) {
      if ( !FindNewFunctionGraph( n->imp->CoNsT ) ) return FALSE;
    }
  }
  return TRUE;
}

static int CallReachable( f,g )
     PNODE f;
     PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PNODE ff;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      for (sg = n->C_SUBS; sg != NULL; sg = sg->gsucc) {
	if (CallReachable(f,sg)) return TRUE;
      }
    }

    if ( IsCall(n) ) {
      ff = FindTheFunction(n->imp->CoNsT);
      if ( !ff->checked ) {
	ff->checked = TRUE;
	if (ff==f || CallReachable(f,ff)) return TRUE;
      }
    }
  }

  return FALSE;
}

static int InRecursiveChain(F)
     PNODE	F;
{
  PNODE		ff;

  /* ------------------------------------------------------------ */
  /* Set all uncommitted functions as unmarked			  */
  /* ------------------------------------------------------------ */
  for(ff=glstop->gsucc; ff; ff=ff->gsucc) ff->checked = FALSE;

  /* ------------------------------------------------------------ */
  /* Look for a cycle that contains F				  */
  /* ------------------------------------------------------------ */
  return CallReachable(F,F);
}

/**************************************************************************/
/* GLOBAL **************          CallReorder      ************************/
/************************************************************************ **/
/* PURPOSE:  */
/**************************************************************************/
void CallReorder()
{
  register PNODE	f,next,last;

  SortedTree = NULL;

  while ( glstop->gsucc ) {
    for ( f = glstop->gsucc; f; f = next ) {
      next = f->gsucc;
      if ( InSortedTree(f,f) || InRecursiveChain(f) ) {
	/* ------------------------------------------------------------ */
	/* Unlink node from glstop list					*/
	/* ------------------------------------------------------------ */
	f->gpred->gsucc = next;
	if ( next ) next->gpred = f->gpred;

	/* ------------------------------------------------------------ */
	/* Add it to SortedTree list					*/
	/* ------------------------------------------------------------ */
	f->gsucc = SortedTree;
	SortedTree = f;
	f->checked = TRUE;	/* Helps speed up InRecursiveChain */
      }
    }
  }

  /* ------------------------------------------------------------ */
  /* Relink the sorted calls onto the glstop list		  */
  /* ------------------------------------------------------------ */
  glstop->gpred = SortedTree;
  last = (PNODE)NULL;
  for(f = SortedTree; f; f = next) {
    next = f->gsucc;

    f->gsucc = last;
    f->gpred = next;
    glstop->gsucc = f;

    last = f;
  }
  last->gpred = glstop;

}

/* $Log$
 * Revision 1.2  1994/04/15  15:50:29  denton
 * Added config.h to centralize machine specific header files.
 *
 * Revision 1.1  1993/11/30  00:26:23  miller
 * Call reordering for recursive call with record bug.
 * */
