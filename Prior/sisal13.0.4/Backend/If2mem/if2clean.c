/* if2clean.c,v
 * Revision 12.7  1992/11/04  22:05:05  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:21  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

/**************************************************************************/
/* LOCAL  **************   FastAreValuesEqual      ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF EDGE v1 AND EDGE v2 CARRY THE SAME VALUE.      */
/*          IF fast THEN DO NOT CROSS GRAPH BOUNDARIES IN THE SEARCH.     */
/**************************************************************************/

static int FastAreValuesEqual( v1, v2, fast )
PEDGE v1;
PEDGE v2;
int   fast;
{
  if ( v1 == NULL || v2 == NULL )
    return( FALSE );

  if ( fast ) 
    return( AreEdgesEqual( v1, v2 ) );
  else
    return( AreEdgesEqual( FindSource(v1), FindSource(v2) ) );
}


/**************************************************************************/
/* LOCAL  **************     MemAreNodesEqual      ************************/
/**************************************************************************/
/* PURPOSE: CHECK IF TWO NODES ARE EQUIVALENT, RETURNING TRUE IF THEY ARE */
/*          AND FALSE IF THEY ARE NOT.                                    */
/**************************************************************************/

static int MemAreNodesEqual( n1, n2, fast )
PNODE n1;
PNODE n2;
int   fast;
{
    register PEDGE i1;
    register PEDGE i2;

    if ( (!IsSimple( n1 )) && (!IsSimple( n2 )) )
      return( FALSE );

    if ( n1->type != n2->type )
      return( FALSE );

    i1 = n1->imp;
    i2 = n2->imp; 
    
    while ( (i1 != NULL) && (i2 != NULL) ) {
      if ( !FastAreValuesEqual( i1, i2, fast ) )
	return( FALSE );

      i1 = i1->isucc;
      i2 = i2->isucc;
      }

    if ( (i1 == NULL) && (i2 == NULL) )
      return( TRUE );

    /* UNEQUAL NUMBER OF IMPORTS */
    return( FALSE );  
}


/**************************************************************************/
/* LOCAL  **************    FoldExceptionHandler   ************************/
/**************************************************************************/
/* PURPOSE: HANDLES ARITHMETIC EXCEPTIONS FOR CONSTANT FOLDER.            */
/**************************************************************************/

/* static int FoldExceptionHandler() */
/* { */
/*   Error1( "ARITHMETIC EXCEPTION DURING CONSTANT FOLDING" ); */
/* } */


/**************************************************************************/
/* LOCAL  **************        MemIsUsed          ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF IMPORT i OF COMPOUND NODE c IS REFERENCED     */
/*          WITHIN c.  THE FIRST IMPORT OF A TAGCASE NODE IS ALWAYS USED. */
/**************************************************************************/

static int MemIsUsed( c, i )
PNODE c;
PEDGE i;
{
    register PNODE g;

    if ( IsTagCase(c) && i->iport == 1 )
	return( TRUE );

    for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
        if ( IsExport( g, i->iport ) )
	    return( TRUE );

    return( FALSE );
}

/**************************************************************************/
/* LOCAL  **************     HandleUselessNode     ************************/
/**************************************************************************/
/* PURPOSE: NODE n HAS BEEN FOLDED INTO CONSTANT const AND IS NOT LONGER  */
/*          NEEDED. NODE n CAN DEFINE ANY OF THE FOLLOWING FORMS:         */
/*          c1 - c2, c1 + c2, c1 * c2, 0 * A, A * 0, c1 / c2, neg( c1 ),  */
/*          AND max( c1, c2 ) WHERE c1 and c2 ARE CONSTANTS AND A IS A    */
/*          NON-CONSTANT. IF EDGE non_const IS NOT NULL, THEN n IS OF THE */
/*          FORM 0 * A OR A * 0, WHERE non_const IS THE REFERENCE TO A    */
/*          AND MUST BE UNLINKED FROM ITS SOURCE NODE SO NOT TO IMPEDE    */
/*          CHAIN FOLDING IN BasicFold.  THIS ROUTINE UNLINKS ALL OF n'S  */
/*          EXPORTS, CONVERTS EACH EXPORT INTO CONSTANT const, AND        */
/*          CONVERTS n INTO A DEAD NODE.                                  */
/**************************************************************************/

static void HandleUselessNode ( n, CoNsT, non_const )
PNODE  n;
char  *CoNsT;
PEDGE  non_const;
{
    register PEDGE e;
    register PEDGE se;

    for ( e = n->exp ; e != NULL ; e = se ) {
	se = e->esucc;

	e->CoNsT = CoNsT;
	e->eport = CONST_PORT;
	e->esucc = e->epred = NULL;
	e->src   = NULL;
        }

    /* MAKE NODE n A DEAD NODE                                            */

    n->exp = NULL;

    if ( non_const != NULL )
	UnlinkExport( non_const );

    n->imp = NULL;
}


/**************************************************************************/
/* LOCAL  **************     SkipIdentityNode      ************************/
/**************************************************************************/
/* PURPOSE: THE DESTINATION OF EDGE i IS AN IDENTITY NODE OF THE FORM     */
/*          A - 0, 0 + A, A + 0, 1 * A, A * 1, OR A / 1 WHERE A IS THE    */
/*          NON-CONSTANT EDGE i. THE EXPORTS OF NODE n ARE CONVERTED INTO */
/*          REFERENCES TO i, CONVERTING n INTO A DEAD NODE.               */
/**************************************************************************/

static void SkipIdentityNode( i )
PEDGE i;
{
    register PEDGE e;
    register PEDGE se;

    /* MOVE i->dst EXPORT REFERENCES TO i->src                            */

    for ( e = i->dst->exp ; e != NULL ; e = se ) {
        se = e->esucc;

	UnlinkExport( e );
        e->eport = i->eport;
        LinkExport( i->src, e);
        }

    /* PREVIOUS LOOP WILL SET i->dst->exp TO NULL                         */

    UnlinkExport( i );

    /* MORE EFFICIENT THEN CALLING UnlinkImport TWICE                     */
    i->dst->imp = NULL;

    ident_cnt++;
}


