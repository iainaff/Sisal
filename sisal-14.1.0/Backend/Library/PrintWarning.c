/**************************************************************************/
/* FILE   **************       PrintWarning.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************        PrintWarning       ************************/
/**************************************************************************/
/* PURPOSE: PRINT op'S COPY WARNING TO stderr.                            */
/**************************************************************************/

void PrintWarning( op, sourceline, funct, file, copy )
char *op;
int   sourceline;
char *funct;
char *file;
int   copy;
{
  if ( !Warnings ) return;      /* Are warnings suppressed? */

  if ( funct == NULL ) 
    funct = "NULL?()";

  if ( file == NULL )  
    file = "NULL?.sis";

  if ( copy )
    FPRINTF(stderr,
            " %s: W - %s ON LINE %d OF %s IN %s INTRODUCES COPYING\n",
            program, op, sourceline, funct, file
            );
  else
    FPRINTF(stderr,
            " %s:  W - %s ON LINE %d OF %s IN %s MAY INTRODUCE COPYING\n",
            program, op, sourceline, funct, file
            );
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:31  patmiller
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
 * Revision 1.5  1994/06/17  20:51:46  mivory
 * make check bug fix and if2mem bug fix MYI
 *
 * Revision 1.4  1994/06/16  21:32:02  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.3  1994/04/01  22:15:47  denton
 * Fixed ANSI trigraph replacement.
 *
 * Revision 1.2  1993/04/16  17:09:57  miller
 * Added support for warnings suppression (-w)
 *
 * Revision 1.1  1993/01/21  23:30:03  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
