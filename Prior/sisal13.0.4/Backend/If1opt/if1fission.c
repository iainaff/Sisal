/* if1fission.c,v
 * Revision 12.7  1992/11/04  22:04:57  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:33  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


#define FAILED   -100                           /* AN ILLEGAL PORT NUMBER */


static int fldcnt = 0;		/* COUNT OF REMOVED FIELD REFERENCES */
static int Tfldcnt = 0;		/* COUNT OF FIELD REFERENCES */
static int recnt = 0;            /* COUNT OF REMOVED RECORD ELEMENT NODES */
static int Trecnt = 0;            /* COUNT OF RECORD ELEMENT NODES */
static int aecnt = 0;                /* COUNT OF REMOVED AElement EXPORTS */
static int Taecnt = 0;                /* COUNT OF AElement EXPORTS */
static int acnt  = 0;                  /* COUNT OF REMOVED AElement NODES */
static int Tacnt  = 0;                  /* COUNT OF AElement NODES */
static int rcnt  = 0;                   /* COUNT OF PUSHED RElement NODES */
static int Trcnt  = 0;                   /* COUNT OF RElement NODES */


static int FindElementSource( c, iport, e, idx )
PNODE c;
int   iport;
PEDGE e;
int idx;
{
    register PEDGE i;
    register PEDGE ii;
    register PEDGE ni;
    register PNODE b;
    register int   p;
    register int   lo;

    i = FindImport( c, iport );

    if ( i == NULL )
	return( FAILED );

    b = i->src;

    switch ( b->type ) {
        case IFABuild:
	    if ( !IsConst( b->imp ) )
                return( FAILED );

            lo = atoi( b->imp->CoNsT );

            if ( lo > idx )
                return( FAILED );

            ii = FindImport( b, idx - lo + 2 );

            if ( ii == NULL )
                return( FAILED );

            p  = ii->eport;
            break;

        case IFSGraph:
            if ( (i->eport == 1) && (IsTagCase( b->G_DAD )) )
                return( FAILED );

            p = FindElementSource( b->G_DAD, i->eport, e, idx );

            if ( p == FAILED )
                return( FAILED );

            ii = i;
            break;

        default:
	    return( FAILED );
        }

    ni = CopyEdge( ii, NULL_NODE, NULL_NODE );

    ni->eport = p;
    ni->info  = e->info;
    ni->iport = ++maxint;

    if ( !IsConst( ii ) )
	LinkExport( ii->src, ni );

    LinkImport( c, ni );

    return( maxint );
}


static void ArrayFission( g )
PNODE g;
{
    register PEDGE e;
    register PEDGE se;
    register PEDGE i;
    register PEDGE ii;
    register PNODE n;
    register PNODE sn;
    register PNODE b;
    register int   p;
    register int   lo;
    register int   idx;

    if ( IsIGraph( g ) )
	return;

    for ( n = g->G_NODES; n != NULL; n = sn ) {
	sn = n->nsucc;
        ++Tacnt;

	if ( IsCompound( n ) )
	    for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		ArrayFission( g );

        if ( !IsAElement( n ) )
	    continue;

	if ( !IsConst( n->imp->isucc ) )
	    continue;

	idx = atoi( n->imp->isucc->CoNsT );

	/* FOR IF2UP */
        for ( e = n->exp; e != NULL; e = e->esucc )
	  if ( e->dst->type == IFAReplace )
	    break;

	if ( e != NULL )
	  continue;

        for ( e = n->exp; e != NULL; e = se ) {
	    se = e->esucc;

	    i = n->imp;
           ++Taecnt;

	    b = i->src;

	    if ( IsCompound( b ) )
		continue;

	    switch ( b->type ) {
                case IFABuild:
		    if ( !IsConst( b->imp ) )
		        continue;

		    lo = atoi( b->imp->CoNsT );

		    if ( lo > idx )
		      continue;

		    ii = FindImport( b, idx - lo + 2 );

		    if ( ii == NULL )
		      continue;

		    p  = ii->eport;
		    break;

                case IFSGraph:
		    if ( (i->eport == 1) && (IsTagCase( b->G_DAD )) )
			continue;

                    p = FindElementSource( b->G_DAD, i->eport, e, idx );

		    if ( p == FAILED )
			continue;

                    ii = i;
		    break;

                default:
		    continue;
		}

	    UnlinkExport( e );  aecnt++;

	    if ( !IsConst( ii ) ) {
		e->eport = p;
                LinkExport( ii->src, e );
		}
            else
		ChangeToConst( e, ii );
	    }

	if ( n->exp == NULL )
	    acnt++;

        OptRemoveDeadNode( n );
	}
}


