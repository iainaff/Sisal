/* if2size.c,v
 * Revision 12.7  1992/11/04  22:05:06  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:22  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


static PEDGE linit;        /* FinalValue MONOTONIC EXPANSION INITAL VALUE */
static PEDGE lxpnd;        /* FinalValue MONOTONIC EXPANSION CONSTANT     */

int paddh = 0;                              /* COUNT OF AAddH PREFERENCES */


/**************************************************************************/
/* LOCAL  **************       PreferAAddH         ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF IFAAddH IS PREFERRED OVER IFAAddHAT.           */
/**************************************************************************/

static int PreferAAddH( n )
PNODE n;
{
  register PEDGE e;

  if ( IsConst( n->imp ) )
    return( FALSE );

  /* IMPORTANT TO CHECK n->imp FIRST SO NOT TO PREMATURELY ANSWER TRUE */
  switch ( n->imp->src->type ) {
    case IFABuild:
    case IFAAddH:
    case IFAAddL:
    case IFACatenate:
    case IFForall:
    case IFLoopA:
    case IFLoopB:
      return( FALSE );

    default:
      break;
    }

  for ( e = n->exp; e != NULL; e = e->esucc )
    switch ( e->dst->type ) {
      case IFAAddH:
      case IFAAddL:
        if ( e->iport != 1 ) /* NOT FIRST IMPORT? */
          return( TRUE );

	return( FALSE );

      case IFACatenate:
	return( FALSE );

      default:
	break;
      }

  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************      BindExportSizes      ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN SIZE INFORMATION (lsize, csize, AND esize) TO ALL      */
/*          EXPORTS OF NODE n.                                            */
/**************************************************************************/

static void BindExportSizes( n, lsize, csize, esize )
PNODE n;
int   lsize;
int   csize;
PNODE esize;
{
    register PEDGE e;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
        e->lsize = lsize;
        e->csize = csize;
        e->esize = esize;
        }
}


/**************************************************************************/
/* LOCAL  **************   PropagateImportSizes    ************************/
/**************************************************************************/
/* PURPOSE: PROPAGATE THE SIZE INFORMATION OF NODE n1's IMPORTS to TO THE */
/*          CORRESPONDING EXPORTS OF NODE n2. IF AN IMPORT IS EXPORTED    */
/*          FROM AN AGather NODE WITH A FILTER, THEN ITS SIZE INFORMATION */
/*          IS NOT PROPAGATED.  NOTE: ASize SIZE EXPRESSIONS ARE ONLY     */
/*          PROPAGATED TO LOOPS.                                          */
/**************************************************************************/

static void PropagateImportSizes( n1, n2 )
PNODE n1, n2 ;
{
    register PEDGE  i;
    register PEDGE  e;

    for ( i = n1->imp ; i != NULL ; i = i->isucc ) {
	if ( !IsConst( i ) )
	    if ( IsAGather( i->src ) )
		if ( i->src->imp->isucc->isucc != NULL )       /* FILTER? */
		    continue;

	if ( IsSizeUnknown( i ) )
	    continue;

	/* ONLY PROPAGATE ASize SIZE EXPRESSIONS TO LOOP SUBGRAPHS        */

	if ( !IsConstSize( i ) )
	    if ( IsASize( i->esize ) )
		if ( IsSGraph( n2 ) )
		    if ( !(IsLoop( n2->G_DAD ) || IsForall( n2->G_DAD )) )
			continue;
                    
        for ( e = n2->exp ; e != NULL ; e = e->esucc )
            if ( i->iport == e ->eport ) {
                e->lsize = i->lsize ;
                e->csize = i->csize ;
                e->esize = i->esize ;
                }
        }
}


