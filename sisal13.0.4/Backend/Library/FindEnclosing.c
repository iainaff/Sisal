#include "world.h"


/**************************************************************************/
/* GLOBAL **************         FindEnclosing        *********************/
/**************************************************************************/
/* PURPOSE: FIND THE STRUCTURE ENCLOSING THIS NODE.                       */
/*          A TOP-LEVEL MODULE RETURNS glstop.                            */
/*          A SUBGRAPH RETURNS ITS GRAPH.                                 */
/*          A GENERAL NODE RETURNS ITS ENCLOSING SUBGRAPH OR GRAPH        */
/**************************************************************************/

PNODE
FindEnclosing(node)
  PNODE	node;
{
  PNODE g=NULL, graph;

  if ( IsGraph(node) ) {
      for (g = node->gpred; g!=NULL; g = g->gpred)
	graph = g;
  } else {
      for (graph = node->npred; !IsGraph(graph); graph = graph->npred);
  }

  return graph;
}

/* $Log$
 * Revision 1.4  1994/06/07  14:40:27  solomon
 * Function was returning NULL when node was a graph.  This is now fixed.
 *
 * Revision 1.3  1994/05/04  18:10:58  denton
 * R_BODY->R_MAP; FindEnclosingCompound->FindEnclosing
 *
 * Revision 1.2  1994/03/03  17:16:02  solomon
 * Fixed a bug in line 20.  It was "for (graph = graph->gpred ...)" when
 * graph had not yet been defined.  I changed it to "grapn = node->gpred."
 *
 * Revision 1.1  1994/01/28  00:09:30  miller
 * new man pages and new helper functions for library
 *
 * */
