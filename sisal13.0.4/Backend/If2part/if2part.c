/* if2part.c,v
 * Revision 12.7  1992/11/04  22:05:08  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:36  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

int   level      = 1;		/* NESTED LOOP PARALLIZATION THRESHOLD */
				/* We default to slicing only the outer */
				/* level loop.  This was previously set to */
				/* slice ALL loop levels, but was throttled */
				/* by Active processor paralellism. */
int atlevel      = -1;       /* ONLY SLICE AT THIS PARALLEL NESTING LEVEL */

static int totedges;            /* NUMBER OF COUNTED EDGES                  */
static int total;             /* TOTAL DISTANCE OF COUNTED EDGES          */




/**************************************************************************/
/* LOCAL  **************      AverageDistance      ************************/
/**************************************************************************/
/* PURPOSE: CALCULATE AND RETURN THE AVERAGE USAGE DISTANCE FOR ALL NODES */
/*          IN GRAPH g.                                                   */
/**************************************************************************/

static void AverageDistance( g )
PNODE g;
{
    register PNODE n;
    register PEDGE e;
    register int   i;

    for ( i = 1, n = g->G_NODES; n != NULL; n = n->nsucc, i++ )
        n->label = i;

    for ( n = g->G_NODES; n != NULL; n = n->nsucc )
        for ( e = n->exp; e != NULL; e = e->esucc )
            if ( !IsGraph( e->dst ) ) {
                totedges++;
                total += e->dst->label - n->label;
                }
}


/**************************************************************************/
/* LOCAL  **************       IsTaskFinished      ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF THE VALUES PRODUCED BY TASK n ARE AVAILABLE    */
/*          FOR USE, ELSE RETURN FALSE; THAT IS, HAS A SYNC BEEN DONE?    */
/**************************************************************************/