/* NEW CANN 2/92 */
static int FirstIterationSkipped( l, n, mult )
PNODE l;
PNODE n;
PEDGE mult;
{
  register PEDGE i;
  register PEDGE ii;

  if ( IsConst( mult->isucc ) )
    return( FALSE );

  if ( !IsSGraph( mult->isucc->src ) )
    return( FALSE );

  /* THE BODY FILTER MUST BE A "TRUE" CONSTANT */
  if ( (i = FindImport( l->L_BODY, mult->isucc->eport )) == NULL )
    return( FALSE );

  if ( !IsConst( i ) || i->CoNsT == NULL )
    return( FALSE );

  switch ( i->CoNsT[0] ) {
    case 't':
    case 'T':
      break;

    default:
      return( FALSE );
    }

  /* THE INITIAL FILTER MUST BE A "FALSE" CONSTANT */
  if ( (ii = FindImport( l->L_INIT, mult->isucc->eport )) == NULL )
    return( FALSE );

  if ( !IsConst( ii ) || ii->CoNsT == NULL )
    return( FALSE );

  switch ( ii->CoNsT[0] ) {
    case 'f':
    case 'F':
      break;

    default:
      return( FALSE );
    }

  /* OK, THE IMPLICIT FIRST ITERATION IS SKIPPED!!! */
  n->umark = TRUE;

  /* GET RID OF THE FILTER!!! */
  UnlinkExport( mult->isucc );
  UnlinkImport( mult->isucc );

  /* FREE THE BODY CONSTANT? */
  if ( UsageCount( l->L_RET, i->iport ) == 0 ) {
    UnlinkImport( i );

    if ( !(IsExport( l->L_TEST, ii->iport ) || 
           IsExport( l->L_BODY, ii->iport )) ) 
      UnlinkImport( ii );
    }

  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************  IsGatherFinalOrReduceOk  ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF AGather, FinalValue, OR Reduce NODE n MEETS    */
/*          THE FOLLOWING CONDITIONS:                                     */
/*                                                                        */
/*             1. THE NUMBER OF LOOP ITERATIONS EXPRESSION IS KNOWN       */
/*             2. NODE n EXPORTS AN ARRAY                                 */
/*             3. NODE n IS NOT PART OF AN returns old STATEMENT          */
/*             4. THE MULTIPLE INPUTS TO NODE n ARE NOT FILTERED OR NODE  */
/*                n IS AN AGather AND fover IS ENABLED.                   */
/*                                                                        */
/*          ELSE FALSE IS RETURNED.                                       */
/**************************************************************************/

static int IsGatherFinalOrReduceOk( l, n, mult )
PNODE l;
PNODE n;
PEDGE mult;
{
    if ( IsSizeUnknown( l ) )
	return( FALSE );

    if ( !IsArray( n->exp->info ) )
	return( FALSE );

    if ( !IsConst( mult ) )
	if ( !IsSGraph( mult->src ) )
	    return( FALSE );

    if ( mult->isucc != NULL ) {
	/* NEW CANN 2/92 */
	if ( IsAGather( n ) && IsLoopB( l ) )
	  if ( FirstIterationSkipped( l, n, mult ) )
	    return( TRUE );
        /* END NEW CANN 2/92 */

	if ( !( IsAGather( n ) && fover ) )
	    return( FALSE );
        }

    return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************    HighMinusLowPlusOne    ************************/
/**************************************************************************/
/* PURPOSE: GENERATE THE IF1 SUBEXPRESSION max( hi - lo + 1, 0 ) GIVEN    */
/*          EDGE lo and EDGE hi.  THE SUBEXPRESSION IS PLACED IN THE      */
/*          NODE LIST CONTAINING NODE rpoint SO NOT TO VIOLATE DATA FLOW  */
/*          ORDERINGS.  THE max NODE IS RETURNED.                         */
/**************************************************************************/

static PNODE HighMinusLowPlusOne( rpoint, lo, hi )
PNODE rpoint;
PEDGE lo;
PEDGE hi;
{
    register PNODE pr;
    register PNODE n;
    register PEDGE e;

    lo = FindSource( lo );
    hi = FindSource( hi );
    pr = MaxSourceInDFO( rpoint, lo, hi );

                        /* HIGH MINUS LOW */

    n = NodeAlloc( ++maxint, IFMinus );
    n->level  = pr->level;
    n->lstack = pr->lstack;

    CopyEdgeAndThreadToUse( hi, n, 1 );
    CopyEdgeAndThreadToUse( lo, n, 2 );

    LinkNode( pr, n );
    pr = n;

                        /* (HIGH MINUS LOW) + 1 */

    n = NodeAlloc( ++maxint, IFPlus );
    n->level  = pr->level;
    n->lstack = pr->lstack;

    e = EdgeAlloc( NULL_NODE, CONST_PORT, n, 2 );
    e->info  = integer;
    e->CoNsT = "1";

    LinkImport( n, e );

    ThreadToUse( pr, 1, n, 1, integer );

    LinkNode( pr, n );
    pr = n;

                        /* MAX ( (HIGH MINUS LOW) + 1, 0 ) */

    n = NodeAlloc( ++maxint, IFMax );
    n->level  = pr->level;
    n->lstack = pr->lstack;

    e = EdgeAlloc( NULL_NODE, CONST_PORT, n, 2 );
    e->info  = integer;
    e->CoNsT = "0";

    LinkImport( n, e );

    ThreadToUse( pr, 1, n, 1, integer );

    LinkNode( pr, n );

    return( n );
}


#define LessEqual    0
#define Less         1
#define GreatEqual   2
#define Great        3


/**************************************************************************/
/* LOCAL  **************     BuildLoopIterExpr      ***********************/
/**************************************************************************/
/* PURPOSE: BUILD THE ITERATION EXPRESSION FOR LoopA OR LoopB NODE n.     */
/*          ONLY THE FOLLOWING LOOP FORMS, RESULT IN EXPRESSION           */
/*          CONSTRUCTION:                                                 */
/*                                                                        */
/*           ->  for initial                                              */
/*                   x := ive, WHERE ive is a CoNsTant OR K-PORT VALUE    */
/*                                                                        */
/*           ->  while ( x binop fve ) repeat ...                         */
/*               repeat ... until ( x binop fve )                         */
/*                   WHERE fve is a CoNsTant OR K-PORT VALUE AND binop IS */
/*                   EITHER <, <=, >, >=                                  */
/*                                                                        */
/*           ->  ... <- x := old x op ide, WHERE op IS EITHER + or - AND  */
/*                   ide IS A POSITIVE CONSTANT                           */
/*                                                                        */
/*          FURTHER, THE LOOP MUST BE WELL BEHAVED:                       */
/*                                                                        */
/*           ->  IF op IS + THEN binop MUST BE EITHER < OR <= FOR LoopB   */
/*               LOOPS and > OR >= FOR LoopA LOOPS                        */
/*                                                                        */
/*           ->  IF op IS - THEN binop MUST BE EITHER >= OR > FOR LoopB   */
/*               LOOPS and < OR <= FOR LoopA LOOPS                        */
/*                                                                        */
/*          THE RESULTING EXPRESSION TAKES THE FOLLOWING FORM; NOTE THAT  */
/*          REGAURDLESS OF THE LOOP TYPE, TEST SUBGRAPHS APPEAR AS WHILE  */
/*          NOT DONE TESTS (ITS ONLY ITS FINAL POSITION WITHIN THE LOOP   */
/*          THAT VARIES):                                                 */
/*                                                                        */
/*           -> FOR THE < CASE: ((((fve - 1) - ive) + ide) / ide)         */
/*           -> FOR THE > CASE: ((((fve + 1) - ive) + ide) / ide)         */
/*           -> ELSE:           ((((fve)     - ive) + ide) / ide)         */
/*                                                                        */
/*          THEN THE SIZE EXPRESSION IS IMPORTED TO A MAX NODE:           */
/*                                                                        */
/*           -> FOR LoopA: max( expr, 1 )                                 */
/*           -> FOR LoopB: max( expr, 0 )                                 */
/*                                                                        */
/*          NOTE: x > fve appears as NOT( x <= fve ) IN IF1 AND x >= fve  */
/*          APPEATS AS NOT( x < fve ) IN IF1.                             */
/**************************************************************************/

static void BuildLoopIterExpr( l )
PNODE l;
{
    register PNODE  n;
    register PNODE  adj;
    register PNODE  pr;
    register PEDGE  lvalue;
    register PNODE  upd;
    register PEDGE  fve;
    register PEDGE  ive;
    register PEDGE  c;
    register int    bop;
    register int    cnt;
    register char  *ide;
	     char   buf[100];

    /* CHECK LOOP TEST STRUCTURE, INITIALIZING fve and bop */

    for ( n = l->L_TEST->G_NODES, cnt = 0; n != NULL; n = n->nsucc )
	cnt++;

    switch ( cnt ) {
	case 1:
	    n = l->L_TEST->G_NODES;

	    switch ( n->type ) {
		case IFLessEqual: bop = LessEqual; break;
		case IFLess:      bop = Less;      break;

		default:
		    return;
                }

	    break;

	case 2:
	    n = l->L_TEST->G_NODES;

	    if ( !IsNot( n->nsucc ) )
		return;

	    switch ( n->type ) {
		case IFLess:      bop = GreatEqual; break;
		case IFLessEqual: bop = Great;      break;

		default:
		    return;
                }

	    break;

	default:
	    return;
        }

    if ( IsConst( n->imp ) )
	return;

    if ( !IsSGraph( n->imp->src ) )
	return;

    lvalue = n->imp;
    fve    = lvalue->isucc;

    if ( !IsConst( fve ) )
	if ( (fve = FindImport( l, fve->eport )) == NULL )
	    return;

    /* INITIALIZE ive, CHECKING ITS STRUCTURE */

    if ( (ive = FindImport( l->L_INIT, lvalue->eport )) == NULL )
	return;

    if ( !IsConst( ive ) ) {
	if ( !IsSGraph( ive->src ) )
	    return;
      
	ive = FindImport( l, ive->eport );
	}

    /* INITIALIZE upd (THE ive UPDATE), CHECKING ITS STRUCTURE */

    if ( (lvalue = FindImport( l->L_BODY, lvalue->eport )) == NULL )
	return;
	
    if ( IsConst( lvalue ) )
	return;

    upd = lvalue->src;

    switch ( upd->type ) {
	case IFPlus:
	case IFMinus:
	    break;

	default:
	    return;
        }

    if ( IsConst( upd->imp ) )
	return;

    if ( !IsSGraph( upd->imp->src ) )
	return;

    if ( upd->imp->eport != lvalue->iport )
	return;

    if ( !IsConst( upd->imp->isucc ) )
	return;

    /* CANN: May 5th */
    if ( upd->imp->isucc->CoNsT == NULL )
       return;

    /* INITIALIZE ide, CHECKING IF THE LOOP IS WELL BEHAVED */

    /* BE CAREFUL NOT TO DIVIDE BY ZERO IN BuildLoopIterExpr */
    if ( atoi( upd->imp->isucc->CoNsT ) == 0 )
	return;

    if ( upd->imp->isucc->CoNsT[0] == '-' )
	return;

    if ( IsPlus( upd ) ) {
	if ( (bop != Less) && (bop != LessEqual) )
	    return;

        ide = upd->imp->isucc->CoNsT;
    } else {
	if ( (bop != Great) && (bop != GreatEqual) )
	    return;

	SPRINTF( buf, "-%s", upd->imp->isucc->CoNsT );
	ide = CopyString( buf );
        }

            /* CONSTRUCT THE NUMBER OF ITERATIONS EXPRESSION */

    fve = FindSource( fve );
    ive = FindSource( ive );
    pr  = MaxSourceInDFO( l, ive, fve );

            /* ((fve MINUS 1) MINUS ive), ((fve PLUS 1) MINUS ive), OR */
	    /* (fve MINUS ive)                                         */

    n = NodeAlloc( ++maxint, IFMinus );
    n->level  = pr->level;
    n->lstack = pr->lstack;

    CopyEdgeAndThreadToUse( ive, n, 2 );

    switch ( bop ) {
	case Great:
	case Less:
	    if ( bop == Great )
	        adj = NodeAlloc( ++maxint, IFPlus  );
            else
	        adj = NodeAlloc( ++maxint, IFMinus );

	    adj->level  = pr->level;
	    adj->lstack = pr->lstack;

	    c = EdgeAlloc( NULL_NODE, CONST_PORT, adj, 2 );
	    c->info  = integer;
	    c->CoNsT = "1";

	    LinkImport( adj, c );
	    CopyEdgeAndThreadToUse( fve, adj, 1 );

	    ThreadToUse( adj, 1, n, 1, integer );

	    LinkNode( pr, adj );
	    pr = adj;
	    break;

	default:
	    CopyEdgeAndThreadToUse( fve, n, 1 );
	    break;
        }

    LinkNode( pr, n );
    pr = n;

            /* (PREVIOUS) PLUS ide */

    n = NodeAlloc( ++maxint, IFPlus );
    n->level  = pr->level;
    n->lstack = pr->lstack;

    ThreadToUse( pr, 1, n, 1, integer );

    c = EdgeAlloc( NULL_NODE, CONST_PORT, n, 2 );
    c->info  = integer;
    c->CoNsT = ide;

    LinkImport( n, c );

    LinkNode( pr, n );
    pr = n;

            /* (PREVIOUS) DIV ide */

    n = NodeAlloc( ++maxint, IFDiv );
    n->level  = pr->level;
    n->lstack = pr->lstack;

    ThreadToUse( pr, 1, n, 1, integer );

    c = EdgeAlloc( NULL_NODE, CONST_PORT, n, 2 );
    c->info  = integer;
    c->CoNsT = ide;

    LinkImport( n, c );

    LinkNode( pr, n );
    pr = n;

            /* MAX( (PREVIOUS), 1 ) OR MAX( (PREVIOUS), 0 ) */

    n = NodeAlloc( ++maxint, IFMax );
    n->level  = pr->level;
    n->lstack = pr->lstack;

    ThreadToUse( pr, 1, n, 1, integer );

    c = EdgeAlloc( NULL_NODE, CONST_PORT, n, 2 );
    c->info  = integer;

    if ( IsLoopA ( l ) )
	c->CoNsT = "1";
    else
        c->CoNsT = "0";

    LinkImport( n, c );

    LinkNode( pr, n );


    l->lsize = n->level;
    l->esize = n;
}


/**************************************************************************/
/* LOCAL  **************    BuildForallIterExpr     ***********************/
/**************************************************************************/
/* PURPOSE: BUILD THE ITERATION EXPRESSION FOR FORALL NODE n.  IF n IS    */
/*          CONTROLLED BY A STREAM SCATTER NODE THEN THE EXPRESSION ISN'T */
/*          BUILT. FURTHER AN EXPRESSION IS NOT BUILT IF AN ARRAY SCATTER */
/*          IMPORT SIZE IS NOT KNOWN.  NOTE THE RESULTING EXPRESSION      */
/*          MIGHT BE A CONSTANT:  for i in array [1: 1 2 3].              */
/**************************************************************************/

static void BuildForallIterExpr( n )
PNODE n;
{
    register PNODE c ;
    register PEDGE lo, hi ;

    /* FIND THE FORALL GENERATE NODE CONTROLLING THE LOOP */
    c = n->F_GEN->imp->src;

    if ( IsAScatter( c ) ) {
        if ((!IsArray(c->imp->info)) || (IsSizeUnknown(c->imp)))
            return;

        n->lsize = c->imp->lsize;
        n->csize = c->imp->csize;
        n->esize = c->imp->esize;

        return;
        }

    /* c MUST BE A IFRangeGenerate NODE */

    /* EACH IMPORT TO c MUST BE A CONSTANT OR A K-PORT VALUE */

    lo = c->imp;
    hi = lo->isucc;

    if ( !IsConst( lo ) ) {
        if( !IsSGraph( lo->src ) )
            return;
        else
	    lo = FindImport( n, lo->eport );
        }

    if ( !IsConst( hi ) ) {
        if( !IsSGraph( hi->src ) )
            return;
        else
	    hi = FindImport( n, hi->eport );
        }

    n->esize = HighMinusLowPlusOne( n, lo, hi ) ;
    n->lsize = n->esize->level ;
}


/**************************************************************************/
/* LOCAL  **************   IsMonotonicConstruction  ***********************/
/**************************************************************************/
/* PURPOSE: CHECK AND RETURN TRUE OR FALSE BASED ON WHETHER FinalValue    */
/*          NODE (n) IS A CANDIDATE FOR PREALLOCATION OR NOT. IF TRUE,    */
/*          GLOBAL VARIABLES lxpnd, AND linit ARE INITIALIZED.  Loop IS   */
/*          THE LOOP NODE OWNING n (A LoopA OR LoopB NODE). THE FOLLOWING */
/*          CONDITIONS MUST HOLD FOR n TO BE A CANDIDATE:                 */
/*                                                                        */
/*          1. n IS NOT PART OF A returns value of "string" STATEMENT     */
/*          2. THE INITIAL ARRAY SEGMENT IS GENERATED BY AN ABuild NODE   */
/*          3. THE MONOTONIC CONSTRUCTION MATCHES ONE OF THE FOLLOWING:   */
/*                                                                        */
/*                 a. x := array_addh( old x, ... );                      */
/*                 b. x := array_addl( old x, ... );                      */
/*                 c. x := old x || (yyy);                                */
/*                 d. x := (yyy) || old x;                                */
/*                                                                        */
/*                 WHERE yyy HAS A LOOP INVARIANT SIZE EXPRESSION.        */
/*                                                                        */
/*          THIS ROUTINE ASSUMES THAT n PASSED ANALYSIS DONE IN FUNCTION  */
/*          IsGatherFinalOrReduceOk.   NOTE: THE BODY NODE CAN ONLY       */
/*          REDEFINE ONE L VALUE (x) AND x'S ONLY REFERENCE IN THE RETURN */
/*          SUBGRAPH MUST BE BY FinalValue NODE n.                        */
/**************************************************************************/

static int IsMonotonicConstruction( l, n )
PNODE l;
PNODE n;
{
    register PEDGE lb;
    register PEDGE i1;
    register PEDGE i2;
    register PEDGE lsrc;
    register int   cnt;

    if ( IsConst( n->imp ) ) 
        return( FALSE );
    
    /* NODE n'S IMPORT CANNOT BE REFERENCED BY ANY OTHER MULTIPLE NODES   */
    if ( UsageCount( n->imp->src, n->imp->eport ) != 1 )
	return( FALSE );

    if ( (linit = FindImport( l->L_INIT, n->imp->eport )) == NULL )
        return( FALSE );

    if ( IsSizeUnknown( linit ) )                    /* A PROPAGATED SIZE */
       return( FALSE );

    /* HENCE, linit CAN'T BE A STRING AND MUST BE A K-PORT VALUE, ALSO    */
    /* NOT A STRING, THAT IS A ABuild EXPORT.                             */

    linit = FindImport( l, linit->eport );
    lsrc  = FindSource( linit );

    if ( !IsABuild( lsrc->src ) )
	return( FALSE );

    if ( (lb = FindImport( l->L_BODY, n->imp->eport )) == NULL )
	return( FALSE );

    if ( IsConst( lb ) )
       return( FALSE );

    /* THE BODY NODE CAN ONLY REDEFINE ONE L VALUE.                       */
    for ( cnt = 0, i1 = l->L_BODY->imp; i1 != NULL; i1 = i1->isucc ) {
	if ( IsConst( i1 ) )
	    continue;

        if ( i1->src == lb->src )
	    cnt++;
        }

    if ( cnt != 1 )
	return( FALSE );


    switch ( lb->src->type ) {
        case IFAAddH:
        case IFAAddL:
	    i1 = lb->src->imp;

            if ( IsConst( i1 ) )
                return( FALSE );

            if ( !IsSGraph( i1->src ) )
                return( FALSE );

            if ( i1->eport != lb->iport )
                return( FALSE );

            lxpnd = NULL;
            break;
        
        case IFACatenate:
	    i1 = lb->src->imp;
	    i2 = i1->isucc;

            /* WE DON't HANDLE CATENATE NODES WITH >= 3 IMPORTS           */
            if ( i2->isucc != NULL )
                return( FALSE );

	    if ( IsSizeUnknown( i1 ) || IsSizeUnknown( i2 ) )
		return( FALSE );

	    /* ONE OF THE IMPORTS MUST BE A REFERENCE TO old x            */
	    if ( IsSGraph( i1->src ) ) {
		if ( i1->eport == lb->iport )
		    lxpnd = i2;
                else if ( IsSGraph( i2->src ) && (i2->eport == lb->iport) )
		    lxpnd = i1;
                else
		    return( FALSE );
                }
            else if ( IsSGraph( i2->src ) && ( i2->eport == lb->iport ) )
		lxpnd = i1;
            else
		return( FALSE );

	    /* IS THE EXPANSION ARRAY SIZE INVARIANT?                     */
	    if ( !IsConstSize( lxpnd ) )
		if ( lxpnd->esize->level >= n->level )
		    return( FALSE );

	    break;

        default:
            return( FALSE );
        }

    return(TRUE);
}


/**************************************************************************/
/* LOCAL  **************     BuildAddLHSize        ************************/
/**************************************************************************/
/* PURPOSE: CALCULATE THE SIZE EXPRESSION FOR AAdd[H/L] NODE n AND ASSIGN */
/*          THE EXPRESSION TO ALL EXPORTS OF n. THE SIZE EXPRESSION IS    */
/*          ONE LARGER THAN THE SIZE OF THE FIRST IMPORT.  THIS ROUTINE   */
/*          ASSUMES THAT THE FIRST IMPORT SIZE IS KNOWN AND n EXPORTS AN  */
/*          ARRAY.                                                        */
/**************************************************************************/

static void BuildAddLHSize( n )
PNODE n;
{
    register PNODE plus;
    register PEDGE one;
    register PNODE pr;

    if ( IsConstSize( n->imp ) ) {
	BindExportSizes( n, n->imp->lsize, n->imp->csize + 1, NULL_NODE );
	return;
	}

    pr = n->imp->esize;

    if ( pr->level < n->level )
	pr = ReferencePoint( n, pr->level );

    plus = NodeAlloc( ++maxint, IFPlus );
    plus->level  = pr->level;
    plus->lstack = pr->lstack;
     
    one = EdgeAlloc( NULL_NODE, CONST_PORT, plus, 2 );
    one->CoNsT = "1";
    one->info  = integer;

    LinkImport( plus, one );
    
    ThreadToUse( n->imp->esize, 1, plus, 1, integer );

    LinkNode( pr, plus );
     
    BindExportSizes( n, plus->level, UNKNOWN_CSIZE, plus );
}


/**************************************************************************/
/* LOCAL  **************      BuildACatSize        ************************/
/**************************************************************************/
/* PURPOSE: CALCULATE THE SIZE EXPRESSION FOR ACatenate NODE n AND ASSIGN */
/*          THE EXPRESSION TO ALL EXPORTS OF n. THIS ROUTINE ASSUMES n    */
/*          HAS ONLY 2 IMPORTS, EXPORTS AN ARRAY, AND BOTH IMPORT SIZES   */
/*          ARE KNOWN.                                                    */
/**************************************************************************/

static void BuildACatSize( n )
PNODE n ;
{
    register PEDGE i1;
    register PEDGE i2;
    register PNODE plus;
    register PEDGE CoNsT;
    register PNODE pr;
    register char *cptr;
    register PNODE non_CoNsT;

    i1 = n->imp ;
    i2 = n->imp->isucc ;

    if ( IsConstSize( i1 ) && IsConstSize( i2 ) ) {
        BindExportSizes( n, Max( i1->lsize, i2->lsize ),
			 i1->csize + i2->csize, NULL_NODE  );

        return;
        }

    if ( (!IsConstSize( i1 )) && (!IsConstSize( i2 )) ) {
        plus = NodeAlloc( ++maxint, IFPlus ) ;
        pr   = MaxNodeInDFO( n, i1->esize, i2->esize ) ;

        plus->level  = pr->level ;
        plus->lstack = pr->lstack ;

        ThreadToUse( i1->esize, 1, plus, 1, integer ) ;
        ThreadToUse( i2->esize, 1, plus, 2, integer ) ;

        LinkNode( pr, plus ) ;

        BindExportSizes( n, plus->level, UNKNOWN_CSIZE, plus ) ;

        return;
        }

   /* EXACTLY ONE ARRAY IMPORT MUST HAVE CONSTANT SIZE                    */

   if (IsConstSize(i1)) {
       cptr = IntToAscii( i1->csize );
       non_CoNsT = i2->esize;
   } else {
       cptr = IntToAscii( i2->csize );
       non_CoNsT = i1->esize;
   }

   pr = non_CoNsT;

   if ( pr->level < n->level )
      pr = ReferencePoint( n, pr->level );

   plus = NodeAlloc( ++maxint, IFPlus );
   plus->level  = pr->level;
   plus->lstack = pr->lstack;

   CoNsT = EdgeAlloc( NULL_NODE, CONST_PORT, plus, 2 );
   CoNsT->info  = integer;
   CoNsT->CoNsT = cptr;
   LinkImport( plus, CoNsT );

   ThreadToUse( non_CoNsT, 1, plus, 1, integer );

   LinkNode( pr, plus );

   BindExportSizes( n, plus->level, UNKNOWN_CSIZE, plus ) ;

   return;
}


/**************************************************************************/
/* LOCAL  **************      BuildABuildSize      ************************/
/**************************************************************************/
/* PURPOSE: CALCULATE THE SIZE EXPRESSION FOR ABuild NODE n AND ASSIGN    */
/*          THE EXPRESSION TO ALL EXPORTS OF n. THIS ROUTINE ASSUMES THAT */
/*          THE EXPORT IS AN ARRAY.                                       */
/**************************************************************************/

static void BuildABuildSize( n )
PNODE n;
{
    register PEDGE i;
    register int   numb;

    /* COUNT NUMBER OF INPUTS TO n IGNORING THE THE LOWER BOUND.         */

    for ( numb = -1, i = n->imp; i != NULL; i = i->isucc )
        numb++;
     
    BindExportSizes( n, n->level, numb, NULL_NODE );
}


/**************************************************************************/
/* LOCAL  **************     BuildAGatherSize      ************************/
/**************************************************************************/
/* PURPOSE: CALCULATE THE SIZE EXPRESSION FOR AGather NODE n AND ASSIGN   */
/*          THE EXPRESSION TO ALL EXPORTS OF n. THIS ROUTINE ASSUMES THAT */
/*          n PASSED ANALYSIS DONE IN FUNCTION  IsGatherFinalOrReduceOk.  */
/*          THE SIZE OF THE GATERED ARRAY IS EQUAL TO THE NUMBER OF LOOP  */
/*          ITERATIONS. NOTE l CAN BE A LoopA, LoopB, OR FORALL NODE.     */
/**************************************************************************/

static void BuildAGatherSize( l, n )
PNODE l ;
PNODE n ;
{
    register PNODE plus;
    register PEDGE one;
    register PNODE pr;

    if ( IsForall( l ) ) {
        BindExportSizes( n, l->lsize, l->csize, l->esize );
	return;
        }

    /* NODE l IS A LoopA OR LoopB NODE, WHOSE ITERATION EXPRESSION IS NOT */
    /* A CONSTANT.                                                        */

    pr = l->esize;

    if ( pr->level < l->level )
	pr = ReferencePoint( l, pr->level );

    plus = NodeAlloc( ++maxint, IFPlus );
    plus->level  = pr->level;
    plus->lstack = pr->lstack;

    one = EdgeAlloc( NULL_NODE, CONST_PORT, plus, 2 ) ;

    /* NEW CANN 2/92 */
    if ( n->umark )
      one->CoNsT = "0" ;  /* NO IMPLICIT FIRST ITERATION */
    else
      one->CoNsT = "1" ;
    /* END NEW CANN 2/92 */

    one->info = integer ;
    LinkImport( plus, one ) ;

    ThreadToUse( l->esize, 1, plus, 1, integer ) ;

    LinkNode( pr, plus ) ;

    BindExportSizes( n, plus->level, UNKNOWN_CSIZE, plus ) ;
}


/**************************************************************************/
/* LOCAL  **************   BuildAFinalValueSize    ************************/
/**************************************************************************/
/* PURPOSE: CALCULATE THE SIZE EXPRESSION FOR FinalValue NODE n AND       */
/*          THE EXPRESSION TO ALL EXPORTS OF n.  THIS ROUTINE ASSUMES     */
/*          THAT n PASSED ANALYSIS DONE IN IsGatherFinalOrReduceOk AND    */
/*          IsMonotonicConstruction, AND l, THE LOOP OWNING n, IS NOT A   */
/*          FORALL NODE. Linit and lxpnd  WERE INITIALIZED IN FUNCTION    */ 
/*          IsMonotonicConstruction. NOTE, IT IS ASSUMED THAT linit IS    */
/*          A CONSTANT SIZE (EXPORT OF AN ABuild NODE). THE CALCULATED    */
/*          EXPRESSION IS:                                                */
/*                                                                        */
/*              ((# iterations) * expansion size) + initial size,         */
/*                                                                        */
/*          WHERE lxpnd  DEFINES THE EXPANSION SIZE AND linit->csize      */
/*          DEFINES THE CONSTANT INITIAL SIZE. IF lxpnd IS NULL, THE      */
/*          EXPANSION SIZE IS 1. IT IS ASSUMED THAT THE NUMBER OF         */
/*          ITERATIONS IS DEFINED BY AN IF1 SUBGRAPH, WHOSE ROOT NODE IS  */
/*          IN THE NODE LIST CONTAINING LOOP l.                           */
/**************************************************************************/

static void BuildFinalValueSize( l, n )
PNODE l, n ;
{
    register PNODE pr;
    register PNODE times, plus ;
    register PEDGE ce ;

    if ( lxpnd == NULL ) {
        pr = l->esize;

        if ( pr->level < l->level )
	    pr = ReferencePoint( l, pr->level );
	}
    else if ( IsConstSize( lxpnd ) ) {
        pr = l->esize;

        if ( pr->level < l->level )
	    pr = ReferencePoint( l, pr->level );
	}
    else
	pr = MaxNodeInDFO( l, lxpnd->esize, l->esize );

    times = NodeAlloc( ++maxint, IFTimes ) ;
    times->level  = pr->level ;
    times->lstack = pr->lstack ;

    ThreadToUse( l->esize, 1, times, 1, integer ) ;

    if ( lxpnd == NULL ) {
        ce = EdgeAlloc( NULL_NODE, CONST_PORT, times, 2 ) ;
        ce->info  = integer ;
        ce->CoNsT = "1";
        LinkImport( times, ce );
	}
    else if ( IsConstSize( lxpnd ) ) {
        ce = EdgeAlloc( NULL_NODE, CONST_PORT, times, 2 ) ;
        ce->info  = integer ;
        ce->CoNsT = IntToAscii( lxpnd->csize );
        LinkImport( times, ce );
        }
    else
	ThreadToUse( lxpnd->esize, 1, times, 2, integer );

    LinkNode( pr, times );

    plus = NodeAlloc( ++maxint, IFPlus ) ;
    plus->level  = times->level ;
    plus->lstack = times->lstack ;

    ce = EdgeAlloc( NULL_NODE, CONST_PORT, plus, 2 ) ;
    ce->info  = integer ;
    ce->CoNsT = IntToAscii( linit->csize ) ;
    LinkImport( plus, ce ) ;

    ThreadToUse( times, 1, plus, 1, integer ) ;

    LinkNode( times, plus );

    BindExportSizes( n, plus->level, UNKNOWN_CSIZE, plus );
}


/**************************************************************************/
/* LOCAL  **************      ExprTimesConst       ************************/
/**************************************************************************/
/* PURPOSE: GENERATE THE IF1 SUBEXPRESSION expr * CoNsT, WHERE expr IS AN */
/*          IF1 SUBEXPRESSION and CoNsT IS AN INTEGER CONSTANT. THE times */
/*          NODE IS RETURNED.                                             */
/**************************************************************************/

static PNODE ExprTimesConst( rpoint, expr, CoNsT )
PNODE rpoint;
PNODE expr;
int   CoNsT;
{
    register PNODE times;
    register PEDGE ce;
    register PNODE pr;

    pr = expr;

    if ( pr->level < rpoint->level )
	pr = ReferencePoint( rpoint, pr->level );

    times = NodeAlloc( ++maxint, IFTimes );
    times->level  = pr->level;
    times->lstack = pr->lstack;

    ThreadToUse( expr, 1, times, 1, integer );

    ce = EdgeAlloc( NULL_NODE, CONST_PORT, times, 2 );
    ce->info  = integer;
    ce->CoNsT = IntToAscii( CoNsT );
    LinkImport( times, ce );

    LinkNode( pr, times );

    return(times);
}


/**************************************************************************/
/* LOCAL  **************     BuildRedCatSize       ************************/
/**************************************************************************/
/* PURPOSE: CALCULATE THE SIZE EXPRESSION FOR Reduce-Catenate NODE n AND  */
/*          ASSIGN THE EXPRESSION TO THE EXPORTS OF n. LOOP l OWNS n AND  */
/*          mult IS THE MULTIPLE VALUE INPUT TO n. THIS ROUTINE ASSUMES   */
/*          THAT n PASSED ANALYSIS DONE IN IsGatherFinalOrReduceOk, mult, */
/*          WHOSE SIZE IS KNOWN OUTSIDE l, IS NOT A STRING, AND l IS A    */ 
/*          FORALL NODE.                                                  */
/**************************************************************************/

static void BuildRedCatSize( l, mult, n )
PNODE l;
PEDGE mult;
PNODE n;
{
    register PNODE times;
    register PNODE pr;

    if ( IsConstSize( mult ) ) {
        if ( IsConstSize( l ) ) {
            BindExportSizes( n, Max( l->lsize, mult->lsize ), 
                             l->csize * mult->csize, NULL_NODE  );
            return;
            }

        times = ExprTimesConst( l, l->esize, mult->csize );
    } else {
        if ( IsConstSize( l ) )
            times = ExprTimesConst( l, mult->esize, l->csize );
        else {
            pr  = MaxNodeInDFO( l, mult->esize, l->esize ) ;

            times = NodeAlloc( ++maxint, IFTimes ) ;
            times->level  = pr->level ;
            times->lstack = pr->lstack ;

            ThreadToUse( l->esize, 1, times, 1, integer ) ;
            ThreadToUse( mult->esize, 1, times, 2, integer ) ;

            LinkNode( pr, times ) ;
            }
        }    

    BindExportSizes( n, times->level, UNKNOWN_CSIZE, times ) ;
}


/**************************************************************************/
/* LOCAL  **************    AssignExportSizes      ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN A SIZE EXPRESSION FOR EACH ARRAY EXPORT OF NODE n      */
/*          WHOSE SIZE IS UNKNOWN (LINKING IN AN ASize NODE). NODE n WILL */
/*          EITHER BE A COMPOUND NODE, SUBGRAPH, FUNCTION GRAPH, OR CALL  */
/*          NODE.                                                         */
/**************************************************************************/

static void AssignExportSizes( n ) 
PNODE  n ;
{
    register PEDGE e;
    register PEDGE ee;
    register PNODE asize;

    for ( e = n->exp ; e != NULL ; e = e->esucc )
         if ( IsSizeUnknown(e) &&  IsArray(e->info) ) {
             asize = NodeAlloc( ++maxint, IFASize );
             asize->level  = n->level;
             asize->lstack = n->lstack;

             CopyEdgeAndThreadToUse( e, asize, 1 );

             LinkNode( n, asize );
     
             /* NOTE THE COPIED EDGE IS NOT ASSIGNED THE SIZE INFORMATION */

             for ( ee = e; ee != NULL; ee = ee->esucc )
                 if ( ee->eport == e->eport) {

                     /* NOTE csize NEEDN'T BE ASSIGNED */

                     ee->esize = asize;
                     ee->lsize = asize->level;
                     }
             }
}


/**************************************************************************/
/* GLOBAL **************        AssignSizes        ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN SIZE EXPRESSIONS TO ALL THE POTENTIAL At-Nodes IN THE  */
/*          NODE LIST OF GRAPH g AND PUSH THEM ON THE At-Node STACK. IF   */
/*          ONLY MINIMAL OPTIMIZATIONS ARE DESIRED, A BUFFER IS LINKED TO */
/*          ONLY ONE NODE, ALL ABuild, AFill, ACatenate, AddH, AND AddL   */
/*          NODES ARE CONVERTED TO AT-NODES, AND ONLY Forall MULTIPLE     */
/*          NODES ARE CONSIDERED CANDIDATES FOR CONVERSION.               */
/**************************************************************************/

void AssignSizes( g )
PNODE g;
{
    register PNODE nd;
    register PNODE sg;
    register PEDGE mult;
             PNODE expr;
             PNODE l;

    AssignExportSizes( g );  

    for ( nd = g->G_NODES; nd != NULL; nd = nd->nsucc )
        switch ( nd->type) {
            case IFAAddH:
		if ( !IsArray( nd->imp->info ) )
                    break;

                if ( IsConst( nd->imp ) )
		    break;

		/* FOR ERROR VALUES */
                if ( IsSizeUnknown( nd->imp ) )
		  nd->imp->csize = 1;

                BuildAddLHSize( nd );

		/* SHOULD WE DELAY EVERYTHING UNTIL RUN TIME? */
		if ( (!minopt) && PreferAAddH( nd ) ) {
		  paddh++;
		  break;
		  }

                PushAtNode( nd );
                break;

            case IFAAddL:
		if ( !IsArray( nd->imp->info ) )
                    break;

		/* FOR ERROR VALUES! */
                if ( IsSizeUnknown( nd->imp ) )
		  nd->imp->csize = 1;

                BuildAddLHSize( nd );
                PushAtNode( nd );
                break;

            case IFACatenate:
                if ( !IsArray( nd->exp->info ) )        /* EXPORTS AN ARRAY? */
		    break;

                if ( nd->imp->isucc->isucc != NULL )
		    Error1( "AssignSizes: ACatenate WITH > 2 IMPORTS" );

		/* FOR ERROR VALUES! */
                if ( IsSizeUnknown ( nd->imp ) )
		  nd->imp->csize = 1;
		if ( IsSizeUnknown( nd->imp->isucc ) )
		  nd->imp->isucc->csize = 1;

                BuildACatSize ( nd ) ;
                PushAtNode( nd );
                break;

            case IFABuild:
                if ( !IsArray( nd->exp->info ) )        /* EXPORTS AN ARRAY? */
                    break;

                BuildABuildSize( nd );

		if ( minopt )
		  break;

                PushAtNode( nd );
                break;

            case IFAFill:
                if ( !IsArray( nd->exp->info ) )        /* EXPORTS AN ARRAY? */
                    break ;

                expr = HighMinusLowPlusOne( nd, nd->imp, nd->imp->isucc);

                BindExportSizes ( nd, expr->level, UNKNOWN_CSIZE, expr );
                PushAtNode ( nd ) ;
                break;

            case IFAGather:
                l = nd->exp->dst->G_DAD;              /* LOOP OWNING NODE nd */

		if ( minopt )                /* PERFORM MINIMAL OPTIMIZATION */
		  break;

                if ( !IsGatherFinalOrReduceOk( l, nd, nd->imp->isucc ) )
                    break;

                BuildAGatherSize( l, nd );
                PushAtNode( nd );
                break;

            case IFFinalValue:
		if ( minopt )           /* ONLY PERFORM MINIMAL OPTIMIZATION */
		  break;

                l = nd->exp->dst->G_DAD;              /* LOOP OWNING NODE nd */

                if ( IsForall( l ) )        /* ONLY FOR LoopA OR LoopB NODES */
                    break;

                if ( !IsGatherFinalOrReduceOk( l, nd, nd->imp ) )
                    break;

                if ( !IsMonotonicConstruction( l, nd ) )
                    break;

                BuildFinalValueSize( l, nd ) ;
                PushAtNode( nd ) ;
                break;

            case IFReduce:
            case IFRedLeft:
            case IFRedRight:
            case IFRedTree:
		if ( minopt )           /* ONLY PERFORM MINIMAL OPTIMIZATION */
		  break;

                if ( nd->imp->CoNsT[0] != REDUCE_CATENATE )     /* CATENATE? */
                    break;

                l    = nd->exp->dst->G_DAD;           /* LOOP OWNING NODE nd */
		mult = nd->imp->isucc->isucc;

                if ( !IsForall(l) )                      /* ONLY FOR FORALLS */
                    break;

                if ( !IsGatherFinalOrReduceOk( l, nd, mult ) )
		    break;

                if ( IsConst( mult ) )          /* NOT IF CATENATING STRINGS */
		    break;

		/* IF mult IS NOT A K-PORT VALUE IT MUST BE A T-PORT VALUE   */
		/* WHOSE SIZE IS EITHER CONSTANT OR KNOWN OUTSIDE THE LOOP   */

                if ( !IsImport( l, mult->eport ) )
                    mult = FindImport( l->F_BODY, mult->eport );

                if ( mult == NULL )
		    break;

		if ( IsSizeUnknown( mult ) )
		    break;

                if ( !IsConstSize( mult ) )
                    if ( mult->esize->level >= nd->level )
                        break;

                BuildRedCatSize( l, mult, nd );
                PushAtNode( nd ) ;
                break ;

            case IFForall:
                PropagateImportSizes( nd, nd->F_GEN ) ;
                PropagateImportSizes( nd, nd->F_BODY ) ;
                PropagateImportSizes( nd, nd->F_RET ) ;

                AssignSizes( nd->F_BODY ) ;

                BuildForallIterExpr( nd ) ;

                AssignSizes( nd->F_RET ) ;
                PropagateImportSizes( nd->F_RET, nd ) ;

                /* ASSIGN SIZES TO THE EDGES NOT HANDLED BY PREVIOUS STEP    */
                AssignExportSizes( nd ) ;
                break;

            case IFLoopA:
            case IFLoopB:
                PropagateImportSizes( nd, nd->L_INIT );
                PropagateImportSizes( nd, nd->L_BODY );

                AssignSizes( nd->L_BODY );

                BuildLoopIterExpr( nd );

                AssignSizes( nd->L_RET );
                PropagateImportSizes( nd->L_RET, nd );

                /* ASSIGN SIZES TO THE EDGES NOT HANDLED BY PREVIOUS STEP    */
                AssignExportSizes( nd );
                break;

            case IFSelect:
                PropagateImportSizes( nd, nd->S_CONS );
                PropagateImportSizes( nd, nd->S_ALT );

                AssignSizes( nd->S_CONS );
                AssignSizes( nd->S_ALT );

                AssignExportSizes( nd );
                break;

            case IFTagCase:
                for ( sg = nd->C_SUBS; sg != NULL; sg = sg->gsucc )
                    PropagateImportSizes( nd, sg );
                
                for (sg = nd->C_SUBS; sg != NULL; sg = sg->gsucc )
                    AssignSizes( sg );

                AssignExportSizes( nd );
                break;

            default:
                AssignExportSizes( nd );
                break;
            }
}
