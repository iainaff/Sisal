/* $Log$
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
 * */

#include "sisalInfo.h"
#include "../Library/IFX.h"


extern int    info;		/* DUMP INFORMATION? */
extern FILE *infoptr;		/* INFORMATION output file */
extern FILE *infoptr1;		/* INFORMATION output file */
extern FILE *infoptr2;		/* INFORMATION output file */
extern FILE *infoptr3;		/* INFORMATION output file */
extern FILE *infoptr4;		/* INFORMATION output file */
extern int    aimp;		/* OPTIMIZE ARRAY DEREFERENCE OPERATIONS? */
extern int    if2opt;		/* OPTIMIZE GatherAT NODES? */

extern int    nmid;		/* NAME STAMP */
extern int    tmpid;		/* TEMPORARY STAMP */
extern int    bounds;		/* GENERATE BOUNDS CHECKS? */

extern int    line;		/* LINE NUMBER OF PREV. LINE OF IF1 FILE */

extern int   gshared;

extern int   sequential;	/* CURRENTLY GENERATING SEQUENTIAL CODE? */

extern int   invtfa;		/* COUNT OF INVARIANT TASK FRAME ARGS */
extern int   vec;		/* FURTHER VECTORIZE CODE? */

extern int   rmsrcnt;		/* COUNT OF ELIMINATED SR PRAGMAS */
extern int   rmpmcnt;		/* COUNT OF ELIMINATED PM PRAGMAS */
extern int   rmcmcnt;		/* COUNT OF ELIMINATED CM PRAGMAS */
extern int   rmcnoop;		/* COUNT OF REMOVED CONDITIONAL COPY NoOpS */
extern int   rmnoop;		/* COUNT OF REMOVED COPY NoOpS */
extern int   rmsmark;		/* COUNT OF REMOVED smarks */
extern int   rmvmark;		/* COUNT OF REMOVED vmarks */

extern int   cRay;		/* COMPILING FOR THE CRAY? */
extern int   alliantfx;		/* COMPILING FOR THE ALLIANT FX SERIES? */
extern int   movereads;		/* MOVE ARRAY READ OPERATIONS? */
extern int   xmpchains;		/* FORM CHAINS FOR THE CRAY X-MP? */
extern int   newchains;  

extern int   rag;		/* IDENTIFY RAGGED MEM-ALLOCS? */
extern int   bip;		/* BIP OPTIMIZATION? */
extern int   bipmv;
extern int   oruntime;/* USE ORIGINAL SISAL MICROTASKING SOFTWARE */

extern int   bindtosisal;	/* BIND INTERFACE CALLS TO SISAL? */

extern int   freeall;		/* FORCE RELEASE OF ALL STORAGE? */

extern int   Iupper;		/* INTERFACE NAME GENERATION COMMANDS */
extern int   IunderR;
extern int   IunderL;

extern int   fva;		/* FORCE ALLIANT VECTORIZATION PRAGMAS? */
extern int   fvc;		/* FORCE CRAY VECTORIZATION PRAGMAS? */
extern int   nltss;		/* COMPILE FOR NLTSS C-COMPILER? */

extern int   SISdebug;		/* REMOVE DEAD FUNCTION CALLS? */

extern int   intrinsics;	/* RECOGNIZE LOGICAL FUNCTIONS: and,or,xor,not? */

extern int   max_dims;		/* MAXIMUM NUMBER OF DESIRED POINTER SWAP DIMENSIONS */
extern int   share;		/* TRY AND SHARE POINTER SWAP STORAGE */

extern int   assoc;		/* DO ASSOCIATIVE TRANSFORMATIONS? */

extern int   standalone;	/* CALLED FROM THE OPERATING SYSTEM? */

extern int   gdata;		/* PREPARE GLOBAL DATA? */

extern int   sdbx;		/* GENERATE SDBX CODE? */

extern FILE *hyfd;		/* HYBRID FILE DESCRIPTOR */
extern char *hybrid;		/* HYBRID FILE NAME */

extern int nobrec;		/* DISABLE BASIC RECORD OPTIMIZATION? */
extern int CodeComments;	/* Show source code lines in code */
extern int MinSliceThrottle;	/* TRUE==> use minslice, F==> No throttle */
/* ------------------------------------------------------------ */
/* if2yank.c */
extern void	If2Yank1();
extern void	If2Yank0();
extern void	WriteYankInfo();

/* if2temp.c */
extern char	*MakeName();
extern PTEMP	GetTemp();
extern void	InitializeSymbolTable();
extern void	ChangeToAllocated();
extern void	FreeTemp();
extern void	PropagateTemp();
extern int	IsTempExported();
extern int	IsTempImported();
extern void	PrintLocals();
extern PNODE 	FindCriticalPath();
extern void	AssignTemps();
extern void	PrintFrameDeallocs();

