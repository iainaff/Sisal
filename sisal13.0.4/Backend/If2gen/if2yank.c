/* if2yank.c,v
 * Revision 12.7  1992/11/04  22:05:04  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:06  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


static PNODE gpred = NULL;                 /* GRAPH INSERTION PREDECESSOR */


static int Used( c, iport )
PNODE c;
int   iport;
{
  register PNODE sg;
  register int   u;

  for ( u= FALSE, sg = c->C_SUBS; sg != NULL; sg = sg->gsucc )
    if ( IsExport( sg, iport ) )
      u = TRUE;

  return( u );
}


/**************************************************************************/
/* LOCAL  **************   WasConvertedToTagTest   ************************/
/**************************************************************************/
/* PURPOSE: ATTEMPT TO CONVERT TagCase NODE n INTO A TAG TEST NODE.  TRUE */
/*          IS RETURNED IF THE CONVERSION IS MADE.                        */
/**************************************************************************/

static int WasConvertedToTagTest( n )
PNODE n;
{
  register PNODE sg;
  register int   f;
  register int   t;

  if ( n->imp->isucc != NULL || n->imp->iport != 1 )
    return( FALSE );

  t = -1;
  f = FALSE;

  for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc ) {
    if ( sg->imp->isucc != NULL )
      return( FALSE );

    if ( !IsConst( sg->imp ) )
      return( FALSE );

    if ( !IsBoolean( sg->imp->info ) )
      return( FALSE );

    if ( sg->imp->CoNsT[0] == 't' || sg->imp->CoNsT[0] == 'T' ) {
      if ( t != -1 )
        return( FALSE );

      if ( sg->G_TAGS == NULL )
        return( FALSE );

      if ( sg->G_TAGS->next != NULL )
        return( FALSE );

      t = sg->G_TAGS->datum;
      continue;
      }

    if ( sg->imp->CoNsT[0] == 'f' || sg->imp->CoNsT[0] == 'F' ) {
      f = TRUE;
      continue;
      }

    return( FALSE );
    }

  /* IS THERE AT LEAST ONE SUBGRAPH RETURNING FALSE AND EXACTLY ONE */
  /* RETURNING TRUE */
  if ( !f || t == -1 )
   return( FALSE );

  n->type = IFUTagTest;
  n->imp->iport = t;

  tagtcnt++;

  return( TRUE );
}


/* SPECIALIZED FOR SELECT NODES */
static int YankIsInvariant( i )
PEDGE i;
{
  register PNODE l;

  if ( i == NULL )
    return( FALSE );

  /* BE CAREFUL ABOUT REFERENCE COUNT OPERATIONS!!! */
  if ( i->pm != 0 || i->cm != 0 || i->sr != 0 || i->pl != 0 )
    return( FALSE );

  if ( IsConst( i ) )
    return( TRUE );

  if ( !IsSGraph( i->src ) )
    return( FALSE );

  l = i->src->G_DAD;

  if ( IsLoop( l ) || IsForall( l ) )
    return( IsImport( l, i->eport ) );

  if ( !IsSelect( l ) )
    return( FALSE );

  return( YankIsInvariant( FindImport( l, i->eport ) ) );
}


