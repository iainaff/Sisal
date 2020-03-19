/**************************************************************************/
/* FILE   **************          stream.c         ************************/
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
 * Revision 1.1  1993/01/14  22:29:10  miller
 * Carry along work to propagate the new pragmas.  Also fixed up to report
 * reasons why loops don't vectorize / parallelize.  Split off some of the
 * work from if2part.c into slice.c stream.c vector.c
 */
/**************************************************************************/


#include "world.h"

/**************************************************************************/
/* GLOBAL **************       IsStreamTask        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF LOOP n IMPORTS OR EXPORTS A STREAM, ELSE FALSE.*/
/**************************************************************************/

int IsStreamTask( n )
PNODE n;
{
    register PEDGE i;
    register PEDGE e;
    register PNODE init;
    register PNODE ret;
    register int   yes;

    if ( IsForall( n ) ) {
        init = n->F_GEN; 
        ret = n->F_RET;
    } else {
        init = n->L_INIT;
        ret = n->L_RET;
        }

    yes = FALSE;

    for ( i = n->imp; i != NULL; i = i->isucc ) 
        if ( IsStream( i->info ) )
            if ( IsExport( init, i->iport ) )
                yes = TRUE;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
        if ( IsStream( e->info ) ) {
            if ( (i = FindImport( ret, e->eport )) == NULL )
              continue;

            switch ( i->src->type ) {
                case IFAGather:
                    yes = TRUE;
                    break;

                default:
                    break;
                }
            }
        else
            return( FALSE );
        }

    return( yes );
}
