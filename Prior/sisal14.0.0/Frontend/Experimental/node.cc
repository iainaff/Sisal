/**************************************************************************/
/* FILE   **************          node.cc          ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include "IFCore.hh"

namespace sisalc {

   void node::writeSelf(ostream& os) const {
      assert(valid());

      IFObject::writeSelf(os);
      for(unsigned int port=1; port <= mInputs.size(); ++port) {
         os << *mInputs[port-1];
      }
   }

   node::node() 
      : mImplementation(0), mParent(0)
   {
   }

   node::node(NodeImplementation* implementation) 
      : mImplementation(implementation), mParent(0)
   {
   }

   node::node(unsigned int opcode)
      : mImplementation((*opcodeTable())[opcode]), mParent(0)
   {
   }


   node::node(const char* s,string fileName,int sourceLine)
      : mImplementation((*nameTable())[s]), mParent(0)
   {
       if ( fileName != "" ) pragma("sf",fileName);
       if ( sourceLine ) pragma("sl",sourceLine);
   }

   void node::setGraph(graph* parent) {
      assert(parent && parent->offset(this) > 0);
      mParent = parent;
   }

   bool node::valid() const { return mImplementation != 0; }

   int node::label() const {
      assert(mParent);
      return mParent->offset(this);
   }
   int node::i1() const {return label();}
   int node::i2() const {
      assert(mImplementation);
      return mImplementation->opcode();
   }

   void node::attachInput(edge* E, unsigned int port) {
      assert(E);

      // Make sure the edge points to node
      E->setDestination(this,port);

      while ( mInputs.size() < port ) mInputs.push_back(0);
      // Must be empty before attaching edge
      assert(mInputs[port-1] == 0);
      mInputs[port-1] = E;
   }

   void node::attachOutput(edge* E, unsigned int port) {
      assert(E);

      E->setSource(this,port);
      if (  mOutputs.size() < port ) mOutputs.resize(port);
      mOutputs[port-1].push_back(E);
   }


   void node::typeBinding() {
      cerr << "op code " << mImplementation->opcode() <<endl;
      mImplementation->typeBinding(this);
   }

   static map<const char*,NodeImplementation*,node::ltCharP>* byName = 0;
   map<const char*,NodeImplementation*,node::ltCharP>* node::nameTable() const { return byName; }
   static map<unsigned int,NodeImplementation*>* byOpcode = 0;
   map<unsigned int,NodeImplementation*>* node::opcodeTable() const { return byOpcode; }
   int node::registration(NodeImplementation* x) {
      if ( byName == 0 ) byName = new map<const char*,NodeImplementation*,ltCharP>;
      if ( byOpcode == 0 ) byOpcode = new map<unsigned int,NodeImplementation*>;

      (*byName)[x->name()] = x;
      (*byOpcode)[x->opcode()] = x;

      return 1234; // Result doesn't really matter
   }
   

   Function* node::function() {
      assert(mParent);
      graph* G = 0;
      for( G = mParent; G && G->mParent != G; G = G->mParent) {
      }
      assert(G);
      assert(G->mParent);
      Function* F = dynamic_cast<Function*>(G->mParent);
      assert(F);
      return F;
   }

   const Function* node::function() const {
      assert(mParent);
      graph* G = 0;
      for( G = mParent; G && G->mParent != G; G = G->mParent) {
      }
      assert(G);
      assert(G->mParent);
      Function* F = dynamic_cast<Function*>(G->mParent);
      assert(F);
      return F;
   }

   /**************************************************************************/
   /* GLOBAL **************         signature         ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   vector<const info*> node::signature() const {
      vector<const info*> result;
      for(vector<edge*>::const_iterator ePtr = mInputs.begin();
          ePtr != mInputs.end();
          ++ePtr) {
         if ( *ePtr ) {
            result.push_back((*ePtr)->type());
         } else {
            result.push_back(0);
         }
      }
      return result;
   }
}
