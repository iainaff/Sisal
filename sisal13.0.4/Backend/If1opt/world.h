/* $Log$
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
 * */

#include "../../config.h"
#include "../Library/IFX.h"

extern int    asetl;		/* REMOVE UNNECESSARY ASetL NODES? */
extern int    ifuse;		/* PERFORM INDEPENDENT FUSION */
extern int    sfuse;		/* PERFORM SELECT FUSION? */
extern int    dfuse;		/* PERFORM DEPENDENT FUSION? */
extern int    info;		/* DUMP INFORMATION? */
extern int    slfis;		/* PERFORM STREAM LOOP FISSION? */
extern int    inter;		/* PERFORM INLINE EXPANSION? */
extern int    inlineall;	/* INLINE EVERYTHING BUT REQUESTS? */
extern int    intrinsics;	/* FOLD Math intrinsic functions */
extern int    native;		/* FLAG NODES NOT SUPPORTED IN NATIVE COMPILATION */
extern int    dope;		/* PEFORM DOPE VECTOR OPTIMIZATIONS? */
extern int    amove;		/* PERFORM ANTI-MOVEMENT */
extern int    unroll;		/* PERFORM FORALL UNROLLING? */
extern int    split;		/* PERFORM FORALL SPLITTING? */
extern int    invert;		/* PERFORM LOOP INVERSION? */
extern int    vec;		/* VECTOR MODE? */
extern int    concur;		/* CONCURRENT MODE? */

extern  int   Oinvar;		/* OUTERMOST LOOP INVARIANT REMOVAL? */

extern int ikcnt;		/* COUNT OF COMBINED K IMPORTS */
extern int unnec;		/* COUNT OF UNNECESSARY EDGES OR LITERALS */
extern int unused;		/* COUNT OF UNUSED VALUES */
extern int dscnt;		/* COUNT OF DEAD SIMPLE NODES */
extern int dccnt;		/* COUNT OF DEAD COMPOUND NODES */
extern int agcnt;		/* COUNT OF DEAD AGather NODES */
extern int tgcse;		/* TRY AND FORCE GCSE IMPORVEMENTS? */

extern int maxunroll;/* MAX NUMBER OF ITERATIONS ALLOWED IN AN UNROLLED LOOP */

extern int cRay;		/* COMPILING FOR THE CRAY? */
extern int alliantfx;		/* COMPILING FOR THE ALLIANT? */

extern int normidx;		/* NORMALIZE ARRAY INDEXING OPERATIONS? */
extern int fchange;		/* WAS A FUSION DONE? */

extern int glue;		/* ELIMINATE DEAD FUNCTION CALLS? */ 
extern int NoInlining;		/* Inline no functions */
extern int noincnt;		/* FUNCTION CALL LIST INDEX */
extern char *noin[];		/* FUNCTION CALL LIST */

extern double iter;		/* LOOP ITERATION COUNT DEFAULT */

extern int agcse;		/* DO ANTI-GLOBAL CSE? */

extern int prof;		/* PROFILE THE OPTIMIZER? */

extern int DeBuG;		/* OPT FOR PROGRAM DEBUGGING? */
extern int noassoc;		/* NO ASSOCIATIVE TRANSFORMATIONS */

extern int AggressiveVectors;	/* Aggressively combine vectorizing loops? */

extern FILE *infoptr;

/* ------------------------------------------------------------ */

/* callreorder.c */
extern void	CallReorder();

/* cascade.c */
extern void	If1TestCascade();

/* if1check.c */
extern void	If1Check();

/* if1count.c */
extern void	If1Count();

/* if1cse.c */
extern void	WriteCseInfo();
extern void	If1Cse();

/* if1dead.c */
extern void	CombineKports();
extern void	FastCleanGraph();
extern void	OptRemoveDeadNode();
extern void	RemoveDeadEdge();
extern void	WriteCleanInfo();
extern void	If1Clean();

/* if1dope.c */
extern void	If1Dope();
extern void	WriteDopeInfo();

/* if1explode.c */
extern void	WriteExplodeInfo();
extern void	If1Explode();

/* if1fission.c */
extern void	WriteFissionInfo();
extern void	If1Fission();

/* if1fold.c */
extern void	OptNormalizeNode();
extern char	*DoubleToReal();
extern void	WriteFoldInfo();
extern void	If1Fold();

/* if1fusion.c */
extern void	If1DFusion();
extern void	WriteFusionInfo();
extern void	If1IFusion();

/* if1gcse.c */
extern void	WriteGCseInfo();
extern void	OptRemoveSCses();
extern void	If1GCse();

/* if1inline.c */
extern void	SpliceInGraph();
extern void	If1Inline();

/* if1invar.c */
extern int	OptIsEdgeInvariant();
extern void	ExposeInvariants();
extern void	AntiMovement();
extern void	WriteInvarInfo();
extern void	If1Invar();

/* if1invert.c */
extern void	WriteInvertInfo();
extern void	If1Invert();

/* if1move.c */
extern void	FindAndLinkToSource();
extern void	RemoveNode();
extern void	InsertNode();

/* if1normal.c */
extern void	EliminateDeadFunctions();
extern void	If1Normalize();

/* if1parallel.c */
extern int	OptIsVecCandidate();
extern void	WriteConcurInfo();
extern void	If1Vec();
extern void	If1Par();

/* if1pprint.c */
extern void	If1PPrint();

/* if1reduce.c */
extern void	If1Reduce();
extern void	WriteReduceInfo();

/* if1split.c */
extern void	WriteSplitInfo();
extern void	If1Split();

/* if1unroll.c */
extern void	WriteUnrollInfo();
extern void	If1Unroll();

/* ifxstuff.c */
extern void	PlaceInEntryTable();
extern void	PlaceInFortranTable();
extern void	PlaceInCTable();
extern void	PragInitPragmas();
extern void	NodeInitPragmas();
extern void	TypeInitPragmas();
extern void	EdgeInitPragmas();
extern void	TypeAssignPragmas();
extern void	EdgeAssignPragmas();
extern void	NodeAssignPragmas();

/* util.c */
extern int	OptIsInvariant();
extern void	DecodeIndexing();
extern void	EncodeIndexing();

/* AssignIDs.c */
extern void	NewCompoundID();
extern void	AssignCompoundIDs();
