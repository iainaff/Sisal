/**************************************************************************/
/* FILE   **************         If1Write.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* GLOBAL **************         If1Write          ************************/
/**************************************************************************/
/* PURPOSE: PRINT ALL IF1 TYPES, STAMPS, AND FUNCTION GRAPHS TO output.   */
/*          THE TYPE INFORMATION IS FOUND TRAVERSING THE BINARY TREE      */
/*          itree; THE STAMPS ARE FOUND IN ARRAY stamps; AND THE FUNCTION */
/*          GRAPHS ARE FOUND TRAVERSING glstop. NODES ARE RELABELED AND   */
/*          ALL PORTS OF COMPOUND NODES INCLUDING THEIR SUBGRAPHS ARE     */
/*          RENUMBERED. PORT NUMBER AND NODE LABEL CONVENTIONS MAY HAVE   */
/*          BEEN VIOLATED DURING GRAPH OPTIMIATION. IT IS EASIER TO FIX   */
/*          THIS HERE!  THE SIDE EFFECTS OF CONSTANT FOLDING ARE UNDONE   */
/*          (SIGNED CONSTANTS ARE ALLOWED IF sgnok IS TRUE).              */
/**************************************************************************/

void If1Write()
{
  register PNODE f;

  AddTimeStamp();

  WriteInfo();
  WriteStamps();

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    WriteNodes( cfunct = f );
}

/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:23  patmiller
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
 * Revision 1.3  1993/03/23  22:48:59  miller
 * date problem
 *
 * Revision 1.2  1994/03/11  18:20:44  miller
 * Added time stamp to all output IF1 and IF2
 *
 * Revision 1.1  1993/01/21  23:28:50  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 *
 */