/**************************************************************************/
/* STATIC **************      FindFieldSource      ************************/
/**************************************************************************/
/* PURPOSE: FIND THE SOURCE OF A RECORD FIELD WITH POSITION e->eport IN   */
/*          THE SCOPE CONTAINING COMPOUND NODE c. IF FOUND, ADD A THREAD  */
/*          FROM THE FIELD'S SOURCE TO c (A NEW K PORT IMPORT).  THEN THE */
/*          NEW K PORT NUMBER IS RETURNED TO THE CALLER.                  */
/**************************************************************************/

static int FindFieldSource( c, iport, e )
PNODE c;
int   iport;
PEDGE e;
{
    register PEDGE i;
    register PEDGE ii;
    register PEDGE ni;
    register PNODE b;
    register int   p;

    i = FindImport( c, iport );

    if ( i == NULL )
	return( FAILED );

Start:
    b = i->src;

    if ( IsCompound( b ) )
	return( FAILED );

    switch ( b->type ) {
        case IFCall:
        case IFAElement:
        case IFXGraph:
        case IFLGraph:
        case IFRElements:
            return( FAILED );

        case IFRBuild:
            ii = FindImport( b, e->eport );
            p  = ii->eport;
            break;

        case IFRReplace:
            ii = FindImport( b, e->eport + 1 );

            if ( ii == NULL ) {
                i = b->imp;
                goto Start;
                }

             p = ii->eport;
             break;

        case IFSGraph:
            if ( (i->eport == 1) && (IsTagCase( b->G_DAD )) )
                return( FAILED );

            p = FindFieldSource( b->G_DAD, i->eport, e );

            if ( p == FAILED )
                return( FAILED );

            ii = i;
            break;

        default:
            Error1( "FindFieldSource FAILURE" );
        }

    ni = CopyEdge( ii, NULL_NODE, NULL_NODE );

    ni->eport = p;
    ni->info  = e->info;
    ni->iport = ++maxint;

    if ( !IsConst( ii ) )
	LinkExport( ii->src, ni );

    LinkImport( c, ni );

    return( maxint );
}


static void PushRElements( re )
PNODE re;
{
  register PEDGE e;
  register PNODE s;
  register int   Fcount;
  register PINFO f;
  register PNODE n;
  register PEDGE se;

  if ( re->imp->info->type != IF_RECORD )
    return;

  s = re->imp->src;

  if ( s->type != IFSelect )
    return;

  if ( UsageCount( s, re->imp->eport ) != 1 )
    return;

  Fcount = 0;

  for ( f = re->imp->info->R_FIRST; f != NULL; f = f->L_NEXT ) {
    if ( IsBasic( f->L_SUB ) )
      Fcount++;
    else
      return;
    }

  /* DON'T OVER-DO IT */
  if ( Fcount != 2 )
    return;

  /* FOR CONSEQUENT */
  n = CopyNode( re );

  e = FindImport( s->S_CONS, re->imp->eport );
  LinkNode( e->src, n );
  UnlinkImport( e );
  e->iport = 1;
  LinkImport( n, e );

  e = EdgeAlloc( n, 1, s->S_CONS, ++maxint );
  e->info = re->imp->info->R_FIRST->L_SUB;
  LinkExport( n, e );
  LinkImport( s->S_CONS, e );
  e = EdgeAlloc( n, 2, s->S_CONS, maxint + 1 );
  e->info = re->imp->info->R_FIRST->L_NEXT->L_SUB;
  LinkExport( n, e );
  LinkImport( s->S_CONS, e );

  /* FOR ALTERNATE */
  n = CopyNode( re );

  e = FindImport( s->S_ALT, re->imp->eport );
  LinkNode( e->src, n );
  UnlinkImport( e );
  e->iport = 1;
  LinkImport( n, e );

  e = EdgeAlloc( n, 1, s->S_ALT, maxint );
  e->info = re->imp->info->R_FIRST->L_SUB;
  LinkExport( n, e );
  LinkImport( s->S_ALT, e );
  e = EdgeAlloc( n, 2, s->S_ALT, maxint + 1 );
  e->info = re->imp->info->R_FIRST->L_NEXT->L_SUB;
  LinkExport( n, e );
  LinkImport( s->S_ALT, e );

  /* FIX SELECT EXPORTS */
  UnlinkExport( re->imp );

  for ( e = re->exp; e != NULL; e = se ) {
    se = e->esucc;

    UnlinkExport( e );
    
    e->eport = (e->eport == 1)? maxint : maxint + 1 ;
    LinkExport( s, e );
    }

  /* CLEAN UP */
  re->imp = NULL;
  re->exp = NULL;
  OptRemoveDeadNode( re );
  rcnt++;
  maxint++;
}


