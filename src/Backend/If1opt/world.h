#ifndef WORLD_H
#define WORLD_H

/**************************************************************************/
/* FILE   **************           world.h         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/


#include "sisalInfo.h"
#include "../Library/IFX.h"

extern int    asetl;            /* REMOVE UNNECESSARY ASetL NODES? */
extern int    ifuse;            /* PERFORM INDEPENDENT FUSION */
extern int    sfuse;            /* PERFORM SELECT FUSION? */
extern int    dfuse;            /* PERFORM DEPENDENT FUSION? */
extern int    info;             /* DUMP INFORMATION? */
extern int    slfis;            /* PERFORM STREAM LOOP FISSION? */
extern int    inter;            /* PERFORM INLINE EXPANSION? */
extern int    inlineall;        /* INLINE EVERYTHING BUT REQUESTS? */
extern int    intrinsics;       /* FOLD Math intrinsic functions */
extern int    native;           /* FLAG NODES NOT SUPPORTED IN NATIVE COMPILATION */
extern int    dope;             /* PEFORM DOPE VECTOR OPTIMIZATIONS? */
extern int    amove;            /* PERFORM ANTI-MOVEMENT */
extern int    unroll;           /* PERFORM FORALL UNROLLING? */
extern int    split;            /* PERFORM FORALL SPLITTING? */
extern int    invert;           /* PERFORM LOOP INVERSION? */
extern int    vec;              /* VECTOR MODE? */
extern int    concur;           /* CONCURRENT MODE? */

extern  int   Oinvar;           /* OUTERMOST LOOP INVARIANT REMOVAL? */

extern int ikcnt;               /* COUNT OF COMBINED K IMPORTS */
extern int unnec;               /* COUNT OF UNNECESSARY EDGES OR LITERALS */
extern int unused;              /* COUNT OF UNUSED VALUES */
extern int dscnt;               /* COUNT OF DEAD SIMPLE NODES */
extern int dccnt;               /* COUNT OF DEAD COMPOUND NODES */
extern int agcnt;               /* COUNT OF DEAD AGather NODES */
extern int tgcse;               /* TRY AND FORCE GCSE IMPORVEMENTS? */

extern int maxunroll;/* MAX NUMBER OF ITERATIONS ALLOWED IN AN UNROLLED LOOP */

extern int cRay;                /* COMPILING FOR THE CRAY? */
extern int alliantfx;           /* COMPILING FOR THE ALLIANT? */

extern int normidx;             /* NORMALIZE ARRAY INDEXING OPERATIONS? */
extern int fchange;             /* WAS A FUSION DONE? */

extern int glue;                /* ELIMINATE DEAD FUNCTION CALLS? */ 
extern int NoInlining;          /* Inline no functions */
extern int noincnt;             /* FUNCTION CALL LIST INDEX */
extern char *noin[];            /* FUNCTION CALL LIST */

extern double iter;             /* LOOP ITERATION COUNT DEFAULT */

extern int agcse;               /* DO ANTI-GLOBAL CSE? */

extern int prof;                /* PROFILE THE OPTIMIZER? */

extern int DeBuG;               /* OPT FOR PROGRAM DEBUGGING? */
extern int noassoc;             /* NO ASSOCIATIVE TRANSFORMATIONS */

extern int AggressiveVectors;   /* Aggressively combine vectorizing loops? */

extern FILE *infoptr;

/* ------------------------------------------------------------ */

/* callreorder.c */
extern void     CallReorder PROTO((void));

/* cascade.c */
extern void     If1TestCascade PROTO((void));

/* if1check.c */
extern void     If1Check PROTO((void));

/* if1count.c */
extern void     If1Count PROTO((char*));

/* if1cse.c */
extern void     WriteCseInfo PROTO((void));
extern void     If1Cse PROTO((int,int));

/* if1dead.c */
extern void     CombineKports PROTO((PNODE));
extern void     FastCleanGraph PROTO((PNODE));
extern void     OptRemoveDeadNode PROTO((PNODE));
extern void     RemoveDeadEdge PROTO((PEDGE));
extern void     WriteCleanInfo PROTO((void));
extern void     If1Clean PROTO((void));

/* if1dope.c */
extern void     If1Dope PROTO((void));
extern void     WriteDopeInfo PROTO((void));

/* if1explode.c */
extern void     WriteExplodeInfo PROTO((void));
extern void     If1Explode PROTO((int));

/* if1fission.c */
extern void     WriteFissionInfo PROTO((void));
extern void     If1Fission PROTO((void));

