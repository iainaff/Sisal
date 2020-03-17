/**************************************************************************/
/* FILE   **************      CompoundGraph.cc     ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "IFObject.hh"
#include "Node.hh"
#include "Compound.hh"
#include "Graph.hh"
#include "CompoundGraph.hh"

CompoundGraph* CompoundGraph::NIL = 0;

CompoundGraph::CompoundGraph(Compound* C) {
   IFObject::parent(C);
}

CompoundGraph::~CompoundGraph() {
   cerr << "Destroy compound graph " << this << endl;
}

const char* CompoundGraph::object() const {
   return "CompoundGraph";
}

unsigned int CompoundGraph::offset() const {
   assert(compound());
   return compound()->offsetOf(this);
}
 
Compound* CompoundGraph::compound() {
   assert(!parent() || dynamic_cast<Compound*>(parent()));
   return dynamic_cast<Compound*>(parent());
}

const Compound* CompoundGraph::compound() const {
   assert(!parent() || dynamic_cast<const Compound*>(parent()));
   return dynamic_cast<const Compound*>(parent());
}

void CompoundGraph::compound(Compound* M) { 
   parent(M);
}
