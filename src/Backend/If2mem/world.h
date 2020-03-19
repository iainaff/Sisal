#ifndef WORLD_H
#define WORLD_H

/**************************************************************************/
/* FILE   **************         world.h<3>        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log$
 * Revision 1.3  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.2  2001/01/01 05:46:22  patmiller
 * Adding prototypes and header info -- all will be broken
 *
 * Revision 1.1.1.1  2000/12/31 17:57:45  patmiller
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
 * Revision 1.11  1994/06/16  21:31:02  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.10  1994/04/15  15:51:14  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.9  1993/03/23  22:35:46  miller
 * date problem
 *
 * Revision 1.8  1994/03/11  23:09:35  miller
 * Moved IFX.h into Backend/Library and added support for Minimal
 * installation (removing source as compiled).
 *
 * Revision 1.7  1993/01/15  22:25:59  miller
 * Moved the definition for info up into IFX.h
 *
 * Revision 1.6  1993/01/14  22:28:00  miller
 * Carry along work to propagate the new pragmas.
 *
 * Revision 1.5  1993/01/07  00:39:12  miller
 * Make changes for LINT and combined files.
 *
 */

#include "sisalInfo.h"
#include "../Library/IFX.h"


extern int    noopt;            /* ARE CSE AND CONSTANT FOLDING DESIRED? */

extern int   invar;             /* GRAPH CLEANUP DISABLE FLAGS */
extern int   cse;
extern int   gcse;
extern int   fold;

extern int   minopt;            /* ONLY PERFORM MINIMAL OPTIMIZATION? */
extern int   fover;             /* OVERRIDE AGather FILTERS? */
extern int   Oinvar;

extern int   paddh;             /* COUNT OF AAddH PREFERENCES */

extern FILE *infoptr;
extern int agg;

/* ------------------------------------------------------------ */
/* if2mem.c */
extern void     PushAtNode PROTO((PNODE));
extern PNODE    PopAtNode PROTO((void));
extern PNODE    ReferencePoint PROTO((PNODE,int));
extern PNODE    MaxNodeInDFO PROTO((PNODE,PNODE,PNODE));
extern PNODE    MaxSourceInDFO PROTO((PNODE,PEDGE,PEDGE));
extern void     If2Mem PROTO((void));

/* if2level.c */
extern void     AssignLevelNumbers PROTO((PNODE));

/* if2size.c */
extern void     AssignSizes PROTO((PNODE));

/* if2alloc.c */
extern void     AllocIf2Nodes PROTO((PNODE));

/* if2clean.c */
extern void     If2Clean PROTO((void));

/* if2pic.c */
extern void     CountNodesAndEdges PROTO((char*));
extern void     TraceBuffer PROTO((PNODE,int));
extern void     WriteIf2memPicture PROTO((void));
extern void     WriteIf2memWarnings PROTO((void));

#endif