/* ASSUMES n HAS NO EXPORTS! */
static void MoveInvariant( c, n )
PNODE c;
PNODE n;
{
  register PEDGE i;
  register PEDGE ii;
  register PEDGE si;

  UnlinkNode( n );
  LinkNode( c->npred, n );

  for ( i = n->imp; i != NULL; i = si ) {
    si = i->isucc;

    if ( IsConst( i ) )
      continue;

    ii = FindImport( c, i->eport );
    UnlinkExport( i );

    i->eport = ii->eport;
    i->src   = ii->src;

    /* PRESERVE REFERENCE COUNT OPERATIONS!!! */
    i->sr = ii->sr;
    i->pm = ii->pm;
    i->dmark = ii->dmark; /* CANN 10-3 */
    i->wmark = ii->wmark; /* CANN 10-3 */

    if ( IsConst( ii ) )
      i->CoNsT = ii->CoNsT;
    else
      LinkExport( ii->src, i );

    tdicnt++;

    if ( !Used( c, ii->iport ) )
      /* DO NOT REMOVE A REFERENCE COUNTED EDGE */
      if ( ii->cm == 0 && ii->pm == 0 && ii->sr == 0 && ii->pl == 0 &&
	   (!(ii->dmark)) ) { /* HELP */
        UnlinkImport( ii );
        UnlinkExport( ii );
        /* free( ii ); */
        dicnt++;
        }
    }

  /* MOVE SINGLE IMPORT NODE CLOSER TO SOURCE */
  if ( n->imp->isucc == NULL )
    if ( !IsConst( n->imp ) ) {
      UnlinkNode( n );
      LinkNode( n->imp->src, n );
      }
}


/**************************************************************************/
/* LOCAL  **************      CallImprover         ************************/
/**************************************************************************/
/* PURPOSE: INSERT SPECIAL ARGUMENT FRAME STORE NODES AND APPLY INVARIANT */
/*          REMOVAL.                                                      */
/**************************************************************************/

static void CallImprover( g, inloop )
PNODE g;
int   inloop;
{
  register PNODE n;
  register PEDGE i;
  register PNODE sg;
  register PNODE f;
  register PNODE nn;
  register PEDGE si;
  register PNODE sn;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        CallImprover( sg, inloop || IsLoop(n) || IsForall(n) );

    switch ( n->type ) {
      case IFCall:
        f = FindFunction( n->imp->CoNsT );

        if ( IsIGraph( f ) && f->mark != 's' ) /* NEW CANN 2/92 */
          break;

        /* SKIP FUNCTION NAME */
        for ( i = n->imp->isucc; i != NULL; i = si ) {
          si = i->isucc;

          nn = NodeAlloc( ++maxint, IFSaveCallParam );
          nn->usucc = n;

          if ( IsConst( i ) )
            LinkNode( n->npred, nn );
          else
            LinkNode( i->src, nn );

          if ( i->cm == 0 && (!(i->dmark)) ) { /* HELP */
            UnlinkImport( i );
            i->iport--;
            LinkImport( nn, i );
            }
          else
            CopyEdgeAndLink( i, nn, i->iport - 1 );

          scpcnt++;
          }

        break;

      default:
        break;
      }
    }

  if ( inloop )
    for ( n = g->G_NODES; n != NULL; n = sn ) {
      sn = n->nsucc;

      switch ( n->type ) {
        case IFSaveCallParam:
	  if ( YankIsInvariant( n->imp ) ) {
            MoveInvariant( g->G_DAD, n );
            scpinvcnt++;
            }

	break;

        default:
	  break;
	}
      }
}


