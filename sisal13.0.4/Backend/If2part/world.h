/* $Log$
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

#include "../../config.h"
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

#define R_SMARK		_InfoMask(1)
#define R_VMARK		_InfoMask(2)
#define R_PMARK		_InfoMask(3)
#define R_MinSlice	_InfoMask(4)
#define R_LoopSlice	_InfoMask(5)
#define R_Style		_InfoMask(6)
#define R_Cost		_InfoMask(7)
#define R_Vector	(R_VMARK)
#define R_Concurrent	(R_SMARK|R_PMARK|R_MinSlice|R_LoopSlice|R_Style)

#define MDB_HELP '?'
#define MDB_PAR  'p'
#define MDB_SEQ  's'
#define MDB_PARX 'P'
#define MDB_SEQX 'S'

extern int    info;		/* DUMP INFO */
extern FILE *infoptr;		/* Info fileptr */

extern double scosts[];		/* SIMPLE NODE COST TABLE */
extern double atcosts[];	/* AT-NODE COST TABLE */

extern int level;		/* NESTED LOOP PARTITION THRESHOLD */
extern int atlevel;		/* ONLY SLICE AT THIS PARALLEL NESTING LEVEL */

extern int nopred;
extern int dovec;

extern int procs;		/* NUMBER OF AVAILABLE PROCESSORS */
extern int cRay;		/* VECTORIZE FOR A CRAY? */
extern int vadjust;		/* VECOTIZATION COST ADJUSTMENT VALUE */

extern char *mdbfile;		/* USE MODULE DATA BASE */
extern FILE *REPORT_OUT;	/* Where to write the loop report */
extern FILE *REPORT_IN;		/* Where to read the loop report */

extern char DefaultStyle;	/* Style of loop parallelism to exploit */

/* ------------------------------------------------------------ */
/* if2cost.c */
extern void	ReadCostFile();
extern double	NumberOfIterations();
extern void	If2Cost();

/* if2part.c */
extern void	If2Part();

/* if2count.c */
extern void	PartIf2Count();

/* if2modules.c */
extern void	MarkParallelFunctions();
extern PMDATA	LookupCallee();
extern void	ReadModuleDataBase();
extern void	WriteModuleDataBase();
extern void	AddModuleStamp();

/* report.c */
extern void	ReadReport();
extern int	UpdatedLoopPragmas();
extern void	PartitionReport();

/* vector.c */
extern int	PartIsVecCandidate();
extern void	VectorSummary();

/* slice.c */
extern int	IsSliceCandidate();

/* stream.c */
extern int	IsStreamTask();

