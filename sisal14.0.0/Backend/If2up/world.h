/* $Log$
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

extern int    mig;		/* MIGRATE NODES TOWARD USES? */
extern FILE *infoptr;		/* information output file */
extern FILE *infoptr2;		/* information output file */

extern PNODE  nohead;		/* NoOp NODE LIST HEAD */
extern PNODE  notail;		/* NoOp NODE LIST TAIL */
extern PSET   gshead;		/* GLOBAL READ/WRITE SET LIST HEAD */
extern PSET   gstail;		/* GLOBAL READ/WRITE SET LIST TAIL */
extern PEDGE  dghead;		/* DATA GROUND EDGE LIST HEAD */
extern PEDGE  dgtail;		/* DATA GROUND EDGE LIST TAIL */
extern PNODE  cohead;		/* CONSTANT AGGREGATE GENERATOR LIST HEAD */
extern PNODE  cotail;		/* CONSTANT AGGREGATE GENERATOR LIST TAIL */

extern int    minopt;		/* ONLY PERFORM MINIMAL OPTIMIZATION? */
extern int    cagg;		/* MARK CONSTANT AGGREGATES? */
extern int    ststr;		/* IDENTIFY SINGLE THREADED STREAMS? */

extern int    univso;		/* UNIVERSAL STREAM OWNERSHIP? */
extern int    univao;		/* UNIVERSAL ARRAY  OWNERSHIP? */
extern int    cmig;		/* COUNT OF MIGRATED NODES */
extern int    seqimp;

extern int    swcnt;		/* COUNT OF SWAP OPTIMIZATIONS */
extern int    hnoops;		/* COUNT OF HOISTED NoOp NODES */

/* ------------------------------------------------------------ */
/* if2up.c */
extern void	If2Up();

/* if2call.c */
extern void	If2CallGraph();

/* if2noop.c */
extern void	If2NoOp();

/* if2refcnt.c */
extern void	AssignCMPragmas();
extern void	If2ReferenceCount();

/* if2class.c */
extern void	If2Classify();

/* if2rwset.c */
extern void	If2ReadWriteSets();

/* if2ade.c */
extern void	If2Ade();

/* if2replace.c */
extern void	If2Replace();

/* if2count.c */
extern void	UpIf2Count();
extern void	WriteIf2upWarnings();

/* if2const.c */
extern void	If2ConstGenerators();

/* if2owner.c */
extern void	If2Ownership();
extern void	OptimizeSwaps();

/* if2refcntopt.c */
extern void	CleanNoOpImports();
extern void	If2ReferenceCountOpt();

/* if2marks.c */
extern void	If2RefineGrounds();
extern void	If2PropagateMarks();

/* if2migrate.c */
extern void	If2Migrate();

/* if2pprint.c */
extern void	If2PPrint();
