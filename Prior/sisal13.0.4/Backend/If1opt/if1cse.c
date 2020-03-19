/* if1cse.c,v
 * Revision 12.7  1992/11/04  22:04:56  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:31  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static int ircnt = 0;                      /* COUNT OF COMBINED R IMPORTS */
static int imcnt = 0;                      /* COUNT OF COMBINED M IMPORTS */
static int itcnt = 0;                      /* COUNT OF COMBINED T IMPORTS */
static int ncnt  = 0;                      /* COUNT OF COMBINED NODES     */
static int oacnt = 0;                 /* COUNT OF OPENED ARRAY REFERENCES */

static int smcnt = 0;                   /* COUNT OF MERGED SELECT EXPORTS */

static int srcnt = 0;                   /* COUNT OF STRIPPED RETURN NODES */


static PNODE ntails[IF1SimpleNodes];    /* SIMPLE-NODE LIST TAIL POINTERS */



/**************************************************************************/
/* LOCAL  **************      StripReturnNodes     ************************/
/**************************************************************************/
/* PURPOSE: STRIP REDUNDANT FORALL RETURN NODES FROM THE FORALL NODES OF  */
/*          GRAPH g TO ELIMINATE UNNECESSARY ARRAY CONSTRUCTIONS.         */
/**************************************************************************/

