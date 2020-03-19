#ifndef WORLD_H
#define WORLD_H

/**************************************************************************/
/* FILE   **************          world.h          ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

#include "sisalInfo.h"
#include "../Library/IFX.h"

/* ------------------------------------------------------------ */
extern int   pmodule;           /* PROGRAM MODULE? */
extern int   smodule;           /* SISAL MODULE? */
extern int   forF;              /* PROGRAM MODULE TYPES */
extern int   forC;
extern int   monolith;          /* MONOLITHIC PROGRAM MODULE? */

/* ------------------------------------------------------------ */
/* if1smash.c */
extern void     LoadSmashTypes PROTO((void));

/* if1names.c */
extern void     AddToImportList PROTO((PNODE,char*,PINFO));
extern void     AddToNameList PROTO((PNODE));
extern void     CheckForUnresolvedNames PROTO((void));

/*
 * $Log$
 * Revision 1.3  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.2  2001/01/01 05:46:22  patmiller
 * Adding prototypes and header info -- all will be broken
 *
 * Revision 1.1.1.1  2000/12/31 17:56:09  patmiller
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
 * Revision 1.9  1994/04/15  15:50:21  denton
 * Added config.h to centralize machine specific header files.
 *
 * Revision 1.8  1993/03/23  22:09:58  miller
 * no change
 *
 * Revision 1.7  1994/03/11  23:09:24  miller
 * Moved IFX.h into Backend/Library and added support for Minimal
 * installation (removing source as compiled).
 *
 * Revision 1.6  1993/01/14  22:23:47  miller
 * Fixed up some pragma stuff in ifxstuff and world.
 *
 * Revision 1.5  1993/01/07  00:37:07  miller
 * Make changes for LINT.
 *
 */

#endif
