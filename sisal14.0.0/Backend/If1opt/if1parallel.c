/* if1parallel.c,v
 * Revision 12.7  1992/11/04  22:04:58  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:36  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


static int   vfcnt = 0;             /* COUNT OF GENERATED VECTOR FORALLS  */
static int   Tvfcnt = 0;             /* COUNT OF VECTOR FORALLS  */
static int   cfcnt = 0;         /* COUNT OF GENERATED CONCURRENT FORALLS  */
static int   Tcfcnt = 0;         /* COUNT OF CONCURRENT FORALLS  */
static int   mcnt  = 0;                           /* COUNT OF MOVED NODES */
static int   Tmcnt  = 0;                           /* COUNT OF NODES */
static int   scnt  = 0;                  /* COUNT OF SPLIT AElement NODES */
static int   Tscnt  = 0;                  /* COUNT OF AElement NODES */
static char printinfo[2000]; 


/**************************************************************************/
/* LOCAL  **************     MoveTheNegNodes       ************************/
/**************************************************************************/
/* PURPOSE: MOVE THE NODES IN SUBGRAPH src THAT HAVE NEGATIVE LABELS TO   */
/*          SUBGRAPH dst, AND WIRE THE NON-NEGATIVE SOURCE EDGES TO dst.  */
/**************************************************************************/

static void MoveTheNegNodes( src, dst )
PNODE src;
PNODE dst;
{
  register PNODE n;
  register PNODE pn;
  register PNODE sn;
  register PEDGE i;

  pn = dst;

  for ( n = src->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    if ( n->label >= 0 )
      continue;

    UnlinkNode( n );
    LinkNode( pn, n );
    pn = n;

    for ( i = n->imp; i != NULL; i = i->isucc ) {
      if ( IsConst( i ) )
	continue;

      if ( !IsSGraph( i->src ) )
	continue;

      UnlinkExport( i );
      LinkExport( dst, i );
      }
    }
}


/**************************************************************************/
/* LOCAL  **************    WireNegExports         ************************/
/**************************************************************************/
/* PURPOSE: WIRE THE NEG NODES NOW IN FORALL NODE f2 THAT ARE DIRECTLY    */
/*          REFERENCED IN BODY SUBGRAPH src WHICH HAS A CONTROL ROD WITH  */
/*          PORT NUMBER cport.                                            */
/**************************************************************************/

static void WireNegExports( f2, src, crod )
PNODE f2;
PNODE src;
PEDGE crod;
{
  register PNODE n;
  register PEDGE e;
  register PEDGE ee;
  register PEDGE se;
  register PNODE aelm;
  register PNODE gat;
  register int   port;
  register int   eport;
  register PINFO minfo;
  register PINFO ainfo;
  register PINFO TheInfo;

  f2->F_BODY->label = -1;

  for ( n = f2->F_BODY; n != NULL; n = n->nsucc ) {
StartOver:
    for ( e = n->exp; e != NULL; e = se ) {
      se = e->esucc;

      if ( e->dst->label < 0 )
	continue;

      port  = ++maxint;
      eport = e->eport;

      TheInfo  = e->info;

      ainfo = FindInfo( ++maxint, IF_ARRAY );
      ainfo->A_ELEM = TheInfo;

      minfo = FindInfo( ++maxint, IF_MULTIPLE );
      minfo->A_ELEM = TheInfo;

      /* ALLOCATE THE AElement NODE */
      aelm = NodeAlloc( ++maxint, IFAElement );
      CopyVitals( n, aelm );
      LinkNode( src, aelm );

      /* LINK IN THE AElement CONTROL ROD REFERENCE */
      ee = EdgeAlloc( src, crod->iport, aelm, 2 );
      ee->info = crod->info;
      LinkExport( src, ee );
      LinkImport( aelm, ee );

      for ( ee = e; ee != NULL; ee = se ) {
	se = ee->esucc;

	if ( ee->eport != eport )
	  continue;

	if ( ee->dst->label < 0 )
	  continue;

	UnlinkExport( ee );
	ee->eport = 1;
	LinkExport( aelm, ee );
	}

      /* WIRE n TO f2->BODY USING PORT */
      ee = EdgeAlloc( n, eport, f2->F_BODY, port );
      ee->info = TheInfo;
      LinkExport( n, ee );
      LinkImport( f2->F_BODY, ee );

      /* ALLOCATE THE AGather NODE */
      gat = NodeAlloc( ++maxint, IFAGather );
      CopyVitals( f2, aelm );
      LinkNode( f2->F_RET, gat );

      /* LINK THE LOWER BOUND TO THE AGather NODE */
      CopyEdgeAndReset( f2->F_GEN->imp->src->imp, f2->F_RET, gat );
      gat->imp->iport = 1;

      /* LINK THE IMPORT VALUE TO THE AGather NODE */
      ee = EdgeAlloc( f2->F_RET, port, gat, 2 );
      ee->info = minfo;
      LinkExport( f2->F_RET, ee );
      LinkImport( gat, ee );

      /* LINK THE gat TO f2->F_BODY */
      ee = EdgeAlloc( gat, 1, f2->F_RET, port );
      ee->info = ainfo;
      LinkExport( gat, ee );
      LinkImport( f2->F_RET, ee );

      /* WIRE f2 TO THE src->G_DAD NODE USING THE NEW ARRAY */
      ee = EdgeAlloc( f2, port, src->G_DAD, port );
      ee->info = ainfo;
      LinkExport( f2, ee );
      LinkImport( src->G_DAD, ee );

      /* WIRE IN THE NEW ARRAY INTO THE AElement NODE */
      ee = EdgeAlloc( src, port, aelm, 1 );
      ee->info = ainfo;
      LinkExport( src, ee );
      LinkImport( aelm, ee );

      goto StartOver;
      }
    }

  f2->F_BODY->label = 0;
}


