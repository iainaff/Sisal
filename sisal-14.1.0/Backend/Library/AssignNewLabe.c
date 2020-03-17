/**************************************************************************/
/* FILE   **************      AssignNewLabe.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************      AssignNewLabels      ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN NEW LABELS TO THE NODES OF GRAPH g AND FIX SIGNED      */
/*          ARITHMETIC CONSTANTS (GIVEN sgnok IS TRUE).                   */
/**************************************************************************/

void  AssignNewLabels( g )
register PNODE g;
{
  register int   lab;
  register PNODE n,nn;

  if ( FixPortsToo ) {
    /*************************************************************************/
    /***************      AssignNewLabelsAndPorts      ***********************/
    /*************************************************************************/
    /* PURPOSE: ASSIGN NEW LABELS TO THE NODES OF GRAPH g AND ASSIGN NEW PORT*/
    /*          NUMBERS TO THE K, M, T, L, AND R PORTS OF ALL COMPOUND NODES */
    /*          SUCH THAT NODE LABELS BEGIN WITH 1 (ASSIGNED IN INCREMENTS OF*/
    /*          ONE) AND PORT NUMBERS OBAY THE FOLLOWING:                    */
    /*                                                                       */
    /*          FORALL:  K < M < T  AND K AND R PORTS  START AT 1 BY 1       */
    /*          LOOPA :  K < L < T  AND K AND R PORTS  START AT 1 BY 1       */
    /*          LOOPB :  K < L      AND K AND R PORTS  START AT 1 BY 1       */
    /*                                                                       */
    /*          NOTE: CONSTANT FOLDING SIDE EFFECTS ARE UNDONE.              */
    /*************************************************************************/
    register int   p;

    FixConstantImports( g, 0 );

    for ( lab = 0, n = g->G_NODES; n != NULL; n = n->nsucc ) {
      lab = FixConstantImports( n, lab );

      n->label = ++lab;

      if ( !IsCompound( n ) )
        continue;

      p = AssignNewKports( n );

      switch ( n->type ) {
       case IFTagCase:
        for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
          AssignNewRports( g );

        break;

       case IFSelect:
        nn = n;
        if (n->S_TEST->imp != NULL)
          n->S_TEST->imp->iport = 1; /* B PORT NUMBER */

        for (nn = n->S_ALT; nn != NULL; nn = nn->gsucc)
          AssignNewRports( nn );
/*
        AssignNewRports( n->S_ALT );
        AssignNewRports( n->S_CONS );
*/
        break;

       case IFForall:
        (void)AssignNewTports( AssignNewMports( p, n ), n );
        AssignNewRports( n->F_RET );
        break;

       case IFLoopA:
        if(n->L_TEST->imp != NULL)
          n->L_TEST->imp->iport = 1; /* B PORT NUMBER */

        (void)AssignNewLoopTports( AssignNewLports( p, n ), n );
        if ((n->L_BODY != NULL) && (n->L_RET != NULL))
          AssignNewRports( n->L_RET );
        break;

       case IFLoopB:
        if((n->L_INIT) && (n->L_TEST) && (n->L_TEST->imp != NULL))
          n->L_TEST->imp->iport = 1; /* B PORT NUMBER */

        AssignNewLports( p, n );
        if ((n->L_BODY != NULL) && (n->L_RET != NULL))
          AssignNewRports( n->L_RET );
        break;

       case IFRepeatLoop:
        if(n->gsucc->imp != NULL)
          n->gsucc->imp->iport = 1; /* B PORT NUMBER */

        AssignNewLports( p, n );
        if ((n->gsucc->gsucc != NULL) && (n->gsucc->gsucc->gsucc != NULL))
          AssignNewRports( n->gsucc->gsucc->gsucc );
        break;
      }
    }
  } else {
    FixSignedConstantImports( g, 0 );

    for ( lab = 0, n = g->G_NODES; n != NULL; n = n->nsucc ) {
      lab = FixSignedConstantImports( n, lab );

      n->label = ++lab;
    }
  }
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:11  patmiller
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
 * Revision 1.2  1994/03/03  17:12:40  solomon
 * Added some tests to help prevent failing when dealing with invalid
 * if1 code.
 *
 * Revision 1.1  1993/04/16  18:59:45  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.1  1993/01/21  23:27:50  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