/**************************************************************************/
/* LOCAL  **************       RecordFission       ************************/
/**************************************************************************/
/* PURPOSE: ATTEMPT TO REDUCE THE NEED FOR RECORD GENERATE AND RECORD     */
/*          ELEMENT SELECTION NODES BY INSERTING EDGES FROM FIELD SOURCES */
/*          DIRECTLY TO THIER USES WITHOUT THE NEED FOR RECORD PACKAGING  */
/*          AND UNPACKAGING.  BOTH RECORD BUILD AND RECORD REPLACE NODES  */
/*          CAN SERVE AS A FILED'S SOURCE.  IF A SOURCE IS NOT DEFINED    */
/*          WITHIN THE SAME SCOPE AS THE RECORD ELEMENT NODE THEN         */
/*          FindFieldSource IS USED TO LOCATE IT IN A  SURROUNDING SCOPE. */
/*          THE NODE LIST DEFINED BY GRAPH g DEFINES THE CURRENT SCOPE OF */
/*          INTEREST.  THE SOURCE SEARCH PROCEEDURE WILL NOT CROSS CALL,  */
/*          AELEMENT, COUPOUND NODE, AND FUNCTION GRAPH BOUNDARIES.       */
/*          BECAUSE THE NODE LIST OF g IS EXAMINED IN DATAFLOW ORDER,     */
/*          MULTIPLE RECORD ELEMENT DEREFERENCING IS NOT A PROBLEM.       */
/**************************************************************************/

static void RecordFission( g )
PNODE g;
{
    register PEDGE e;
    register PEDGE se;
    register PEDGE i;
    register PEDGE ii;
    register PNODE n;
    register PNODE sn;
    register PNODE b;
    register int   p;

    if ( IsIGraph( g ) )
	return;

    for ( n = g->G_NODES; n != NULL; n = sn ) {
	sn = n->nsucc;

	if ( IsRElements( n ) ) {
          ++Trcnt;
	  PushRElements( n );
        }
	}

    for ( n = g->G_NODES; n != NULL; n = sn ) {
	sn = n->nsucc;
        ++Trecnt;

	if ( IsCompound( n ) )
	    for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		RecordFission( g );

        if ( !IsRElements( n ) )
	    continue;

        for ( e = n->exp; e != NULL; e = se ) {
	    se = e->esucc;
	    i = n->imp;
            ++Tfldcnt;

	    /* PSA OPTIMIZATION FIX 4/25/90 8 LINES */
	    switch ( e->dst->type ) {
	      case IFAElement:
	      case IFAReplace:
		continue;

              default:
		break;
              }

Start:
	    b = i->src;

	    if ( IsCompound( b ) )
		continue;

	    switch ( b->type ) {
		case IFCall:
		case IFAElement:
		case IFXGraph:
		case IFLGraph:
		case IFRElements:
		    continue;

                case IFRBuild:
		    ii = FindImport( b, e->eport );
		    p  = ii->eport;
		    break;

                case IFRReplace:
		    ii = FindImport( b, e->eport + 1 );

		    if ( ii == NULL ) {
			i = b->imp;
			goto Start;
			}

                    p = ii->eport;
		    break;

                case IFSGraph:
		    if ( (i->eport == 1) && (IsTagCase( b->G_DAD )) )
			continue;

                    p = FindFieldSource( b->G_DAD, i->eport, e );

		    if ( p == FAILED )
			continue;

                    ii = i;
		    break;

                default:
		    Error1( "RecordFission FAILURE" );
		}

	    UnlinkExport( e ); fldcnt++;

	    if ( !IsConst( ii ) ) {
		e->eport = p;
                LinkExport( ii->src, e );
		}
            else
		ChangeToConst( e, ii );
	    }

	if ( n->exp == NULL )
	    recnt++;

        OptRemoveDeadNode( n );
	}
}


/**************************************************************************/
/* LOCAL  **************      WriteFissionInfo     ************************/
/**************************************************************************/
/* PURPOSE: PRINT INFORMATION GATHERED DURING RECORD FISSION TO stderr.   */
/**************************************************************************/

void WriteFissionInfo()
{
    FPRINTF( infoptr, "\n **** RECORD FISSION\n\n" );
    FPRINTF( infoptr, " Removed Field References:     %d of %d\n", fldcnt,Tfldcnt );
    FPRINTF( infoptr, " Removed Record Element Nodes: %d of %d\n", recnt,Trecnt );
    FPRINTF( infoptr, " Pushed RElement Nodes:        %d of %d\n", rcnt,Trcnt  );

    FPRINTF( infoptr, "\n **** ARRAY FISSION\n\n" );
    FPRINTF( infoptr, " Removed AElement Nodes:   %d of %d\n", acnt,Tacnt  );
    FPRINTF( infoptr, " Removed AElement Exports: %d of %d\n", aecnt,Taecnt );
}


/**************************************************************************/
/* GLOBAL **************         If1Fission        ************************/
/**************************************************************************/
/* PURPOSE: PERFORM RECORD AND ARRAY FISSION ON ALL FUNCTION GRAPHS.      */
/**************************************************************************/

void If1Fission()
{
    register PNODE f;

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
	RecordFission( cfunct = f );
	ArrayFission( cfunct = f );
	}
}
