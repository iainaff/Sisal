#include "world.h"


/**************************************************************************/
/* GLOBAL **************      FindLargestImport     ***********************/
/**************************************************************************/
/* PURPOSE: RETURN THE LARGEST IMPORT TO NODE node.                       */
/**************************************************************************/

int FindLargestImport( node )
PNODE node;
{
  register PEDGE edge;
  int largest_port = 0;

  if ( IsCompound ( node ) ) {
    PNODE       subgraph;
    int         sub_largest_port = 0;
    for (subgraph = node->C_SUBS; subgraph!=NULL; subgraph = subgraph->gsucc) {
      sub_largest_port = FindLargestImport( subgraph );
      if ( sub_largest_port > largest_port )
        largest_port = sub_largest_port;
    }
  }

  for ( edge = node->imp; edge != NULL; edge = edge->isucc ) {
    if ( edge->iport > largest_port )
        largest_port = edge->iport;
    if ( edge->isucc == NULL )
      break;
  }

  return largest_port;
}

/* $Log$
 * Revision 1.1  1994/01/28  00:09:34  miller
 * new man pages and new helper functions for library
 *
 * */
