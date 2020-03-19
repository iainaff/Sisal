/**************************************************************************/
/* FILE   **************      AreEdgesEqual.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

/**************************************************************************/
/* FILE   **************      AreEdgesEqual.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************       AreEdgesEqual       ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF THE EDGES e1 AND e2 ARE EQUAL.  TO BE EQUAL,  */
/*          THEY MUST EITHER BOTH DEFINE THE SAME CONSTANTS OR ADDRESS    */
/*          THE SAME SOURCE NODE AND HAVE THE SAME SOURCE PORT NUMBERS.   */
/*          IF THEY ARE NOT EQUAL, FALSE IS RETURNED.                     */
/**************************************************************************/

int AreEdgesEqual( e1, e2 )
PEDGE e1;
PEDGE e2;
{
  if ( ExpandedEqual ) {
    if ( IsConst( e1 ) ^ IsConst( e2 ) ) /* XOR */
      return( FALSE );

    if ( IsConst( e1 ) )
      return( AreConstsEqual( e1, e2 ) );

    if ( (e1->src == e2->src) && (e1->eport == e2->eport) )
      return( TRUE );

  } else {
    if ( IsConst( e1 ) ) {
      if ( IsConst( e2 ) )
        return( AreConstsEqual( e1, e2 ) );

      return( FALSE );
    }

    /* IF e2 IS A CONST, e2->src WILL BE NULL AND e2->eport WILL BE CONST_PORT */
    if ( (e1->src == e2->src) && (e1->eport == e2->eport) )
      return( TRUE );

  }

  return( FALSE );
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:10  patmiller
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
 * Revision 1.1  1993/01/21  23:27:44  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