static int IsSliceParamNeeded( g, i )
PNODE g;
PEDGE i;
{
    register PNODE f;
    register PEDGE e;
    register int   u;

    f = g->imp->src;

    u = FALSE;

    for ( e = g->exp; e != NULL; e = e->esucc )
      if ( e->eport == i->iport ) {
        u = TRUE;

        if ( !IsForall( e->dst ) )
          goto SaveIt;
        }

    if ( !u )
      goto SkipIt;

    if ( !IsExport( f->F_BODY, i->iport ) )
      if ( !(IsBuffer( i->info ) && IsExport( f->F_GEN, i->iport )) ) {
        for ( e = f->F_RET->exp; e != NULL; e = e->esucc ) {
          if ( e->eport != i->iport )
            continue;

          switch ( e->dst->type ) {
	    case IFAGatherATDVI:
	    case IFAGatherATDV:
            case IFAGatherAT:
              /* VALUE OR BUFFER                            */
              if ( e->iport == 2 || e->iport == 4 )
                goto SaveIt;
                            
              break;

            case IFReduce:
            case IFRedLeft:
            case IFRedRight:
            case IFRedTree:
              /* VALUE OR FILTER                            */
              if ( e->iport == 3 || e->iport == 4 )
                goto SaveIt;

              break;

            case IFReduceAT:
            case IFRedLeftAT:
            case IFRedRightAT:
            case IFRedTreeAT:
	    case IFReduceATDV:
	    case IFReduceATDVI:
              /* VALUE OR FILTER (NOTE FILTER SHOULD NEVER HAPPEN) */
              /* OR SHIFT SIZE OR BUFFER                           */
              if ( e->iport == 3 || e->iport == 4 || 
                   e->iport == 5 || e->iport == 6  )
                goto SaveIt;

              break;

            default:
              break;
            }
          }

        goto SkipIt;
        }

SaveIt:
  return( TRUE );

SkipIt:
  return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************      SliceImprover        ************************/
/**************************************************************************/
/* PURPOSE: INSERT SPECIAL ARGUMENT TASK FRAME STORE NODES AND APPLY      */
/*          INVARIANT REMOVAL IF inloop.                                  */
/**************************************************************************/

static void SliceImprover( g, inloop )
PNODE g;
int   inloop;
{
  register PNODE n;
  register PEDGE i;
  register PNODE sg;
  register PNODE f;
  register PNODE nn;
  register PEDGE si;
  register PEDGE lo;
  register PEDGE hi;
  register PEDGE size;
  register PNODE sn;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        SliceImprover( sg, inloop || IsLoop(n) || IsForall(n) );

    switch ( n->type ) {
      case IFLoopPoolEnq:
        f = n->usucc;

        lo = GetSliceParam( f->imp->src->F_GEN->imp->src->imp, n );
        hi = GetSliceParam( f->imp->src->F_GEN->imp->src->imp->isucc, n );

	if ( oruntime ) {
          nn = NodeAlloc( ++maxint, IFBuildSlices );
          nn->usucc = n;
	  nn->ID = n->ID;
	  nn->Style = n->Style;
	  nn->MinSlice = n->MinSlice;
	  nn->LoopSlice= n->LoopSlice;

          LinkNode( n->npred, nn );

          CopyEdgeAndLink( lo, nn, 1 );
          CopyEdgeAndLink( hi, nn, 2 );

	  n->type = IFOptLoopPoolEnq;
	  }

        for ( i = n->imp; i != NULL; i = si ) {
          si = i->isucc;

          if ( !IsSliceParamNeeded( f, i ) )
            continue;

          nn = NodeAlloc( ++maxint, IFSaveSliceParam );
          nn->usucc = n;

          if ( IsConst( i ) )
            LinkNode( n->npred, nn );
          else if ( i->info->type == IF_BUFFER )
            LinkNode( n->npred, nn );
          else
            LinkNode( i->src, nn );

          CopyEdgeAndLink( i, nn, i->iport );

          if ( i->info->type == IF_BUFFER ) {
            CopyEdgeAndLink( lo, nn, i->iport + 1 );

            size = FindExport( f->imp->src->F_GEN, i->iport );

            if ( size != NULL ) {
              if ( size->isucc == NULL )
                Error2( "SliceImprover", "SIZE IMPORT NOT PRESENT" );

              size = GetSliceParam( size->isucc, n );
              CopyEdgeAndLink( size, nn, i->iport + 2 );
              }
            }

          sspcnt++;
          tleicnt++;

          if ( !IsExport( f->imp->src->F_GEN, i->iport ) && 
               !IsExport( f->imp->src->F_RET, i->iport )  )
            if ( i->cm == 0 && i->pm == 0 && i->sr == 0 && i->pl == 0 &&
		 (!(i->dmark)) ) { /* HELP */
              UnlinkExport( i );
              UnlinkImport( i );
              leicnt++;
              }
          }

        break;

      default:
        break;
      }
    }

  if ( inloop )
    for ( n = g->G_NODES; n != NULL; n = sn ) {
      sn = n->nsucc;

      switch ( n->type ) {
	case IFBuildSlices:
        case IFSaveSliceParam:
          for ( i = n->imp; i != NULL; i = i->isucc )
            if ( !YankIsInvariant( i ) )
              break;

          if ( i == NULL ) {
            MoveInvariant( g->G_DAD, n );
            sspinvcnt++;
            }

	  break;

        default:
	  break;
	}
      }
}


