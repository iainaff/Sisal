#ifndef WORLD_H
#define WORLD_H

/**************************************************************************/
/* FILE   **************         world.h<2>        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log$
 * Revision 1.1.1.1  2000/12/31 17:57:34  patmiller
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
 * Revision 1.15  1994/06/16  21:30:43  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.14  1994/04/15  15:51:07  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.13  1994/02/15  23:40:56  miller
 * Changes to allow new IF1/2 types (complex, typesets, etc...)
 *
 * Revision 1.12  1993/03/23  22:46:16  miller
 * date problem
 *
 * Revision 1.11  1994/03/11  23:09:32  miller
 * Moved IFX.h into Backend/Library and added support for Minimal
 * installation (removing source as compiled).
 *
 * Revision 1.10  1993/02/27  00:29:49  miller
 * Added MinSliceThrottle (to turn off %MS pragmas)
 *
 * Revision 1.9  1993/02/24  18:48:36  miller
 * Added new PrintHiRange and PrintLowRange to support new loop styles.
 *
 * Revision 1.8  1993/01/15  22:25:57  miller
 * Moved the definition for info up into IFX.h
 *
 * Revision 1.7  1993/01/14  22:27:25  miller
 * Now allow code comments if the -% option is used.  Had to make changes
 * to reflect the new ThinCopy pragma.  Fixed some other pragma stuff
 * in ifxstuff.c
 *
 * Revision 1.6  1993/01/07  00:38:50  miller
 * Make changes for LINT and combined files.
 *
 */
/**************************************************************************/

#include "sisalInfo.h"
#include "../Library/IFX.h"


extern int    info;             /* DUMP INFORMATION? */
extern FILE *infoptr;           /* INFORMATION output file */
extern FILE *infoptr1;          /* INFORMATION output file */
extern FILE *infoptr2;          /* INFORMATION output file */
extern FILE *infoptr3;          /* INFORMATION output file */
extern FILE *infoptr4;          /* INFORMATION output file */
extern int    aimp;             /* OPTIMIZE ARRAY DEREFERENCE OPERATIONS? */
extern int    if2opt;           /* OPTIMIZE GatherAT NODES? */

extern int    nmid;             /* NAME STAMP */
extern int    tmpid;            /* TEMPORARY STAMP */
extern int    bounds;           /* GENERATE BOUNDS CHECKS? */

extern int    line;             /* LINE NUMBER OF PREV. LINE OF IF1 FILE */

extern int   gshared;

extern int   sequential;        /* CURRENTLY GENERATING SEQUENTIAL CODE? */

extern int   invtfa;            /* COUNT OF INVARIANT TASK FRAME ARGS */
extern int   vec;               /* FURTHER VECTORIZE CODE? */

extern int   rmsrcnt;           /* COUNT OF ELIMINATED SR PRAGMAS */
extern int   rmpmcnt;           /* COUNT OF ELIMINATED PM PRAGMAS */
extern int   rmcmcnt;           /* COUNT OF ELIMINATED CM PRAGMAS */
extern int   rmcnoop;           /* COUNT OF REMOVED CONDITIONAL COPY NoOpS */
extern int   rmnoop;            /* COUNT OF REMOVED COPY NoOpS */
extern int   rmsmark;           /* COUNT OF REMOVED smarks */
extern int   rmvmark;           /* COUNT OF REMOVED vmarks */

extern int   cRay;              /* COMPILING FOR THE CRAY? */
extern int   alliantfx;         /* COMPILING FOR THE ALLIANT FX SERIES? */
extern int   movereads;         /* MOVE ARRAY READ OPERATIONS? */
extern int   xmpchains;         /* FORM CHAINS FOR THE CRAY X-MP? */
extern int   newchains;  

extern int   rag;               /* IDENTIFY RAGGED MEM-ALLOCS? */
extern int   bip;               /* BIP OPTIMIZATION? */
extern int   bipmv;
extern int   oruntime;/* USE ORIGINAL SISAL MICROTASKING SOFTWARE */

extern int   bindtosisal;       /* BIND INTERFACE CALLS TO SISAL? */

extern int   freeall;           /* FORCE RELEASE OF ALL STORAGE? */

extern int   Iupper;            /* INTERFACE NAME GENERATION COMMANDS */
extern int   IunderR;
extern int   IunderL;