/**************************************************************************/
/* GLOBAL **************       OptIsVecCandidate      ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF Forall NODE f IS A CANDIDATE FOR VECTOR        */
/*          EXECUTION.                                                    */
/**************************************************************************/

int OptIsVecCandidate( f )
PNODE f;
{
  register PNODE n;
  register PEDGE i;

  if ( alliantfx ) {
    /* f MUST HAVE UNFILTERED AGathers ONLY */
    for ( n = f->F_RET->G_NODES; n != NULL; n = n->nsucc ) {
      if ( n->type != IFAGather )
        return( FALSE );

      if ( n->imp->isucc->isucc != NULL )
        return( FALSE );
      }

    /* f MUST HAVE A SIMPLIFIED GENERATE GRAPH */
    for ( n = f->F_GEN->G_NODES; n != NULL; n = n->nsucc ) {
      if ( n->type != IFRangeGenerate )
        return( FALSE );

      if ( n->nsucc != NULL )
        return( FALSE );
      }

    /* ONLY SPECIFIC NODES ARE ALLOWED IN THE BODY */
    for ( n = f->F_BODY->G_NODES; n != NULL; n = n->nsucc )
      switch ( n->type ) {
        case IFPlus:
        case IFMinus:
        case IFTimes:
        case IFDiv:
        case IFAbs:
        case IFNeg:
        case IFDouble:
        case IFTrunc:
        case IFSingle:
          break;
  
        case IFAElement:
          if ( !IsConst( n->imp ) )
            if ( IsAElement( n->imp->src ) )
              return( FALSE );
  
	  break;
  
        default:
	  return( FALSE );
        }

    return( TRUE );
    }

  if ( cRay ) {
    if ( IsInnerLoop( f->F_BODY ) ) {
      /* NO FILTERS ALLOWED!!! */
      for ( n = f->F_RET->G_NODES; n != NULL; n = n->nsucc ) {
	switch ( n->type ) {
	  case IFAGather:
	    /* FOR NOW, DO NOT VECTORIZE ARRAYS OF ARRAYS OR BOOLEANS */
	    i = n->imp->isucc;

	    if ( IsMultiple( i->info ) ) {
	      if ( IsArray( i->info->A_ELEM ) )
	        return( FALSE );
              else if ( IsBoolean( i->info->A_ELEM ) )
		return( FALSE );
            } else {
	      if ( IsArray( i->info ) )
	        return( FALSE );
              else if ( IsBoolean( i->info ) )
		return( FALSE );
	      }

	    if ( i->isucc != NULL )
	      return( FALSE );

	    break;

	  case IFReduce:
	  case IFRedTree:
	  case IFRedLeft:
	  case IFRedRight:
	    if ( n->imp->isucc->isucc->isucc != NULL )
	      return( FALSE );

	    break;

	  default:
	    break;
	  }
        }

      return( TRUE );
      }

    return( FALSE );
    }

  return( TRUE );
}


