/* if2cost.c,v
 * Revision 12.7  1992/11/04  22:05:08  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:35  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


double mcosts[100];                            /* MISC COST TABLE        */
double scosts[IF1SimpleNodes];                 /* SIMPLE NODE COST TABLE */
double atcosts[IF2AtNodes];                    /* AT-NODE COST TABLE     */


/**************************************************************************/
/* LOCAL  **************      SetArithmeticCost    ************************/
/**************************************************************************/
/* PURPOSE:								  */
/**************************************************************************/

static void SetArithmeticCost( n )
PNODE n;
{
   register int extra;

   switch ( n->exp->info->type ) {
     case IF_DOUBLE:
	extra = DoubleCost;
	break;

     case IF_REAL:
	extra = RealCost;
	break;

     default:
	extra = IntegerCost;
	break;
     }

   n->ccost += scosts[ n->type - IFAAddH ] + extra;
}

/**************************************************************************/
/* GLOBAL **************        ReadCostFile       ************************/
/**************************************************************************/
/* PURPOSE: INITIALIZE THE MISCILANIOUS, SIMPLE NODE, AND AT-NODE COST    */
/*          TABLES.  COSTS ARE READ FROM THE COST FILE fn. THE EXPECTED   */
/*          FORMAT IS: [name index cost]*, WHERE name IS THE NODES        */
/*          SYMBOLIC NAME, index IS THE NODES TYPE IDENTIFIER, AND cost   */
/*          IS THE ASSOCIATED COST. EIGHT SPECIAL ENTRIES ARE RECOGNIZED: */
/*                                                                        */
/*          index -> 0:  THE Huge THRESHOLD                               */
/*                   1:  THE ASSUMED NUMBER OF LOOP ITERATIONS            */
/*                   2:  COST OF AN INTRINSIC FUNCTION EXECUTION          */
/*                   3:  DEFAULT SISAL FUNCTION EXECUTION COST            */
/*                   4:  COST TO INCREMENT A REFERENCE COUNT              */
/*                   5:  COST TO DECREMENT A REFERENCE COUNT              */
/*                   6:  COST TO COPY A DOPE VECTOR OR CONSUMER BLOCK     */
/*                   7:  COST TO COPY AN ARRAY'S PYSICAL SPACE            */
/*                   8:  COST TO DEALLOCATE AN AGGREGATE                  */
/**************************************************************************/

void ReadCostFile( fn )
char *fn;
{
    FILE  *fd;
    char   buf[100];
    int    i;
    double c;

    if ( (fd = fopen( fn, "r" )) == NULL )
        Error2( "CAN'T OPEN", fn );

    while ( fscanf( fd, "%s %d %le", buf, &i, &c ) != EOF ) {
        if ( (i >= 0) && (i <= 11) )
            mcosts[ i ] = c;
        else if ( (i >= IFAAddH) && (i <= IFTrunc) )
            scosts[ i - IFAAddH ] = c;
        else if ( (i >= IFAAddLAT) && (i <= IFRedTreeAT) )
            atcosts[ i - IFAAddLAT ] = c;
        else
            Error2( "COST FILE FORMAT ERROR:", fn );
        }
}


/**************************************************************************/
/* LOCAL  **************       WriteCostTables     ************************/
/**************************************************************************/
/* PURPOSE: WRITE COST TABLES TO stderr.                                  */
/**************************************************************************/

static void WriteCostTables()
{
 /*   register int i;

      if ( RequestInfo(I_Info4,info)  ) {

      FPRINTF( infoptr, "\n **** COST TABLES\n" );

      for ( i = 0; i < IF1SimpleNodes; i++ ) {
        if ( (i % 7) == 0 )
	  FPRINTF( infoptr, "\n" );

        FPRINTF( infoptr, " %-5.5s %4d", SimpleName(i), (int) scosts[i] );
      }

      FPRINTF( infoptr, "\n" );

      for ( i = 0; i < IF2AtNodes; i++ ) {
        if ( (i % 7) == 0 )
	  FPRINTF( infoptr, "\n" );

        FPRINTF( infoptr, " %-6.6s %3d", AtName(i), (int) atcosts[i] );
      }
      FPRINTF( infoptr, "\n\n" );
    }
*/

    if ( RequestInfo(I_Info4,info)  ) {
      FPRINTF( infoptr, "\n **** COST TABLES\n\n" );
    FPRINTF( infoptr, " Slice Threshold:         %g\n", SliceThreshold);
    FPRINTF( infoptr, " Loop Iterations:         %d\n", (int) Iterations);

      FPRINTF( infoptr, " Default Function Cost:   %d\n",
	      (int) DefaultFunctionCost);
      FPRINTF( infoptr, " RefCnt Increment Cost:   %d\n",
	      (int) RefCntIncrementCost);
      FPRINTF( infoptr, " RefCnt Decrement Cost:   %d\n",
	      (int)RefCntDecrementCost);
      FPRINTF( infoptr, " Dope Vector Copy Cost:   %d\n",
	      (int) DopeVectorCopyCost);
      FPRINTF( infoptr, " Record Copy Cost:        %d\n",
	      (int) RecordCopyCost);
      FPRINTF( infoptr, " Array Copy Cost:         %d\n",
	      (int) ArrayCopyCost);
      FPRINTF( infoptr, " Deallocation Cost:       %d\n",
	      (int) DeallocCost);
      FPRINTF( infoptr, " Integer Arithmetic Cost: %d\n",
	      (int) IntegerCost);
      FPRINTF( infoptr, " Real Arithmetic Cost:    %d\n",
	      (int) RealCost);
      FPRINTF( infoptr, " Double Arithmetic Cost:  %d\n",
	      (int) DoubleCost);
    }
}


