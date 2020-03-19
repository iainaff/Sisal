/**************************************************************************/
/* FILE   **************        Compound.cc        ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "IFObject.hh"
#include "Node.hh"
#include "Edge.hh"
#include "Graph.hh"
#include "Compound.hh"
#include "CompoundGraph.hh"

Compound* Compound::NIL = 0;

INHERITED_CHILD(Compound,Node,node,Edge,edge)
CHILD_IMPLEMENTATION(Compound,compound,CompoundGraph,compoundGraph,IFauto)

Compound::Compound(Graph* G)
   : Node(G)
{
}
Compound::Compound(int opcode,Graph* G)
   : Node(opcode,G)
{
   mCompoundGraph.clear();
}

Compound::Compound(string s,Graph* G)
   : Node(s,G)
{
   cerr << "Destroy compound " << this << endl;
}

Compound::~Compound() {

}

const char* Compound::object() const {
   return "Compound";
}

const char* Compound::letter() const {
   return "{ Compound";
}

void Compound::endDump(ostream& os,unsigned level=0) const {
   dumpObjects(mCompoundGraph.begin(),mCompoundGraph.end(), os, level);
   indent(os,level-1);
   os << "}" << endl;
   Node::endDump(os,level);
}

