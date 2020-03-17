/**************************************************************************/
/* FILE   **************          Node.cc         ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 23 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include <map>

#include "IFObject.hh"
#include "Node.hh"
#include "Graph.hh"
#include "Edge.hh"
#include "EdgeCluster.hh"

static map<string,unsigned int> opcodes;

// -----------------------------------------------
// Standard opcodes
// -----------------------------------------------
static int setup() {
   opcodes["Forall"] = 0;
   opcodes["Select"] = 1;
   opcodes["TagCase"] = 2;
   opcodes["LoopA"] = 3;
   opcodes["LoopB"] = 4;
   opcodes["IfThenElse"] = 5;
   opcodes["Iterate"] = 6;
   opcodes["WhileLoop"] = 7;
   opcodes["RepeatLoop"] = 8;
   opcodes["SeqForall"] = 9;
   opcodes["UReduce"] = 10;

   opcodes["AAddH"] = 100;
   opcodes["AAddL"] = 101;
   opcodes["AAdjust"] = 102;
   opcodes["ABuild"] = 103;
   opcodes["ACatenate"] = 104;
   opcodes["AElement"] = 105;
   opcodes["AFill"] = 106;
   opcodes["AGather"] = 107;
   opcodes["AIsEmpty"] = 108;
   opcodes["ALimH"] = 109;
   opcodes["ALimL"] = 110;
   opcodes["ARemH"] = 111;
   opcodes["ARemL"] = 112;
   opcodes["AReplace"] = 113;
   opcodes["AScatter"] = 114;
   opcodes["ASetL"] = 115;
   opcodes["ASize"] = 116;
   opcodes["Abs"] = 117;
   opcodes["BindArguments"] = 118;
   opcodes["Bool"] = 119;
   opcodes["Call"] = 120;
   opcodes["Char"] = 121;
   opcodes["Div"] = 122;
   opcodes["Double"] = 123;
   opcodes["Equal"] = 124;
   opcodes["Exp"] = 125;
   opcodes["FirstValue"] = 126;
   opcodes["FinalValue"] = 127;
   opcodes["Floor"] = 128;
   opcodes["Int"] = 129;
   opcodes["IsError"] = 130;
   opcodes["Less"] = 131;
   opcodes["LessEqual"] = 132;
   opcodes["Max"] = 133;
   opcodes["Min"] = 134;
   opcodes["Minus"] = 135;
   opcodes["Mod"] = 136;
   opcodes["Neg"] = 137;
   opcodes["NoOp"] = 138;
   opcodes["Not"] = 139;
   opcodes["NotEqual"] = 140;
   opcodes["Plus"] = 141;
   opcodes["RangeGenerate"] = 142;
   opcodes["RBuild"] = 143;
   opcodes["RElements"] = 144;
   opcodes["RReplace"] = 145;
   opcodes["RedLeft"] = 146;
   opcodes["RedRight"] = 147;
   opcodes["RedTree"] = 148;
   opcodes["Reduce"] = 149;
   opcodes["RestValues"] = 150;
   opcodes["Single"] = 151;
   opcodes["Times"] = 152;
   opcodes["Trunc"] = 153;
   opcodes["PrefixSize"] = 154;
   opcodes["Error"] = 155;
   opcodes["ReplaceMulti"] = 156;
   opcodes["Convert"] = 157;
   opcodes["CallForeign"] = 158;
   opcodes["AElementN"] = 159;
   opcodes["AElementP"] = 160;
   opcodes["AElementM"] = 161;
   opcodes["AAddLAT"] = 170;
   opcodes["AAddHAT"] = 171;
   opcodes["ABufPartition"] = 172;
   opcodes["ABuildAT"] = 173;
   opcodes["ABufScatter"] = 174;
   opcodes["ACatenateAT"] = 175;
   opcodes["AElementAT"] = 176;
   opcodes["AExtractAT"] = 177;
   opcodes["AFillAT"] = 178;
   opcodes["AGatherAT"] = 179;
   opcodes["ARemHAT"] = 180;
   opcodes["ARemLAT"] = 181;
   opcodes["AReplaceAT"] = 182;
   opcodes["ArrayToBuf"] = 183;
   opcodes["ASetLAT"] = 184;
   opcodes["DefArrayBuf"] = 185;
   opcodes["DefRecordBuf"] = 186;
   opcodes["FinalValueAT"] = 187;
   opcodes["MemAlloc"] = 188;
   opcodes["BufElements"] = 189;
   opcodes["RBuildAT"] = 190;
   opcodes["RecordToBuf"] = 191;
   opcodes["RElementsAT"] = 192;
   opcodes["ReduceAT"] = 193;
   opcodes["ShiftBuffer"] = 19;
   opcodes["ScatterBufPartitions"] = 195;
   opcodes["RedLeftAT"] = 196;
   opcodes["RedRightAT"] = 197;
   opcodes["RedTreeAT"] = 198;
   return 9999;
}

static int dummy = setup();

Node* Node::NIL = 0;

CHILD_IMPLEMENTATION(Node,node,Edge,edge,IFauto)
CHILD_IMPLEMENTATION(Node,node,EdgeCluster,edgecluster,IFauto)

Node::Node(Graph* G)
   : IFObject(G), mOpcode(IFObject::UNDEFINED)
{
}


Node::Node(int opcode,Graph* G)
   : IFObject(G), mOpcode(opcode)
{
}

Node::Node(string s,Graph* G)
   : IFObject(G), mOpcode(IFObject::UNDEFINED)
{
   map<string,unsigned int>::iterator location = opcodes.find(s);
   if ( location != opcodes.end() ) {
      opcode((*location).second);
   }
}

Node::~Node() {
   cerr << "Destroy node  " << this << endl;

}

const char* Node::object() const {
   return "Node";
}

const char* Node::letter() const {
   return "N";
}

int Node::i1() const {
   if ( mParent ) {
      return offset();
   } else {
      return IFObject::UNDEFINED;
   }
}

int Node::i2() const {
   return opcode();
}

int Node::label() const {
   if ( mParent ) {
      return offset();
   } else {
      return IFObject::UNDEFINED;
   }
}

unsigned int Node::offset() const {
   assert(graph());
   return graph()->offsetOf(this);
}
 
Graph* Node::graph() {
   assert((!parent()) || dynamic_cast<Graph*>(parent()));
   return dynamic_cast<Graph*>(parent());
}

const Graph* Node::graph() const {
   assert((!parent()) || dynamic_cast<const Graph*>(parent()));
   return dynamic_cast<const Graph*>(parent());
}

void Node::graph(Graph* M) { 
   parent(M);
}

void Node::endDump(ostream& os,unsigned level=0) const {
   dumpObjects(mEdge.begin(),mEdge.end(), os, level);
   IFObject::endDump(os,level);
}

int Node::opcode() const {
   return mOpcode;
}

void Node::opcode(int opcode) {
   mOpcode = opcode;
}

int Node::nodeNumber() const {
   return offset();
}
