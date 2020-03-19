/**************************************************************************/
/* FILE   **************          Edge.cc          ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "IFObject.hh"
#include "Info.hh"
#include "Node.hh"
#include "Edge.hh"
#include "EdgeCluster.hh"

Edge* Edge::NIL = 0;

Edge::Edge(Info* T)
   : mType(T)
{
}

Edge::~Edge() {
   cerr << "Destroy edge " << this << endl;
}

const char* Edge::object() const {
   return "Edge";
}

const char* Edge::letter() const {
   return "E";
}

int Edge::i1() const {
   if ( mEdgeCluster ) {
      if ( mEdgeCluster->node() ) {
         mEdgeCluster->node()->nodeNumber();
      }
   } else {
      return IFObject::UNDEFINED;
   }
}
int Edge::i2() const {
   if ( mEdgeCluster ) {
      mEdgeCluster->offset();
   } else {
      return IFObject::UNDEFINED;
   }
}

int Edge::i3() const {
   if ( node() ) {
      return node()->label();
   } else {
      return IFObject::UNDEFINED;
   }
}

int Edge::i4() const {
   if ( node() ) {
      return offset();
   } else {
      return IFObject::UNDEFINED;
   }
}

int Edge::i5() const {
   if ( type() && type()->module() ) {
      return type()->offset();
   } else {
      return IFObject::UNDEFINED;
   }
}

unsigned int Edge::offset() const {
   assert(node());
   return node()->offsetOf(this);
}
 
Node* Edge::node() {
   assert((!parent()) || dynamic_cast<Node*>(parent()));
   return dynamic_cast<Node*>(parent());
}

const Node* Edge::node() const {
   assert((!parent()) || dynamic_cast<const Node*>(parent()));
   return dynamic_cast<const Node*>(parent());
}

void Edge::node(Node* M) { 
   parent(M);
}


Info* Edge::type() {
   return mType.pointer();
}

const Info* Edge::type() const {
   return mType.pointer();
}

void Edge::type(Info* T) {
   mType.assign(T);
}

EdgeCluster* Edge::edgecluster() {
   return mEdgeCluster.pointer();
}

const EdgeCluster* Edge::edgecluster() const {
   return mEdgeCluster.pointer();
}

void Edge::edgecluster(EdgeCluster* T) {
   mEdgeCluster.assign(T);
}
