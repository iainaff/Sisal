/**************************************************************************/
/* FILE   **************       EdgeCluster.cc      ************************/
/************************************************************************ **/
/* Author: Patrick Miller July  1 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "EdgeCluster.hh"
#include "Edge.hh"
#include "Node.hh"

EdgeCluster* EdgeCluster::NIL = 0;

CHILD_IMPLEMENTATION(EdgeCluster,edgecluster,Edge,edge,IF)

EdgeCluster::EdgeCluster(Node* N)
   : IFObject(N)
{
}

EdgeCluster::~EdgeCluster() {
   cerr << "Destroy EdgeCluster" << endl;
   mEdge.clear();
}

unsigned int EdgeCluster::offset() const {
   assert(node());
   return node()->offsetOf(this);
}

Node* EdgeCluster::node() {
   assert((!parent()) || dynamic_cast<Node*>(parent()));
   return dynamic_cast<Node*>(parent());
}

const Node* EdgeCluster::node() const {
   assert((!parent()) || dynamic_cast<const Node*>(parent()));
   return dynamic_cast<const Node*>(parent());
}

void EdgeCluster::node(Node* M) { 
   parent(M);
}
