/* if1fusion.c,v
 * Revision 12.7  1992/11/04  22:04:57  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:34  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static int good;                                   /* CURRENT LEGAL COLOR */
static int bad;                                  /* CURRENT ILLEGAL COLOR */

static int dfcnt = 0;                  /* COUNT OF DEPENDENT FUSIONS      */
static int Tdfcnt = 0;                  /* COUNT OF DEPENDENT NODES      */
static int dScNt = 0;                  /* COUNT OF FUSED SELECT EXPORTS   */
static int TdScNt = 0;                  /* COUNT OF SELECT EXPORTS   */
static int aerm  = 0;                  /* COUNT OF REMOVED AElement NODES */
static int Taerm  = 0;                  /* COUNT OF AElement NODES */

static int ifcnt = 0;                       /* COUNT OF DEPENDENT FUSIONS */
static int Tifcnt = 0;                       /* COUNT OF DEPENDENT NODES */
static int iscnt = 0;                    /* COUNT OF FUSED SELECT EXPORTS */
static int Tiscnt = 0;                    /* COUNT OF SELECT EXPORTS */
DYNDECLARE(fuseinfo, fusebuf, fuselen, fusecount, char, 2000);


static void SummarizeForall( f, lvl )
PNODE f;
int lvl;
{
  register int   fi = 0;
  register int   fe = 0;
  register int   bi = 0;
  register int   be = 0;
  register int   ri = 0;
  register int   re = 0;
  register int   bn = 0;
  register int   rn = 0;
  register PEDGE e;
  register PEDGE i;
  register PNODE n;

  for ( e = f->exp; e != NULL; e = e->esucc ) fe++;
  for ( e = f->F_BODY->exp; e != NULL; e = e->esucc ) be++;
  for ( e = f->F_RET->exp; e != NULL; e = e->esucc ) re++;

  for ( i = f->imp; i != NULL; i = i->isucc ) fi++;
  for ( i = f->F_BODY->imp; i != NULL; i = i->isucc ) bi++;
  for ( i = f->F_RET->imp; i != NULL; i = i->isucc ) ri++;

  for ( n = f->F_BODY->G_NODES; n != NULL; n = n->nsucc ) bn++;
  for ( n = f->F_RET->G_NODES; n != NULL; n = n->nsucc ) rn++;

  FPRINTF( infoptr, 
    "DEBUG FORALL[%d] [fi=%d,fe=%d][bi=%d,be=%d][ri=%d,re=%d][bn=%d,rn=%d]\n",
    lvl, fi,fe, bi,be, ri,re, bn,rn );
}

static void DumpThem( g, lvl )
PNODE g;
int lvl;
{
  register PNODE n;
  register PNODE sg;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc )
    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	DumpThem( sg, lvl+1 );

      if ( IsForall(n) )
	SummarizeForall( n, lvl );
      }
}


/**************************************************************************/
/* LOCAL  **************    RenumberForallPorts    ************************/
/**************************************************************************/
/* PURPOSE: RENUMBER THE K, M, T, AND R PORTS OF FORALL NODE n SO THAT    */
/*          THEY ARE UNIQUE ACROSS THE ENTIRE IF1 FILE.                   */
/**************************************************************************/

static void RenumberForallPorts( n )
PNODE n;
{
    register PEDGE i;

    for ( i = n->imp; i != NULL; i = i->isucc ) {              /* K PORTS */
	ChangeExportPorts( n->F_GEN,  i->iport, ++maxint );
	ChangeExportPorts( n->F_BODY, i->iport,   maxint );
	ChangeExportPorts( n->F_RET,  i->iport,   maxint );

        i->iport = maxint;
	}

    for ( i = n->F_GEN->imp; i != NULL; i = i->isucc ) {       /* M PORTS */
	ChangeExportPorts( n->F_BODY, i->iport, ++maxint );
	ChangeExportPorts( n->F_RET,  i->iport,   maxint );

        i->iport = maxint;
	}

    for ( i = n->F_BODY->imp; i != NULL; i = i->isucc ) {       /* T PORTS */
	ChangeExportPorts( n->F_RET, i->iport, ++maxint );
        i->iport = maxint;
	}

    for ( i = n->F_RET->imp; i != NULL; i = i->isucc ) {       /* R PORTS */
	ChangeExportPorts( n, i->iport, ++maxint );
	i->iport = maxint;
	}
}


/**************************************************************************/
/* LOCAL  **************      AreForallsEqual      ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF THE GENERATE SUBGRAPHS OF FORALL NODES f1 AND  */
/*          f2 ARE EQUAL.                                                 */
/**************************************************************************/