/**************************************************************************/
/* GLOBAL **************      WriteConcurInfo      ************************/
/**************************************************************************/
/* PURPOSE: WRITE LOOP AND FORALL CONCURRENTIZATION INFORMATION TO stderr.*/
/**************************************************************************/

void WriteConcurInfo()
{
  FPRINTF( infoptr, "\n\n **** FORALL SPLITTING FOR CONCURRENTIZATION\n\n%s\n" ,printinfo);
  FPRINTF( infoptr, " Generated Vector Forall Nodes:     %d of %d\n", vfcnt,Tvfcnt );
  FPRINTF( infoptr, " Generated Concurrent Forall Nodes: %d of %d\n", cfcnt,Tcfcnt );
  FPRINTF( infoptr, " Moved Nodes:                       %d of %d\n", mcnt,Tmcnt );
  FPRINTF( infoptr, " Split AElement Nodes:              %d of %d\n", scnt,Tscnt  );
}


/**************************************************************************/
/* LOCAL  **************    IsFractureCandidate    ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF THE NODE LIST OF BODY SUBGRAPH b CONTAINS     */
/*          SOME EXTRACTABLE VECTOR NODES (MARKING THEM WITH NEGATIVE     */
/*          LABELS). B IS ASSUMED TO BE THE BODY OF AN OUTER LOOP. Cport  */
/*          IS THE PORT NUMBER OF THE LOOP'S CONTROL ROD.                 */
/**************************************************************************/

static int IsFractureCandidate( b, cport )
PNODE b;
int   cport;
{
  register PNODE n;
  register PEDGE i;
  register int   c;

  for ( c = 0, n = b->G_NODES; n != NULL; n = n->nsucc ) {
    ++Tmcnt;
    switch ( n->type ) {
      /* case IFAbs: HURTS VECTORIZATION (SEE if2vector.c IN If2gen) */
      case IFDiv:             
      case IFExp:             
      case IFMax:             
      case IFMin:             
      case IFNeg:             
      case IFPlus:            
      case IFTimes:
      case IFMinus:           
      /* case IFDouble:          */
      /* case IFFloor:           */
      /* case IFInt:             */
      /* case IFSingle:          */
      /* case IFTrunc:           */
	if ( IsArithmetic( n->imp->info ) )
if ( n->imp->info->type != IF_INTEGER )
	  break;

	continue;

      case IFAElementN:
      case IFAElementP:
      case IFAElementM:
	n->label = -(n->label);
	continue;

      default:
        continue;
      }

    for ( i = n->imp; i != NULL; i = i->isucc ) {
      if ( IsConst( i ) )
        continue;

      if ( IsSGraph( i->src ) ) {
	if ( !IsImport( b->G_DAD, i->eport ) ) {
	  if ( i->eport == cport ) {
	    /* IS i LOOP CARRIED??? */
	    if ( IsLoopB( b->G_DAD ) )
	      if ( IsImport( b, i->eport ) )
		goto SkipOut;

	    continue;
	    }

	  goto SkipOut;
	  }

	continue;
	}

      if ( i->src->label < 0 )
	continue;

SkipOut:
      break;
      }

    if ( i != NULL )
      continue;

    if ( n->type != IFAElement )
      c++;

    n->label = -(n->label);
    }

  mcnt += c;

  return( (c > 0)? TRUE : FALSE );
}


/**************************************************************************/
/* LOCAL  **************  DoTheConcurrentization   ************************/
/**************************************************************************/
/* PURPOSE: YANK THE CONCURRENT NODES FROM LOOP OR FORALL f1 WITH BODY b  */
/*          RETURN r, AND CONTORL ROD crod WITH LOWER BOUND low AND UPPER */
/*          BOUND high, AND FORM A NEW FORALL.                            */
/**************************************************************************/

