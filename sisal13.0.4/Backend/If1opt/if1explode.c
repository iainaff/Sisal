/* if1explode.c,v
 * Revision 12.7  1992/11/04  22:04:57  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:33  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

static int efs   = 0;                        /* COUNT OF EXPLODED Foralls */
static int Tefs   = 0;                        /* COUNT OF Foralls */
static int rff   = 0;             /* COUNT OF RESULTING Forall FRAGEMENTS */
static int maelm = 0;                 /* COUNT OF MODIFIED AElement NODES */
DYNDECLARE(printinfo, printbuf, printlen, printcount, char, 2000);

#define MAX_HASH  31                  /* HASH TABLES FOR FAST INFO LOOKUP */

static PINFO mihash[MAX_HASH];
static PINFO aihash[MAX_HASH];

/**************************************************************************/
/* LOCAL  **************    IsExplodeCandidate     ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF NODE n IS A CANDIDATE FOR EXPLOSION.          */
/**************************************************************************/

static int IsExplodeCandidate( n , ReasonP)
PNODE n;
char **ReasonP;
{
  register PEDGE i;

  switch ( n->type ) {
    case IFAElementN:
    case IFAElementP:
    case IFAElementM:
      *ReasonP = "loop is the wrong type";
      return( FALSE );

    default:
      break;
    }

  /* ARE ALL IMPORTS ONE OF FOUR FROMS: CONSTANTS, SPECIAL AElement NODES, */
  /* OR K PORT VALUES.                                                     */
  for ( i = n->imp; i != NULL; i = i->isucc ) {
    if ( IsConst( i ) )
      continue;

    switch ( i->src->type ) {
      case IFSGraph:
      case IFAElementN:
      case IFAElementM:
      case IFAElementP:
	break;

      default:
      *ReasonP = "loop import is the wrong type";
	return( FALSE );
      }
    }

  /* ARE ALL USES WITHIN THE SUBGRAPH? */
  /* for ( e = n->exp; e != NULL; e = e->esucc )
    if ( IsSGraph( e->dst ) )
      return( FALSE ); */

  return( TRUE );
}



/**************************************************************************/
/* LOCAL  **************      BuildFragement       ************************/
/**************************************************************************/
/* PURPOSE: REMOVE NODE n FROM Forall f1 AND ENCAPSOLATE IT IN ITS OWN    */
/*          FORALL NODE.  THEN LINK THE NEW Forall TO f1.                 */
/**************************************************************************/