/**************************************************************************/
/* LOCAL  **************      PushYankedNode       ************************/
/**************************************************************************/
/* PURPOSE: PUSH NODE n ACROSS SELECT SUBGRAPHS.                          */
/**************************************************************************/

static void PushYankedNode( n ) 
PNODE n;
{
  register PNODE nn;
  register PNODE s;
  register PEDGE e1;
  register PEDGE e2;

  switch ( n->type ) {
    case IFAStore:
    case IFLeast:
    case IFGreatest:
    case IFProduct:
    case IFSum:
      break;

    default:
      return;
    }

  if ( IsConst( n->imp ) )
    return;

  if ( n->imp->pm > 0 || n->imp->cm == -1 )
    return;

  s = n->imp->src;

  if ( UsageCount( s, n->imp->eport ) != 1 )
    return;

  if ( !IsSelect( s ) )
    return;

  e1 = FindImport( s->S_CONS, n->imp->eport );

  if ( e1 == NULL )
    return;

  e2 = FindImport( s->S_ALT, n->imp->eport );

  if ( e2 == NULL )
    return;

  UnlinkExport( n->imp );
  UnlinkImport( n->imp );

  UnlinkImport( e1 );
  UnlinkImport( e2 );
  e1->iport = 1;
  e2->iport = 1;

  UnlinkNode( n );
  nn = NodeAlloc( ++maxint, n->type );
  *nn = *n;

  LinkImport( n, e1 );
  LinkImport( nn, e2 );

  if ( IsConst( e1 ) )
    LinkNode( s->S_CONS, n );
  else
    LinkNode( e1->src, n );

  if ( IsConst( e2 ) )
    LinkNode( s->S_ALT, nn );
  else
    LinkNode( e2->src, nn );

  pycnt++;

  PushYankedNode( n );
  PushYankedNode( nn );
}


/**************************************************************************/
/* LOCAL  **************        PrepareConst       ************************/
/**************************************************************************/
/* PURPOSE: PREPARE BASIC CONSTANTS FOR PRINTING IN C.                    */
/**************************************************************************/

static void PrepareConst( n )
PNODE n;
{
    register PEDGE  i;
    register char  *p;

    for ( i = n->imp; i != NULL; i = i->isucc ) {
        if ( !IsConst( i ) )
            continue;

        switch ( i->info->type ) {
            case IF_DOUBLE:
            case IF_REAL:
                for ( p = i->CoNsT; *p != '\0'; p++ )
                    if ( (*p == 'd') || (*p == 'D') )
                        *p = 'e';

                break;

            case IF_BOOL:
                if ( (i->CoNsT[0] == 't') || (i->CoNsT[0] == 'T') )
                    i->CoNsT = "True";
                else
                    i->CoNsT = "False";

                break;

            case IF_NULL:
                i->CoNsT = "Nil";
                break;

            default:
                break;
            }
        }
}


/**************************************************************************/
/* LOCAL  **************    BuildFunctionTypes     ************************/
/**************************************************************************/
/* PURPOSE: BUILD A FUNCTION TYPE ENTRY IN THE SYMBOL TABLE FOR FUNCTION  */
/*          g (A NEW FUNCTION GRAPH).  A NAME IS ASSIGNED TO g AND ITS    */
/*          INFORMATION FIELD IS SET.                                     */
/**************************************************************************/

