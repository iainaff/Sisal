#include "world.h"

/**************************************************************************/
/* GLOBAL **************         DirectEdge        ************************/
/**************************************************************************/
/* PURPOSE: DIRECT AN EDGE FROM src TO dst IN SIMPLE OR COMPOUND GRAPH.   */
/**************************************************************************/

PEDGE
DirectEdge(src, eport, dst, iport, info)
  PNODE	src;
  int	eport;
  PNODE	dst;
  int	iport;
  PINFO	info;
{
  PNODE	src_graph;
  PNODE	dest_graph;
  PEDGE	edge;
  PNODE	compound_graph;
  int	new_port;


  for (src_graph=src; !IsGraph(src_graph); src_graph = src_graph->npred);
  for (dest_graph=dst; !IsGraph(dest_graph); dest_graph = dest_graph->npred);

  /**********************************************************************/
  /* If the nodes are in the same graph, just link them together.	*/
  /**********************************************************************/

  if ( src_graph == dest_graph ) {
    edge = EdgeAlloc( src, eport, dst, iport );

    EdgeAssignPragmas( edge );
    StandardPragmas(&pragmas);
    edge->info = info;
    edge->funct = NULL;
    edge->file  = NULL;

    LinkImport( dst, edge );
    LinkExport( src, edge );

  /**********************************************************************/
  /* Add an edge from the graph to the destination node and		*/
  /* recursively add an edge from the source to the compound.		*/
  /* Find the enclosing compound and first free port.			*/
  /**********************************************************************/

  } else {
    compound_graph = FindEnclosing(dest_graph);
    new_port = FindLargestPort(compound_graph) + 1;
    (void)DirectEdge(dest_graph, new_port, dst, iport, info);
    edge = DirectEdge(src, eport, compound_graph, new_port, info);
  }

  return edge;
}

/* $Log$
 * Revision 1.3  1994/05/04  18:10:57  denton
 * R_BODY->R_MAP; FindEnclosingCompound->FindEnclosing
 *
 * Revision 1.2  1994/04/14  20:57:13  solomon
 * Changed variable tmax to LargestTypeLabelSoFar.  Variable tmax was used
 * for a couple things.  I created the LargestTypeLabelSoFar variable for
 * type labels only.
 *
 * Revision 1.1  1994/01/28  00:09:29  miller
 * new man pages and new helper functions for library
 *
 * */