static PNODE BuildFragement( f1, n )
PNODE f1;
PNODE n;
{
  register PNODE gen;
  register PNODE body;
  register PNODE ret;
  register PNODE f2;
  register PNODE nn;
  register PNODE nnn;
  register PEDGE i;
  register PEDGE e;
  register PEDGE ee;
  register PEDGE se;
  register PNODE aelm;
  register PNODE gat;
  register int   port;
  register PINFO TypeInfo;

  f2 = NodeAlloc( ++maxint, IFForall );

  CopyPragmas( f1, f2 );

  f2->info    = f1->info;
  f2->gname   = f1->gname;
  f2->alst    = f1->alst;
  f2->scnt    = f1->scnt;
  f2->if1line = f1->if1line;

  LinkNode( f1->npred, f2 );

  gen = CopyNode( f1->F_GEN );
  gen->G_DAD = f2;
  gen->gsucc = NULL;
  gen->gpred = NULL;

  LinkGraph( f2, gen ); /* THIS ASSUMES F_GEN IS THE FIRST GRAPH!!! */

  body = NodeAlloc( 0, IFSGraph );
  body->G_DAD = f2;
  CopyPragmas( f1->F_BODY, body );
  LinkGraph( gen, body );  /* THIS ASSUMES F_BODY IS THE SECOND GRAPH!!! */

  ret = NodeAlloc( 0, IFSGraph );
  ret->G_DAD = f2;
  CopyPragmas( f1->F_RET, ret );
  LinkGraph( body, ret );  /* THIS ASSUMES F_RET IS THE LAST GRAPH!!! */

  nn = CopyNode( n );
  LinkNode( body, nn );

  for ( i = n->imp; i != NULL; i = i->isucc ) {
    if ( IsConst( i ) ) {
      CopyEdgeAndReset( i, NULL_NODE, nn );
      continue;
      }

    if ( IsSGraph( i->src ) ) {
      CopyEdgeAndReset( i, body, nn );
      continue;
      }

    /* MUST BE A SPECIAL AElement NODE! */
    nnn = CopyNode( i->src );
    LinkNode( body, nnn );

    CopyEdgeAndReset( i->src->imp, body, nnn );
    CopyEdgeAndReset( i->src->imp->isucc, body, nnn );

    if ( i->src->imp->isucc->isucc != NULL )
      CopyEdgeAndReset( i->src->imp->isucc->isucc, body, nnn );

    CopyEdgeAndReset( i, nnn, nn );
    }

  for ( e = n->exp; e != NULL; e = n->exp ) {
    /* ALLOCATE AND LINK THE NEW NODE TO NEW BODY AND THREAD THE SYSTEM!! */
    port = ++maxint;
    ee = CopyEdge( e, nn, body );
    ee->iport = port;
    LinkExport( nn, ee   );
    LinkImport( body, ee );

    /* LINK THE LOWER BOUND AND COMPONENT VALUE TO THE NEW AGather NODE */
    gat = NodeAlloc( ++maxint, IFAGather );
    LinkNode( ret, gat );

    CopyEdgeAndReset( gen->imp->src->imp, ret, gat );
    gat->imp->iport = 1;

    ee = EdgeAlloc( ret, port, gat, 2 );

    if ( (TypeInfo = mihash[(e->info->label)%MAX_HASH]) == NULL ) {
      ee->info = FindInfo( ++maxint, IF_MULTIPLE );
      ee->info->A_ELEM = e->info;
      mihash[(e->info->label)%MAX_HASH] = ee->info;
      }
    else
      ee->info = TypeInfo;

    LinkExport( ret, ee );
    LinkImport( gat, ee );

    /* LINK THE NEW AGather TO THE NEW RETURN GRAPH */
    port = ++maxint;
    ee = EdgeAlloc( gat, 1, ret, port );

    if ( (TypeInfo = aihash[(e->info->label)%MAX_HASH]) == NULL ) {
      TypeInfo = FindInfo( ++maxint, IF_ARRAY ); 
      TypeInfo->A_ELEM = e->info;
      ee->info = TypeInfo;
      aihash[(e->info->label)%MAX_HASH] = TypeInfo;
      }
    else
      ee->info = TypeInfo;

    LinkExport( gat, ee );
    LinkImport( ret, ee );

    /* LINK THE TWO Foralls */
    ee = EdgeAlloc( f2, port, f1, port );
    ee->info = TypeInfo;
    LinkExport( f2, ee );
    LinkImport( f1, ee );

    /* BUILD THE NEW AElement NODE---SPECIAL */
    aelm = NodeAlloc( ++maxint, IFAElementN );
    LinkNode( n->npred, aelm );

    /* LINK IN THE CONTROL INDEX */
    ee = CopyEdge( f1->F_GEN->imp, f1->F_BODY, aelm );
    ee->eport = f1->F_GEN->imp->iport;
    ee->iport = 2;
    LinkImport( aelm, ee );
    LinkExport( f1->F_BODY, ee );

    /* LINK IN THE NEW ARRAY SOURCE TO aelm */
    ee = EdgeAlloc( f1->F_BODY, port, aelm, 1 );
    ee->info = TypeInfo;
    LinkExport( f1->F_BODY, ee );
    LinkImport( aelm, ee );

    /* MOVE n EXPORTS for e->eport TO THE NEW AElement NODE aelm */
    for ( ee = e->esucc; ee != NULL; ee = se ) {
      se = ee->esucc;

      if ( ee->eport != e->eport )
	continue;

      UnlinkExport( ee );
      ee->eport = 1;
      LinkExport( aelm, ee );
      }

    UnlinkExport( e );
    e->eport = 1;
    LinkExport( aelm, e );
    }

  /* COPY f1 IMPORTS TO f2 */
  for ( i = f1->imp; i != NULL; i = i->isucc ) {
    if ( IsExport( f2->F_GEN, i->iport ) || IsExport( f2->F_BODY, i->iport ) )
      CopyEdgeAndLink( i, f2, i->iport );
    }

  /* REMOVE n FROM f1 */
  OptRemoveDeadNode( n );

  return( f2 );
}