static void BuildFunctionTypes( s, g, nm )
PNODE  s;
PNODE  g;
char  *nm;
{
    register PEDGE e;
    register PEDGE i;
    register int   mx;
    register int   port;
    register int   in  = 0;
    register int   out = 0;
    int		   Unique;

    /* MAKE INPUT TUPLES: NOTE THERE MIGHT NOT BE ANY! ENTRIES ARE MADE   */
    /* IN LABEL ORDER!                                                    */

    if ( g->exp != NULL ) {
        in = ++maxint;

        /* FIND THE LARGEST PORT NUMBER */
        for ( mx = 0, e = g->exp; e != NULL; e = e->esucc )
            if ( e->eport > mx )
                mx = e->eport;

        for ( mx--, port = 1; port <= mx; port++ ) {
            if ( (e = FindExport( g, port )) == NULL )
                continue;

            MakeInfo( maxint, IF_TUPLE, e->info->label, maxint + 1 );
            maxint++;
            }

        e = FindExport( g, mx + 1 );
        MakeInfo( maxint, IF_TUPLE, e->info->label, 0 );
        }

    /* MAKE OUTPUT TUPLES: WE ASSUME THEIR IS AT LEAST ONE IMPORT TO g    */

    out = ++maxint;

    for ( i = g->imp; i->isucc != NULL; i = i->isucc ) {
        MakeInfo( maxint, IF_TUPLE, i->info->label, maxint + 1 );
        maxint++;
        }

    MakeInfo( maxint, IF_TUPLE, i->info->label, 0 );

    /* MAKE FUNCTION TYPE ENTRY AND FINISH INITIALIZING g                 */

    MakeInfo( ++maxint, IF_FUNCTION, in, out );
    g->info   = FindInfo( maxint, IF_FUNCTION );

    /* ------------------------------------------------------------ */
    /* Build the name.  Use the source ID if set else use the nmid  */
    /* to generate a unique one.				    */
    /* ------------------------------------------------------------ */
    Unique = (s->ID)?(s->ID):(++nmid);

    if ( s->funct == NULL ) {
      g->G_NAME = MakeName( nm, cfunct->G_NAME, Unique );
    } else {
      g->G_NAME = MakeName( nm, UpperCase(s->funct,FALSE,FALSE), Unique );
    }
}


/**************************************************************************/
/* LOCAL  **************     InsertUGetTagNode     ************************/
/**************************************************************************/
/* PURPOSE: INSERT A UGetTag NODE FOR TagCase NODE n;  THE TAG IS WIRED   */
/*          TO PORT ++maxint OF n. PORT 1 IS LEFT AS IS. n->usucc IS SET  */
/*          TO ADDRESS THIS NODE!                                         */
/**************************************************************************/

static void InsertUGetTagNode( n )
PNODE n;
{
  register PNODE nn;
  register PEDGE e;

  nn = NodeAlloc( ++maxint, IFUGetTag );
  LinkNode( n->npred, nn );

  e = EdgeAlloc( n->imp->src, n->imp->eport, nn, 1 );
  e->info = n->imp->info;

  /* PRESERVE REFERENCE COUNT INFORMATION */
  e->sr = n->imp->sr;
  e->pm = n->imp->pm;
  e->dmark = n->imp->dmark; /* CANN 10-3 */
  e->wmark = n->imp->wmark; /* CANN 10-3 */

  if ( !IsConst( n->imp ) )
    LinkExport( n->imp->src, e );

  LinkImport( nn, e );

  e = EdgeAlloc( nn, 1, n, ++maxint );
  e->info = integer;

  LinkExport( nn, e );
  LinkImport( n, e );
  n->usucc = nn;
}


