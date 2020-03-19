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
 * Revision 1.4  2002/11/21 04:05:02  patmiller
 * Continued updates.  A number of 15 year old bugs have been
 * fixed up:
 *
 * 1) Merging union values where the tags are different, but the value
 *    is the same (e.g. all tags are NULL type)
 *
 * 2) Literals were capped at size 127 bytes! (now 10240)
 *
 * Revision 1.3  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.2  2001/01/01 05:46:22  patmiller
 * Adding prototypes and header info -- all will be broken
 *
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
extern int   stream_io;

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
extern char     *MakeName PROTO((char*,char*,int));
extern PTEMP    GetTemp PROTO((char*,PINFO,int));
extern void     InitializeSymbolTable PROTO((void));
extern void     ChangeToAllocated PROTO((PEDGE,PNODE));
extern void     FreeTemp PROTO((PEDGE));
extern void     PropagateTemp PROTO((PNODE,int,int,PTEMP));
extern int      IsTempExported PROTO((PNODE,PTEMP));
extern int      IsTempImported PROTO((PNODE,PTEMP));
extern void     PrintLocals PROTO((void));
extern PNODE    FindCriticalPath PROTO((PNODE,PNODE));
extern void     AssignTemps PROTO((PNODE));
extern void     PrintFrameDeallocs PROTO((void));

/* if2names.c */
extern char     *GetCopyFunction PROTO((PINFO));
extern char     *GetReadFunction PROTO((int));
extern char     *GetWriteFunction PROTO((int));
extern char     *GetIncRefCountName PROTO((PINFO));
extern char     *GetSetRefCountName PROTO((PINFO));

/* if2preamble.c */
extern void     PrintFilePrologue PROTO((void));
extern void     MarkRecursiveFunctions PROTO((void));
extern void     CheckParallelFunctions PROTO((void));
extern void     PrintFunctPrologue PROTO((PNODE));
extern void     PrintFunctEpilogue PROTO((PNODE));
extern void     PrintFileEpilogue PROTO((void));

/* if2smash.c */
extern void     GenSmashTypes PROTO((void));

/* if2print.c */
extern void     PrintIndentation PROTO((int));
extern void     PrintTemp PROTO((PEDGE));
extern void     PrintFldRef PROTO((char*,char*,PEDGE,char*,int));
extern void     PrintAssgn PROTO((int,PEDGE,PEDGE));
extern void     PrintFldAssgn PROTO((int,char*,char*,PEDGE,char*,int,PEDGE));
extern void     PrintMacro PROTO((int,char*,PNODE,char*));
extern void     PrintSetRefCount PROTO((int,PEDGE,int,int));
extern void     PrintFreeCall PROTO((int,PEDGE));
extern void     PrintConsumerModifiers PROTO((int,PNODE));
extern void     PrintProducerLastModifiers PROTO((int,PNODE));
extern void     PrintProducerModifiers PROTO((int,PNODE));
extern int      GenIsIntrinsic PROTO((PNODE));
extern void     PrintGraph PROTO((int,PNODE));

/* if2record.c */
extern void     PrintUGetTag PROTO((int,PNODE));
extern void     PrintUElement PROTO((int,PNODE));
extern void     PrintUBuild PROTO((int,PNODE));
extern void     PrintRBuild PROTO((int,PNODE));
extern void     PrintRElements PROTO((int,PNODE));
extern void     PrintRReplace PROTO((int,PNODE));
extern void     PrintRecordNoOp PROTO((int,PNODE));
extern void     PrintBRAStore PROTO((int,PNODE,PNODE));
extern void     PrintBRBuild PROTO((int,PNODE));
extern void     PrintBROptAElement PROTO((int,PNODE));
extern void     PrintBRElements PROTO((int,PNODE));
extern void     PrintBRReplace PROTO((int,PNODE));
extern void     PrintUTagTest PROTO((int,PNODE));

/* if2array.c */
extern char     *GetSisalInfo PROTO((PNODE,char*));
extern char     *GetSisalInfoOnEdge PROTO((PEDGE,char*));
extern void     PrintBoundsCheck PROTO((int,PNODE,PEDGE,PEDGE));
extern void     PrintRagged PROTO((int,PNODE));
extern void     PrintPSMemAllocDVI PROTO((int,PNODE));
extern void     PrintPSScatter PROTO((int,PNODE));
extern void     PrintPSManager PROTO((int,PNODE,char*));
extern void     PrintPSFree PROTO((int,PNODE,char*,PNODE));
extern void     PrintPSAlloc PROTO((int,PNODE,char*));
extern void     PrintMemAlloc PROTO((int,PNODE));
extern void     PrintGABase PROTO((int,PNODE));
extern void     PrintOptAElement PROTO((int,PNODE));
extern void     PrintArrayMacro PROTO((int,char*,char*,PNODE));
extern void     PrintAReplace PROTO((int,PNODE));
extern void     PrintABuild PROTO((int,PNODE));
extern void     PrintABuildAT PROTO((int,PNODE));
extern void     PrintAAddHLAT PROTO((int,PNODE));
extern void     PrintAAddH PROTO((int,PNODE));
extern void     PrintACatenateAT PROTO((int,PNODE));
extern void     PrintArrayNoOp PROTO((int,PNODE));