/**************************************************************************/
/* LOCAL  **************       ExplodeForalls      ************************/
/**************************************************************************/
/* PURPOSE: EXPLODE THE FORALL NODES IN GRAPH g. THIS ROUTINE RETURNS     */
/*          TRUE IF g IS THE BODY OF AN INNER LOOP.                       */
/**************************************************************************/

static int ExplodeForalls( g, explodeI )
PNODE g;
int   explodeI;
{
  register PNODE sg;
  register PNODE n;
  register PNODE nn;
  register PNODE sn;
  register PNODE snn;
  register PNODE f;
  register int   in1;
  register int   nin;
  char *Reason;


  for ( in1 = FALSE, n = g->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    if ( IsCompound( n ) ) {
      for ( nin = FALSE, sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        nin = nin ||  ExplodeForalls( sg, explodeI );

      in1 = in1 || nin;

      if ( IsLoop( n ) ) {
	in1 = TRUE;
	continue;
	}
      }

    if ( !IsForall( n ) )
      continue;

    ++Tefs;

    in1 = TRUE;

    if ( explodeI && nin )
      continue;

    /* CHECK IF THE CONTROL IS SUITABLE FOR EXPLOSION: A SINGLE Range NODE */
    if ( n->F_GEN->G_NODES->nsucc != NULL ||
    	n->F_GEN->imp->src->type != IFRangeGenerate ) 
      continue;

    efs++;
    EncodeIndexing( n->F_BODY, n->F_GEN->imp->iport, &maelm );

    for ( nn = n->F_BODY->G_NODES; nn != NULL; nn = snn ) {
      snn = nn->nsucc;

      if ( !IsExplodeCandidate( nn , &Reason) ) 
	continue;
      if (RequestInfo(I_Info1,info)) {
      DYNEXPAND(printinfo, printbuf, printlen, printcount, char, printlen+200);
      printlen += (SPRINTF(printinfo + printlen,
        " Exploding loop %d at line %d, funct %s, file %s\n\n",
         nn->ID, nn->line, nn->funct, nn->file), strlen(printinfo + printlen));
      }

      f = BuildFragement( n, nn ); /* THIS WILL REMOVE nn FROM n->F_BODY */

      DecodeIndexing( f->F_BODY );
      rff++;
      }

    DecodeIndexing( n->F_BODY );
    OptRemoveDeadNode( n ); /* NEW CANN 2/92 */
    }

  return( in1 );
}

/**************************************************************************/
/* GLOBAL **************     WriteExplodeInfo      ************************/
/**************************************************************************/
/* PURPOSE: WRITE FORALL EXPLOSION INFORMATION TO stderr.                 */
/**************************************************************************/

void WriteExplodeInfo()
{
  FPRINTF( infoptr, "\n\n **** FORALL EXPLOSIONS\n\n%s\n", printinfo);
  DYNFREE(printinfo, printbuf, printlen, printcount, char, NULL);
  FPRINTF( infoptr, " Exploded Foralls:            %d of %d\n", efs,Tefs   );
/*  FPRINTF( infoptr, " Resulting Forall Fragements: %d\n", rff   );
  FPRINTF( infoptr, " Modified AElement Nodes:     %d\n", maelm ); */
}


/**************************************************************************/
/* GLOBAL **************       If1Explode          ************************/
/**************************************************************************/
/* PURPOSE: EXPLODE THE FORALL NODES OF ALL FUNCTION GRAPHS.              */
/**************************************************************************/

void If1Explode( explodeI )
int explodeI;
{
  register PNODE f;
  register int   i;

  if ( noassoc ) return;

  for ( i = 0; i < MAX_HASH; i++ ) {
    mihash[i] = NULL;
    aihash[i] = NULL;
    }

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    ExplodeForalls( f, explodeI );
}