/**************************************************************************/
/* LOCAL  **************     InsertUElementNode    ************************/
/**************************************************************************/
/* PURPOSE: INSERT A UElement NODE in SUBGRAPH sg OF TagCase NODE t IF    */
/*          THE UNION VALUE IS REFERENCED.                                */
/**************************************************************************/

static void InsertUElementNode( t, sg )
PNODE t;
PNODE sg;
{
    register PEDGE e;
    register PEDGE se;
    register PNODE n;

    if ( !IsExport( sg, 1 ) )
        return;

    n = NodeAlloc( ++maxint, IFUElement );
    LinkNode( sg, n );

    /* MAKE n THE SOURCE OF THE REFERENCED VALUE */
    for ( e = sg->exp; e != NULL; e = se ) {
        se = e->esucc;

        if ( e->eport != 1 )
            continue;

        UnlinkExport( e );
        LinkExport( n, e );
        }

    /* LINK THE UNION TO THE UElement NODE IN sg */
    e = EdgeAlloc( sg, 1, n, 1 );
    e->info = t->imp->info;
    LinkExport( sg, e );
    LinkImport( n, e );
}


/**************************************************************************/
/* LOCAL  **************      YankSlicedLoop       ************************/
/**************************************************************************/
/* PURPOSE: YANK SLICED LOOP s. A NEW GRAPH NODE IS BUILT CONTAINING THE  */
/*          YANKED FORALL NODE. THE TYPE OF s IS CHANGED TO LoopPoolEnq.  */
/*          NOTE: THE PRAGMAS ON s IMPORTS AND EXPORTS ARE NOT CHANGED.   */
/**************************************************************************/

static void YankSlicedLoop( s )
PNODE s;
{
  register PNODE g;
  register PNODE f;
  register PEDGE i;
  register PEDGE e;
  register PNODE sg;

  /* ------------------------------------------------------------ */
  /* Change the opcode of the yanked loop marker left behind	  */
  /* ------------------------------------------------------------ */
  s->type = IFLoopPoolEnq;

  /* ------------------------------------------------------------ */
  /* Allocate a new graph for the function and one for a new	  */
  /* forall node within the new function			  */
  /* ------------------------------------------------------------ */
  g = NodeAlloc( ++maxint, IFLPGraph );
  g->Pmark = TRUE;
  g->file  = s->file;
  g->funct = s->funct;
  g->line  = s->line;
  g->LoopSlice = s->LoopSlice;
  g->MinSlice = s->MinSlice;
  g->Style    = s->Style;
  g->ID	      = s->ID;

  f = NodeAlloc( ++maxint, IFForall );
  f->ccost = s->ccost;
  f->file  = s->file;
  f->funct = s->funct;
  f->line  = s->line;

  f->gsucc = s->gsucc;
  f->smark = s->smark;
  f->vmark = s->vmark;
  f->LoopSlice = s->LoopSlice;
  f->MinSlice = s->MinSlice;
  f->Style    = s->Style;
  f->ID	      = s->ID;

  /* ------------------------------------------------------------ */
  /* Modify all the subgraphs to point to a new parent (f)	  */
  /* ------------------------------------------------------------ */

  for ( sg = s->C_SUBS; sg != NULL; sg = sg->gsucc )
    sg->G_DAD = f;

  LinkGraph( gpred, g ); gpred = g;
  LinkNode( g, f );

  for ( i = s->imp; i != NULL; i = i->isucc ) {
    e = EdgeAlloc( g, i->iport, f, i->iport );
    e->info = i->info;
    LinkExport( g, e );
    LinkImport( f, e );
    }

  for ( i = f->F_RET->imp; i != NULL; i = i->isucc ) {
    if ( i->iport == 0 )
      continue;

    e = EdgeAlloc( f, i->iport, g, i->iport );
    e->info = i->info;
    LinkExport( f, e );
    LinkImport( g, e );
    }

  s->usucc = g;

  BuildFunctionTypes( s, g, "SliceBody" );
}


