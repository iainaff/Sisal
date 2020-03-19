/**************************************************************************/
/* FILE   **************        WriteNodes.c       ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************        WriteNodes         ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE NODES OF GRAPH g AND THEIR IMPORT EDGES TO output.  */
/*          NODE RELABELING AND (maybe) COMPOUND NODE PORT RENUMBERING IS */
/*          DONE BEFORE PRINTING.                                         */
/**************************************************************************/

void WriteNodes( g )
PNODE g;
{
  register PNODE  n;
  register PNODE  sg;
  register PALIST l;

  switch ( g->type ) {
   case IFLGraph:
    FPRINTF( output, "G %2d", g->G_INFO->label );
    FPRINTF( output, " \"%s\"", g->G_NAME );
    break;

   case IFIGraph:
    FPRINTF( output, "I %2d", g->G_INFO->label );
    FPRINTF( output, " \"%s\"", g->G_NAME );
    break;

   case IFXGraph:
    FPRINTF( output, "X %2d", g->G_INFO->label );
    FPRINTF( output, " \"%s\"", g->G_NAME );
    break;

   case IFSGraph:
    FPRINTF( output, "G %2d", g->label );
    break;
  }

  WritePragmas( g );

  AssignNewLabels( g );
  /* WriteImports( g ); */

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    /* ------------------------------------------------------------ */
    /* Compound nodes                                               */
    /* ------------------------------------------------------------ */
    if ( IsCompound(n) ) {
      FPRINTF( output, "{ Compound %2d %2d\n", n->label, n->type );

      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        WriteNodes( sg );

      FPRINTF( output, "} %2d %2d %2d   ", n->label, 
              n->type, n->C_SCNT        );

      for ( l = n->C_ALST; l != NULL; l = l->next )
        FPRINTF( output, " %d", l->datum );

    } else {
      /* ------------------------------------------------------------ */
      /* Simple nodes                                                 */
      /* ------------------------------------------------------------ */
      FPRINTF( output, "N %2d %2d", n->label, n->type );
    }

    WritePragmas( n );
    WriteImports( n );
  }

  WriteImports( g ); 
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:46  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:36  patmiller
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
 * Revision 1.4  1994/03/09  23:17:48  miller
 * Now, ANY compound node (opcode < 100) will be output, not just
 * the predefined kinds.  The previous restriction was crimping
 * the Frontend90 design.
 *
 * Revision 1.3  1994/03/03  17:17:54  solomon
 * Added "case IFRepeatLoop" to the list of case statements that is
 * testing for compound nodes.
 *
 * Revision 1.2  1993/11/12  19:58:39  miller
 * Support for new IF90 compounds
 *
 * Revision 1.1  1993/01/21  23:30:53  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
