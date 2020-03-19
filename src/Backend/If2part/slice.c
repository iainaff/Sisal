/**************************************************************************/
/* FILE   **************          slice.c          ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:57:51  patmiller
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
 * Revision 1.2  1994/03/10  17:15:21  denton
 * Accept non-scalar exports from user-defined reductions in parallel for returns.
 *
 * Revision 1.1  1993/01/14  22:29:09  miller
 * Carry along work to propagate the new pragmas.  Also fixed up to report
 * reasons why loops don't vectorize / parallelize.  Split off some of the
 * work from if2part.c into slice.c stream.c vector.c
 */
/**************************************************************************/

#include "world.h"

/**************************************************************************/
/* GLOBAL **************       IsSliceCandidate    ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF Forall NODE f IS STRUCTURALLY A CANDIDATE FOR  */
/*          SLICING, ELSE RETURN FALSE. A CANDIDATE MUST EITHER SCATTER   */
/*          ARRAYS OR VALUES FROM A RangeGenerate AND RETURN NON-FILTERED */
/*          PREALLOCATED GATHERED ARRAYS AND SCALARS RESULTING FROM       */
/*          REDUCTIONS. ONLY MAX_DOTS DOT PRODUCTS ARE ALLOWED. A Forall  */
/*          RETURNING A STREAM IS NOT CONSIDERED A CANDIDATE.             */
/**************************************************************************/

int IsSliceCandidate( f,reasonp )
     PNODE      f;
     char       **reasonp;
{
    register PNODE n;
    register int   dots = -1;

    /* ------------------------------------------------------------ */
    /* IF1OPT: if1split.c DISABLE CONCURRENTIZATION OF THIN COPY */
    if ( f->ThinCopy ) {
      *reasonp = "Is THIN COPY loop (0-1 iterations) split by if1split";
      return( FALSE );
    }

    /* ------------------------------------------------------------ */
    if ( IsStream( f->exp->info ) ) {
      *reasonp = "Generates a stream";
      return( FALSE );
    }

    /* ------------------------------------------------------------ */
    for ( n = f->F_GEN->G_NODES; n != NULL; n = n->nsucc ) {
      switch ( n->type ) {
       case IFRangeGenerate:
        dots++;
        break;

       case IFAScatter:
        *reasonp = "Scatters a non-buffer array";
        return( FALSE );

       case IFScatterBufPartitions:
        break;

       default:
        *reasonp = "Non-standard value generator";
        return( FALSE );
      }
    }

    /* ------------------------------------------------------------ */
    /* ASSUMES if1opt REMOVED DOTS!!! */
    if ( dots > 0 ) {
      *reasonp = "Uses a range generator";
      return( FALSE );
    }

    /* ------------------------------------------------------------ */
    for ( n = f->F_RET->G_NODES; n != NULL; n = n->nsucc ) {
      switch ( n->type ) {
       case IFAGatherAT:
        if ( n->imp->isucc->isucc->iport == 3 ) {
          *reasonp = "Conditional filter for array gather";
          return( FALSE );
        }
        break;

       case IFReduceAT:
       case IFRedLeftAT:
       case IFRedRightAT:
       case IFRedTreeAT:
        if ( !(n->imp->isucc->isucc->pmark) ) {
          *reasonp = "Non-parallel reduction";
          return( FALSE );
        }
        break;

       case IFReduce:
       case IFRedTree:
       case IFRedLeft:
       case IFRedRight:
        if ( n->imp->CoNsT[0] == REDUCE_USER )
          break;
        if ( (n->imp->CoNsT[0] == REDUCE_SUM ||
              n->imp->CoNsT[0] == REDUCE_PRODUCT) && nopred &&
            (n->exp->info->type == IF_DOUBLE || 
             n->exp->info->type == IF_REAL) ) {
          *reasonp = "Non-associative floating point reduction";
          return( FALSE );
        }

        if ( !IsBasic( n->exp->info ) ) {
          *reasonp = "Non-scalar reduction";
          return( FALSE );
        }
        break;

       default:
        *reasonp = "Non-standard reduction operation";
        return( FALSE );
      }
    }

    *reasonp = NULL;
    return( TRUE );
}
