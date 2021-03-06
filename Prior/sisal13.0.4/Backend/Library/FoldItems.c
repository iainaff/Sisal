#include "world.h"


int fcnt  = 0;                            /* COUNT OF FOLDED NODES */
int pcnt  = 0;                    /* COUNT OF PROPAGATED CONSTANTS */
int sfcnt = 0;             /* RUNNING COUNT OF FOLDED SELECT NODES */
int lfcnt = 0;                         /* COUNT OF FOLDED LOGICALS */
int esccnt = 0;              /* COUNT OF EXPORTED SELECT CONSTANTS */
int expcnt = 0;                         /* EXP STRENGTH REDUCTIONS */

long   iop1;			/* INTEGER, CHAR, OR BOOLEAN OPERANDS */
long   iop2;
double dop1;			/* DOUBLE_REAL OR REAL OPERANDS */
double dop2;

int chain_cnt = 0;             /* COUNT OF FOLDED +/- CHAINS       */
int zero_cnt  = 0;             /* COUNT OF A * 0 AND 0 * A FOLDS   */
int ident_cnt = 0;             /* COUNT OF FOLDED IDENTITY NODES   */
int dncnt     = 0;          /* COUNT OF DIVIDE TO NEG CONVERSIONS  */
int pncnt     = 0;                /* COUNT OF PROPAGATED NEGATIONS */
int idxm      = 0;        /* COUNT OF ARRAY INDEXING MODIFICATIONS */
int negcnt    = 0;        /* COUNT OF NEG REDUCTIONS               */
int setlcnt   = 0;        /* COUNT OF SETL REDUCTIONS              */

int kcnt      = 0;             /* COUNT OF COMBINED K IMPORTS      */
int ckcnt     = 0;             /* COMBINED K IMPORTS DURING CSE    */
int gkcnt     = 0;             /* COMBINED K IMPORTS DURING GCSE   */
int ccnt      = 0;             /* COUNT OF COMBINED NODES          */
int vcnt      = 0;             /* COUNT OF INVARIANTS REMOVED      */
int gccnt     = 0;             /* COUNT OF GLOBALLY COMBINED NODES */
int norm_cnt  = 0;             /* COUNT OF SIMPLE FOLDS            */
int neg_cnt   = 0;             /* COUNT OF FOLDED NEGATIVE NODES   */
int ama_cnt   = 0;          /* COUNT OF FOLDED ADD-MAX-ADD CHAINS  */
int asize_cnt = 0;          /* COUNT OF FOLDED ASize-MAX CHAINS    */
int sccnt     = 0;          /* COUNT OF SELECT SUBGRAPH MOVEMENTS  */

int pycnt     = 0;          /* COUNT OF PUSHED YANKED RETURN NODES */

int scpcnt    = 0;          /* COUNT OF SAVED CALL PARAMETERS      */
int scpinvcnt = 0;          /* COUNT OF INVARIANT SAVED CALL PARAM */
int sspcnt    = 0;          /* COUNT OF SAVED SLICE PARAMETERS     */
int sspinvcnt = 0;          /* COUNT OF INVARIANT SAVED SLICE PARAM*/
int dicnt     = 0;          /* COUNT OF REMOVED COMPOUND IMPORTS   */
int tdicnt     = 0;          /* COUNT OF REMOVED COMPOUND IMPORTS   */
int leicnt    = 0;          /* COUNT OF REMOVED LOOP ENQUE IMPORTS */
int tleicnt    = 0;          /* COUNT OF REMOVED LOOP ENQUE IMPORTS */
int tagtcnt   = 0;          /* COUNT OF TAG TEST CONVERSIONS       */

/* $Log$
 * Revision 1.2  1994/06/16  21:31:53  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.1  1993/01/21  23:28:41  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