/* if2names.c */
extern char	*GetCopyFunction();
extern char	*GetReadFunction();
extern char	*GetWriteFunction();
extern char	*GetIncRefCountName();
extern char	*GetSetRefCountName();

/* if2preamble.c */
extern void	PrintFilePrologue();
extern void	MarkRecursiveFunctions();
extern void	CheckParallelFunctions();
extern void	PrintFunctPrologue();
extern void	PrintFunctEpilogue();
extern void	PrintFileEpilogue();

/* if2smash.c */
extern void	GenSmashTypes();

/* if2print.c */
extern void	PrintIndentation();
extern void	PrintTemp();
extern void	PrintFldRef();
extern void	PrintAssgn();
extern void	PrintFldAssgn();
extern void	PrintMacro();
extern void	PrintSetRefCount();
extern void	PrintFreeCall();
extern void	PrintConsumerModifiers();
extern void	PrintProducerLastModifiers();
extern void	PrintProducerModifiers();
extern int	GenIsIntrinsic();
extern void	PrintGraph();

/* if2record.c */
extern void	PrintUGetTag();
extern void	PrintUElement();
extern void	PrintUBuild();
extern void	PrintRBuild();
extern void	PrintRElements();
extern void	PrintRReplace();
extern void	PrintRecordNoOp();
extern void	PrintBRAStore();
extern void	PrintBRBuild();
extern void	PrintBROptAElement();
extern void	PrintBRElements();
extern void	PrintBRReplace();
extern void	PrintUTagTest();

/* if2array.c */
extern char	*GetSisalInfo();
extern char	*GetSisalInfoOnEdge();
extern void	PrintBoundsCheck();
extern void	PrintRagged();
extern void	PrintPSMemAllocDVI();
extern void	PrintPSScatter();
extern void	PrintPSManager();
extern void	PrintPSFree();
extern void	PrintPSAlloc();
extern void	PrintMemAlloc();
extern void	PrintGABase();
extern void	PrintOptAElement();
extern void	PrintArrayMacro();
extern void	PrintAReplace();
extern void	PrintABuild();
extern void	PrintABuildAT();
extern void	PrintAAddHLAT();
extern void	PrintAAddH();
extern void	PrintACatenateAT();
extern void	PrintArrayNoOp();

/* if2select.c */
extern void	PrintSelect();
extern void	PrintTagCase();

/* if2loop.c */
extern void	PrintAStore();
extern void	PrintYankedRed();
extern int	AreAllUnitFanout();
extern void	PrintSumOfTerms();
extern PEDGE	GetSliceParam();
extern void	PrintRanges();
extern void	PrintRangeLow();
extern void	PrintRangeHigh();
extern void	PrintSliceTaskInit();
extern void	PrintForall();
extern void	PrintLoop();
extern void	PrintFirstSum();
extern void	PrintTri();
extern void	PrintVMinMax();
extern int	LCMSize();
extern void	PrintReturnRapUp();

/* if2aimp.c */
extern void	NormalizeVectorLoop();
extern void 	AssignNewPortNums();
extern void	If2AImp();
extern void	WriteIf2AImpInfo();

/* if2opt.c */
extern void	PrepareGraph();
extern void	If2Opt();
extern void	WriteIf2OptInfo();
extern void     WriteIf2AImpInfo2();
extern void     WriteIf2OptInfo2();

/* if2vector.c */
extern void	WriteVectorInfo();
extern void	If2Vectorize();
extern void	PrintNOVECTOR();
extern void	PrintVECTOR();
extern void	PrintASSOC();
extern void	PrintSAFE();

/* if2prebuild.c */
extern void	GenNormalizeNode();
extern void	PointerSwap();
extern void	WritePrebuildInfo();
extern void	OptimizeBIPs();
extern void	If2Prebuild0();
extern void	If2Prebuild1();
extern void	If2Prebuild2();

/* if2fibre.c */
extern void	PrintReadFibreInputs();
extern void	PrintTypeWriters();
extern void	PrintWriteFibreOutputs();
extern void	PrintWriteFibreInputs();
extern void	PrintPeek();

/* if2free.c */
extern char	*GetFreeName();
extern void	PrintFreeUtilities();
extern void	PrintInputDeallocs();
extern void	PrintOutputDeallocs();

/* if2interface.c */
extern void	WriteInterfaceInfo();
extern int	GetLanguage();
extern char	*BindInterfaceName();
extern void	PrintInterfaceCall();
extern int	GenIsReadOnly();
extern void	PrintInterfaceUtilities();
extern void	PrintInterface();

/* if2sdbx.c */
extern void	PrintSdbxFunctionList();
extern void	BuildAndPrintSdbxScope();
extern void	UpdateSdbxScopeNames();
extern void	SaveSdbxState();

/* if2ureduce.c */
extern void     PrintUReduceRapUp();
extern void     PrintUReduceUpd();
extern void     PrintUReduceInit();