static void DoTheConcurrentization( f1, b, r, crod, low, high )
PNODE f1;
PNODE b;
PNODE r;
PEDGE crod;
PEDGE low;
PEDGE high;
{
  register PNODE n;
  register PEDGE e;
  register PEDGE i;
  register int   neg;
  register int   pos;
  register PNODE nn;
  register PEDGE se;
  register PNODE gen;
  register PNODE body;
  register PNODE ret;
  register PNODE f2;
  register PEDGE ee;
  register PNODE rg;
  register int   port;
  register PALIST l;

  /* UNTANGLE AElement[NPM] FANOUT */

  for ( n = FindLastNode( b ); n != b; n = n->npred ) {
    ++Tscnt;
    switch( n->type ) {
      case IFAElementN:
      case IFAElementP:
      case IFAElementM:
	neg = 0;
	pos = 0;

	for ( e = n->exp; e != NULL; e = e->esucc ) {
	  if ( e->dst->label < 0 )
	    neg++;
          else
	    pos++;
	  }

	if ( neg + pos == 0 ) {
	  n->label = -(n->label);
	  break;
	  }

	/* NEG ONLY */
	if ( (neg > 0) && (pos == 0) )
	  break;

	/* POS ONLY */
	if ( (neg == 0) && (pos > 0) ) {
	  n->label = -(n->label);
	  break;
	  }

	/* BOTH NEG AND POS, SO SPLIT THE SUCKER! */
	scnt++;

        nn = CopyNode( n );
        LinkNode( n->npred, nn );

        for ( i = n->imp; i != NULL; i = i->isucc ) {
          if ( IsConst( i ) )
            CopyEdgeAndReset( i, NULL_NODE, nn );
          else
	    CopyEdgeAndReset( i, i->src, nn );

          continue;
          }

        n->label = -(n->label);

	for ( e = n->exp; e != NULL; e = se ) {
	  se = e->esucc;

	  if ( e->dst->label >= 0 )
	    continue;

	  UnlinkExport( e );
	  LinkExport( nn, e );
	  }

	break;

      default:
	break;
      }
  }

  if ( IsForall(f1) )
    vfcnt++;
  else
    cfcnt++;

  /* DO THE MOVEMENT!!! */

  f2 = NodeAlloc( ++maxint, IFForall );
  CopyPragmas( f1, f2 );
  f2->info    = f1->info;
  f2->gname   = f1->gname;
  NewCompoundID(f2);

  l = AssocListAlloc( 0 );
  l = LinkAssocLists( l, AssocListAlloc( 1 ) );
  l = LinkAssocLists( l, AssocListAlloc( 2 ) );
  f2->alst = l;
  f2->scnt = 3;

  f2->if1line = f1->if1line;
  LinkNode( f1->npred, f2 );

    gen = NodeAlloc( 0, IFSGraph );
    CopyVitals( f2, gen );
    rg  = NodeAlloc( ++maxint, IFRangeGenerate );
    CopyVitals( f2, rg );
    LinkNode( gen, rg );

    ee = EdgeAlloc( rg, 1, gen, crod->iport );
    ee->info = crod->info;
    LinkExport( rg, ee );
    LinkImport( gen, ee );

    port = low->iport;
    low->iport = 1;
    CopyEdgeAndReset( low, gen, rg );
    low->iport = port;

    port = high->iport;
    high->iport = 2;
    CopyEdgeAndReset( high, gen, rg );
    high->iport = port;

  gen->G_DAD = f2;
  gen->gsucc = NULL;
  gen->gpred = NULL;
  LinkGraph( f2, gen );    /* THIS ASSUMES F_GEN IS THE FIRST GRAPH!!! */

  body = NodeAlloc( 0, IFSGraph );
  CopyVitals( f2, body );
  body->G_DAD = f2;
  CopyPragmas( b, body );
  LinkGraph( gen, body );  /* THIS ASSUMES F_BODY IS THE SECOND GRAPH!!! */

  ret = NodeAlloc( 0, IFSGraph );
  CopyVitals( f2, ret );
  ret->G_DAD = f2;
  CopyPragmas( r, ret );
  LinkGraph( body, ret );  /* THIS ASSUMES F_RET IS THE LAST GRAPH!!! */

  /* BLINDLY COPY ALL OF f1'S IMPORTS TO f2 */
  for ( i = f1->imp; i != NULL; i = i->isucc )
    CopyEdgeAndLink( i, f2, i->iport );

  MoveTheNegNodes( b, f2->F_BODY );
  WireNegExports( f2, b, f2->F_GEN->imp );

  DecodeIndexing( f2->F_BODY );
}


/**************************************************************************/
/* LOCAL  **************      VectorizeForalls     ************************/
/**************************************************************************/
/* PURPOSE: TRY AND EXTRACT AND REPACKAGE THE VECTOR OPERATIONS FOUND IN  */
/*          THE NON-VECTOR FORALL NODES OF GRAPH g.                       */
/**************************************************************************/

