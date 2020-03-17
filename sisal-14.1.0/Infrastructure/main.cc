/**************************************************************************/
/* FILE   **************          main.cc          ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 24 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include <iostream>

#include "IFObject.hh"
#include "IF.hh"
#include "Module.hh"
#include "Info.hh"
#include "Edge.hh"
#include "EdgeCluster.hh"
#include "Literal.hh"
#include "Stamp.hh"
#include "Node.hh"
#include "Graph.hh"
#include "Function.hh"      
#include "Compound.hh"
#include "CompoundGraph.hh"

#include "BasicType.hh"
#include "ChainType.hh"
#include "TupleType.hh"
#include "Use.hh"
#include "FunctionType.hh"

// -----------------------------------------------
// -----------------------------------------------

int main() {
   try {
      
      Module* M = new Module("foobar");

      M->set(new Use("stuff"));

      cerr << "x" << endl;
      M->set(new BasicType(BasicType::IFBOOL));
      cerr << "x" << endl;
      M->set(new BasicType(BasicType::IFCHAR));
      M->set(new BasicType(BasicType::IFDOUBLE));
      M->set(new BasicType(BasicType::IFINTEGER));
      M->set(new BasicType(BasicType::IFNULL));
      M->set(new BasicType(BasicType::IFREAL));
      M->set(new BasicType(BasicType::IFWILD));

      TupleType* C = new TupleType();
      C->append(M->info(4));
      M->set(C);
      cerr << C->canonicalName() << endl;

      TupleType* C2 = new TupleType();
      M->set(C2);

      FunctionType* FT = new FunctionType(C2,C);
      M->set(FT);
      cerr << FT->canonicalName() << endl;

      M->set(new Stamp("Faked IF1CHECK"),'C');
      M->set(new Stamp("Nodes are DFOrdered"),'D');
      M->set(new Stamp("OpenSource Frontend Version 0.1"),'F');

      Function* F = new Function("three",FT);
      M->set(F);
      F->set(new Literal(M->info(4),"3"));

      M->dump(cerr);

      Compound* CC = new Compound;
      CompoundGraph* G = new CompoundGraph;
      CC->set(G,99);
      Node* N1 = new Node(100);
      Node* N2 = new Node(200);
      G->set(N1);
      G->set(N2);

      Edge* E = new Edge;
      N2->set(E);
      EdgeCluster* EE = new EdgeCluster();
      N1->set(EE,5);
      EE->set(E);
      E->edgecluster(EE);

      Edge* E2 = new Edge;
      G->set(E2,9);
      EdgeCluster* EE2 = new EdgeCluster();
      N1->set(EE2,3);
      EE2->set(E2);
      E2->edgecluster(EE2);

      Edge* E3 = new Edge;
      N1->set(E3,7);
      EdgeCluster* EE3 = new EdgeCluster();
      G->set(EE3,3);
      EE3->set(E3);
      E3->edgecluster(EE3);


      CC->dump(cerr);

   } catch (const char* msg) {
      cerr << "OOPS: " << msg << endl;
   }
   return 0;
}

void std::free(void* p) {
   cerr << "free(" << p << ")\n";
}
