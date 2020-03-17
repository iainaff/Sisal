/**************************************************************************/
/* FILE   **************       WriteImports.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* MACRO  **************          WriteAde         ************************/
/**************************************************************************/
/* PURPOSE: WRITE ARITFICIAL DEPENDENCE EDGE x TO OUTPUT.                 */
/**************************************************************************/

#define WriteAde(x)    FPRINTF( output, "D %2d      %2d\n",                \
                                        (x)->src->label, (x)->dst->label )


/**************************************************************************/
/* MACRO  **************         WriteEdge         ************************/
/**************************************************************************/
/* PURPOSE: WRITE EDGE e TO output. THE PORT NUMBERS MAY BE NEGATIVE.     */
/**************************************************************************/

#define WriteEdge(e)   FPRINTF( output, "E %2d %2d   %2d %2d  %2d",        \
                                        e->src->label, abs( e->eport ),    \
                                        e->dst->label, abs( e->iport ),    \
                                        e->info->label               )


/**************************************************************************/
/* GLOBAL **************        WriteImports       ************************/
/**************************************************************************/
/* PURPOSE: WRITE NODE n's IMPORTS, BOTH DATA AND CONTROL, TO output.     */
/**************************************************************************/

void WriteImports( n )
PNODE n;
{
  register PEDGE  i;
  register PADE   a;

  for ( i  = n->imp; i != NULL; i = i->isucc ) {
    if (CheckForBadEdges)
      CheckEdge(i);
    if ( IsConst( i ) )
      WriteConst( i );
    else
      WriteEdge( i );

    WritePragmas( i );
  }

  for ( a = n->aimp; a != NULL; a = a->isucc )
    WriteAde( a );
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:46  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:35  patmiller
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
 * Revision 1.2  1994/04/14  21:45:36  solomon
 * Added edge checking to WriteImports().
 *
 * Revision 1.1  1993/01/21  23:30:46  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