/**************************************************************************/
/* LOCAL  **************      MaxSubgraphCost      ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE MAXIMUM COST OF ALL SUBGRAPHS OF COMPOUND NODE n.  */
/*          THE COST OF A Select NODE TEST SUBGRAPH IS NOT CONSIDERED.    */
/**************************************************************************/

static double MaxSubgraphCost( n )
PNODE n;
{
    register PNODE  g;
    register double x;

    g = (IsSelect( n ))? n->C_SUBS->gsucc : n->C_SUBS;
    x = 0.0;

    for ( /* NOTHING */; g != NULL; g = g->gsucc )
        if ( g->ccost > x )
            x = g->ccost;

    return( x );
}


/**************************************************************************/
/* LOCAL  **************   CalculateRefCntCosts    ************************/
/**************************************************************************/
/* PURPOSE: CALCULATE THE REFERENCE COUNT COST ASSOCIATED WITH NODE n AND */
/*          ADD IT TO ITS EXECUTION COST.                                 */
/**************************************************************************/

static void CalculateRefCntCosts( n )
PNODE n;
{
    register PEDGE i;
    register PEDGE e;
    register PEDGE ee;

    for ( i = n->imp; i != NULL; i = i->isucc ) {
        if ( i->cm != 0 )
            n->ccost += RefCntDecrementCost;
        else if ( i->dmark )
            n->ccost += DeallocCost;
        }

    for ( e = n->exp; e != NULL; e = e->esucc ) {
        if ( e->eport < 0 ) {
            e->eport = -(e->eport);
            continue;
            }

        if ( e->pm != -2 ) {
            n->ccost += RefCntIncrementCost;

            for ( ee = e->esucc; ee != NULL; ee = ee->esucc )
                if ( ee->eport == e->eport )
                    ee->eport = -(ee->eport);
            }
        }
}


/**************************************************************************/
/* GLOBAL **************    NumberOfIterations     ************************/
/**************************************************************************/
/* PURPOSE: IF THE GENERATE SUBGRAPH OF Forall f CONTAINS A RangeGen NODE */
/*          WITH CONSTANT IMPORTS THEN RETURN Hi-Lo+1, ELSE THE ASSUMED   */
/*          ITERATION NUMBER.                                             */
/**************************************************************************/

double NumberOfIterations( f )
PNODE f;
{
    register PNODE n;

    n = f->F_GEN->imp->src; /* THE CONTROL */

    if ( IsRangeGenerate( n ) )
        if ( IsNonErrorConst( n->imp ) && IsNonErrorConst( n->imp->isucc ) )
            return( (double) (atoi( n->imp->isucc->CoNsT ) -
                              atoi( n->imp->CoNsT ) + 1  )   );

    return( Iterations );
}


/**************************************************************************/
/* LOCAL  **************       CalculateCost       ************************/
/**************************************************************************/
/* PURPOSE: CALCULATE THE EXECUTION COST OF GRAPH g.  IT IS ASSUMED THAT  */
/*          THE CALL GRAPHS ARE BEING PROCESSED FROM THE LEAVES UP; WHEN  */
/*          A CALL NODE IS ENCOUNTERED, THE ASSOCIATED GRAPH IS LOCATED   */
/*          AND ITS ALREADY CALCULATED COST (MIGHT BE THE DEFAULT COST IF */
/*          THE FUNCTION WAS BROKEN TO ELIMINATE A CYCLE) IS USED.        */
/**************************************************************************/

