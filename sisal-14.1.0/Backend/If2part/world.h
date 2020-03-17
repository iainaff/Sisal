#ifndef WORLD_H
#define WORLD_H

/**************************************************************************/
/* FILE   **************         world.h<4>        ************************/
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
 * Revision 1.1.1.1  2000/12/31 17:57:53  patmiller
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
 * Revision 1.14  1994/06/16  21:31:19  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.13  1994/04/15  15:51:21  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.12  1993/03/23  22:43:37  miller
 * date problem
 *
 * Revision 1.11  1994/03/11  23:09:39  miller
 * Moved IFX.h into Backend/Library and added support for Minimal
 * installation (removing source as compiled).
 *
 * Revision 1.10  1994/03/11  18:33:51  miller
 * Allow mask selection on Loop report override.
 *
 * Revision 1.9  1993/02/24  18:47:19  miller
 * Needed an exter for the default loop style.
 *
 * Revision 1.8  1993/01/15  22:24:40  miller
 * Added support for the loop report files and new routines in report.c
 *
 * Revision 1.7  1993/01/14  22:29:12  miller
 * Carry along work to propagate the new pragmas.  Also fixed up to report
 * reasons why loops don't vectorize / parallelize.  Split off some of the
 * work from if2part.c into slice.c stream.c vector.c
 *
 * Revision 1.6  1993/01/08  17:29:12  miller
 * Missing IFX.h (I think?)
 *
 * Revision 1.5  1993/01/07  00:39:27  miller
 * Make changes for LINT and combined files.
 * */

#include "sisalInfo.h"
#include "../Library/IFX.h"

#define SliceThreshold      mcosts[0]
#define Iterations          mcosts[1]
#define IntrinsicCost       mcosts[2]
#define DefaultFunctionCost mcosts[3]
#define RefCntIncrementCost mcosts[4]
#define RefCntDecrementCost mcosts[5]
#define DopeVectorCopyCost  mcosts[6]
#define RecordCopyCost      mcosts[7]
#define ArrayCopyCost       Iterations
#define DeallocCost         mcosts[8]
#define IntegerCost         mcosts[9]
#define RealCost            mcosts[10]
#define DoubleCost          mcosts[11]

#define R_SMARK         _InfoMask(1)
#define R_VMARK         _InfoMask(2)
#define R_PMARK         _InfoMask(3)
#define R_MinSlice      _InfoMask(4)
#define R_LoopSlice     _InfoMask(5)
#define R_Style         _InfoMask(6)
#define R_Cost          _InfoMask(7)
#define R_Vector        (R_VMARK)
#define R_Concurrent    (R_SMARK|R_PMARK|R_MinSlice|R_LoopSlice|R_Style)

#define MDB_HELP '?'
#define MDB_PAR  'p'
#define MDB_SEQ  's'
#define MDB_PARX 'P'
#define MDB_SEQX 'S'

extern int    info;             /* DUMP INFO */
extern FILE *infoptr;           /* Info fileptr */

extern double scosts[];         /* SIMPLE NODE COST TABLE */
extern double atcosts[];        /* AT-NODE COST TABLE */

extern int level;               /* NESTED LOOP PARTITION THRESHOLD */
extern int atlevel;             /* ONLY SLICE AT THIS PARALLEL NESTING LEVEL */

extern int nopred;
extern int dovec;

extern int procs;               /* NUMBER OF AVAILABLE PROCESSORS */
extern int cRay;                /* VECTORIZE FOR A CRAY? */
extern int vadjust;             /* VECOTIZATION COST ADJUSTMENT VALUE */

extern char *mdbfile;           /* USE MODULE DATA BASE */
extern FILE *REPORT_OUT;        /* Where to write the loop report */
extern FILE *REPORT_IN;         /* Where to read the loop report */

extern char DefaultStyle;       /* Style of loop parallelism to exploit */

/* ------------------------------------------------------------ */
/* if2cost.c */
extern void     ReadCostFile PROTO((char*));
extern double   NumberOfIterations PROTO((PNODE));
extern void     If2Cost PROTO((void));

/* if2part.c */
extern void     If2Part PROTO((void));

/* if2count.c */
extern void     PartIf2Count PROTO((void));

/* if2modules.c */
extern void     MarkParallelFunctions PROTO((void));
extern PMDATA   LookupCallee PROTO((char*));
extern void     ReadModuleDataBase PROTO((void));
extern void     WriteModuleDataBase PROTO((void));
extern void     AddModuleStamp PROTO((void));

/* report.c */
extern void     ReadReport PROTO((void));
extern int      UpdatedLoopPragmas PROTO((PNODE,unsigned));
extern void     PartitionReport PROTO((void));

/* vector.c */
extern int      PartIsVecCandidate PROTO((PNODE,char**));
extern void     VectorSummary PROTO((void));

/* slice.c */
extern int      IsSliceCandidate PROTO((PNODE,char**));

/* stream.c */
extern int      IsStreamTask PROTO((PNODE));

#endif
