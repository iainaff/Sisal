#ifndef WORLD_H
#define WORLD_H

/**************************************************************************/
/* FILE   **************          world.h          ************************/
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
 * Revision 1.1.1.1  2000/12/31 17:58:06  patmiller
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
 * Revision 1.11  1994/06/16  21:31:34  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.10  1994/04/15  15:51:26  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.9  1993/03/23  22:42:34  miller
 * date problem
 *
 * Revision 1.8  1994/03/11  23:09:42  miller
 * Moved IFX.h into Backend/Library and added support for Minimal
 * installation (removing source as compiled).
 *
 * Revision 1.7  1993/01/15  22:26:00  miller
 * Moved the definition for info up into IFX.h
 *
 * Revision 1.6  1993/01/14  22:29:42  miller
 * Propagation work for new pragmas (TC ID MinSlice)
 *
 * Revision 1.5  1993/01/07  00:39:53  miller
 * Make changes for LINT and combined files.
 * */

#include "sisalInfo.h"
#include "../Library/IFX.h"

extern int    mig;              /* MIGRATE NODES TOWARD USES? */
extern FILE *infoptr;           /* information output file */
extern FILE *infoptr2;          /* information output file */

extern PNODE  nohead;           /* NoOp NODE LIST HEAD */
extern PNODE  notail;           /* NoOp NODE LIST TAIL */
extern PSET   gshead;           /* GLOBAL READ/WRITE SET LIST HEAD */
extern PSET   gstail;           /* GLOBAL READ/WRITE SET LIST TAIL */
extern PEDGE  dghead;           /* DATA GROUND EDGE LIST HEAD */
extern PEDGE  dgtail;           /* DATA GROUND EDGE LIST TAIL */
extern PNODE  cohead;           /* CONSTANT AGGREGATE GENERATOR LIST HEAD */
extern PNODE  cotail;           /* CONSTANT AGGREGATE GENERATOR LIST TAIL */

extern int    minopt;           /* ONLY PERFORM MINIMAL OPTIMIZATION? */
extern int    cagg;             /* MARK CONSTANT AGGREGATES? */
extern int    ststr;            /* IDENTIFY SINGLE THREADED STREAMS? */

extern int    univso;           /* UNIVERSAL STREAM OWNERSHIP? */
extern int    univao;           /* UNIVERSAL ARRAY  OWNERSHIP? */
extern int    cmig;             /* COUNT OF MIGRATED NODES */
extern int    seqimp;

extern int    swcnt;            /* COUNT OF SWAP OPTIMIZATIONS */
extern int    hnoops;           /* COUNT OF HOISTED NoOp NODES */

/* ------------------------------------------------------------ */
/* if2up.c */
extern void     If2Up PROTO((int));

/* if2call.c */
extern void     If2CallGraph PROTO((void));

/* if2noop.c */
extern void     If2NoOp PROTO((void));

/* if2refcnt.c */
extern void     AssignCMPragmas PROTO((PNODE,int));
extern void     If2ReferenceCount PROTO((void));

/* if2class.c */
extern void     If2Classify PROTO((void));

/* if2rwset.c */
extern void     If2ReadWriteSets PROTO((void));

/* if2ade.c */
extern void     If2Ade PROTO((void));

/* if2replace.c */
extern void     If2Replace PROTO((void));

/* if2count.c */
extern void     UpIf2Count PROTO((int,char*));
extern void     WriteIf2upWarnings PROTO((void));

/* if2const.c */
extern void     If2ConstGenerators PROTO((void));

/* if2owner.c */
extern void     If2Ownership PROTO((void));
extern void     OptimizeSwaps PROTO((void));

/* if2refcntopt.c */
extern void     CleanNoOpImports PROTO((void));
extern void     If2ReferenceCountOpt PROTO((void));

/* if2marks.c */
extern void     If2RefineGrounds PROTO((void));
extern void     If2PropagateMarks PROTO((void));

/* if2migrate.c */
extern void     If2Migrate PROTO((void));

/* if2pprint.c */
extern void     If2PPrint PROTO((void));

#endif