/**************************************************************************/
/* LOCAL  **************       AddMaxAddChain      ************************/
/**************************************************************************/
/* PURPOSE: CHECK IF NODE add1 HEADS A SINGLE THREADED ADD-MAX-ADD CHAIN. */
/*          IF THE SECOND IMPORT OF EACH NODE IS A CONSTANT, THEN THE     */
/*          CHAIN IS FOLDED.  FOR EXAMPLE max( A+c1, c2 ) + c3 BECOMES    */
/*          max( A+c4, c5 ) WHERE c4 = c1 + c3 AND c5 = c2 + c3. FALSE IS */
/*          RETURNED IF SUCH A CHAIN IS NOT FOUND AND FOLDED.             */
/**************************************************************************/

static int AddMaxAddChain( c1, add1 )
int   c1;
PNODE add1;
{
    register PNODE add2;
    register PNODE max;
    register int   c3;

    if ( (!IsOneExport( add1 )) || (add1->exp->iport != 1) )
        return( FALSE );

    max = add1->exp->dst;

    if ( !IsMax( max ) )
	return( FALSE );

    /* MAKE SURE THE SECOND IMPORT TO max IS A NON-ERROR VALUE CONSTANT   */
    /* AND max's EXPORT IS USED ONLY BY ONE NODE (IMPORTED ON PORT 1)     */

    if ( !IsNonErrorConst( max->imp->isucc ) )
        return( FALSE );

    if ( (!IsOneExport(max)) || (max->exp->iport != 1) )
        return( FALSE );

    /* Add1 AND add2 CAN BE EITHER PLUS OR MINUS NODES AND THE SECOND     */
    /* IMPORT TO add2 MUST BE A NON-ERROR VALUE CONSTANT.  Add2 MAY HAVE  */
    /* FAN-OUT GREATER THAN ONE.                                          */

    add2 = max->exp->dst;

    switch ( add2->type ) {
        case IFPlus:
        case IFMinus:
            if ( !IsNonErrorConst( add2->imp->isucc ) )
                return( FALSE );

            break;

        default:
            return( FALSE );
         }

    /* A CHAIN HAS BEEN FOUND; HENCE, FOLD IT                             */

    ama_cnt++;

    c3 = atoi( add2->imp->isucc->CoNsT );

    if ( IsMinus( add2 ) )
        c3 = -c3;

    max->imp->isucc->CoNsT  = IntToAscii( atoi( max->imp->isucc->CoNsT ) + c3 );
    add1->imp->isucc->CoNsT = IntToAscii( c1 + c3 );

    /* MOVE add2'S EXPORT LIST TO max, MAKING add2 A DEAD NODE            */

    add2->imp = NULL;
    UnlinkExport( max->exp );

    LinkExportLists( max, add2 );

    return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************      ChainPresent         ************************/
/**************************************************************************/
/* PURPOSE: CHECK IF NODE n HEADS A SINGLE THREADED CHAIN OF PLUS OR      */
/*          MINUS NODES.  CONSTANT c1 IS THE SECOND IMPORT TO n, ITSELF   */ 
/*          A PLUS OR MINUS NODE.  IF THE CHAIN CONTAINS ANOTHER NODE     */
/*          WITH A CONSTANT SECOND IMPORT, THE NODE IS REMOVED.  FOR      */
/*          EXAMPLE, (A + c1 - B + c2) BECOMES (A + c3 - B) WHERE         */
/*          c3 = c1 +c2. IF A CHAIN IS FOLDED, TRUE IS RETURNED. NOTE:    */
/*          e->const DEFINES c1.                                          */
/**************************************************************************/


static int ChainPresent( c1, n, e )
int   c1;
PNODE n;
PEDGE e;
{
    register PNODE prevn;
    register int   op1;

    if ( (!IsOneExport( n )) || (n->exp->iport != 1) )
        return( FALSE );

    for ( ;; ) {
        prevn = n;
        n     = n->exp->dst;

        if ( !(IsPlus( n ) || IsMinus( n )) )
            return( FALSE );

	if ( !IsNonErrorConst( n->imp->isucc ) ) {
            if ( (!IsOneExport( n )) || (n->exp->iport != 1) )
                return( FALSE );

            continue;
	    }

	break;
        }

    /* A FOLDABLE CHAIN EXISTS; HENCE, FOLD IT                            */

    chain_cnt++;

    op1 = atoi( n->imp->isucc->CoNsT );

    if ( IsMinus( n ) )
        op1 = -op1;

    e->CoNsT = IntToAscii( op1 + c1 );

    /* APPEND THE EXPORT LIST OF n TO prevn, MAKING n A DEAD NODE         */

    n->imp = NULL;
    UnlinkExport( prevn->exp );

    LinkExportLists( prevn, n );

    return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************        BasicFold          ************************/
/**************************************************************************/
/* PURPOSE: THIS ROUTINE ATTEMPTS TO FOLD CONSTANT NODES FOUND IN SIZE    */
/*          EXPRESSIONS WITHIN GRAPH g.  THE FOLLOWING PATTERNS ARE       */
/*          FOLDED:                                                       */
/*                                                                        */
/*       1) Const1 op Const2 (WHERE op IS +, -, *, /, or Max)             */
/*       2) Neg(Const)                                                    */
/*       3) A + 0, 0 + A, A * 0, 0 * A, A - 0, AND A / 1                  */
/*       4) (A [+/-] Const1) [+/-]... [[+/-] Const2] WHERE EACH EXPORT IS */
/*          REFERENCED ONCE                                               */
/*       5) max( A [+/-] Const1, Const2 ) [+/-] Const3                    */
/*       6) A - (-Const)                                                  */
/*       7) A + (-Const)                                                  */
/*       8) -(A - B)                                                      */
/*       9) A / -1                                                        */
/*      10) max( Asize(?), 0 )                                            */
/*                                                                        */
/**************************************************************************/


static void BasicFold( g )
PNODE g;
{
    register PNODE n;
    register PEDGE i1;
    register PEDGE i2;
    register int   const1;
    register int   const2;
    register int   op1;
    register int   op2;
    register PNODE sn;
    register PEDGE e, se;

    for ( n = g->G_NODES ; n != NULL ; n = sn ) {
        sn = n->nsucc;

        if ( IsCompound( n ) ) {
            PropagateConst( n );

            for ( g = n->C_SUBS ; g != NULL ; g = g->gsucc )
                BasicFold( g );

            continue;
            }

        if ( !IsCandidate( n ) )
            continue;

        i1 = n->imp;
        i2 = i1->isucc;
    
        const1 = const2 = FALSE;

        /* FETCH OPERANDS: CAN'T BE ERROR VALUE CONSTANTS!                */

        if ( IsConst( i1 ) ) {
            if ( i1->CoNsT == NULL )
                continue;

            const1 = TRUE;
            op1    = atoi( i1->CoNsT );
            }

	/* Neg AND ASize NODES ARE CANDIDATES WITH ONE IMPORT             */

        if ( !(IsNeg( n ) || IsASize( n ) || IsALimL( n )) ) {
            if ( IsConst( i2 ) ) {
                if ( i2->CoNsT == NULL )
                    continue;

                const2 = TRUE;
                op2    = atoi( i2->CoNsT );
                }

            if ( !( const1 || const2 ) )   /* AT LEAST ONE MUST BE A CONSTANT */
                continue;
	    }

        switch ( n->type ) {
            case IFMinus:
                if ( const1 && const2 ) {      /* CONSTANT MINUS CONSTANT */
                    norm_cnt++;
                    HandleUselessNode( n, IntToAscii( op1 - op2 ), NULL_EDGE );
                    break;
                    }

                if ( const1 )
                    break;
    
                if ( op2 == 0 ) {              /* NON-CONSTANT MINUS ZERO */
                    SkipIdentityNode( i1 );
                    break;
                    }

     
                if ( AddMaxAddChain( -op2, n ) ) {
		    n->type = IFPlus;
                    sn      = n;                       /* PROCESS n AGAIN */
		    break;
		    }

                if ( ChainPresent( -op2, n, i2 ) ) {
                    n->type = IFPlus;
                    sn = n;                            /* PROCESS n AGAIN */
		    break;
                    }

		if ( op2 < 0 ) {          /* A - (Const) WHERE Const < 0  */
		    n->type = IFPlus; pncnt++;
		    i2->CoNsT++;
		    }

		break;

            case IFPlus:
                if ( const1 && const2 ) {       /* CONSTANT PLUS CONSTANT */
                    norm_cnt++;
                    HandleUselessNode( n, IntToAscii( op1 + op2 ), NULL_EDGE );
                    break;
                    }

                if ( const1 && (op1 == 0) ) {   /* ZERO PLUS NON-CONSTANT */
                    SkipIdentityNode( i2 );
                    break;
                    }

                if ( const2 && (op2 == 0) ) {   /* NON-CONSTANT PLUS ZERO */
                    SkipIdentityNode( i1 );
                    break;
                    }

                if ( const1 )
                    break;

                if ( AddMaxAddChain( op2, n ) ) {
		    n->type = IFPlus;
                    sn      = n;                      /* PROCESS n AGAIN  */
                    break;
                    }

                if ( ChainPresent( op2, n, i2 ) ) {
                    sn = n;                           /* PROCESS n AGAIN  */
		    break;
		    }

		if ( op2 < 0 ) {
		    n->type = IFMinus; pncnt++;
		    i2->CoNsT++;
		    }

                break;

            case IFTimes:
                if ( const1 && const2 ) {      /* CONSTANT TIMES CONSTANT */
                    norm_cnt++;
                    HandleUselessNode(n, IntToAscii( op1 * op2 ), NULL_EDGE );
                    break;
                    }
    
                if ( const1 ) {
                    if ( op1 == 1 ) {           /* ONE TIMES NON-CONSTANT */
                        SkipIdentityNode( i2 );
                        break;
                        }
    
                    if ( op1 == 0 ) {          /* ZERO TIMES NON-CONSTANT */
                        zero_cnt++;
                        HandleUselessNode( n, "0", i2 );
                        break;
                        }
    
                    break;
                    }
    
                if ( const2 ) {
                    if ( op2 == 1 ) {           /* NON-CONSTANT TIMES ONE */
                        SkipIdentityNode( i1 );
                        break;
                        }
    
                    if ( op2 == 0 ) {          /* NON-CONSTANT TIMES ZERO */
                        zero_cnt++;
                        HandleUselessNode( n, "0", i1 );
                        }
                    }
    
                break;
            
            case IFDiv:
                if ( const1 && const2 ) {        /* CONSTANT DIV CONSTANT */
		    if ( op2 == 0 ) break;
                    norm_cnt++;
                    HandleUselessNode(n, IntToAscii( op1 / op2 ), NULL_EDGE );
                    break;
                    }

                if ( const2 ) {
		    if ( op2 == 0 ) break;

                    if (op2 == 1 ) {              /* NON-CONSTANT DIV ONE */
                        SkipIdentityNode( i1 );
                        break;
                        }

                    if (op2 == -1) {                  /* NON-CONST DIV -1 */
		        dncnt++;
                        UnlinkImport( i2 );
                        n->type = IFNeg;
                        sn = n;                     /* START AGAIN WITH n */
                        }
                    }

                break;

            case IFNeg:
                if ( const1 ) {
                    neg_cnt++;
                    HandleUselessNode( n, IntToAscii( -op1 ), NULL_EDGE );
                    break;
                    }
        
                /* CHECK FOR -(A-B)                                       */

                if ( IsMinus( i1->src ) && IsOneExport( i1->src ) ) {
		    pncnt++ ;

		    /* SWAP IMPORTS TO THE MINUS NODE AND MAKE n DEAD     */

                    UnlinkImport( (e= i1->src->imp) );
                    e->iport = 2;
                    i1->src->imp->iport = 1;
                    LinkImport( i1->src, e );
    
                    UnlinkExport( i1->src->exp );
                    LinkExportLists( i1->src, n );
                    n->imp = NULL;
                    }

                break;
         
            case IFMax:
                if ( const1 && const2 ) {    /* MAX( CONSTANT, CONSTANT ) */
                    norm_cnt++;
                    HandleUselessNode( n, IntToAscii( Max( op1, op2 ) ), NULL_EDGE );
                    }

                break;

            case IFASize:
                for ( e = n->exp ; e != NULL ; e = se ) {
                    se = e->esucc;

                    if ( !IsMax( e->dst) )
                        continue;

		    if ( !IsNonErrorConst( e->dst->imp->isucc ) )
		        continue;

                    if ( atoi( e->dst->imp->isucc->CoNsT) != 0 )
                        continue;

		    asize_cnt++;
		    sn = n;                   /* WHEN DONE, CHECK n AGAIN */

                    LinkExportLists( n, e->dst );
                    e->dst->imp = NULL;
                    UnlinkExport( e );
                    }

                break;

            default:
                break;
            } 
        }
}


/**************************************************************************/
/* LOCAL  **************    RemoveAndInsertNodes   ************************/
/**************************************************************************/
/* PURPOSE: REMOVE NODE aa AND cc FROM THE SUBGRAPHS OF COMPOUND NODE n   */
/*          AND THREAD IN THE REFERENCES.                                 */
/**************************************************************************/

static void RemoveAndInsertNodes( aa, cc, n )
PNODE aa;
PNODE cc;
PNODE n;
{
  register PNODE nn;
  register PEDGE i;
  register PEDGE si;
  register PEDGE ii;
  register PEDGE e;
  register PEDGE se;

  nn = NodeAlloc( ++maxint, aa->type );
  nn->lstack = n->lstack;
  nn->level  = n->level;
  LinkNode( n->npred, nn );

  /* THREAD A REFERENCE FROM nn'S EXPORT TO EACH USE IN cc's SUBGRAPH */

  for ( e = cc->exp; e != NULL; e = se ) {
   se = e->esucc;

   UnlinkImport( e );
   ThreadToUse( nn, 1, e->dst, e->iport, e->info );
   }
                    
  /* ATTACH nn'S IMPORTS                                */

  for ( i = aa->imp; i != NULL; i = si ) {
    si = i->isucc;

    UnlinkImport( i );
    UnlinkExport( i );

    if ( IsConst( i ) ) {
      LinkImport( nn, i );
      continue;
      }

    ii = FindImport( n, i->eport );

    CopyEdgeAndThreadToUse( ii, nn, i->iport );

    /* IF ii IS NO LONGER USED WITHIN n, UNLINK IT    */
    /* NOW TO FACILITATE CHAIN FOLDING.               */

    if ( !MemIsUsed( n, ii ) ) {
      UnlinkImport( ii );
      UnlinkExport( ii );
      }
    }

  /* THREAD A REFERENCE FROm nn'S EXPORT TO EACH USE IN aa's SUBGRAPH */

  for ( e = aa->exp; e != NULL; e = se ) {
   se = e->esucc;

   UnlinkImport( e );
   ThreadToUse( nn, 1, e->dst, e->iport, e->info );
   }

  /* MAKE nd A DEAD NODE */
  aa->exp = NULL; 
  cc->exp = NULL;
}


/**************************************************************************/
/* LOCAL   **************   MemIsEdgeInvariant   ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF EDGE i IS LOOP INVARIANT.                      */
/**************************************************************************/

static int MemIsEdgeInvariant( i )
PEDGE i;
{
  if ( IsConst( i ) )
    return( TRUE );

  if ( !IsSGraph( i->src ) )
    return( FALSE );

  if ( !IsImport( i->src->G_DAD, i->eport ) )
    return( FALSE );

  return( TRUE );
}


/**************************************************************************/
/* LOCAL   **************      MemIsInvariant      ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF NODE n IS LOOP INVARIANT. NODE n IS INVARIANT  */
/*          IF ITS IMPORT ARE CONSTANTS OR IMPORTS TO ITS OWNING COMPOUND */
/*          NODE.                                                         */
/**************************************************************************/

static int MemIsInvariant( n )
PNODE n;
{
    register PEDGE i;

    for ( i = n->imp; i != NULL; i = i->isucc ) {
	if ( !MemIsEdgeInvariant( i ) )
	  return( FALSE );
	}

    return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************     MemCombineKports      ************************/
/**************************************************************************/
/* PURPOSE: COMBINE REDUNDANT IMPORTS TO COMPOUND NODE c AND ADJUST ALL   */
/*          REFERENCES. NOTE, THE FIRST IMPORT TO A TAGCASE NODES IS      */
/*          NEVER COMBINED WITH OTHER IMPORTS.                            */
/**************************************************************************/

static void MemCombineKports( c )
PNODE c;
{
    register PEDGE i1;
    register PEDGE i2;
    register PNODE g;
    register PEDGE si;

    i1 = (IsTagCase( c ))? c->imp->isucc : c->imp;

    for ( ; i1 != NULL; i1 = i1->isucc ) 
	for ( i2 = i1->isucc; i2 != NULL; i2 = si ) {
	    si = i2->isucc;

	    if ( AreEdgesEqual( i1, i2 ) ) {
		for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
	            ChangeExportPorts( g, i2->iport, i1->iport );
                
		UnlinkImport( i2 );
		UnlinkExport( i2 ); kcnt++;
		}
	    }
}


/**************************************************************************/
/* LOCAL  **************      MemRemoveSCses       ************************/
/**************************************************************************/
/* PURPOSE: MOVE COMMON SUBEXPRESSIONS BETWEEN SELECT SUBGRAPHS TO        */
/*          IMPROVE CHANCES FOR INVARIANT REMOVAL AND CSE.                */
/**************************************************************************/

static void MemRemoveSCses( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PNODE aa;
  register PNODE cc;
  register PNODE sa;
  register PEDGE e;

  /* DEPTH FIRST */
  for ( n = g->G_NODES; n != NULL; n = n->nsucc )
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        MemRemoveSCses( sg );

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( !IsSelect( n ) ) continue;

    for ( aa = n->S_ALT->G_NODES; aa != NULL; aa = sa ) {
      sa = aa->nsucc;

      if ( !IsCandidate( aa ) )
	continue;

      for ( cc = n->S_CONS->G_NODES; cc != NULL; cc = cc->nsucc ) {
	/* A SHORT CIRCUIT FOR FASTER EXECUTION */
	if ( aa->type != cc->type )
	  continue;

	/* DEAD ALREADY???? */
	if ( cc->exp == NULL )
	  continue;

        if ( !MemIsInvariant( aa ) )
	  continue;

	if ( !MemIsInvariant( cc ) )
	  continue;

        if ( !MemAreNodesEqual(aa,cc,FALSE) )
          continue;

	/* BE CAREFUL NOT TO HURT UPDATE-IN-PLACE ANALYSIS */
        if ( aa->type == IFAElement ) {
          for ( e = aa->imp->src->exp; e != NULL; e = e->esucc )
            if ( e->dst->type == IFAReplace )
              goto MoveOn;

          for ( e = cc->imp->src->exp; e != NULL; e = e->esucc )
            if ( e->dst->type == IFAReplace )
              goto MoveOn;
	  }

        /* RemoveNode( cc, n->S_CONS ); */
        /* InsertNode( n, cc );         */
        /* RemoveNode( aa, n->S_ALT );  */
        /* InsertNode( n, aa );         */
        RemoveAndInsertNodes( aa, cc, n );

        sccnt++;

	/* SUCCESS!!! SO BREAK OUT AND MOVE ON */
	break;
        }

      MoveOn: continue;
      }
    }
}


static void RemoveCCses( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PNODE aa;
  register PNODE cc;
  register PNODE sa;
  register PEDGE e;
  register PEDGE se;
  register PEDGE si;
  register PEDGE i;
  register PEDGE ii;
  register PNODE nn;

/* NOTE: */
/* THIS ROUTINE RELIES ON RemoveGCse TO REMOVE cc AND COMBINE IT WITH aa */

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	RemoveCCses( sg );

    if ( !IsSelect( n ) ) continue;

    for ( aa = n->S_ALT->G_NODES; aa != NULL; aa = sa ) {
      sa = aa->nsucc;

      if ( !IsCandidate( aa ) )
	continue;

      for ( cc = n->S_CONS->G_NODES; cc != NULL; cc = cc->nsucc ) {
	if ( !IsCandidate( cc ) )
	  continue;

	if ( !MemIsInvariant( aa ) )
	  continue;

	if ( !MemIsInvariant( cc ) )
	  continue;

	if ( !MemAreNodesEqual(aa,cc,FALSE) )
	  continue;

	/* RemoveNode( aa, n->S_ALT );
	InsertNode( n, aa ); */

		    nn = NodeAlloc( ++maxint, aa->type );
		    nn->lstack = n->lstack;
		    nn->level  = n->level;
		    LinkNode( n->npred, nn );
                    
                    /* ATTACH nn'S IMPORTS                                */

		    for ( i = aa->imp; i != NULL; i = si ) {
			si = i->isucc;

			UnlinkImport( i );
			UnlinkExport( i );

                        if ( IsConst( i ) ) {
			    LinkImport( nn, i );
			    continue;
			    }
			    
			ii = FindImport( n, i->eport );

                        CopyEdgeAndThreadToUse( ii, nn, i->iport );

			/* IF ii IS NO LONGER USED WITHIN n, UNLINK IT    */
			/* NOW TO FACILITATE CHAIN FOLDING.               */

                        if ( !MemIsUsed( n, ii ) ) {
			    UnlinkImport( ii );
			    UnlinkExport( ii );
                            }

			}

		    /* THREAD A REFERENCE TO nn'S EXPORT TO EACH USE IN n */

                    for ( e = aa->exp; e != NULL; e = se ) {
			se = e->esucc;

			UnlinkImport( e );
			ThreadToUse( nn, 1, e->dst, e->iport, e->info );
			}

		    aa->exp = NULL; /* MAKE nd A DEAD NODE */

	}

      continue;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************    BasicGCseRemoval       ************************/
/**************************************************************************/
/* PURPOSE: COMBINE SIZE EXPRESSION NODES DEFINED IN THE NODE LIST OF     */
/*          GRAPH g WITH IDENTICAL NODES FOUND AT LEVELS BELOW g.  ONLY   */
/*          INVARIANT NODES ARE CONSIDERED.                               */
/**************************************************************************/

static void BasicGCseRemoval( g )
PNODE g;
{
    register PNODE sn;
    register PNODE n1;
    register PNODE n2;
    register PEDGE e;
    register int   f;
    register int   lft;

    for ( n1 = g->G_NODES; n1 != NULL; n1 = sn ) {
	sn = n1->nsucc;

	if ( IsCompound( n1 ) ) {
	    for ( g = n1->C_SUBS; g != NULL; g = g->gsucc )
		BasicGCseRemoval( g );        /* MAY MOVE n1'S SUCCESSOR */

	    sn = n1->nsucc;
	    continue;
	    }

        if ( !IsCandidate( n1 ) )
	    continue;

        if ( !MemIsInvariant( n1 ) )
	    continue;

        for ( f = n1->level - 1; f >= 0; f-- ) {
	    lft = TRUE;

	    for ( n2 = (n1->lstack[f])->G_NODES; n2 != NULL; n2 = n2->nsucc ) {
	        if ( n2 == (n1->lstack[f+1])->G_DAD )
		    lft = FALSE;

		if ( n2->exp == NULL )                /* IS NODE n2 DEAD? */
		    continue;

                if ( !MemAreNodesEqual( n1, n2, FALSE ) )
		    continue;

                goto CombineAndThread;
	        }
            }

	continue;

CombineAndThread:

        if ( !lft ) {                  /* PRESERVE THE DATA FLOW ORDERING */
            UnlinkNode( n2 );
            LinkNode( (n1->lstack[f+1])->G_DAD->npred, n2 );
            }

        for ( e = n1->exp; e != NULL; e = e->esucc ) {
	    ThreadToUse( n2, 1, e->dst, e->iport, e->info );
	    UnlinkImport( e );
	    }

        n1->exp = NULL; gccnt++;                   /* MAKE n1 A DEAD NODE */

        for ( f++; f <= n1->level; f++ )
            MemCombineKports( (n1->lstack[f])->G_DAD );
	}
}


/**************************************************************************/
/* LOCAL  **************      BasicCseRemoval      ************************/
/**************************************************************************/
/* PURPOSE: REMOVE COMMON NODES FROM GRAPH g. DURING COMMON NODE REMOVAL  */
/*          REDUNDANT K PORT IMPORTS OF ALL COMPOUND NODES ARE REMOVED.   */
/**************************************************************************/

static void BasicCseRemoval( g )
PNODE g;
{
    register PNODE n1;
    register PNODE n2;
    register PNODE sn;
    register PEDGE i;

    for ( n1 = g->G_NODES; n1 != NULL; n1 = n1->nsucc ) {
	if ( IsCandidate( n1 ) ) {
            /* BEGIN SHORT CIRCUIT TESTS */
	    if ( n1->imp != NULL ) {
              i = n1->imp;
              if ( i->src != NULL && i->esucc == NULL && i->epred == NULL )
	        continue;

              i = i->isucc;
              if ( i != NULL )
                if ( i->src != NULL && i->esucc == NULL && i->epred == NULL )
	          continue;
              /* END OF SHORT CIRCUIT TESTS */
	      }

            for ( n2 = n1->nsucc; n2 != NULL; n2 = sn ) {
                sn = n2->nsucc;

		if ( n2->exp == NULL )                /* IS NODE n2 DEAD? */
		    continue;

	        if ( MemAreNodesEqual( n1, n2, TRUE ) ) {
	            LinkExportLists( n1, n2 );
	            ccnt++;
	            }
                }     

	    continue;
            }

	if ( IsCompound( n1 ) ) {
	    MemCombineKports( n1 );

            for ( g = n1->C_SUBS; g != NULL; g = g->gsucc )
                BasicCseRemoval( g );
	    }
        }
}


/**************************************************************************/
/* LOCAL  **************     BasicInvarRemoval     ************************/
/**************************************************************************/
/* PURPOSE: REMOVE LOOP INVARIANT SIZE EXPRESSION NODES FROM LOOP AND     */
/*          FORALL NODES DEFINED IN GRAPH g.  INVARIANTS OF INNERMOST     */
/*          LOOPS ARE REMOVED FIRST.  THE NODES ARE EXAMINED IN DATAFLOW  */
/*          ORDER; HENCE, AN INVARIANT NODE WHOSE IMPORTS COME FROM OTHER */
/*          INVARIANT NODES IN THE SAME SCOPE ARE EASILY DETECTED.        */
/**************************************************************************/

static void BasicInvarRemoval(lvl, g )
int lvl;
PNODE g;
{
    register PNODE n;
    register PNODE nd;
    register PEDGE i;
    register PEDGE e;
    register PEDGE se;
    register PEDGE si;
    register PNODE sn;
    register PNODE nn;
    register PEDGE ii;

    for ( n = g->G_NODES; n != NULL; n = n->nsucc )
	if ( IsCompound( n ) ) {
	    for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		BasicInvarRemoval(lvl+1, g );

            if ( !(IsForall( n ) || IsLoop( n )) )
		continue;

	    if ( (lvl == 1) && (!Oinvar) )
		continue;

            for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		for ( nd = g->G_NODES; nd != NULL; nd = sn ) {
	            sn = nd->nsucc;

		    if ( !IsCandidate( nd ) )
			continue;

	            if ( !MemIsInvariant( nd ) ) 
			continue;

		    /* PLACE A COPY OF NODE nd BEFORE THE LOOP NODE       */

		    nn = NodeAlloc( ++maxint, nd->type );
		    nn->lstack = n->lstack;
		    nn->level  = n->level;
		    LinkNode( n->npred, nn );
                    
                    /* ATTACH nn'S IMPORTS                                */

		    for ( i = nd->imp; i != NULL; i = si ) {
			si = i->isucc;

			UnlinkImport( i );
			UnlinkExport( i );

                        if ( IsConst( i ) ) {
			    LinkImport( nn, i );
			    continue;
			    }
			    
			ii = FindImport( n, i->eport );

                        CopyEdgeAndThreadToUse( ii, nn, i->iport );

			/* IF ii IS NO LONGER USED WITHIN n, UNLINK IT    */
			/* NOW TO FACILITATE CHAIN FOLDING.               */

                        if ( !MemIsUsed( n, ii ) ) {
			    UnlinkImport( ii );
			    UnlinkExport( ii );
                            }

			}

		    /* THREAD A REFERENCE TO nn'S EXPORT TO EACH USE IN n */

                    for ( e = nd->exp; e != NULL; e = se ) {
			se = e->esucc;

			UnlinkImport( e );
			ThreadToUse( nn, 1, e->dst, e->iport, e->info );
			}

		    nd->exp = NULL; vcnt++;       /* MAKE nd A DEAD NODE */
		    }
	    }
}


/**************************************************************************/
/* LOCAL  **************      MemRemoveDeadNode       ************************/
/**************************************************************************/
/* PURPOSE: REMOVE DEAD NODE n AND CHECK IF ITS REMOVAL UNCOVERS OTHER    */
/*          DEAD NODES.                                                   */
/**************************************************************************/

static void MemRemoveDeadNode( n )
PNODE n;
{
    register PEDGE i;

    if ( IsSGraph( n ) )
	return;

    if ( IsPeek( n ) )
      return;

    if ( glue && IsCall( n ) )
      return;

    for ( i = n->imp; i != NULL; i = i->isucc )
	if ( !IsConst( i ) ) {
	    UnlinkExport( i );

	    if ( i->src->exp == NULL )
		MemRemoveDeadNode( i->src );
            }

    UnlinkNode( n );
}


/**************************************************************************/
/* LOCAL  ************** MemRemoveDeadNodesFromGraph  ************************/
/**************************************************************************/
/* PURPOSE: REMOVE THE DEAD NODES FROM GRAPH g.  A NODE IS DEAD IF ITS    */
/*          EXPORT LIST IS NULL.                                          */
/**************************************************************************/

static void MemRemoveDeadNodesFromGraph( g )
PNODE g;
{
    register PNODE n;
    register PNODE sn;
    register PEDGE i;
    register PEDGE si;

    for ( n = g->G_NODES; n != NULL; n = sn ) {
	sn = n->nsucc;

	if ( !IsCompound( n ) ) {
	    if ( n->exp == NULL )
		MemRemoveDeadNode( n );

            continue;
	    }

        for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
	    MemRemoveDeadNodesFromGraph( g );

	/* ARE ALL K-IMPORTS TO n USED? DISCARD THOSE THAT ARE NOT        */

	for ( i = n->imp; i != NULL; i = si ) {
	    si = i->isucc;

            if ( MemIsUsed( n, i ) )
		continue;

	    UnlinkImport( i );

            if ( !IsConst( i ) ) {
		UnlinkExport( i );

		if ( i->src->exp == NULL )
		    MemRemoveDeadNode( i->src );
		}
	    }
	}
}


/**************************************************************************/
/* LOCAL  ************** ComputeAndAttachLowerBound ***********************/
/**************************************************************************/
/* PURPOSE: INSERT CODE TO CALCULATE THE LOWER BOUND OF ARRAY i AND       */
/*          IMPORT THE RESULT TO NODE dst AT IMPORT PORT iport.           */
/**************************************************************************/

static void ComputeAndAttachLowerBound( i, dst, iport )
PEDGE i;
PNODE dst;
int   iport;
{
  register PNODE lml;
  register PEDGE e;
  register PNODE n;
  register PEDGE lo;
  register PEDGE ee;
  register PNODE rt;
  register PNODE sg;

  n = i->src;

  if ( n == NULL )
    Error2( "ComputeAndAttachLowerBound", "CONSTANT ARRAY ENCOUNTERED!" );

  /* CALCULATE IT, OR DO WE ALREADY HAVE IT? */
  switch ( n->type ) {
    case IFABuildAT:
      lo = n->imp;
      AttachEdge( lo->src, lo->eport, dst, iport, integer, lo->CoNsT ); 
      return;

    case IFASetL:
      e = n->imp->isucc;
      AttachEdge( e->src, e->eport, dst, iport, integer, e->CoNsT ); 
      return;

    case IFForall:
    case IFLoopA:
    case IFLoopB:
      sg = (n->type == IFForall)? n->F_RET : n->L_RET;

      if ( (ee = FindImport( sg, i->eport )) == NULL )
	Error2( "FindAndAttachLowerBound", "FindImport FAILURE (B)" );

      rt = ee->src;

      switch ( rt->type ) {
	case IFReduceAT:
	case IFRedLeftAT:
	case IFRedRightAT:
	case IFRedTreeAT:
	case IFAGatherAT:
	case IFAGather:
	  ee = (rt->type == IFAGatherAT || rt->type == IFAGather)? 
		rt->imp : rt->imp->isucc;

	  if ( !IsConst( ee ) ) {
	    if ( (ee = FindImport( n, ee->eport )) == NULL )
	      Error2( "FindAndAttachLowerBound", "FindImport FAILURE (C)" );

	    AttachEdge( ee->src, ee->eport, dst, iport, integer, ee->CoNsT );
	    }
	  else
	    AttachEdge( NULL_NODE, ee->eport, dst, iport, integer, ee->CoNsT );

	  return;

	default:
	  break;
        }

      break;

    default:
      break;
    }

  /* CALCULATE THE LOWER BOUND */
  lml = NodeAlloc( ++maxint, IFALimL );
  lml->lstack = dst->lstack;
  lml->level  = dst->level;
  LinkNode( dst->npred, lml );

  AttachEdge( n, i->eport, lml, 1, i->info, (char*)NULL );
  AttachEdge( lml, 1, dst, iport, integer, (char*)NULL );
}


/**************************************************************************/
/* LOCAL  **************  FindAndAttachLowerBound   ***********************/
/**************************************************************************/
/* PURPOSE: SEARCH FOR THE LOWER BOUND OF ARRAY e AND WIRE A REFERENCE TO */
/*          NODE dst AT PORT iport. EDGE e IS ASSUMED TO CARRY A P MARK.  */
/**************************************************************************/

static void FindAndAttachLowerBound( e, dst, iport )
PEDGE e;
PNODE dst;
int   iport;
{
  register PNODE n;
  register PEDGE lo;
  register PEDGE ee;
  register PNODE sg;
  register PNODE rt;
  register int   port;

  n = e->src;

  if ( n == NULL )
    Error2( "FindAndAttachLowerBound", "CONSTANT ARRAY ENCOUNTERED!" );

  switch ( n->type ) {
    case IFASetL:
      lo = n->imp->isucc;
      AttachEdge( lo->src, lo->eport, dst, iport, integer, lo->CoNsT ); 
      break;

    case IFAAddLAT:
    case IFAAddHAT:
    case IFACatenateAT:
      if ( (lo = FindImport( n, 5 )) == NULL )
	Error2( "FindAndAttachLowerBound", "FindImport FAILURE (A)" );

      AttachEdge( lo->src, lo->eport, dst, iport, integer, lo->CoNsT );
      break;

    case IFForall:
    case IFLoopA:
    case IFLoopB:
      sg = (n->type == IFForall)? n->F_RET : n->L_RET;

      if ( (ee = FindImport( sg, e->eport )) == NULL )
	Error2( "FindAndAttachLowerBound", "FindImport FAILURE (B)" );

      rt = ee->src;

      switch ( rt->type ) {
	case IFReduceAT:
	case IFRedLeftAT:
	case IFRedRightAT:
	case IFRedTreeAT:
	case IFAGatherAT:
	case IFAGather:
	  ee = (rt->type == IFAGatherAT || rt->type == IFAGather)? 
	       rt->imp : rt->imp->isucc;

	  if ( !IsConst( ee ) ) {
	    if ( (ee = FindImport( n, ee->eport )) == NULL )
	      Error2( "FindAndAttachLowerBound", "FindImport FAILURE (C)" );

	    AttachEdge( ee->src, ee->eport, dst, iport, integer, ee->CoNsT );
	    }
	  else
	    AttachEdge( NULL_NODE, ee->eport, dst, iport, integer, ee->CoNsT );

	  break;

	default:
          ComputeAndAttachLowerBound( e, dst, iport );
	  break;
        }

      break;

    case IFSGraph:
      switch ( e->dst->type ) {
	case IFACatenateAT:
	  if ( e->iport != 1 ) {
            ComputeAndAttachLowerBound( e, dst, iport );
	    break;
	    }

	case IFAAddHAT:
          if ( (ee = FindImport( n->G_DAD->L_INIT, e->eport )) == NULL )
	    Error2( "FindAndAttachLowerBound", "FindImport FAILURE (D)" );
          if ( (ee = FindImport( n->G_DAD, ee->eport )) == NULL )
	    Error2( "FindAndAttachLowerBound", "FindImport FAILURE (E)" );

	  port = ++maxint;
	  ComputeAndAttachLowerBound( ee, n->G_DAD, port );
	  AttachEdge( n->G_DAD->L_BODY, port, dst, iport, integer, (char*)NULL );
	  break;

	case IFAAddLAT:
	  ComputeAndAttachLowerBound( e, dst, iport );
	  break;

	default:
	  Error2( "FindAndAttachLowerBound", "IFSGraph switch FAILURE" );
	  break;
        }

      break;

    default:
      ComputeAndAttachLowerBound( e, dst, iport );
      break;
    }
}


/**************************************************************************/
/* LOCAL  **************      AssignLowerBounds     ***********************/
/**************************************************************************/
/* PURPOSE: ASSIGN LOWER BOUNDS TO THE AT-NODES IN GRAPH g. THE ReduceAT  */
/*          AGatherAT NODES ALREADY HAVE ATTACHED LOWER BOUNDS.           */
/**************************************************************************/

static void AssignLowerBounds( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PNODE nn;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	AssignLowerBounds( sg );

      continue;
      }

    switch( n->type ) {
      case IFACatenateAT:
      case IFAAddHAT:
	if ( !(n->imp->mark == 'P') )
	  ComputeAndAttachLowerBound( n->imp, n, 5 );
        else
	  FindAndAttachLowerBound( n->imp, n, 5 );

	break;

      case IFAAddLAT:
	nn = NodeAlloc( ++maxint, IFMinus );
        nn->lstack = n->lstack;
        nn->level  = n->level;
	LinkNode( n->npred, nn );

	AttachEdge( NULL_NODE, CONST_PORT, nn, 2, integer, CopyString( "1" ) );
	AttachEdge( nn, 1, n, 5, integer, (char*)NULL );

	if ( !(n->imp->mark == 'P') )
	  ComputeAndAttachLowerBound( n->imp, nn, 1 );
        else
	  FindAndAttachLowerBound( n->imp, nn, 1 );
	break;

      default:
	break;
      }
    }
}


/**************************************************************************/
/* GLOBAL **************          If2Clean         ************************/
/**************************************************************************/
/* PURPOSE: CLEAN ALL FUNCTION GRAPHS, PERFROMING DEAD CODE ELIMINATION,  */
/*          LOOP INVARIANT REMOVAL, COMMON SUBEXPRESSION ELIMINATION,     */
/*          GLOBAL COMMON SUBEXPRESSION ELIMINATION, AND CONSTANT         */
/*          FOLDING.  IF THE info FLAG IS TRUE, INFORMATION ABOUT THE     */
/*          PERFORMED OPTIMIZATIONS IS PRINTED TO infoptr. THE ALGORITHMS  */
/*          ARE WRITTEN TO ATTACK THOSE NODES FOUND IN SIZE EXPRESSIONS   */
/*          AND NOT THE ENTIRE GAMUT OF IF1 AND IF2 NODES: IFPlus,        */
/*          IFMinus, IFDiv, IFTimes, AND IFMax (ALL FOR INTEGERS) IN      */
/*          ADDITION TO IFASize FOR ARRAYS.  THE ARITHMETIC EXCEPTION     */
/*          HANDLER FoldExceptionHandler IS INSTALLED BEFORE FOLDING AND  */
/*          THE OLD HANDLER IS REINSTALLED AFTER FOLDING.                 */
/**************************************************************************/

void If2Clean()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    MemRemoveDeadNodesFromGraph( f );
/*
  if ( RequestInfo(I_Info2,info)  ) {
    CountNodesAndEdges( "AFTER ARRAY MEMORY OPTIMIZATION" );
  } */

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    AssignLowerBounds( f );

    if ( invar ) {
      MemRemoveSCses( f );
      BasicInvarRemoval(1, f );
      }

    if ( fold ) {
      /* signal( SIGFPE, FoldExceptionHandler ); */

      BasicFold( f );

      /* signal( SIGFPE, SIG_DFL ); */
      }

    if ( cse ) {
      BasicCseRemoval( f );

      ckcnt = kcnt;
      kcnt  = 0;
      }

    if ( gcse ) {
      /* RemoveCCses( f ); */ /* MADE OBSOLETE BY MemRemoveSCses */
      BasicGCseRemoval( f );

      gkcnt = kcnt;
      }

    if ( invar || cse || gcse || fold )
      MemRemoveDeadNodesFromGraph( f );
    }

  if ( RequestInfo(I_Info2,info)  ) {
/*    FPRINTF( infoptr, "\n **** MISCELLANEOUS IF2MEM INFOMATION\n\n" );
    FPRINTF( infoptr, " AAddH Preferences: %d\n", paddh ); */

/*    if ( invar ) {
      FPRINTF( infoptr, "\n **** LOOP IMVARIANT NODE REMOVAL\n\n"     );
      FPRINTF( infoptr, " Loop Invariants Removed:                %d\n", vcnt  );
      FPRINTF( infoptr, " Select Invariant Movements:             %d\n", sccnt );
      FPRINTF( infoptr, " MemoryAlloc Select Invariant Movements: %d\n", sccnt );
      }

    if ( fold ) {
      FPRINTF( infoptr, "\n **** CONSTANT FOLDING\n\n" );
      FPRINTF( infoptr, " SIMPLE NODES FOLDED:          %d\n", norm_cnt  );
      FPRINTF( infoptr, " A * 0 OR 0 * A NODES FOLDED:  %d\n", zero_cnt  );
      FPRINTF( infoptr, " IDENTITY NODES FOLDED:        %d\n", ident_cnt );
      FPRINTF( infoptr, " +/- CHAINS FOLDED:            %d\n", chain_cnt );
      FPRINTF( infoptr, " NEGATIVE NODES FOLDED:        %d\n", neg_cnt   );
      FPRINTF( infoptr, " PROPAGATED CONSTANTS:         %d\n", pcnt      );
      FPRINTF( infoptr, " ADD-MAX-ADD CHAINS FOLDED:    %d\n", ama_cnt   );
      FPRINTF( infoptr, " ASize-MAX CHAINS FOLDED:      %d\n", asize_cnt );
      FPRINTF( infoptr, " DIV-TO-NEG CONVERSIONS        %d\n", dncnt     );
      FPRINTF( infoptr, " PROPAGATED NEGATIONS          %d\n", pncnt     );
      }

    if ( cse ) {
      FPRINTF( infoptr, "\n **** COMMON NODE ELIMINATION\n\n"         );
      FPRINTF( infoptr, " Combined Nodes:               %d\n", ccnt  );
      FPRINTF( infoptr, " Combined K Imports:           %d\n", ckcnt );
      }

    if ( gcse ) {
      FPRINTF( infoptr, "\n **** GLOBAL COMMON NODE ELIMINATION\n\n"  );
      FPRINTF( infoptr, " Combined Nodes:               %d\n", gccnt );
      FPRINTF( infoptr, " Combined K Imports:           %d\n", gkcnt );
      }
*/

  /*  if ( invar || cse || gcse || fold )
      CountNodesAndEdges( "AFTER GRAPH CLEANUP" ); */
    }
}