static void StripReturnNodes( g )
PNODE g;
{
  register PNODE f1;
  register PNODE f2;
  register PEDGE e;
  register PEDGE ee;
  register PEDGE eee;
  register PEDGE se;
  register PNODE aelm;
  register PNODE f1s;
  register PEDGE i;
  register PEDGE see;
  register PNODE r1;
  register PNODE r2;
  register PNODE sg;

  if ( DeBuG ) return;

  for ( f1 = g->G_NODES; f1 != NULL; f1 = f1s ) {
    f1s = f1->nsucc;

    if ( f1->exp == NULL ) {
      OptRemoveDeadNode( f1 );
      continue;
      }

    if ( !IsCompound( f1 ) )
      for ( sg = f1->C_SUBS; sg != NULL; sg = sg->gsucc )
	StripReturnNodes( sg );

    if ( !IsForall( f1 ) )
      continue;

    for ( e = f1->exp; e != NULL; e = se ) {
      se = e->esucc;
      f2 = e->dst;

      if ( !IsForall( f2 ) )
	continue;

      if ( UsageCount( f1, e->eport ) != 1 )
	continue;

      if ( e->info->type != IF_ARRAY )
	continue;

      /* CHECK IF ISOMORPHIC */
      if ( f2->F_GEN->G_NODES == NULL )
        continue;
      if ( f1->F_GEN->G_NODES == NULL )
        continue;
      if ( f2->F_GEN->G_NODES->nsucc != NULL )
        continue;
      if ( f1->F_GEN->G_NODES->nsucc != NULL )
        continue;
      if ( f2->F_GEN->G_NODES->type != IFRangeGenerate )
        continue;
      if ( f1->F_GEN->G_NODES->type != IFRangeGenerate )
        continue;
      if ( !AreNodesEqual( f1->F_GEN->imp->src, f2->F_GEN->imp->src ) )
        continue;

      /* GET AND CHECK f1'S RETURN NODE FOR e */
      if ( (ee = FindImport( f1->F_RET, e->eport )) == NULL )
	continue;
      r1 = ee->src;
      if ( r1->exp->esucc != NULL )
	continue;
      if ( !IsGather( r1 ) )
	continue;
      if ( r1->imp->isucc->isucc != NULL ) /* FILTER? */
	continue;
      if ( !AreValuesEqual( r1->imp, f1->F_GEN->imp->src->imp ) )
	continue;

      /* CHECK e'S USE IN f2'S BODY */
      if ( IsExport( f2->F_GEN, e->iport ) )
	continue;
      if ( IsExport( f2->F_RET, e->iport ) )
	continue;
      if ( UsageCount( f2->F_BODY, e->iport ) != 1 )
	continue;

      ee = FindExport( f2->F_BODY, e->iport );

      aelm = ee->dst;
      if ( !IsAElement( aelm ) )
	continue;
      if ( ee->iport != 1 )
	continue;
      if ( IsConst( ee->isucc ) )
	continue;
      if ( !IsSGraph( ee->isucc->src ) )
	continue;
      if ( ee->isucc->eport != f2->F_GEN->imp->iport )
	continue;

      ee = aelm->exp;
      if ( ee == NULL )
	continue;
      if ( ee->esucc != NULL )
	continue;
      if ( !IsSGraph( ee->dst ) )
	continue;

      /* CHECK f2'S RETURN NODE FOR ee */
      if ( UsageCount( f2->F_RET, ee->iport ) != 1 )
	continue;

      eee = FindExport( f2->F_RET, ee->iport );
      r2 = eee->dst;

      eee = r2->exp;
      if ( eee == NULL )
	continue;
      if ( eee->esucc != NULL )
	continue;

      /* FILTERS ARE NOT ALLOWED!!! */
      switch ( r2->type ) {
	case IFAGather:
          if ( !AreValuesEqual( r2->imp, f2->F_GEN->imp->src->imp ) )
	    goto MoveOn;

	  if ( r2->imp->isucc->isucc != NULL )
	    goto MoveOn;

           break;

	case IFReduce:
	case IFRedLeft:
	case IFRedRight:
	case IFRedTree:
	  if ( !IsConst( r2->imp ) )
	    goto MoveOn;
	  if ( !IsConst( r2->imp->isucc ) )
	    goto MoveOn;
	  if ( r2->imp->isucc->isucc->isucc != NULL )
	    goto MoveOn;

	  break;

	default:
	  goto MoveOn;
	}


      /* OK, DO IT!!! */
      srcnt++;

      if ( !IsGather( r2 ) ) {
	r1->type = r2->type;

	r1->imp->isucc->iport = 3;

	UnlinkExport( r1->imp );
	UnlinkImport( r1->imp );

	/* BOTH SHOULD BE CONSTANTS */
	i = r2->imp;
	UnlinkImport( i );
	LinkImport( r1, i );
	i = r2->imp;
	UnlinkImport( i );
	LinkImport( r1, i );

	r1->exp->info = r2->exp->info;
	}

      for ( eee = f2->exp; eee != NULL; eee = see ) {
	see = eee->esucc;

	if ( eee->eport != r2->exp->iport )
	  continue;

	UnlinkExport( eee );
	eee->eport = r1->exp->iport;
	LinkExport( f1, eee );
	}

      /* CLEAN UP f2 RETURN SUBGRAPH */
      UnlinkNode( r2 );
      UnlinkImport( r2->exp );

      for ( i = r2->imp;  i != NULL; i = i->isucc )
	UnlinkExport( i );

      /* free( r2 ); */

      UnlinkImport( e );
      UnlinkExport( e );
      /* free( e ); */

      /* CLEAN UP f2 BODY */
      UnlinkNode( aelm );
      UnlinkExport( aelm->imp );
      UnlinkExport( aelm->imp->isucc );
      UnlinkImport( aelm->exp );
      /* free( aelm ); */

MoveOn:;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************     MergeSelectExports    ************************/
/**************************************************************************/
/* PURPOSE: MOVE SUBGRAPH IMPORT FANOUT OUTSIDE SELECT NODE s.            */
/**************************************************************************/

static void MergeSelectExports( s )
PNODE s;
{
  register PEDGE ii1;
  register PEDGE ii2;
  register PEDGE i1;
  register PEDGE i2;
  register PEDGE si1;
  register PEDGE si2;

  i1 = s->S_ALT->imp;
  i2 = s->S_CONS->imp; 

  while ( i1 != NULL ) {

    for ( ii1 = i1->isucc, ii2 = i2->isucc; ii1 != NULL; ii1 = si1, ii2 =si2 ) {
      si1 = ii1->isucc;
      si2 = ii2->isucc;

      if ( !AreValuesEqual( i1, ii1 ) )
        continue;

      if ( !AreValuesEqual( i2, ii2 ) )
        continue;

      /* THEY SHOULD BE THE SAME, BUT WHO KNOWS!!! */
      if ( i1->iport != i2->iport )
        continue;

      if ( ii1->iport != ii2->iport )
        continue;

      ChangeExportPorts( s, ii1->iport, i1->iport );
      UnlinkExport( ii1 );
      UnlinkImport( ii1 );
      UnlinkExport( ii2 );
      UnlinkImport( ii2 );
      smcnt++;
      }

    i1 = i1->isucc;
    i2 = i2->isucc;
    }
}


/**************************************************************************/
/* LOCAL  **************         IsDisjoint        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF THE VALUE CARRIED ON EDGE e2 IS DIFFERENT THAN */
/*          THE VALUE DEFINED BY n MODIFIED BY v0.                        */
/**************************************************************************/

static int IsDisjoint( e2, n, v0 )
PEDGE e2;
PNODE n;
int   v0;
{
  register int   v1;
  register int   v2;
  register PNODE nn;

  if ( !(IsPlus( n ) || IsMinus( n )) )
    return( FALSE );

  OptNormalizeNode( n );

  if ( !IsConst( n->imp->isucc ) )
    return( FALSE );

  if ( IsConst( n->imp ) )
    return( FALSE );

  v1 = atoi( n->imp->isucc->CoNsT );
  v2 = (IsPlus( n ))? v0 + v1 : v0 - v1;

  if ( AreValuesEqual( n->imp, e2 ) ) {
    if ( v2 != 0 )
      return( TRUE );

    return( FALSE );
    }

  if ( IsDisjoint( e2, n->imp->src, v2 ) )
    return( TRUE );

  if ( IsConst( e2 ) )
    return( FALSE );

  nn = e2->src;

  if ( nn->type != n->type )
    return( FALSE );

  OptNormalizeNode( nn );

  if ( !(IsConst( nn->imp->isucc )) )
    return( FALSE );

  if ( atoi( nn->imp->isucc->CoNsT ) != v2 )
    return( FALSE );

  if ( AreValuesEqual( nn->imp, n->imp ) )
    return( FALSE );

  return( IsDisjoint( nn->imp, n, v2 ) );
}


/**************************************************************************/
/* LOCAL  **************    OpenArrayReferences    ************************/
/**************************************************************************/
/* PURPOSE: RUN GRAPH g AND USE SOME SIMPLE SUBSCRIPT ANALYSIS TO OPEN    */
/*          AS MANY ARRAY REFERENCES AS POSSIBLE.  FOR EXAMPLE,           */
/*                                                                        */
/*             A := B[i:B[i]]; C := A[j:A[i+1];   BECOMES                 */
/*             A := B[i:B[i]]; C := A[j:B[i+1];                           */
/*                                                                        */
/*          THIS OPENS MORE OPPORTUNITIES FOR LATER OPTIMIZATION.         */
/**************************************************************************/

static void OpenArrayReferences( g )
PNODE g;
{
  register PNODE n;
  register PNODE nn;
  register PEDGE e1;
  register PEDGE e2;
  register int   v1;
  register int   v2;
  register PNODE n1;
  register PNODE n2;
  register PNODE pn;

  for ( n = FindLastNode( g->G_NODES ); n != g->G_NODES; n = pn ) {
    pn = n->npred;

    if ( !IsAElement( n ) )
      continue;

    if ( !IsArray( n->imp->info ) )
      continue;

    /* 12/10/91 CANN: THIS TRANSFORMATION CAN INTRODUCE COPYING FOR  */
    /* A := old A[1,2:v; 2,2:vv] SO SKIP  OUT IF THE ARRAY IS NOT    */
    /* A VECTOR OF SCALARS                                           */
    if ( !IsBasic( n->imp->info->A_ELEM ) )
      continue;

    if ( IsConst( n->imp ) )
      continue;

    e1 = n->imp->isucc;

    /* CHECK FOR A REPLACEMENT SOURCE */
    nn = n->imp->src;

    if ( !IsAReplace( nn ) )
      continue;

    if ( IsConst( nn->imp ) )
      continue;

    e2 = nn->imp->isucc;

    /* ONLY ONE REPLACEMENT VALUE ALLOWED */
    if ( e2->isucc->isucc != NULL )
      continue;

    /* CHECK IF e2 AND e1 ARE DIFFERENT: SIMPLE SUBSCRIPT ANALYSIS */
    if ( IsConst( e1 ) ) {
      if ( !IsConst( e2 ) )
        continue;

      v1 = atoi( e1->CoNsT );
      v2 = atoi( e2->CoNsT );

      if ( v1 == v2 )
        continue;

      goto DoIt;
      }

    if ( IsConst( e2 ) )
      continue;

    n1 = e1->src;
    n2 = e2->src;

    /* MAKE SURE AT LEAST ONE INDEX SOURCE IS A PLUS OR MINUS NODE */
    if ( !((IsPlus( n1 ) || IsMinus( n1 )) ||
           (IsPlus( n2 ) || IsMinus( n2 ))) )
      continue;

    /* FIRST IS A PLUS OR MINUS NODE */
    if ( IsPlus( n1 ) || IsMinus( n1 ) )
      if ( IsDisjoint( e2, n1, 0 ) ) 
        goto DoIt;

    /* SECOND IS A PLUS OR MINUS NODE */
    if ( IsPlus( n2 ) || IsMinus( n2 ) )
      if ( IsDisjoint( e1, n2, 0 ) ) 
        goto DoIt;

    /* BOTH ARE PLUS AND MINUS NODES */
    if ( !AreValuesEqual( n1->imp, n2->imp ) )
      continue;

    /* THE RIGHT EDGES MUST BE CONSTANTS AND MUST BE POSITIVE */
    if ( !(IsConst( n1->imp->isucc ) && IsConst( n2->imp->isucc )) )
      continue;

    v1 = atoi( n1->imp->isucc->CoNsT );
    v2 = atoi( n2->imp->isucc->CoNsT );

    if ( v1 < 0 || v2 < 0 )
      continue;

    if ( n1->type == n2->type ) {
      if ( v1 == v2 )
        continue;
    } else {
      if ( v1 == 0 && v2 == 0 )
        continue;
      }

DoIt:
    UnlinkExport( n->imp );
    n->imp->eport = nn->imp->eport;
    LinkExport( nn->imp->src, n->imp );
    OptRemoveDeadNode( nn );
    oacnt++;
    pn = n;
    }
}

/**************************************************************************/
/* STATIC **************       CombineRports       ************************/
/**************************************************************************/
/* PURPOSE: COMBINE REDUNDANT IMPORTS OF LOOP RETURN SUBGRAPH g AND       */
/*          ADJUST ALL EXPORTS OF THE LOOP NODE FOR WHICH g BELONGS.      */
/**************************************************************************/

static void CombineRports( g )
PNODE g;
{
    register PEDGE i1;
    register PEDGE i2;
    register PEDGE si;

    for ( i1 = g->imp; i1 != NULL; i1 = i1->isucc ) 
        for ( i2 = i1->isucc; i2 != NULL; i2 = si ) {
            si = i2->isucc;

            if ( AreValuesEqual( i1, i2 ) ) {
                ChangeExportPorts( g->G_DAD, i2->iport, i1->iport );
                RemoveDeadEdge( i2 ); ircnt++;
                }
            }
}


/**************************************************************************/
/* STATIC **************       CombineMports       ************************/
/**************************************************************************/
/* PURPOSE: COMBINE REDUNDANT IMPORTS TO FORALL NODE f'S GENERATE SUBGRAPH*/
/*          AND ADJUST ALL REFERENCES.                                    */
/**************************************************************************/

static void CombineMports( f )
PNODE f;
{
    register PEDGE i1;
    register PEDGE i2;
    register PEDGE si;

    for ( i1 = f->F_GEN->imp; i1 != NULL; i1 = i1->isucc ) 
        for ( i2 = i1->isucc; i2 != NULL; i2 = si ) {
            si = i2->isucc;

            if ( AreValuesEqual( i1, i2 ) ) {
                ChangeExportPorts( f->F_BODY, i2->iport, i1->iport );
                ChangeExportPorts( f->F_RET,  i2->iport, i1->iport );
                
                RemoveDeadEdge( i2 ); imcnt++;
                }
            }
}


/**************************************************************************/
/* STATIC **************       CombineTports       ************************/
/**************************************************************************/
/* PURPOSE: COMBINE REDUNDANT IMPORTS TO FORALL NODE f'S BODY SUBGRAPH    */
/*          AND ADJUST ALL REFERENCES.                                    */
/**************************************************************************/

static void CombineTports( f )
PNODE f;
{
    register PEDGE i1;
    register PEDGE i2;
    register PEDGE si;

    for ( i1 = f->F_BODY->imp; i1 != NULL; i1 = i1->isucc ) 
        for ( i2 = i1->isucc; i2 != NULL; i2 = si ) {
            si = i2->isucc;

            if ( AreValuesEqual( i1, i2 ) ) {
                ChangeExportPorts( f->F_RET,  i2->iport, i1->iport );
                RemoveDeadEdge( i2 ); itcnt++;
                }
            }
}


/**************************************************************************/
/* LOCAL  **************   WillIntroduceCopying    ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF COMBINING n WITH ANOTHER NODE COULD INTRODUCE  */
/*          COPYING.                                                      */
/**************************************************************************/

static int WillIntroduceCopying( n )
PNODE n;
{
  switch ( n->type ) {
    case IFAGather:
      if ( !IsReadOnly( n->exp->dst->G_DAD, n->exp->iport ) )
        return( TRUE );

      break;

   case IFAFill:
   case IFACatenate:
   case IFAAddH:
   case IFAAddL:
   case IFABuild:
     if ( !IsReadOnly( n, 1 ) )
       return( TRUE );

   default:
     break;
   }
  
  return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************    InitializeNodeTails    ************************/
/**************************************************************************/
/* PURPOSE: INITIALIZE THE NODE TAILS TABLE USED TO QUICKEN CSE SEARCHES. */
/**************************************************************************/

static void InitializeNodeTails( g )
PNODE g;
{
  register int i,j;
  register PNODE n;
  register PNODE nt;

  for ( i = 0; i < IF1SimpleNodes; i++ )
    ntails[i] = NULL;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    n->usucc = NULL;

    if ( IsPeek(n) || !IsSimple(n) )
      continue;

    if ( WillIntroduceCopying( n ) )
      continue;

    j = (n->type)-IFAAddH;

    if ( (nt = ntails[j]) != NULL )
      nt->usucc = n;

    ntails[j] = n;
    }
}



/**************************************************************************/
/* LOCAL  **************        CombineNodes       ************************/
/**************************************************************************/
/* PURPOSE: COMBINE ALL NODES COMMON WITH n1 IN THE NODE LIST BEGINNING   */
/*          WITH n1.  ONLY SIMPLE NODES ARE COMBINED. TO SIMPLIFY         */
/*          THE ALGORITHM, NODE n1 IS PRESERVED, AND THE OTHER COMMON     */
/*          NODES ARE REMOVED---PRESERVING THE DATAFLOW ORDERING.         */
/**************************************************************************/

static void CombineNodes( n1 )
PNODE n1;
{
    register PNODE n2;
    register PNODE sn;
    register PNODE pn;
    register PEDGE i;

    for ( pn = n1, n2 = n1->usucc; n2 != NULL; n2 = sn ) {
      sn = n2->usucc;

      /* BEGIN SHORT CIRCUIT TESTS */
      if ( n2->imp != NULL ) {
        i = n2->imp;
        if ( i->src != NULL && i->esucc == NULL && i->epred == NULL ) {
	  pn = n2;
	  continue;
	  }

        i = i->isucc;
        if ( i != NULL )
          if ( i->src != NULL && i->esucc == NULL && i->epred == NULL ) {
	    pn = n2;
	    continue;
	    }
        /* END OF SHORT CIRCUIT TESTS */
	}

      if ( FastAreNodesEqual(n1,n2) ) {
	pn->usucc = n2->usucc;

        LinkExportLists( n1, n2 );

        /* OptRemoveDeadNode( n2 );  */
	for ( i = n2->imp; i != NULL; i = i->isucc )
	  UnlinkExport( i );
        UnlinkNode( n2 );

	ncnt++;
        continue;
	}

      pn = n2;
      }     
}


/**************************************************************************/
/* LOCAL  **************         RemoveCses        ************************/
/**************************************************************************/
/* PURPOSE: REMOVE COMMON NODES FROM GRAPH g. DURING COMMON NODE REMOVAL  */
/*          REDUNDANT K PORT IMPORTS OF ALL COMPOUND NODES ARE REMOVED,   */
/*          ALL REDUNDANT M AND T PORT VALUES OF FORALL SUBGRAPHS ARE     */
/*          REMOVED, AND ALL REDUNDANT R PORT VALUES OF ALL LOOP FORMS    */
/*          ARE REMOVED.                                                  */
/**************************************************************************/

static void RemoveCses( g )
PNODE g;
{
  register PNODE n;
  register PEDGE i;

  if ( !DeBuG ) OpenArrayReferences( g );

  InitializeNodeTails( g );

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsSimple( n ) ) {
      /* BEGIN SHORT CIRCUIT TESTS */
      if ( n->imp != NULL ) {
        i = n->imp;
        if ( i->src != NULL && i->esucc == NULL && i->epred == NULL )
	  continue;

        i = i->isucc;
        if ( i != NULL )
          if ( i->src != NULL && i->esucc == NULL && i->epred == NULL ) 
	    continue;
        /* END SHORT CIRCUIT TESTS */
	}

      /* OK, GO AHEAD AND TRY TO COMBINE n WITH THE OTHER NODES */
      CombineNodes( n );                   /* WILL NOT REMOVE n */
      continue;
      }

    CombineKports( n );

    ASSERT( IsCompound( n ), "Not a compound" );
    switch ( n->type ) {
      case IFSelect:
	RemoveCses( n->S_ALT );
	RemoveCses( n->S_CONS );

	MergeSelectExports( n );
	break;

      case IFTagCase:
        for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
          RemoveCses( g );

	break;

      case IFLoopA:
      case IFLoopB:
	RemoveCses( n->L_TEST );
	RemoveCses( n->L_BODY );
	RemoveCses( n->L_RET );

	CombineRports( n->L_RET );
	break;

      case IFForall:
        RemoveCses( n->F_GEN );
	CombineMports( n );

        RemoveCses( n->F_BODY );
        CombineTports( n );

        RemoveCses( n->F_RET  );
        CombineRports( n->F_RET );
        break;

      case IFUReduce:	/* TBD: can do some combination of ports too. */
	RemoveCses( n->R_INIT );
	RemoveCses( n->R_BODY );
	RemoveCses( n->R_RET );
        break;

      default:
        UNEXPECTED("Unknown compound");
    }
  }
}


/**************************************************************************/
/* LOCAL  **************        WriteCseInfo       ************************/
/**************************************************************************/
/* PURPOSE: PRINT INFORMATION GATHERED DURING CSE TO stderr.              */
/**************************************************************************/

void WriteCseInfo()
{
    FPRINTF( infoptr, "\n **** COMMON SUBEXPRESSION ELIMINATION\n\n" );

    FPRINTF( infoptr, " Combined Nodes:         %d\n", ncnt  );
    FPRINTF( infoptr, " Combined K Imports:     %d\n", ikcnt  );
    FPRINTF( infoptr, " Combined R Imports:     %d\n", ircnt );
    FPRINTF( infoptr, " Combined M Imports:     %d\n", imcnt );
    FPRINTF( infoptr, " Combined T Imports:     %d\n", itcnt );
    FPRINTF( infoptr, " Opened Array References %d\n", oacnt );

    FPRINTF( infoptr, "\n **** NODE STRIPPING\n\n" );
    FPRINTF( infoptr, " Removed Return Nodes:     %d\n", srcnt );
}


/**************************************************************************/
/* GLOBAL **************          If1Cse           ************************/
/**************************************************************************/
/* PURPOSE: REMOVE COMMON SUBEXPRESSIONS FROM ALL FUNCTION GRAPHS IF cse  */
/*          IS TRUE. STRIP RETURN NODES IF strip IS TRUE.                 */
/**************************************************************************/

void If1Cse( cse, strip )
     int cse;
     int strip;
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {

    if ( IsIGraph( f ) ) continue;

    if ( strip ) StripReturnNodes( f );

    if ( cse ) RemoveCses( f );
  }
}