static int IsTaskFinished( n )
PNODE n;
{
    register PEDGE e;

    for ( e = n->exp; e != NULL; e = e->esucc )
        if ( e->dst->wmark )
            return( TRUE );

    return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************       SetSyncMarks        ************************/
/**************************************************************************/
/* PURPOSE: ENABLE SYNCRONIZATION MARKS ON NODES IN g'S NODE LIST THAT    */
/*          INPUT RESULTS OF STREAM CONSUMING, NON-STREAM PRODUCING LOOPS.*/
/*          ONLY THE FIRST NODE USING THE LOOP'S OUTPUTS DOES THE SYNC.   */
/**************************************************************************/

static void SetSyncMarks( g )
PNODE g;
{
    register PEDGE i;
    register PNODE n;

    /* BUG FIX 10/16 NOTE STILL DOESN't WORK */
    if ( g->G_NODES == NULL )
        return;

    for ( n = g->G_NODES; n != g; ) {
        for ( i = n->imp; i != NULL; i = i->isucc )  {
            if ( IsConst( i ) )
                continue;

            if ( i->src->pmark && !IsStream( i->src->exp->info ) )
                if ( !IsTaskFinished( i->src ) )
                    n->wmark = TRUE;
            }
        /* BUG FIX 10/16 */
        if ( n->nsucc == NULL )
            n = g;
        else
            n = n->nsucc;
        }
}


/**************************************************************************/
/* LOCAL  ****************   GetNewActiveProcCount   **********************/
/**************************************************************************/
/* PURPOSE:  Find out how many processors are in use			  */
/**************************************************************************/
static int GetNewActiveProcCount( f, active )
PNODE f;
int   active;
{
  register int i;

  i = (int) NumberOfIterations( f );

  /* ASSUMES LOOPS DIVIDED INTO proc CHUNKS */
  return(((i < procs)? i : procs) * active);
}

/**************************************************************************/
/* LOCAL  **************       PartitionGraph      ************************/
/**************************************************************************/
/* PURPOSE: MARK APPROPRIATE Forall, LoopA, AND LoopB NODES IN GRAPH g    */
/*          FOR PARALLEL EXECUTION (A FUNCTION OF PARALLEL NESTING LEVEL  */
/*          AND ESTIMATED EXECUTION COST). IF A Forall NODE CAN BE SLICED,*/
/*          ITS SUBGRAPHS ARE PROCESSED USING LEVEL plvl + 1.  WHEN A CALL*/
/*          NODE IS ENCOUNTERED, THE CALLEE IS FOUND AND plvl IS ADDED TO */
/*          ITS RUNNING SUM TO BE USED LATER TO CALCULATE THE AVERAGE     */
/*          PARALLEL NESTING LEVEL OF ALL CALLERS. HENCE, THIS ROUTINE    */
/*          ASSUMES THAT THE FUNCTION CALL GRAPH IS BEING TRAVERSED       */
/*          TOWARD THE LEAVES. WHEN DONE, SYNC OPERATIONS ARE INSERTED.   */
/**************************************************************************/

static void PartitionGraph( g, plvl, pbusy )
PNODE g;
int   plvl;
int   pbusy; /* BUSY PROCESSORS */
{
  register PNODE  	n;
  register PNODE	gg;
  register PNODE	phead = NULL;
  register PNODE	ptail = NULL;
  char			*reason;
  int			active;
  double		PerIterationCost;
  int			StaticMinSlice;
  char			StaticMinSliceBuffer[32];

  AverageDistance( g );

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    n->level = plvl;
    active = pbusy;
    reason = NULL;

    switch ( n->type ) {
    case IFForall:
      /* ------------------------------------------------------------ */
      /* Here, we only handle the sliceable loops, below we will      */
      /* handle parallel stream generators (by falling through).      */
      /* ------------------------------------------------------------ */

      /* ------------------------------------------------------------ */
      /* See if we have an override set in the report		      */
      /* ------------------------------------------------------------ */
      if ( UpdatedLoopPragmas(n, R_SMARK) ) {
	if ( n->smark ) {
	  goto ConcurrentLoop;
	} else {
	  n->reason2 = "Serial loop set by loop report";
	  goto SerialLoop;
	}
      }

      /* ------------------------------------------------------------ */
      /* See if the loop is too deep				      */
      /* ------------------------------------------------------------ */
      if ( plvl > level ) {
	n->reason2 = "Loop nested too deep (see -n option)";
	goto SerialLoop;
      }

      /* ------------------------------------------------------------ */
      /* Now see if we can slice it anyway			      */
      /* ------------------------------------------------------------ */
      if ( !IsSliceCandidate( n,&reason ) ) {
	n->reason2 = reason;
	goto SerialLoop;
      }
      /* ------------------------------------------------------------ */
      /* Make sure that there is enough work to do		      */
      /* ------------------------------------------------------------ */
      if ( (n->ccost) < SliceThreshold ) {
	n->reason2 = "Not enough work (see -h and -i options)";
	goto SerialLoop;
      }

      /* ------------------------------------------------------------ */
      /* If the inner slice option is on, see if this is the level    */
      /* ------------------------------------------------------------ */
      if ( (atlevel != -1) && (atlevel != plvl) ) {
	n->reason2 = "This loop level not selected (see -A option)";
	goto SerialLoop;
      }

      /* ------------------------------------------------------------ */
    ConcurrentLoop:
      /* ------------------------------------------------------------ */
      /* Set the parallelism mark				      */
      /* ------------------------------------------------------------ */
      n->smark = TRUE;

      /* ------------------------------------------------------------ */
      /* If this is a last parallelism level, okay to use fast        */
      /* scheduling mechanism at runtime.                             */
      /* ------------------------------------------------------------ */
      if( level == 1 )
		n->Fmark = TRUE;

      /* ------------------------------------------------------------ */
      /* Select the default parallelism style			      */
      /* ------------------------------------------------------------ */
      n->Style = DefaultStyle;

      /* ------------------------------------------------------------ */
      /* Find out what we think is the minimum number of iterations   */
      /* needed to break even on splitting			      */
      /* ------------------------------------------------------------ */
      PerIterationCost = n->ccost / NumberOfIterations(n);
      StaticMinSlice = (int)(ceil( SliceThreshold / PerIterationCost ));
      SPRINTF(StaticMinSliceBuffer,"%d",StaticMinSlice);
      n->MinSlice = CopyString(StaticMinSliceBuffer);

      /* ------------------------------------------------------------ */
      /* Some concurrency options may be overridden in the report     */
      /* ------------------------------------------------------------ */
      (void)UpdatedLoopPragmas(n, R_Concurrent);

      /* ------------------------------------------------------------ */
      /* Adjust the active processor count and add it to the list of  */
      /* parallel nodes						      */
      /* ------------------------------------------------------------ */
      active = GetNewActiveProcCount( n, active );
      AppendToUtilityList( phead, ptail, n );

      /* ------------------------------------------------------------ */
      /* Partition deeper nesting levels			      */
      /* ------------------------------------------------------------ */
      for ( gg = n->C_SUBS; gg != NULL; gg = gg->gsucc ) {
	PartitionGraph( gg, plvl + 1, active );
      }
      break;

      /* ------------------------------------------------------------ */
    SerialLoop:			/* Fall Through... */


    case IFLoopA:
    case IFLoopB:
      /* ------------------------------------------------------------ */
      /* OK, now we're looking for stream producers		      */
      /* ------------------------------------------------------------ */

      /* ------------------------------------------------------------ */
      /* See if the loop report overrides the computed choice	      */
      /* ------------------------------------------------------------ */
      if ( UpdatedLoopPragmas(n, R_PMARK) ) {
	if ( n->pmark ) {
	  goto StreamLoop;
	} else {
	  n->reason2 = "Loop report set this loop to non-stream producer";
	  goto NonStreamLoop;
	}	      
      }

      /* ------------------------------------------------------------ */
      /* See if we think this is a stream producer		      */
      /* ------------------------------------------------------------ */
      if ( !IsStreamTask( n ) ) goto NonStreamLoop;

      /* ------------------------------------------------------------ */
    StreamLoop:
      n->pmark = TRUE;
      n->reason2 = NULL;	/* clear the failure from above */
      AppendToUtilityList( phead, ptail, n );

    NonStreamLoop:
      /* Fall through */

    case IFSelect:
    case IFTagCase:
      for ( gg = n->C_SUBS; gg != NULL; gg = gg->gsucc ) {
	PartitionGraph( gg, plvl, active );
      }
      break;

    case IFCall:
      gg = FindFunction( n->imp->CoNsT );

      if ( !(IsIGraph( gg ) && gg->mark == 's') )
	if ( n->bmark )
	  break;

      if ( gg->cnum == 0 ) {
	gg->pbusy = active;
	gg->level  = plvl;
      } else {
	gg->pbusy += active;
	gg->level  += plvl;
      }

      gg->cnum++;
      break;

    default:
      break;
    }
  }

  if ( phead != NULL ) SetSyncMarks( g );
}


