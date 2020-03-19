/**************************************************************************/
/* FILE   **************       callreorder.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

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
     PNODE      F;
{
  PNODE         ff;

  /* ------------------------------------------------------------ */
  /* Set all uncommitted functions as unmarked                    */
  /* ------------------------------------------------------------ */
  for(ff=glstop->gsucc; ff; ff=ff->gsucc) ff->checked = FALSE;

  /* ------------------------------------------------------------ */
  /* Look for a cycle that contains F                             */
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
  register PNODE        f,next,last;

  SortedTree = NULL;

  while ( glstop->gsucc ) {
    for ( f = glstop->gsucc; f; f = next ) {
      next = f->gsucc;
      if ( InSortedTree(f,f) || InRecursiveChain(f) ) {
        /* ------------------------------------------------------------ */
        /* Unlink node from glstop list                                 */
        /* ------------------------------------------------------------ */
        f->gpred->gsucc = next;
        if ( next ) next->gpred = f->gpred;

        /* ------------------------------------------------------------ */
        /* Add it to SortedTree list                                    */
        /* ------------------------------------------------------------ */
        f->gsucc = SortedTree;
        SortedTree = f;
        f->checked = TRUE;      /* Helps speed up InRecursiveChain */
      }
    }
  }

  /* ------------------------------------------------------------ */
  /* Relink the sorted calls onto the glstop list                 */
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

/*
 * $Log$
 * Revision 1.3  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.2  2001/01/01 05:46:22  patmiller
 * Adding prototypes and header info -- all will be broken
 *
 * Revision 1.1.1.1  2000/12/31 17:56:10  patmiller
 * Well, here is the first set of big changes in the distribution
 * in 5 years!  Right now, I did a lot of work on configuration/
 * setup (now all autoconf), breaking out the machine dependent
 * #ifdef's (with a central acconfig.h driven config file), changed
 * the installation directories to be more gnu style /usr/local
 * (putting data in the /share/sisal14 dir for instance), and
 * reduced the footprint in the top level /usr/local/xxx hierarchy.
 *
 * I also wrote a new compiler tool (sisalc) to replace osc.  I
 * found that the old logic was too convoluted.  This does NOT
 * replace the full functionality, but then again, it doesn't have
 * 300 options on it either.
 *
 * Big change is making the code more portably correct.  It now
 * compiles under gcc -ansi -Wall mostly.  Some functions are
 * not prototyped yet.
 *
 * Next up: Full prototypes (little) checking out the old FLI (medium)
 * and a new Frontend for simpler extension and a new FLI (with clean
 * C, C++, F77, and Python! support).
 *
 * Pat
 *
 *
 * Revision 1.2  1994/04/15  15:50:29  denton
 * Added config.h to centralize machine specific header files.
 *
 * Revision 1.1  1993/11/30  00:26:23  miller
 * Call reordering for recursive call with record bug.
 * */