extern int   fva;               /* FORCE ALLIANT VECTORIZATION PRAGMAS? */
extern int   fvc;               /* FORCE CRAY VECTORIZATION PRAGMAS? */
extern int   nltss;             /* COMPILE FOR NLTSS C-COMPILER? */

extern int   SISdebug;          /* REMOVE DEAD FUNCTION CALLS? */

extern int   intrinsics;        /* RECOGNIZE LOGICAL FUNCTIONS: and,or,xor,not? */

extern int   max_dims;          /* MAXIMUM NUMBER OF DESIRED POINTER SWAP DIMENSIONS */
extern int   share;             /* TRY AND SHARE POINTER SWAP STORAGE */

extern int   assoc;             /* DO ASSOCIATIVE TRANSFORMATIONS? */

extern int   standalone;        /* CALLED FROM THE OPERATING SYSTEM? */

extern int   gdata;             /* PREPARE GLOBAL DATA? */

extern int   sdbx;              /* GENERATE SDBX CODE? */

extern FILE *hyfd;              /* HYBRID FILE DESCRIPTOR */
extern char *hybrid;            /* HYBRID FILE NAME */

extern int nobrec;              /* DISABLE BASIC RECORD OPTIMIZATION? */
extern int CodeComments;        /* Show source code lines in code */
extern int MinSliceThrottle;    /* TRUE==> use minslice, F==> No throttle */
/* ------------------------------------------------------------ */
/* if2yank.c */
extern void     If2Yank1 PROTO((void));
extern void     If2Yank0 PROTO((void));
extern void     WriteYankInfo PROTO((void));

/* if2temp.c */
extern char     *MakeName PROTO((void));
extern PTEMP    GetTemp PROTO((void));
extern void     InitializeSymbolTable PROTO((void));
extern void     ChangeToAllocated PROTO((void));
extern void     FreeTemp PROTO((void));
extern void     PropagateTemp PROTO((void));
extern int      IsTempExported PROTO((void));
extern int      IsTempImported PROTO((void));
extern void     PrintLocals PROTO((void));
extern PNODE    FindCriticalPath PROTO((void));
extern void     AssignTemps PROTO((void));
extern void     PrintFrameDeallocs PROTO((void));

/* if2names.c */
extern char     *GetCopyFunction PROTO((void));
extern char     *GetReadFunction PROTO((void));
extern char     *GetWriteFunction PROTO((void));
extern char     *GetIncRefCountName PROTO((void));
extern char     *GetSetRefCountName PROTO((void));

/* if2preamble.c */
extern void     PrintFilePrologue PROTO((void));
extern void     MarkRecursiveFunctions PROTO((void));
extern void     CheckParallelFunctions PROTO((void));
extern void     PrintFunctPrologue PROTO((void));
extern void     PrintFunctEpilogue PROTO((void));
extern void     PrintFileEpilogue PROTO((void));

/* if2smash.c */
extern void     GenSmashTypes PROTO((void));

/* if2print.c */
extern void     PrintIndentation PROTO((void));
extern void     PrintTemp PROTO((void));
extern void     PrintFldRef PROTO((void));
extern void     PrintAssgn PROTO((void));
extern void     PrintFldAssgn PROTO((void));
extern void     PrintMacro PROTO((void));
extern void     PrintSetRefCount PROTO((void));
extern void     PrintFreeCall PROTO((void));
extern void     PrintConsumerModifiers PROTO((void));
extern void     PrintProducerLastModifiers PROTO((void));
extern void     PrintProducerModifiers PROTO((void));
extern int      GenIsIntrinsic PROTO((void));
extern void     PrintGraph PROTO((void));

/* if2record.c */
extern void     PrintUGetTag PROTO((void));
extern void     PrintUElement PROTO((void));
extern void     PrintUBuild PROTO((void));
extern void     PrintRBuild PROTO((void));
extern void     PrintRElements PROTO((void));
extern void     PrintRReplace PROTO((void));
extern void     PrintRecordNoOp PROTO((void));
extern void     PrintBRAStore PROTO((void));
extern void     PrintBRBuild PROTO((void));
extern void     PrintBROptAElement PROTO((void));
extern void     PrintBRElements PROTO((void));
extern void     PrintBRReplace PROTO((void));
extern void     PrintUTagTest PROTO((void));

