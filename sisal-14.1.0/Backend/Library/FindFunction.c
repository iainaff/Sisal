/**************************************************************************/
/* FILE   **************       FindFunction.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************       FindFunction        ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE FUNCTION GRAPH WITH NAME nm.  IF THE FUNCTION IS   */
/*          NOT FOUND, AN ERROR MESSAGE IS PRINTED.  NOTE: THIS ROUTINE   */
/*          WILL NOT WORK CORRECTLY UNTIL AFTER THE CALL GRAPH HAS BEEN   */
/*          BUILT AND THE FUNCTION GRAPHS HAVE BEEN SORTED.               */
/*          RETURN THE GRAPH NODE REPRESENTING FUNCTION nm.  IF IT ISN'T  */
/*          FOUND, AN ERROR MESSAGE IS PRINTED.                           */
/**************************************************************************/

PNODE FindFunction( nm )
char *nm;
{
  register PNODE f;

  for ( f = (fhead)?(fhead):(glstop->gsucc); f != NULL; f = f->gsucc ) {
    if ( strcmp( f->G_NAME, nm ) == 0 ) return( f );
  }

  Error2( "FindFunction: CAN'T FIND", nm );
  return NULL;
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:15  patmiller
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
 * Revision 1.2  1993/11/12  19:55:54  miller
 * Cosmetic change
 *
 * Revision 1.1  1993/01/21  23:28:28  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
