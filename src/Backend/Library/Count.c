/**************************************************************************/
/* FILE   **************          Count.c          ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"
/**************************************************************************/
/* GLOBAL **************           Count           ************************/
/**************************************************************************/
/* PURPOSE: COUNT NODES, LITERALS, AND EDGES IN GRAPH g, PRODUCING A PER- */
/*          LEVEL SUMMARY AND TOTALS.                                     */
/**************************************************************************/

void Count( g )
PNODE g;
{
  register PNODE n;
  register PEDGE i;

  if ( (++topl) >= MaxLevel )
    Error1( "Count: LEVEL STACK OVERFLOW" );

  if ( topl > maxl )
    maxl = topl;

  levels[topl].graphs++;

  gnodes[ g->type - IFSGraph ]++; graphs++;

  for ( i = g->imp; i != NULL; i = i->isucc )
    if ( IsConst( i ) ) {
      lits++;
      levels[topl].lits++;
    } else {
      edges++;
      levels[topl].edges++;
    }

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    for ( i = n->imp; i != NULL; i = i->isucc )
      if ( IsConst( i ) ) {
        lits++;
        levels[topl].lits++;
      } else {
        edges++;
        levels[topl].edges++;
      }

    if ( IsSimple( n ) ) {
      if ( IsPeek( n ) ) 
        continue;

      snodes[ n->type - IFAAddH ]++; simples++; levels[topl].simples++;
            
      if ( IsReduction( n ) )
        switch ( n->imp->CoNsT[0] ) {
         case REDUCE_SUM:
          rsum++;
          break;

         case REDUCE_PRODUCT:
          rprod++;
          break;

         case REDUCE_LEAST:
          rleast++;
          break;

         case REDUCE_GREATEST:
          rgreat++;
          break;

         case REDUCE_CATENATE:
          rcat++;
          break;

         case REDUCE_USER:
          break;

         default:
          Error1( "ILLEGAL REDUCTION FUNCTION" );
        }

      continue;
    }

    if ( IsAtNode( n ) ) {
      atnodes[ n->type - IFAAddLAT ]++; ats++; levels[topl].ats++;
      continue;
    }

    if ( IsCompound( n ) ) {
      cnodes[ n->type - IFForall ]++; 
      comps++; 
      levels[topl].comps++;

      for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
        Count( g );
    }
  }

  topl--;
}


/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:14  patmiller
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
 * Revision 1.2  1994/03/09  01:12:33  denton
 * Fix -info1-32 for reductions
 *
 * Revision 1.1  1993/01/21  23:28:14  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