/* if2select.c */
extern void     PrintSelect PROTO((int,PNODE));
extern void     PrintTagCase PROTO((int,PNODE));

/* if2loop.c */
extern void     PrintAStore PROTO((int,PNODE,PNODE));
extern void     PrintYankedRed PROTO((int,PNODE,char*));
extern int      AreAllUnitFanout PROTO((PNODE));
extern void     PrintSumOfTerms PROTO((int,PEDGE));
extern PEDGE    GetSliceParam PROTO((PEDGE,PNODE));
extern void     PrintRanges PROTO((PNODE));
extern void     PrintRangeLow PROTO((PNODE));
extern void     PrintRangeHigh PROTO((PNODE));
extern void     PrintSliceTaskInit PROTO((int,PNODE));
extern void     PrintForall PROTO((int,PNODE));
extern void     PrintLoop PROTO((int,PNODE));
extern void     PrintFirstSum PROTO((int,PNODE));
extern void     PrintTri PROTO((int,PNODE));
extern void     PrintVMinMax PROTO((int,PNODE,char*));
extern int      LCMSize PROTO((PNODE));
extern void     PrintReturnRapUp PROTO((int,PNODE));

/* if2aimp.c */
extern void     NormalizeVectorLoop PROTO((PNODE));
extern void     AssignNewPortNums PROTO((PNODE,int));
extern void     If2AImp PROTO((void));
extern void     WriteIf2AImpInfo PROTO((void));

/* if2opt.c */
extern void     PrepareGraph PROTO((PNODE));
extern void     If2Opt PROTO((void));
extern void     WriteIf2OptInfo PROTO((void));
extern void     WriteIf2AImpInfo2 PROTO((void));
extern void     WriteIf2OptInfo2 PROTO((void));

/* if2vector.c */
extern void     WriteVectorInfo PROTO((void));
extern void     If2Vectorize PROTO((int));
extern void     PrintNOVECTOR PROTO((void));
extern void     PrintVECTOR PROTO((void));
extern void     PrintASSOC PROTO((void));
extern void     PrintSAFE PROTO((char*));

/* if2prebuild.c */
extern void     GenNormalizeNode PROTO((PNODE));
extern void     PointerSwap PROTO((PNODE,PNODE));
extern void     WritePrebuildInfo PROTO((void));
extern void     OptimizeBIPs PROTO((PNODE));
extern void     If2Prebuild0 PROTO((void));
extern void     If2Prebuild1 PROTO((void));
extern void     If2Prebuild2 PROTO((void));

/* if2fibre.c */
extern void     PrintReadFibreInputs PROTO((PNODE));
extern void     PrintTypeWriters PROTO((PNODE));
extern void     PrintWriteFibreOutputs PROTO((PNODE));
extern void     PrintWriteFibreInputs PROTO((PNODE));
extern void     PrintPeek PROTO((int,PNODE));

/* if2free.c */
extern char     *GetFreeName PROTO((PINFO));
extern void     PrintFreeUtilities PROTO((void));
extern void     PrintInputDeallocs PROTO((char*,int,PNODE));
extern void     PrintOutputDeallocs PROTO((int,PNODE));

/* if2interface.c */
extern void     WriteInterfaceInfo PROTO((void));
extern int      GetLanguage PROTO((PNODE));
extern char     *BindInterfaceName PROTO((char*,int,int));
extern void     PrintInterfaceCall PROTO((int,PNODE,PNODE));
extern int      GenIsReadOnly PROTO((PNODE,int));
extern void     PrintInterfaceUtilities PROTO((void));
extern void     PrintInterface PROTO((PNODE));

/* if2sdbx.c */
extern void     PrintSdbxFunctionList PROTO((void));
extern void     BuildAndPrintSdbxScope PROTO((PNODE));
extern void     UpdateSdbxScopeNames PROTO((PNODE));
extern void     SaveSdbxState PROTO((PNODE));

/* if2ureduce.c */
extern void     PrintUReduceRapUp PROTO((int,PNODE));
extern void     PrintUReduceUpd PROTO((int,PNODE));
extern void     PrintUReduceInit PROTO((int,PNODE));

#endif