/* if2array.c */
extern char     *GetSisalInfo PROTO((void));
extern char     *GetSisalInfoOnEdge PROTO((void));
extern void     PrintBoundsCheck PROTO((void));
extern void     PrintRagged PROTO((void));
extern void     PrintPSMemAllocDVI PROTO((void));
extern void     PrintPSScatter PROTO((void));
extern void     PrintPSManager PROTO((void));
extern void     PrintPSFree PROTO((void));
extern void     PrintPSAlloc PROTO((void));
extern void     PrintMemAlloc PROTO((void));
extern void     PrintGABase PROTO((void));
extern void     PrintOptAElement PROTO((void));
extern void     PrintArrayMacro PROTO((void));
extern void     PrintAReplace PROTO((void));
extern void     PrintABuild PROTO((void));
extern void     PrintABuildAT PROTO((void));
extern void     PrintAAddHLAT PROTO((void));
extern void     PrintAAddH PROTO((void));
extern void     PrintACatenateAT PROTO((void));
extern void     PrintArrayNoOp PROTO((void));

/* if2select.c */
extern void     PrintSelect PROTO((void));
extern void     PrintTagCase PROTO((void));

/* if2loop.c */
extern void     PrintAStore PROTO((void));
extern void     PrintYankedRed PROTO((void));
extern int      AreAllUnitFanout PROTO((void));
extern void     PrintSumOfTerms PROTO((void));
extern PEDGE    GetSliceParam PROTO((void));
extern void     PrintRanges PROTO((void));
extern void     PrintRangeLow PROTO((void));
extern void     PrintRangeHigh PROTO((void));
extern void     PrintSliceTaskInit PROTO((void));
extern void     PrintForall PROTO((void));
extern void     PrintLoop PROTO((void));
extern void     PrintFirstSum PROTO((void));
extern void     PrintTri PROTO((void));
extern void     PrintVMinMax PROTO((void));
extern int      LCMSize PROTO((void));
extern void     PrintReturnRapUp PROTO((void));

/* if2aimp.c */
extern void     NormalizeVectorLoop PROTO((void));
extern void     AssignNewPortNums PROTO((void));
extern void     If2AImp PROTO((void));
extern void     WriteIf2AImpInfo PROTO((void));

/* if2opt.c */
extern void     PrepareGraph PROTO((void));
extern void     If2Opt PROTO((void));
extern void     WriteIf2OptInfo PROTO((void));
extern void     WriteIf2AImpInfo2 PROTO((void));
extern void     WriteIf2OptInfo2 PROTO((void));

/* if2vector.c */
extern void     WriteVectorInfo PROTO((void));
extern void     If2Vectorize PROTO((void));
extern void     PrintNOVECTOR PROTO((void));
extern void     PrintVECTOR PROTO((void));
extern void     PrintASSOC PROTO((void));
extern void     PrintSAFE PROTO((void));

/* if2prebuild.c */
extern void     GenNormalizeNode PROTO((void));
extern void     PointerSwap PROTO((void));
extern void     WritePrebuildInfo PROTO((void));
extern void     OptimizeBIPs PROTO((void));
extern void     If2Prebuild0 PROTO((void));
extern void     If2Prebuild1 PROTO((void));
extern void     If2Prebuild2 PROTO((void));

/* if2fibre.c */
extern void     PrintReadFibreInputs PROTO((void));
extern void     PrintTypeWriters PROTO((void));
extern void     PrintWriteFibreOutputs PROTO((void));
extern void     PrintWriteFibreInputs PROTO((void));
extern void     PrintPeek PROTO((void));

/* if2free.c */
extern char     *GetFreeName PROTO((void));
extern void     PrintFreeUtilities PROTO((void));
extern void     PrintInputDeallocs PROTO((void));
extern void     PrintOutputDeallocs PROTO((void));

/* if2interface.c */
extern void     WriteInterfaceInfo PROTO((void));
extern int      GetLanguage PROTO((void));
extern char     *BindInterfaceName PROTO((void));
extern void     PrintInterfaceCall PROTO((void));
extern int      GenIsReadOnly PROTO((void));
extern void     PrintInterfaceUtilities PROTO((void));
extern void     PrintInterface PROTO((void));

/* if2sdbx.c */
extern void     PrintSdbxFunctionList PROTO((void));
extern void     BuildAndPrintSdbxScope PROTO((void));
extern void     UpdateSdbxScopeNames PROTO((void));
extern void     SaveSdbxState PROTO((void));

/* if2ureduce.c */
extern void     PrintUReduceRapUp PROTO((void));
extern void     PrintUReduceUpd PROTO((void));
extern void     PrintUReduceInit PROTO((void));

#endif
