#include "world.h"

/**************************************************************************/
/* GLOBAL **************          CopyNode         ************************/
/**************************************************************************/
/* PURPOSE: RETURN A COPY OF NODE n.  IF n IS A GRAPH NODE, THEN EACH     */
/*          NODE IN ITS NODE LIST IS COPIED ALONG WITH ITS IMPORT LIST.   */
/*          IF n IS A COMPOUND NODE, THEN EACH SUBGRAPH IN ITS SUBGRAPH   */
/*          LIST IS COPIED.                                               */
/**************************************************************************/

PNODE CopyNode( n )
PNODE n;
{
  register PNODE nn;
  register PNODE nd;
  register PNODE p;
  register PNODE g;
  
  n->copy = nn = NodeAlloc( n->label, n->type );
  
  CopyPragmas( n, nn );
  
  nn->info    = n->info;
  nn->gname   = n->gname;
  nn->alst    = n->alst;
  nn->scnt    = n->scnt;
  nn->if1line = n->if1line;
  
  switch ( n->type ) 
    {
    case IFIGraph:
      break;
      
    case IFSGraph:
      nn->G_DAD = n->G_DAD->copy;
    case IFLGraph:
    case IFXGraph:
      p = nn;
      
      for ( nd = n->G_NODES; nd != NULL; nd = nd->nsucc ) {
	LinkNode( p, CopyNode( nd ) );
	p = nd->copy;
      }
      
      CopyImports( n, nn, TRUE );
      
      for ( nd = n->G_NODES; nd != NULL; nd = nd->nsucc )
	CopyImports( nd, nd->copy, TRUE );
      
      break;
      
    case IFSelect:
    case IFTagCase:
    case IFForall:
    case IFLoopA:
    case IFLoopB:
    case IFIfThenElse:
    case IFWhileLoop:
    case IFRepeatLoop:
    case IFSeqForall:
      p = nn;
      
      for ( g = n->C_SUBS; g != NULL; g = g->gsucc ) {
	LinkGraph( p, CopyNode( g ) );
	p = g->copy;
      }
      
      break;
      
    default:
      break;
    }
  
  return( nn );
}
