#include <assert.h>
#include "graph.hh"

namespace sisalc {

   GRAPH graph::null(0,true);

   graph::graph() {
      mOpCode = 1000; 
   }

   graph::graph(unsigned opCode) {
      mOpCode = opCode;
      assert(valid());
   }

   graph::graph(const char* s)
   {
      mOpCode = lookup(s);
   }

   static graph::operation_t definitions[] = {
      {"SGraph",1000},
      {"LGraph",1001},
      {"UGraph",1002},
      {"XGraph",1003},
      {"FLGraph",1004},
      {"RLGraph",1005},
      {0,0}
   };

   const graph::operation_t* graph::getTable() const {
      return definitions;
   }

   void graph::writeSelf(ostream& os) const {
      assert(valid());

      node::writeSelf(os);
      for(vector< SP<node> >::const_iterator itr = mNodes.begin();
          itr != mNodes.end();
          ++itr) {
         // I had better own this thing!
         assert((*itr)->parent().get() == this);
         (*itr)->writeSelf(os);
      }
      os << endl;
   }

   void graph::addNode(SP<node> N) {
      mNodes.push_back(N);
      N.get()->setParent(mSelf);
   }

   void graph::setType(INFO I) {
      mType = I;
   }

   int graph::offset(const SP<node> N) const {
      for(vector< SP<node> >::const_iterator itr = mNodes.begin();
          itr != mNodes.end();
          ++itr) {
         if  ( N.get() == (*itr).get() ) return itr - mNodes.begin() + 1;
      }
      return 0;
   }
   
   int graph::i1() const {
      if ( mType.get() ) {
         mType->label();
      } else {
         return 0;
      }
   }

   void graph::self(GRAPH p) {
      node::self(p);
      assert(p.get()==this);
      mSelf = p; 
   }   
   GRAPH graph::self() const {
      assert(mSelf.get() == this);
      return mSelf;
   }

   bool graph::valid() const {
      return true;
   }
}