/**************************************************************************/
/* LOCAL  **************         YankNodes         ************************/
/**************************************************************************/
/* PURPOSE: YANK SLICED LOOPS FROM GRAPH g, INSERTIN THEM AT THE HEAD OF  */
/*          THE FUNCTION GRAPH LIST.  ALSO, LINK CONSTANT AGGREGATE       */
/*          GENERATOR NODES TO THE GLOBAL LIST HEADED BY chead AND INSERT */
/*          UElement NODES IN TagCase SUBGRAPHS. GRAPH g IS TRAVERSED     */
/*          FROM THE BOTTOM-UP. CALL NAMES ARE CONVERTED TO UPPER CASE.   */
/**************************************************************************/

static void YankNodes( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;
  register PEDGE i;

  PrepareConst( g );

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    PrepareConst( n );

    if ( n->type == IFAFill || n->type == IFAFillAT )
      Error2( "IsEntryPartOfCycle", "IFAFill or IFAFillAT ENCOUNTERED" );

    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        YankNodes( sg );

    switch ( n->type ) {
      case IFNoOp:
        break;

      case IFTagCase:
        if ( WasConvertedToTagTest( n ) )
          break;

        for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
          InsertUElementNode( n, sg );

        InsertUGetTagNode( n );
        break;

      case IFForall:
        if ( n->smark )
          YankSlicedLoop( n );

        break;

      case IFLoopA:
      case IFLoopB:
        break;

      case IFRBuild:
        if ( IsUnion( n->exp->info ) ) {
          n->type = IFUBuild;
          break;
          }

      case IFABuildAT:
      case IFBRBuild:
        if ( n->cmark ) {
          /* NO CONSTANT BRecord CONSTITUENTS ALLOWED!!! */
          for ( i = n->imp; i != NULL; i = i->isucc )
            if ( IsBRecord( i->info ) )
              break;

          if ( i != NULL ) {
            if ( IsABuildAT( n ) ) {
              i = FindLastImport( n );
              i->src->cmark = FALSE;
              }

            n->cmark = FALSE;
            break;
	    }

          AppendToUtilityList( chead, ctail, n );
          n->G_NAME = MakeName( "gbl", "", ++nmid );
	  }

        break;

      default:
        break;
      }
    }
}


/**************************************************************************/
/* LOCAL  **************     YankReturnNodes       ************************/
/**************************************************************************/
/* PURPOSE: YANK AND PUSH SELECTED RETURN NODES INTO THE LOOP BODY.       */
/**************************************************************************/