static int AreForallsEqual( f1, f2, indep )
PNODE f1;
PNODE f2;
int   indep;
{
    register PNODE n1;
    register PNODE n2;
    register int   s;

    /* A VECTOR CANDIDATE AND A NON-VECTOR CANDIDATE ARE NOT EQUAL */
    /* SUCH FUSION MAY HURT VECTORIZATION */
    s = 0;

    if ( OptIsVecCandidate( f1 ) ) s++;
    if ( OptIsVecCandidate( f2 ) ) s++;

    /* VECTORIZATION COMPLEXITY THROTTLE */
    if ( AggressiveVectors ) {
      /* The default for CRAYS assumes that its better to */
      /* independently vectorize.  If we want aggressive vectors, we */
      /* allow fusion when vectorizing, independent, and */
      /* (either or both are vector candidates) */
      if ( vec && indep && (s==0) ) return FALSE;

    } else {
      if ( vec && indep && (s > 0) )
	return( FALSE );

      if ( vec && (s == 1) ) {
        return( FALSE );
      }
    }

    n1 = f1->F_GEN->G_NODES;
    n2 = f2->F_GEN->G_NODES;

    if ( n1 == NULL || n2 == NULL ) {
      return( FALSE );
      }

    if ( n1->nsucc != NULL || n2->nsucc != NULL ) {
      return( FALSE );
      }

    if ( AreNodesEqual(n1,n2) ) {
      return( TRUE );
      }

    return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************    RenumberSelectPorts    ************************/
/**************************************************************************/
/* PURPOSE: RENUMBER THE PORT NUMBERS OF SELECT NODE n.                   */
/**************************************************************************/

static void RenumberSelectPorts( n )
PNODE n;
{
  register PEDGE i;
  register int   iport;

  for ( i = n->imp; i != NULL; i = i->isucc ) {              /* K PORTS */
    ChangeExportPorts( n->S_ALT,  i->iport, ++maxint );
    ChangeExportPorts( n->S_CONS, i->iport,   maxint );

    i->iport = maxint;
    }

  for ( i = n->S_ALT->imp; i != NULL; i = i->isucc ) {       /* R PORTS */
    iport = i->iport;

    ChangeExportPorts( n, iport, ++maxint );

    ChangeImportPorts( n->S_ALT,  iport, maxint );
    ChangeImportPorts( n->S_CONS, iport, maxint );
    }
}


/**************************************************************************/
/* LOCAL  **************  LinkSelectDependencies   ************************/
/**************************************************************************/
/* PURPOSE: LINK THE DEPENDENCIES BETWEEN THE SUBGRAPHS OF SELECT NODE s1 */
/*          AND s2.                                                       */
/**************************************************************************/

static void LinkSelectDependencies( s1, s2 )
PNODE s1;
PNODE s2;
{
  register PEDGE e;
  register PEDGE se;
  register PEDGE i;
  register PEDGE si;

  /* REWIRE THE s2 TO s1 ALTERNATE DEPENDENCIES */
  for ( e = s2->S_ALT->exp; e != NULL; e = se ) {
    se = e->esucc;

    i = FindImport( s2, e->eport );

    if ( i->src != s1 )
      continue;

    i = FindImport( s1->S_ALT, i->eport );
    UnlinkExport( e );

    if ( IsConst( i ) )
      ChangeToConst( e, i );
    else {
      e->eport = i->eport;
      LinkExport( i->src, e );
      }
    }

  /* REWIRE THE s2 TO s1 CONSEQUENT DEPENDENCIES */
  for ( e = s2->S_CONS->exp; e != NULL; e = se ) {
    se = e->esucc;

    i = FindImport( s2, e->eport );

    if ( i->src != s1 )
      continue;

    i = FindImport( s1->S_CONS, i->eport );
    UnlinkExport( e );

    if ( IsConst( i ) )
      ChangeToConst( e, i );
    else {
      e->eport = i->eport;
      LinkExport( i->src, e );
      }
    }

  /* UNLINK ALL THE DEPENDENCIES BETWEEN s1 and s2 */
  for ( i = s2->imp; i != NULL; i = si ) {
    si = i->isucc;

    if ( i->src != s1 )
      continue;

    UnlinkImport( i );
    UnlinkExport( i );
    }
}


/**************************************************************************/
/* LOCAL  **************    DoTheSelectFusion       ************************/
/**************************************************************************/
/* PURPOSE: FUSE s1 AND s2 IN GRAPH g, AND REMOVE s2.                     */
/**************************************************************************/

static void DoTheSelectFusion( g, s1, s2 )
PNODE g;
PNODE s1;
PNODE s2;
{
  RenumberSelectPorts( s2 );

  LinkSelectDependencies( s1, s2 );

  LinkImportLists( s1, s2 );
  LinkExportLists( s1, s2 );

  LinkImportLists( s1->S_ALT, s2->S_ALT );
  LinkExportLists( s1->S_ALT, s2->S_ALT );
  LinkNodeLists( s1->S_ALT, s2->S_ALT );

  LinkImportLists( s1->S_CONS, s2->S_CONS );
  LinkExportLists( s1->S_CONS, s2->S_CONS );
  LinkNodeLists( s1->S_CONS, s2->S_CONS );

  UnlinkNode( s2 );

  /* free( s2->S_TEST  ); */
  /* free( s2->S_ALT   ); */
  /* free( s2->S_CONS  ); */
  /* free( s2 );  */
}


/**************************************************************************/
/* LOCAL  **************     TryAndFuseSelects     ************************/
/**************************************************************************/
/* PURPOSE: TRY AND FUSE Select NODES s1 AND s2, REMOVING s2 FROM GRAPH g.*/
/**************************************************************************/

static int TryAndFuseSelects( g, s1, s2 )
PNODE s1;
PNODE s2;
PNODE g;
{
  if ( !sfuse )
    return( FALSE );

  if ( !AreValuesEqual( s1->S_TEST->imp, s2->S_TEST->imp ) )
    return( FALSE );

  DoTheSelectFusion( g, s1, s2 ); 
  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************   AssignSelectAttributes  ************************/
/**************************************************************************/
/* PURPOSE: IF SELECT NODE n IS PART OF A LEGAL DEPENDENCY CHAIN THEN     */
/*          NOTE IF ITS IMMEDAITE DECENDENTS ARE ALSO PART OF THE SAME    */
/*          LEGAL CHAIN.                                                  */
/**************************************************************************/

static void AssignSelectAttributes( n )
PNODE n;
{
  register PEDGE e;
  register int   c;
  register PNODE dst;

  c = n->label;

  /* NOT PART OF THE CURRENT DEPENDENCY CHAIN */
  if ( c < good )
    return;

  /* IF n IS PART OF AN ILLEGAL DEPENDENCY CHAIN THEN BLINDLY PROPOGATE */
  /* THE CONDITION TO n'S DECENDENTS.                                   */
  if ( c == bad ) {
    for ( e = n->exp; e != NULL; e = e->esucc )
      if ( !IsGraph( e->dst ) )
        e->dst->label = c;

    return;
    }

  /* n IS CURRENTLY PART OF A LEGAL DEPENDENCY CHAIN, SO TRY AND */
  /* PROPAGATE THE CONDITION, BUT BE CAREFUL!!! */
  for ( e = n->exp; e != NULL; e = e->esucc ) {
    dst = e->dst;

    /* ONLY DIRECT Select TO Select DEPENDENCIES ARE ALLOWED */
    c = (IsSelect(dst))? good : bad;

    if ( !IsGraph( dst ) )
      if ( dst->label != bad )
	dst->label = c;
    }
}


/**************************************************************************/
/* LOCAL  **************    CleanDependentPaths    ************************/
/**************************************************************************/
/* PURPOSE: REMOVE THE DEAD CODE BETWEEN Forall NODES f1 AND f2 BY        */
/*          REMOVING f2's IMPORTS FROM f1 AND WIRING IT TO g USING        */
/*          IMPORT PORT 0.                                                */
/**************************************************************************/

static void CleanDependentPaths( g, f2 )
register PNODE g;
register PNODE f2;
{
  register PEDGE si;
  register PEDGE i;
  register PNODE ag;

  for ( i = f2->imp; i != NULL; i = si ){
    si = i->isucc;

    if ( i->uedge == NULL )
      continue;

    if ( IsForall( i->src ) ) {
      UnlinkImport( i );
      UnlinkExport( i );

      if ( !IsExport( i->src, i->eport ) ) {
	ag = i->uedge->dst;

	UnlinkExport( i->uedge );
	UnlinkExport( ag->imp );
	UnlinkImport( ag->exp );
	UnlinkNode( ag );
	}

      continue;
      }

    UnlinkImport( i );
    i->iport = 0;
    LinkImport( g, i );
    }
}


/**************************************************************************/
/* LOCAL  **************       ChangeDstPath       ************************/
/**************************************************************************/
/* PURPOSE: CHANGE ALL REFERENCES TO THE ARRAY CONTAINING VALUE i IN      */
/*          SUBGRAPH g INTO DIRECT REFERENCES TO i.                       */
/**************************************************************************/

static void ChangeDstPath( g, i )
PNODE g;
PEDGE i;
{
  register PEDGE e;
  register PEDGE ee;
  register PNODE n;
  register PEDGE se;
  register PEDGE see;
  register PNODE sg;

  for ( e = g->exp; e != NULL; e = se ) {
    se = e->esucc;

    ++Taerm;

    if ( e->eport != i->iport )
      continue;

    e->info = e->info->A_ELEM;

    n = e->dst;

    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	ChangeDstPath( sg, e );

      continue;
      }

    for ( ee = n->exp; ee != NULL; ee = see ) {
      see = ee->esucc;
      UnlinkExport( ee );

      ee->eport = e->eport;
      /* THIS WILL ONLY WORK IF LinkExport LINKS TO THE HEAD OF g->exp */
      /* WHICH IT DOES!!!            **  WARNING **                    */
      LinkExport( g, ee );
      }

    UnlinkExport( n->imp->isucc ); /* THIS MIGHT REMOVE se SAVED ABOVE */
    se = e->esucc;
    UnlinkExport( n->imp );
    UnlinkNode( n );
    /* free( n->imp->isucc ); */
    /* free( n->imp ); */
    /* free( n ); */
    aerm++;
    }
}


/**************************************************************************/
/* LOCAL  **************    DoTheForallMerge       ************************/
/**************************************************************************/
/* PURPOSE: MERGE Forall NODE f1 WITH f2 AND REMOVE f2 FROM GRAPH g.      */
/*          THE MERGE WILL NOT BREAK g'S DATA FLOW ORDERING, BECAUSE THE  */
/*          PREDECESSORS OF f2 WERE MOVED TO THE LEFT OF f1 DURING THE    */
/*          SEARCH FOR f1.                                                */
/**************************************************************************/

static void  DoTheForallMerge( g, f1, f2, indep )
PNODE g;
PNODE f1;
PNODE f2;
int   indep;                          /* INDEPENDENT OR DEPENDENT FUSION */
{
  register PEDGE i;
  register PEDGE ii;
  register PEDGE e;
  register PEDGE se;
  register PEDGE idx1;

  RenumberForallPorts( f2 );

  idx1 = f1->F_GEN->imp;

  /* MAKE SURE REFERENCES TO f2's CONTROL ROD REFERENCE f1's CONTROL ROD */
  ChangeExportPorts( f2->F_BODY, f2->F_GEN->imp->iport, idx1->iport );
  ChangeExportPorts( f2->F_RET,  f2->F_GEN->imp->iport, idx1->iport );

  /* WIRE THE DEPENDENCIES BETWEEN f1 AND f2 */
  if ( !indep ) {
    for ( i = f2->imp; i != NULL; i = i->isucc )
      if ( i->uedge != NULL )
        ChangeDstPath( f2->F_BODY, i );

    for ( e = f2->F_BODY->exp; e != NULL; e = se ) {
      se = e->esucc;

      if ( (i = FindImport( f2, e->eport )) == NULL )
        continue;

      if ( i->uedge == NULL )
        continue;

      /* CHANGE e to an i->uedge REFERENCE */
      UnlinkExport( e );

      if ( IsConst( i->uedge ) ) {
        ChangeToConst( e, i->uedge );
        }

      else if ( (ii = FindImport( f1, i->uedge->eport )) != NULL ) {     /* K */
        e->eport = ii->iport;
        LinkExport( f1->F_BODY, e );
        }

      else if ( i->uedge->eport == idx1->iport ) {          /* idx1 REFERENCE */
        e->eport = idx1->iport;
        LinkExport( f1->F_BODY, e );
        }

      else {                                                 /* BUILT IN BODY */
        ii = FindImport( f1->F_BODY, i->uedge->eport );

        if ( IsConst( ii ) )
	  ChangeToConst( e, ii );
        else {
	  e->eport = ii->eport;
	  LinkExport( ii->src, e );
	  }
        }
      }

    CleanDependentPaths( g, f2 );
    }

  /* DO THE MERGE */
  LinkImportLists( f1, f2 );
  LinkExportLists( f1, f2 );

  LinkImportLists( f1->F_BODY, f2->F_BODY );

  LinkExportLists( f1->F_BODY, f2->F_BODY );
  LinkNodeLists(   f1->F_BODY, f2->F_BODY );

  LinkImportLists( f1->F_RET, f2->F_RET );
  LinkExportLists( f1->F_RET, f2->F_RET );
  LinkNodeLists(   f1->F_RET, f2->F_RET );

  /* CombineKports( f1 ); */
  /* free( f2->F_GEN ); */
  /* free( f2->F_BODY ); */
  /* free( f2->F_RET ); */
  UnlinkNode( f2 );
  /* free( f2 ); */
}


/**************************************************************************/
/* LOCAL  **************      IsIndexingOk         ************************/
/**************************************************************************/
/* PURPOSE: ARE e AND idx REFERENCES TO THE SAME VALUE.                   */
/**************************************************************************/

static int IsIndexingOk( e, idx )
PEDGE e;
PEDGE idx;
{
  if ( e == NULL )
    return( FALSE );

  if ( IsConst( e ) )
    return( FALSE );

  switch ( e->src->type ) {
    case IFSGraph:
      if ( e->src == idx->src ) {
	if ( e->eport == idx->eport )
	  return( TRUE );

	return( FALSE );
	}

      return( IsIndexingOk( FindImport( e->src->G_DAD, e->eport ), idx ) );

    default:
      return( FALSE );
    }
}


/**************************************************************************/
/* LOCAL  **************        IsAReadOnly        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF ARRAY a IS READ ONLY IN GRAPH b AND ONLY       */
/*          INDEXED BY idx. READ ONLY IMPLIES THAT IT IS ONLY USED BY     */
/*          AElement NODES.                                               */
/**************************************************************************/

static int IsAReadOnly( b, a, idx )
PNODE b;
PEDGE a;
PEDGE idx;
{
  register PEDGE e;
  register PNODE sg;

  if ( a == NULL )
    return( TRUE );

  for ( e = b->exp; e != NULL; e = e->esucc ) {
    if ( e->eport != a->eport )
      continue;

    if( e->dst->type==IFAElement ) {
        if ( !IsIndexingOk( e->dst->imp->isucc, idx ) )
	  return( FALSE );
    } else if ( IsCompound( e->dst ) ) {
	for ( sg = e->dst->C_SUBS; sg != NULL; sg = sg->gsucc )
          if ( !IsAReadOnly( sg, FindExport( sg, e->iport ), idx ) )
	    return( FALSE );
    } else {
	return( FALSE );
    }
  }

  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************     TryAndFuseForalls     ************************/
/**************************************************************************/
/* PURPOSE: TRY AND FUSE Forall NODES f1 AND f2, REMOVING f2 FROM GRAPH g.*/
/**************************************************************************/

static int TryAndFuseForalls( g, f1, f2, indep, ReasonP )
PNODE g;
PNODE f1;
PNODE f2;
int   indep;                          /* INDEPENDENT OR DEPENDENT FUSION */
char **ReasonP;
{
  register PEDGE i;
  register PEDGE ii;
  register PEDGE j;
  register PEDGE idx2;
  register PEDGE a;

  /* ------------------------------------------------------------ */
  /* SEE IF ISOMORPHIC, 1 GENERATOR NODE (A RANGE GENERATOR) */
  if ( !AreForallsEqual( f1, f2, indep ) ) {
    *ReasonP = (RequestInfo(I_Info1,info))?"Cannot prove equivalence":NULL;
    return( FALSE );
  }

  /* ------------------------------------------------------------ */
  if ( f1->F_GEN->G_NODES == NULL ) {
    *ReasonP = "First loop has no generator";
    return( FALSE );
  }

  /* ------------------------------------------------------------ */
  if ( f1->F_GEN->G_NODES->nsucc != NULL ) {
    *ReasonP = "First loop has multiple generators";
    return( FALSE );
  }

  /* ------------------------------------------------------------ */
  if ( f1->F_GEN->G_NODES->type != IFRangeGenerate ) {
    *ReasonP = "Loop not controled with an integer count";
    return( FALSE );
  }

  /* ------------------------------------------------------------ */
  if ( indep ) {
    DoTheForallMerge( g, f1, f2, TRUE );
    return( TRUE );
    }

  /* ------------------------------------------------------------ */
  /* Must be dependent */
  /* FIND A REFERENCE TO f2's CONTROL ROD IN f2'S BODY */
  idx2 = f2->F_GEN->imp;
  idx2 = FindExport( f2->F_BODY, idx2->iport );

  if ( idx2 == NULL )
    return( FALSE );

  for ( i = f2->imp; i != NULL; i = i->isucc ) {
    i->uedge = NULL;

    if ( IsConst( i ) )
      continue;

    if ( i->src->label != good )
      continue;

    /* MUST BE REFERENCED IN THE BODY ONLY */
    /* WE ALREADY KNOW IT CANNOT BE REFERENCED IN THE GENERATE SUBGRAPH */
    if ( !IsExport( f2->F_BODY, i->iport ) ) {
      *ReasonP = "Dependence var not used in body of second loop";
      return( FALSE );
    }

    if ( IsExport( f2->F_RET, i->iport ) ) {
      *ReasonP = "Dependence var used in returns clause of second loop";
      return( FALSE );
    }

    /* FIND THE f1 EXPORT CORRESPONDING TO i. IT IS EITHER A DIRECT  */
    /* REFERENCE OR A STRING OF AAddH OR AAddL NODES */
    for ( j = i; j->src != f1; j = j->src->imp ) ;

    /* MAKE SURE f1 IS BUILDING j USING AN AGather NODE */
    if ( (ii = FindImport( f1->F_RET, j->eport )) == NULL ) {
      *ReasonP = "Dependence var must be formed in a return array clause";
      return( FALSE );
    }
    if ( ii->src->type != IFAGather ) {
      *ReasonP = "Dependence var must be formed in a return array clause";
      return( FALSE );
    }

    /* FILTERED? */
    if ( ii->src->imp->isucc->isucc != NULL ) {
      *ReasonP = "Dependence var must be formed by an unfiltered return clause";
      return( FALSE );
    }

    /* MAKE SURE i IS ONLY READ IN f2 AND IN THE SAME ORDER AS IT WAS BUILT */
    if ( (a = FindExport( f2->F_BODY, i->iport )) == NULL ) {
      *ReasonP = "Dependence var is not read only in second loop";
      return( FALSE );
    }

    if ( !IsAReadOnly( f2->F_BODY, a, idx2 ) ) {
      *ReasonP = "Dependence var is not read only in second loop";
      return( FALSE );
    }

    i->uedge = ii->src->imp->isucc;
  }

  DoTheForallMerge( g, f1, f2, FALSE );
  *ReasonP = "Successful";
  return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************   AssignForallAttributes  ************************/
/**************************************************************************/
/* PURPOSE: IF NODE n IS PART OF A LEGAL DEPENDENCY CHAIN THEN NOTE IF    */
/*          ITS IMMEDAITE DECENDENTS ARE ALSO PART OF THE SAME LEGAL      */
/*          CHAIN.                                                        */
/**************************************************************************/

static void AssignForallAttributes( n )
PNODE n;
{
  register PEDGE e;
  register int   c;
  register PNODE dst;

  c = n->label;

  /* NOT PART OF THE CURRENT DEPENDENCY CHAIN */
  if ( c < good )
    return;

  /* IF n IS PART OF AN ILLEGAL DEPENDENCY CHAIN THEN BLINDLY PROPOGATE */
  /* THE CONDITION TO n'S DECENDENTS.                                   */
  if ( c == bad ) {
    for ( e = n->exp; e != NULL; e = e->esucc )
      if ( !IsGraph( e->dst ) )
        e->dst->label = c;

    return;
    }

  /* n IS CURRENTLY PART OF A LEGAL DEPENDENCY CHAIN, SO TRY AND */
  /* PROPAGATE THE CONDITION, BUT BE CAREFUL!!! */
  for ( e = n->exp; e != NULL; e = e->esucc ) {
    dst = e->dst;

    switch ( dst->type ) {
      case IFAAddH:
      case IFAAddL:
	c = (e->iport == 1)? good : bad;
	break;

      case IFForall:
	c = (IsArray(e->info))? good : bad; 
	break;

      default:
	c = bad;
	break;
      }

    if ( !IsGraph( dst ) )
      if ( dst->label != bad )
	dst->label = c;
    }
}


/**************************************************************************/
/* LOCAL  **************      DependentFusion      ************************/
/**************************************************************************/
/* PURPOSE: FUSE THE DEPENDENT NODES IN GRAPH g.                          */
/**************************************************************************/
static void DependentFusion( g )
PNODE g;
{
  register PNODE f1;
  register PNODE f2;
  register PEDGE e;
  register PNODE f2s;
  register PNODE f1s;
  register PNODE dst;
  register PNODE sg;
  char		 *Reason;

  for ( f1 = g->G_NODES; f1 != NULL; /* DONE IN THE BODY */ ) {
    if ( IsCompound( f1 ) ) {
      switch ( f1->type ) {
      case IFForall:
	++Tdfcnt;
	if ( f1->label < 0 ) {       /* ARE WE ALL DONE PROCESSING f1? */
	  f1->label = -(f1->label);
	  break;
	  }

	/* SHOULD Forall FUSION EVEN BE TRIED!!! */
	for ( e = f1->exp; e != NULL; e = e->esucc ) {
	  dst = e->dst;

	  if ( IsForall( dst ) )
	    break;

	  if ( dst->type == IFAAddH )
	    break;

	  if ( dst->type == IFAAddL )
	    break;
	  }

	if ( e != NULL ) /* YES, TRY IT!!! */
	  goto ContinueTheFusion;

	DependentFusion( f1->F_BODY );
	break;

      case IFSelect:
        ++TdScNt;
	if ( f1->label < 0 ) {       /* ARE WE ALL DONE PROCESSING f1? */
	  f1->label = -(f1->label);
	  break;
	  }

	goto ContinueTheFusion;

      case IFTagCase:
	for ( sg = f1->C_SUBS; sg != NULL; sg = sg->gsucc )
	  DependentFusion( sg );

	break;

      case IFLoopA:
      case IFLoopB:
	DependentFusion( f1->L_BODY );
	break;

      case IFUReduce:		/* TBD: Check this fusion? */
	DependentFusion( f1->R_BODY );
	break;

      default:
	UNEXPECTED("Unknown compound");
      }
    }

    f1 = f1->nsucc;
    continue;

ContinueTheFusion:
    f1s = NULL;

    good = ++maxint;
    bad  = ++maxint;

    f1->label = good; /* ESTABLISH THE SEARCH DOMAIN */

    if ( IsSelect( f1 ) )
      AssignSelectAttributes( f1 );
    else
      AssignForallAttributes( f1 );

    for ( f2 = f1->nsucc; f2 != NULL; f2 = f2s ) {
      f2s = f2->nsucc;

      if ( IsForall( f1 ) )
        /* A Forall THAT WAS NOT COMPLETELY PROCESSED AT AN EARLIER TIME? */
        if ( IsForall( f2 ) && (f2->label >= 0)  ) {
	  if ( f2->label == good ) {     /* ARE THE DEPENDENCIES CORRECT? */
	    /* ------------------------------------------------------------ */
            if ( TryAndFuseForalls( g, f1, f2, FALSE,&Reason ) ) {
	      /* FUSION worked! */
              if(RequestInfo(I_Info1,info)) {
              DYNEXPAND(fuseinfo,fusebuf,fuselen,fusecount,char,fuselen+300);
              fuselen += (SPRINTF(fuseinfo + fuselen,
			" Fusing loop %d at line %d, funct %s, file %s\n",
			f1->ID, f1->line, f1->funct, f1->file), 
                strlen(fuseinfo + fuselen));
              fuselen += (SPRINTF(fuseinfo + fuselen,
			" with loop %d at line %d, funct %s, file %s\n\n",
			f2->ID, f2->line, f2->funct, f2->file),
                strlen(fuseinfo + fuselen));
	      }
	      dfcnt++;
	      fchange = TRUE;
	      break;
	    } 

	    f2->label = bad; /* IT WAS GOOD, BUT OBVIOUSLY IT NO LONGER IS */
	    }
          }

      if ( IsSelect( f1 ) )
        /* A Select THAT WAS NOT COMPLETELY PROCESSED AT AN EARLIER TIME? */
        if ( IsSelect( f2 ) && (f2->label >= 0)  ) {
	  if ( f2->label == good ) { /* ARE THE DEPENDENCIES CORRECT? */
            if ( TryAndFuseSelects( g, f1, f2 ) ) {
	      dScNt++;
	      fchange = TRUE;
	      break;
	      }

	    f2->label = bad; /* IT WAS GOOD, BUT OBVIOUSLY IT NO LONGER IS */
	    }
	  }

      /* FUSION WAS NOT DONE, SO GET READY FOR THE NEXT STEP */

      if ( f2->label < good ) {      /* NOT A DECENDENT OF f1? */
	UnlinkNode( f2 ); 
	LinkNode( f1->npred, f2 );

	/* SHOULD WE START WITH f2 IN THE NEXT ITERATION OF THE OUTER LOOP? */
	if ( f1s == NULL )
	  if ( IsCompound( f2 ) )
	    f1s = f2;
      } else {
        if ( IsSelect( f1 ) )
          AssignSelectAttributes( f2 );
        else
          AssignForallAttributes( f2 );
        }
      }

    if ( f2 == NULL ) { /* WAS FUSION DONE? */
      if ( f1s != NULL ) /* BUT WILL WE ENCOUNTER f1 AGAIN? */
	f1->label = -(f1->label);
      else {
	f1s = f1->nsucc; /* NO, SO PUT f1 BEHIND US, AND MOVE ON */
	}

      /* FINISH UP f1, AS WE WILL NEVER PROCESS IT AGAIN */
      if ( IsSelect( f1 ) ) {
	DependentFusion( f1->S_ALT );
	DependentFusion( f1->S_CONS );
	}
      else {
        DependentFusion( f1->F_BODY );
        /* CombineKports( f1 ); */
	}

      f1 = f1s;
      continue;
      }

    /* FUSION WAS DONE */

    /* START AGAIN WITH A PREVIOUSLY MOVED FUSION CANDIDATE? */
    if ( f1s != NULL ) 
      f1 = f1s;
    }
}


/**************************************************************************/
/* GLOBAL **************        If1DFusion         ************************/
/**************************************************************************/
/* PURPOSE: APPLY DEPENDENT FUSION TO ALL THE NODES IN THE PROGRAM.       */
/**************************************************************************/

void If1DFusion()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    /* DumpThem( f, 1 ); */
    DependentFusion( f );
    FastCleanGraph( f );
    }
}


/**************************************************************************/
/* LOCAL  **************      AssignAttributes     ************************/
/**************************************************************************/
/* PURPOSE: PROPAGATE THE LABEL OF n TO ALL ITS DECENDENTS.               */
/**************************************************************************/

static void AssignAttributes( n )
PNODE n;
{
  register PEDGE e;
  register int   c;

  c = n->label;

  /* PART OF THE CURRENT DEPENDENCY CHAIN? */
  if ( c != good )
    return;

  for ( e = n->exp; e != NULL; e = e->esucc )
    if ( !IsGraph( e->dst ) )
      e->dst->label = c;
}


/**************************************************************************/
/* LOCAL  **************     IndependentFusion     ************************/
/**************************************************************************/
/* PURPOSE: FUSE THE INDEPENDENT NODES IN GRAPH g.                        */
/**************************************************************************/

static void IndependentFusion( g )
PNODE g;
{
  register PNODE f1;
  register PNODE f2;
  register PNODE f2s;
  register PNODE f1s;
  register PNODE sg;
  char		 *Reason;

  for ( f1 = g->G_NODES; f1 != NULL; /* DONE IN THE BODY */ ) {
    if ( IsCompound( f1 ) ) {
      switch ( f1->type ) {
      case IFForall:  
        ++Tifcnt;	/* DROP THROUGH */
      case IFSelect:
        ++Tiscnt;
	if ( f1->label < 0 ) {       /* ARE WE ALL DONE PROCESSING f1? */
	  f1->label = -(f1->label);
	  break;
	  }

	goto ContinueTheFusion;

      case IFTagCase:
	for ( sg = f1->C_SUBS; sg != NULL; sg = sg->gsucc )
	  IndependentFusion( sg );

	break;

      case IFLoopA:
      case IFLoopB:
	IndependentFusion( f1->L_BODY );
	break;

      case IFUReduce:		/* TBD: Check this fusion? */
	IndependentFusion( f1->R_BODY );
	break;

      default:
	UNEXPECTED("Unknown compound");
      }
    }

    f1 = f1->nsucc;
    continue;

ContinueTheFusion:
    f1s = NULL;

    good = ++maxint;
    bad  = ++maxint;

    f1->label = good; /* ESTABLISH THE SEARCH DOMAIN */

    AssignAttributes( f1 );

    for ( f2 = f1->nsucc; f2 != NULL; f2 = f2s ) {
      f2s = f2->nsucc;

      if ( IsForall( f1 ) )
        /* A Forall THAT WAS NOT COMPLETELY PROCESSED AT AN EARLIER TIME? */
        if ( IsForall( f2 ) && (f2->label >= 0)  )
	  if ( f2->label != good )       /* ARE THE DEPENDENCIES CORRECT? */
	    /* ------------------------------------------------------------ */
            if ( TryAndFuseForalls( g, f1, f2, TRUE,&Reason ) ) {
	      /* FUSION worked! */
              if (RequestInfo(I_Info1,info)) {
              DYNEXPAND(fuseinfo,fusebuf,fuselen,fusecount,char,fuselen+300);
              fuselen += (SPRINTF(fuseinfo + fuselen,
			" Fusing loop %d at line %d, funct %s, file %s\n",
			 f1->ID, f1->line, f1->funct, f1->file), 
                strlen(fuseinfo + fuselen));
              fuselen += (SPRINTF(fuseinfo + fuselen,
			" with loop %d at line %d, funct %s, file %s\n\n",
			 f2->ID, f2->line, f2->funct, f2->file),
                strlen(fuseinfo + fuselen));
	      }
	      ifcnt++;
	      fchange = TRUE;
	      break;
	    } 

      if ( IsSelect( f1 ) )
        /* A Select THAT WAS NOT COMPLETELY PROCESSED AT AN EARLIER TIME? */
        if ( IsSelect( f2 ) && (f2->label >= 0)  )
	  if ( f2->label != good )       /* ARE THE DEPENDENCIES CORRECT? */
            if ( TryAndFuseSelects( g, f1, f2 ) ) {
	      iscnt++;
	      fchange = TRUE;
	      break;
	      }

      /* FUSION WAS NOT DONE, SO GET READY FOR THE NEXT STEP */

      if ( f2->label != good ) {      /* NOT A DECENDENT OF f1? */
	UnlinkNode( f2 ); 
	LinkNode( f1->npred, f2 );

	/* SHOULD WE START WITH f2 IN THE NEXT ITERATION OF THE OUTER LOOP? */
	if ( f1s == NULL )
	  if ( IsCompound( f2 ) )
	    f1s = f2;
	}
      else
        AssignAttributes( f2 );
      }

    if ( f2 == NULL ) { /* WAS FUSION DONE? */
      if ( f1s != NULL ) /* BUT WILL WE ENCOUNTER f1 AGAIN? */
	f1->label = -(f1->label);
      else
	f1s = f1->nsucc; /* NO, SO PUT f1 BEHIND US, AND MOVE ON */

      /* CLEAN UP f1, AS WE WILL NEVER PROCESS IT AGAIN */
      if ( IsSelect( f1 ) ) {
	IndependentFusion( f1->S_ALT );
	IndependentFusion( f1->S_CONS );
	}
      else
        IndependentFusion( f1->F_BODY );

      f1 = f1s;
      continue;
      }

    /* FUSION WAS DONE */

    /* START AGAIN WITH A PREVIOUSLY MOVED FUSION CANDIDATE? */
    if ( f1s != NULL ) 
      f1 = f1s;
    }
}


/**************************************************************************/
/* GLOBAL **************      WriteFusionInfo      ************************/
/**************************************************************************/
/* PURPOSE: WRITE FUSION INFORMATION TO stderr.                           */
/**************************************************************************/

void WriteFusionInfo()
{
  FPRINTF( infoptr, "\n\n **** LOOP FUSION\n\n%s\n" ,fuseinfo);
  DYNFREE(fuseinfo, fusebuf, fuselen, fusecount, char, NULL);
  FPRINTF( infoptr, " **** INDEPENDENT NODE FUSION\n\n" );
  FPRINTF( infoptr, " Fused Independent Foralls:     %d of %d\n", ifcnt,Tifcnt );
  FPRINTF( infoptr, " Fused Independent Selects:     %d of %d\n", iscnt,Tiscnt );

  FPRINTF( infoptr, "\n **** DEPENDENT NODE FUSION\n\n" );
  FPRINTF( infoptr, " Fused Dependent Foralls:     %d of %d\n", dfcnt,Tdfcnt );
  FPRINTF( infoptr, " Removed AElement Nodes:      %d of %d\n", aerm,Taerm  );
  FPRINTF( infoptr, " Fused Dependent Selects:     %d of %d\n", dScNt,TdScNt );
}


/**************************************************************************/
/* GLOBAL **************        If1IFusion         ************************/
/**************************************************************************/
/* PURPOSE: APPLY INDEPENDENT FUSION TO ALL THE NODES IN THE PROGRAM.     */
/**************************************************************************/

void If1IFusion()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    IndependentFusion( f );
    FastCleanGraph( f );
    }
}
