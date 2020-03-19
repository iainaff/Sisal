/**************************************************************************/
/* FILE   **************          Node.hh         ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 23 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include "IFObject.hh"
#include <string>

#ifndef NODE_HH
#define NODE_HH

class IFObject;
class Graph;
class Edge;
class EdgeCluster;

class Node : public IFObject {
public:
   static Node* NIL;

   Node(Graph* G=0);
   Node(int, Graph* G=0);
   Node(string, Graph* G=0);
   virtual ~Node();

   // -----------------------------------------------
   // IFObject interface
   // -----------------------------------------------
   virtual const char* object() const;
   virtual const char* letter() const;
   virtual int i1() const;
   virtual int i2() const;

   // -----------------------------------------------
   // Parent interface
   // -----------------------------------------------
   virtual unsigned int offset() const;
   Graph* graph();
   const Graph* graph() const;
   void graph(Graph*);

   CHILD_INTERFACE(Node,node,Edge,edge);
   CHILD_INTERFACE(Node,node,EdgeCluster,edgecluster);

   // -----------------------------------------------
   // At last!!! Node stuff
   // -----------------------------------------------
   virtual int nodeNumber() const;
   virtual int label() const;
   virtual int opcode() const;
   virtual void opcode(int);

protected:
   virtual void endDump(ostream&,unsigned level=0) const;
   vector< IFauto<Edge> > mEdge;
   vector< IFauto<EdgeCluster> > mEdgeCluster;
   int mOpcode;
};

#endif