static void CalculateCost( g )
PNODE g;
{
  register PNODE n;
  register PNODE gg;
  char     *Reason;

  g->ccost = 0.0;

  CalculateRefCntCosts( g );

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( n->cmark )
      continue;

    if ( IsCompound( n ) )
      for ( gg = n->C_SUBS; gg != NULL; gg = gg->gsucc )
	CalculateCost( gg );

    CalculateRefCntCosts( n );

    switch ( n->type ) {
     case IFSelect:
     case IFTagCase:
      n->ccost += MaxSubgraphCost( n );
      break;

     case IFForall:
      /* ------------------------------------------------------------ */
      /* See if we have a preset idea of the vectorization	      */
      /* ------------------------------------------------------------ */
      if ( UpdatedLoopPragmas(n,R_Vector) ) {
	if ( !n->vmark ) {
	  if ( dovec ) n->reason1 = "Cleared by loop report";
	} else {
	  n->F_BODY->ccost /= vadjust;
	}
      } else {
	if ( dovec ) {
	  /* ------------------------------------------------------------ */
	  /* Vectorize if we can pass the convoluted test		  */
	  /* ------------------------------------------------------------ */
	  if ( PartIsVecCandidate(n,&Reason) ) {
	    n->vmark = TRUE;
	    n->F_BODY->ccost /= vadjust;
	  } else {
	    n->reason1 = Reason;
	    if ( RequestInfo(I_Info4,info)  ) {
	      if ( IsInnerLoop( n->F_BODY ) ) {
		FPRINTF( infoptr, "INNER LOOP VECTORIZATION FAILURE ");
		FPRINTF( infoptr, "%d=(%s,%s,%d)\n", 
			n->ID, n->file, n->funct, n->line );
		if ( Reason ) FPRINTF( infoptr, "%s\n",Reason);
	      }
	    }
	  }
	}
      }

      /* ------------------------------------------------------------ */
      /* See if we have overridden the cost.  If not, use the default */
      /* estimate.						      */
      /* ------------------------------------------------------------ */
      if ( !UpdatedLoopPragmas(n,R_Cost) ) {
	n->ccost += ((n->F_GEN->ccost + n->F_BODY->ccost +
		      n->F_RET->ccost ) * NumberOfIterations( n ));
      }
      break;

     case IFLoopA:
     case IFLoopB:

      n->ccost += ((n->L_INIT->ccost + n->L_TEST->ccost +
		    n->L_BODY->ccost + n->L_RET->ccost) *
		   Iterations);
      break;

     case IFCall:
      gg = FindFunction( n->imp->CoNsT );

      n->ccost += (gg->ccost + scosts[ IFCall - IFAAddH ]);
      break;

     case IFReduce:
     case IFRedTree:
     case IFRedRight:
     case IFRedLeft:
      if ( n->imp->CoNsT[0] == REDUCE_CATENATE ) {
	n->ccost += ArrayCopyCost;
	break;
      }

      SetArithmeticCost( n );
      /* n->ccost += scosts[ n->type - IFAAddH ]; */
      break;

     case IFReduceAT:
     case IFRedTreeAT:
     case IFRedLeftAT:
     case IFRedRightAT:
      if ( n->imp->isucc->isucc->pmark )
	n->ccost += atcosts[ n->type - IFAAddLAT ];
      else
	n->ccost += ArrayCopyCost;

      break;

     case IFACatenate:
      n->ccost += (2 * ArrayCopyCost) + 
	scosts[ IFACatenate - IFAAddH ];
      break;

     case IFAAddHAT:
     case IFAAddLAT:
      n->ccost += atcosts[ n->type - IFAAddLAT ];

      if ( !(n->imp->pmark) )
	n->ccost += ArrayCopyCost;

      break;

     case IFACatenateAT:
      if ( n->imp->pmark )
	n->ccost += atcosts[ n->type - IFAAddLAT ];
      else
	n->ccost += ArrayCopyCost;

      if ( n->imp->isucc->pmark )
	n->ccost += atcosts[ n->type - IFAAddLAT ];
      else
	n->ccost += ArrayCopyCost;

      break;

     case IFNoOp:
      switch ( n->imp->info->type ) {
       case IF_ARRAY:
	if ( n->imp->pmark ) {
	  if ( n->imp->rmark1 != RMARK )
	    n->ccost += DopeVectorCopyCost;

	  break;
	}

	if ( n->imp->rmark1 != RMARK )
	  n->ccost += (DopeVectorCopyCost + ArrayCopyCost);

	break;

       case IF_STREAM:
	if ( !(n->imp->rmark1 == RMARK) )
	  n->ccost += DopeVectorCopyCost;

	break;


       default:
	if ( !(n->imp->rmark1 == RMARK) )
	  n->ccost += RecordCopyCost;

	break;
      }

      break;

     case IFAbs:
     case IFDiv:
     case IFEqual:
     case IFLess:
     case IFLessEqual:
     case IFMax:
     case IFMin:
     case IFMinus:
     case IFNeg:
     case IFNotEqual:
     case IFPlus:
     case IFTimes:
     case IFAElement:
      SetArithmeticCost( n );
      break;

     default:
      if ( IsOther( n ) )
	n->ccost += 0.0;
      else if ( IsSimple( n ) )
	n->ccost += scosts[ n->type - IFAAddH ];
      else
	n->ccost += atcosts[ n->type - IFAAddLAT ];

      break;
    }

    g->ccost += n->ccost;
  }
}


/**************************************************************************/
/* GLOBAL **************          If2Cost          ************************/
/**************************************************************************/
/* PURPOSE: ESTIMATE EXECUTION COST FOR ALL FUNCTIONS WORKING FROM THE    */
/*          LEAVES UP (GUARANTEED IF THE INPUT FILE HAS HAD MINIMAL       */
/*          UPDATE-IN-PLACE ANALYSIS PERFORMED). IF info IS TRUE, THE     */
/*          COST TABLES ARE PRINTED TO stderr.                            */
/**************************************************************************/

void If2Cost()
{
    register PNODE f;

    if ( RequestInfo(I_Info4,info)  ) {
      WriteCostTables();
    }


    for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
        if ( !IsIGraph( f ) )
            CalculateCost( f );
}