static void YankReturnNodes( g )
PNODE g;
{
  register PNODE n, r;
  register PNODE nn, sg;
  register PEDGE v;
  register PEDGE e;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( alliantfx && n->vmark ) continue;

    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        YankReturnNodes( sg );

    switch ( n->type ) {
      case IFForall:
        for ( r = n->F_RET->G_NODES; r != NULL; r = r->nsucc ) {
          switch ( r->type ) {
	    case IFAGatherATDVI:
	    case IFAGatherATDV:
            case IFAGatherAT:
              v = r->imp->isucc;

              if ( IsConst( v ) )
                break;

              if ( v->pm > 0 )
                break;

              if ( UsageCount( n->F_RET, v->eport ) != 1 )
                break;

              if ( v->isucc->iport == 3 )
                break;

              e = FindImport( n->F_BODY, v->eport );

              if ( e == NULL )
                break;

              if ( IsConst( e ) )
                break;

              nn = NodeAlloc( ++maxint, 0 );
              UnlinkImport( e );
              e->iport = 1;
              LinkImport( nn, e );
              LinkNode( e->src, nn );

              r->gsucc  = nn;
              nn->gsucc = r;
              nn->type  = IFAStore;

              if ( !n->vmark )
                PushYankedNode( nn );

              break;

            case IFReduce:
            case IFRedLeft:
            case IFRedRight:
            case IFRedTree:
              if ( r->imp->CoNsT[0] == REDUCE_CATENATE ||
                   r->imp->CoNsT[0] == REDUCE_USER )
                break;

              v = r->imp->isucc->isucc;

              if ( v->isucc != NULL )
                break;

              if ( IsConst( v ) )
                break;

              if ( UsageCount( n->F_RET, v->eport ) != 1 )
                break;

              e = FindImport( n->F_BODY, v->eport );

              if ( e == NULL )
                break;

              if ( IsConst( e ) )
                break;

              nn = NodeAlloc( ++maxint, 0 );
              UnlinkImport( e );
              e->iport = 1;
              LinkImport( nn, e );
              LinkNode( e->src, nn );
              r->gsucc  = nn;
              nn->gsucc = r;

              switch ( r->imp->CoNsT[0] ) {
                case REDUCE_LEAST:
                  nn->type = IFLeast;
                  break;

                case REDUCE_GREATEST:
                  nn->type = IFGreatest;
                  break;

                case REDUCE_PRODUCT:
                  nn->type = IFProduct;
                  break;

                case REDUCE_SUM:
                  nn->type = IFSum;
                  break;

                default:
		  fprintf(stderr,"Bad reduction constant \"%s\" on line %d\n",
			  r->imp->CoNsT,r->imp->if1line);
		  exit(1);
                  break;
                }

              if ( !n->vmark )
                PushYankedNode( nn );

              break;

            default:
              break;
            }
          }
          break;

        default:
          break;
        }
      }
}


/**************************************************************************/
/* GLOBAL **************          If2Yank1         ************************/
/**************************************************************************/
/* PURPOSE: YANK RETURN NODES AND OPTIMIZE ARGUMENT TRANSMISSION.         */
/**************************************************************************/

void If2Yank1()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    YankReturnNodes( f );

    SliceImprover( f, FALSE );
    CallImprover( f, FALSE );
    }
}


/**************************************************************************/
/* GLOBAL **************          If2Yank0         ************************/
/**************************************************************************/
/* PURPOSE: YANK SLICED LOOPS AND CONSTANT AGGREGATES FORM ALL FUNCTIONS  */
/*          AND INSERT UElement NODES IN TagCase NODES. USER DEFINED      */
/*          FUNCTION NAMES ARE CONVERTED TO UPPER CASE. CONSTANTS ARE     */
/*          PROPAGATED INTO INTO COMPOUND NODES, AND OTHER OPTIMIZATIONS  */
/*          ARE DONE.                                                     */
/**************************************************************************/

void If2Yank0()
{
  register PNODE f;

  gpred = glstop;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    AssignNewPortNums( f, TRUE );
    YankNodes( cfunct = f );
    }
}


void WriteYankInfo()
{
/*  FPRINTF( infoptr4, "\n **** NODE YANK OPTIMIZATIONS\n\n" );
  FPRINTF( infoptr4, " Pushed Yanked Return Nodes:   %d\n", pycnt   );
  FPRINTF( infoptr4, " Converted Tag Test Nodes:     %d\n", tagtcnt ); */

  FPRINTF( infoptr4, "\n **** INVOCATION OPTIMIZATIONS\n\n" );
  FPRINTF( infoptr4, " Inserted Saved Slice Params:  %d\n", sspcnt    );
  FPRINTF( infoptr4, " Invariant Saved Slice Params: %d\n", sspinvcnt );
  FPRINTF( infoptr4, " Inserted Saved Call Params:   %d\n", scpcnt    );
  FPRINTF( infoptr4, " Invariant Saved Call Params:  %d\n", scpinvcnt );
  FPRINTF( infoptr4, " Removed Compound Imports:     %d of %d\n", dicnt, tdicnt     );
  FPRINTF( infoptr4, " Removed Loop Enque Imports:   %d of %d\n", leicnt, tleicnt   );
}
