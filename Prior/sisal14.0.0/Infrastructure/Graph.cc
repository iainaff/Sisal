/**************************************************************************/
/* FILE   **************          Graph.cc         ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 23 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "IFObject.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Graph.hh"

Graph* Graph::NIL = 0;

CHILD_IMPLEMENTATION(Graph,graph,Node,node,IFauto)
INHERITED_CHILD(Graph,Node,node,Edge,edge)
INHERITED_CHILD(Graph,Node,node,EdgeCluster,edgecluster)

Graph::Graph() {
}

Graph::~Graph() {
   cerr << "Destroy graph " << this << endl;

}

const char* Graph::object() const {
   return "Graph";
}

const char* Graph::letter() const {
   return "G";
}

int Graph::i1() const {
   return 0;
}
int Graph::i2() const {
   return IFObject::BLANK;
}

int Graph::label() const {
   return 0;
}


void Graph::endDump(ostream& os,unsigned level=0) const {
   Node::endDump(os,level);
   os << endl;
   dumpObjects(mNode.begin(),mNode.end(), os, level);

}
int Graph::nodeNumber() const {
   return 0; // Graphs are always label 0 for edges
}
