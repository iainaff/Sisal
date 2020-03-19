/* if2up.c,v
 * Revision 12.7  1992/11/04  22:05:12  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:10:04  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


PNODE nohead = NULL;            /* NoOp NODE LIST HEAD                    */
PNODE notail = NULL;            /* NoOp NODE LIST TAIL                    */
PSET  gshead = NULL;            /* GLOBAL READ/WRITE SET LIST HEAD        */
PSET  gstail = NULL;            /* GLOBAL READ/WRITE SET LIST TAIL        */
PNODE fhead  = NULL;            /* HEAD OF SORTED FUNCTION GRAPH LIST     */
PNODE ftail  = NULL;            /* TAIL OF SORTED FUNCTION GRAPH LIST     */
PEDGE dghead = NULL;            /* DATA GROUND EDGE LIST HEAD             */
PEDGE dgtail = NULL;            /* DATA GROUND EDGE LIST TAIL             */
PNODE cohead = NULL;            /* CONSTANT AGGREGATE GENERATOR LIST HEAD */
PNODE cotail = NULL;            /* CONSTANT AGGREGATE GENERATOR LIST TAIL */

int   univao = FALSE;           /* UNIVERSAL STREAM OWNERSHIP?            */
int   univso = FALSE;           /* UNIVERSAL ARRAY  OWNERSHIP?            */
int   cycle  = 0;               /* COUNT OF CYCLE CAUSING ADEs            */
int   cmig   = 0;               /* COUNT OF MIGRATED NODES                */


/**************************************************************************/
/* GLOBAL **************           If2Up           ************************/
/**************************************************************************/
/* PURPOSE: DRIVER FOR THE UPDATE-IN-PLACE ANALYSIS.  MINIMALLY A CALL    */
/*          GRAPH IS BUILT AND SORTED, NoOp NODES ARE INSERTED, AND       */ 
/*          UNOPTIMIZED REFERENCE COUNT VALUES ARE ASSIGNED. IF FULL      */
/*          OPTIMIZATION IS DESIRED, AGGREGATE EDGES ARE CLASSIFIED AS    */
/*          READ OR WRITE, Replace NODES ARE REORDERED TO ELIMINATE       */
/*          POSSIBLE PHYSICAL SPACE COPYING, READ/WRITE SETS ARE BUILT,   */
/*          ADES ARE INSERTED, CONSTANT AGGREGATE CONSTRUCTORS  ARE       */
/*          IDENTIFIED, REFERENCE COUNT VALUES ARE OPTIMIZED, MARKS       */
/*          ARE ASSIGNED, UNIVERSAL ARRAY AND STREAM OWNERSHIP IS         */
/*          DETERMINED, GROUND EDGES ARE REFINED, AND UNNECESSARY ADES    */
/*          ARE REMOVED. IF mig IS TRUE, MIGRATE NODES TOWARD THEIR       */
/*          DEPENDENTS.                                                   */
/**************************************************************************/

void If2Up( sdbx )
int sdbx;
{
    If2CallGraph();
    If2NoOp();
    If2ReferenceCount();

    if ( cagg )
      If2ConstGenerators();

/*    if ( RequestInfo(I_Info3,info)  ) {
      UpIf2Count( FALSE, " **** INITIAL ANALYSIS" );
    } */

    if ( minopt )
      return;

    If2Classify();
    If2Replace();
    If2ReadWriteSets();
    If2Ade();

    If2ReferenceCountOpt();
    If2PropagateMarks();

    If2Ownership();
    OptimizeSwaps();
    If2RefineGrounds();
    CleanNoOpImports();

    if ( mig )
      If2Migrate();

    if ( RequestInfo(I_Info2,info) ) 
      FPRINTF (infoptr2, "\n **** ARRAY PREBUILDS\n\n");

    if ( RequestInfo(I_Info2,info)  || RequestInfo(I_Info3,info)) 
      UpIf2Count( TRUE, " **** COPY ANALYSIS" );

    if ( !sdbx )
      WriteIf2upWarnings();
}