/**************************************************************************/
/* GLOBAL **************          If2Part          ************************/
/**************************************************************************/
/* PURPOSE: PARTITION ALL GRAPHS IN ALL FUNCTIONS.  THE CALL GRAPH IS     */
/*          TRAVERSED FROM THE ROOT DOWN; HENCE, UPON PROCESSING OF A     */
/*          GIVEN FUNCTION, WE ARE GUARANTEED THAT ALL CALL SITES HAVE    */
/*          VISITED, ALLOWING CALCULATION OF THE FUNCTIONS AVERAGE        */
/*          PARALLEL NESTING LEVEL (A PARTITIONING PARAMETER).            */
/**************************************************************************/

void If2Part()
{
    register PNODE f;

    if ( RequestInfo(I_Info4,info)) {
      FPRINTF( infoptr, "\n **** LOCALITY MAP\n\n" );
    }

    for ( f = FindLastGraph( glstop ); f != glstop; f = f->gpred ) {
        /* if ( IsIGraph( f ) ) */
            /* continue; */

        if ( f->bmark )
            cycle = TRUE;

        if ( f->cnum != 0 ) {
            f->level = f->level / f->cnum;
            f->pbusy = f->pbusy / f->cnum;
            }

        totedges = total = 0;

        PartitionGraph( f, f->level, f->pbusy );

	if ( RequestInfo(I_Info4,info) ) {
            FPRINTF( infoptr, " FUNCTION %s: ", f->G_NAME );

            if ( totedges == 0 )
                FPRINTF( infoptr, "Average Distance = 0.0\n" );
            else
                FPRINTF( infoptr, "Average Distance = %e\n",
                             ((double) total) / ((double) totedges) );
            }
        }

    if (  dovec && RequestInfo(I_Info4,info)) {
      VectorSummary();
    }
}