static void VectorizeForalls( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( !IsCompound( n ) )
      continue;

    if ( IsForall( n ) )
      if ( IsInnerLoop( n->F_BODY ) )
        continue;

    for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
      VectorizeForalls( sg );

    if ( !IsForall( n ) )
      continue;

    ++Tvfcnt;

    /* CHECK IF THE CONTROL IS SUITABLE FOR VECTORIZATION */
    if ( n->F_GEN->G_NODES->nsucc != NULL )
      continue;
    if ( n->F_GEN->imp->src->type != IFRangeGenerate )
      continue;

    /* OK, GIVE IT A SHOT! */
    EncodeIndexing( n->F_BODY, n->F_GEN->imp->iport, (int*)NULL );

    if ( IsFractureCandidate( n->F_BODY, n->F_GEN->imp->iport ) ) {
#ifdef MYI
      SPRINTF(printinfo,
        "%s Parallelizing loop %d at line %d, funct %s, file %s\n\n",
        printinfo, n->ID, n->line, n->funct, n->file);	
#endif
      DoTheConcurrentization( n, n->F_BODY, n->F_RET, n->F_GEN->imp,
			      n->F_GEN->imp->src->imp,
			      n->F_GEN->imp->src->imp->isucc      );
      }

    DecodeIndexing( n->F_BODY );
    }
}


/**************************************************************************/
/* GLOBAL **************           If1Vec          ************************/
/**************************************************************************/
/* PURPOSE: YANK AND REPACKAGE VECTOR OPERATIONS FOUND IN THE PROGRAM'S   */
/*          NONVECTOR FORALLS.                                            */
/**************************************************************************/

void If1Vec()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    VectorizeForalls( f );
}


/**************************************************************************/
/* LOCAL  **************      ParallelizeLoops     ************************/
/**************************************************************************/
/* PURPOSE: TRY AND EXTRACT AND REPACKAGE THE PARALLEL OPERATIONS FOUND   */
/*          IN THE SEQUENTIAL LOOP NODES (LoopB) OF GRAPH g.              */
/**************************************************************************/

static void ParallelizeLoops( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PEDGE crod;
  register PNODE nn;
  register PEDGE ee;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( !IsCompound( n ) )
      continue;

    for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
      ParallelizeLoops( sg );

    if ( !IsLoopB( n ) )
      continue;

    ++Tcfcnt;

    /* CHECK IF THE CONTROL IS SUITABLE FOR PARALLELIZATION */

    /* crod <= [Const,K] */
    nn = n->L_TEST->G_NODES;
    if ( nn == NULL )
      continue;
    if ( nn->nsucc != NULL )
      continue;
    if ( !IsLessEqual( nn ) )
      continue;

    crod = nn->imp;
    if ( IsConst( crod ) )
      continue;
    if ( !IsSGraph( crod->src ) )
      continue;
    if ( crod->info->type != IF_INTEGER )
      continue;
    if ( IsImport( n, crod->eport ) )
      continue;
    if ( (crod = FindImport( n->L_INIT, crod->eport )) == NULL )
      continue;

    ee = nn->imp->isucc;

    if ( !IsConst( ee ) ) {
      if ( !IsSGraph( ee->src ) )
        continue;
      if ( !IsImport( n, ee->eport ) )
	continue;
      }

    /* BODY:  crod := old crod + 1 */
    if ( (ee = FindImport( n->L_BODY, crod->iport )) == NULL )
      continue;
    if ( IsConst( ee ) )
      continue;
    if ( !IsPlus( ee->src ) )
      continue;

    OptNormalizeNode( ee->src );

    ee = ee->src->imp;
    if ( IsConst( ee ) )
      continue;
    if ( !IsSGraph( ee->src ) )
      continue;
    if ( ee->eport != crod->iport )
      continue;

    ee = ee->isucc;
    if ( !IsConst( ee ) )
      continue;
    if ( atoi( ee->CoNsT ) != 1 )
      continue;

    /* OK, GIVE IT A SHOT! */
    EncodeIndexing( n->L_BODY, crod->iport, (int*)NULL );

    if ( IsFractureCandidate( n->L_BODY, crod->iport ) ) {
      DoTheConcurrentization( n, n->L_BODY, n->L_RET, crod,
			      crod, nn->imp->isucc       );
      }

    DecodeIndexing( n->L_BODY );
    }
}


/**************************************************************************/
/* GLOBAL **************           If1Par          ************************/
/**************************************************************************/
/* PURPOSE: YANK AND REPACKAGE PARALLEL OPERATIONS FOUND IN THE PROGRAM'S */
/*          SEQUENTIAL LOOPS.                                             */
/**************************************************************************/

void If1Par()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    ParallelizeLoops( f );
}
