/**************************************************************************/
/* FILE   **************          Graph.hh         ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 23 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include <vector>
#include "IFObject.hh"

#ifndef GRAPH_HH
#define GRAPH_HH

class IFObject;
class Node;
class Edge;

class Graph : public Node {
public:
   static Graph* NIL;

   Graph();
   virtual ~Graph();

   // -----------------------------------------------
   // IFObject interface
   // -----------------------------------------------
   virtual const char* object() const;
   virtual const char* letter() const;
   virtual int i1() const;
   virtual int i2() const;

   CHILD_INTERFACE(Graph,graph,Node,node);
   CHILD_INTERFACE(Node,node,Edge,edge);
   CHILD_INTERFACE(Node,node,EdgeCluster,edgecluster);

   // -----------------------------------------------
   // Node stuff
   // -----------------------------------------------
   virtual int label() const;
   virtual int nodeNumber() const;


protected:
   virtual void endDump(ostream&,unsigned level=0) const;

   vector< IFauto< Node > > mNode;
};

#endif