/* if1fold.c */
extern void     OptNormalizeNode PROTO((PNODE));
extern char     *DoubleToReal PROTO((char*));
extern void     WriteFoldInfo PROTO((void));
extern void     If1Fold PROTO((void));

/* if1fusion.c */
extern void     If1DFusion PROTO((void));
extern void     WriteFusionInfo PROTO((void));
extern void     If1IFusion PROTO((void));

/* if1gcse.c */
extern void     WriteGCseInfo PROTO((void));
extern void     OptRemoveSCses PROTO((PNODE));
extern void     If1GCse PROTO((void));

/* if1inline.c */
extern void     SpliceInGraph PROTO((PNODE,PNODE));
extern void     If1Inline PROTO((void));

/* if1invar.c */
extern int      OptIsEdgeInvariant PROTO((PEDGE));
extern void     ExposeInvariants PROTO((PNODE,PNODE));
extern void     AntiMovement PROTO((PNODE));
extern void     WriteInvarInfo PROTO((void));
extern void     If1Invar PROTO((void));

/* if1invert.c */
extern void     WriteInvertInfo PROTO((void));
extern void     If1Invert PROTO((void));

/* if1move.c */
extern void     FindAndLinkToSource PROTO((PNODE,PEDGE,PNODE));
extern void     RemoveNode PROTO((PNODE,PNODE));
extern void     InsertNode PROTO((PNODE,PNODE));

/* if1normal.c */
extern void     EliminateDeadFunctions PROTO((void));
extern void     If1Normalize PROTO((void));

/* if1parallel.c */
extern int      OptIsVecCandidate PROTO((PNODE));
extern void     WriteConcurInfo PROTO((void));
extern void     If1Vec PROTO((void));
extern void     If1Par PROTO((void));

/* if1pprint.c */
extern void     If1PPrint PROTO((void));

/* if1reduce.c */
extern void     If1Reduce PROTO((void));
extern void     WriteReduceInfo PROTO((void));

/* if1split.c */
extern void     WriteSplitInfo PROTO((void));
extern void     If1Split PROTO((void));

/* if1unroll.c */
extern void     WriteUnrollInfo PROTO((void));
extern void     If1Unroll PROTO((void));

/* util.c */
extern int      OptIsInvariant PROTO((PNODE));
extern void     DecodeIndexing PROTO((PNODE));
extern void     EncodeIndexing PROTO((PNODE,int,int*));

/* AssignIDs.c */
extern void     NewCompoundID PROTO((PNODE));
extern void     AssignCompoundIDs PROTO((void));

/*
 * $Log$
 * Revision 1.3  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.2  2001/01/01 05:46:22  patmiller
 * Adding prototypes and header info -- all will be broken
 *
 * Revision 1.1.1.1  2000/12/31 17:56:43  patmiller
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
 * Revision 1.15  1994/06/16  21:30:06  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.14  1994/04/15  15:50:37  denton
 * Added config.h to centralize machine specific header files.
 *
 * Revision 1.13  1994/03/03  19:29:38  solomon
 * Removed the if1copy.c file from this directory.  Therefore, I removed
 * the external function declarations associated with that file.
 *
 * Revision 1.12  1993/11/30  00:25:02  miller
 * folding math intrinsics
 *
 * Revision 1.11  1993/03/23  22:27:35  miller
 * no change
 *
 * Revision 1.10  1994/03/11  23:09:28  miller
 * Moved IFX.h into Backend/Library and added support for Minimal
 * installation (removing source as compiled).
 *
 * Revision 1.9  1993/02/27  00:32:08  miller
 * Support for no inlining
 *
 * Revision 1.8  1993/01/15  22:25:48  miller
 * Moved the definition for info up into IFX.h
 *
 * Revision 1.7  1993/01/14  22:26:05  miller
 * Added a compound node identifier field to the PRAGMA set.  Modified the
 * vinfo and info options (-I and -i).  Modified the if1split to use
 * a pragma %TC (ThinCopy) instead of line number to identify split off
 * loop bodies.
 *
 * Revision 1.6  1993/01/08  17:28:14  miller
 * There was a duplicate use of HI and LOW.  I changed them to HIGH_PRI
 * and LOW_PRI to reflect their use as ADE priorities.  In world.h, I
 * somehow belew away the reference to ../IFX.h
 *
 * Revision 1.5  1993/01/07  00:37:59  miller
 * Make changes for LINT and combined files.
 *
 */

#endif
