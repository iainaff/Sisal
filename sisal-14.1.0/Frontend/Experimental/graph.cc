#include <assert.h>
#include "IFCore.hh"
#include "FunctionInfo.hh"
#include <algorithm>

namespace sisalc {

   graph::graph() {
      mParent = this;
   }

   graph::graph(unsigned int opcode)
      : node((*opcodeTable())[opcode])
   {
      mParent = this;
      cerr << "Bulid a graph " << opcode << " with impl " << mImplementation << endl;
      assert(valid());
   }

   graph::graph(const char* s)
      : node((*nameTable())[s])
   {
      mParent = this;
      cerr << "Bulid a graph " << s << " with impl " << mImplementation << endl;
      assert(valid());
   }


   void graph::writeSelf(ostream& os) const {
      assert(valid());

      node::writeSelf(os);
      for(vector< node* >::const_iterator itr = mNodes.begin();
          itr != mNodes.end();
          ++itr) {
         // I had better own this thing!
         assert((*itr)->parent() == this);
         (*itr)->writeSelf(os);
      }
      os << endl;
   }

   void graph::addNode(node* N) {
      mNodes.push_back(N);
      N->setGraph(this);
   }

   int graph::offset(const node* N) const {
      if ( N == this ) return 0;
      vector< node* >::const_iterator position = 
         find(mNodes.begin(), mNodes.end(), N);
      assert(position != mNodes.end());
      return position - mNodes.begin() + 1;
   }
   
   int graph::i1() const {
      return 0;
   }

   int graph::i2() const {
      return -1;
   }

   bool graph::valid() const {
      return true;
   }

/**************************************************************************/
/* GLOBAL **************        typeBinding        ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
   void graph::typeBinding() {
      cerr << "Bind graph\n";
      assert(false);
   }

/**************************************************************************/
/* GLOBAL **************        registration       ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
   static map<const char*,NodeImplementation*,node::ltCharP>* byName = 0;
   map<const char*,NodeImplementation*,node::ltCharP>* graph::nameTable() const { return byName; }
   static map<unsigned int,NodeImplementation*>* byOpcode = 0;
   map<unsigned int,NodeImplementation*>* graph::opcodeTable() const { return byOpcode; }
   int graph::registration(NodeImplementation* x) {
      if ( byName == 0 ) byName = new map<const char*,NodeImplementation*,ltCharP>;
      if ( byOpcode == 0 ) byOpcode = new map<unsigned int,NodeImplementation*>;

      (*byName)[x->name()] = x;
      (*byOpcode)[x->opcode()] = x;

      return 1234; // Result doesn't really matter
   }
}
