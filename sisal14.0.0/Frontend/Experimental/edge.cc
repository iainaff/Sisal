#include "edge.hh"

namespace sisalc {
   SP<edge> edge::null(0,true);

   void edge::self(SP<edge> p) {
      assert(p.get()==this);
      mSelf = p;
   }
   SP<edge> edge::self() const {
      assert(mSelf.get() == this);
      return mSelf;
   }

   edge::edge() 
      : mType(info::null)
   {
   }

   edge::edge(INFO type)
      : mSource(node::null), mSourcePort(0),
        mDestination(node::null), mDestinationPort(0),
        mType(type)
   {
   }


   bool edge::valid() const {
      return mDestination.get() && mSource.get();
   }

   int edge::i1() const {
      return (mSource.get())?(mSource->label()):0;
   }
   int edge::i2() const {
      return mSourcePort;
   }
   int edge::i3() const {
      return (mDestination.get())?(mDestination->label()):0;
   }
   int edge::i4() const {
      return mDestinationPort;
   }
   int edge::i5() const {
      if ( mType.get() ) {
         mType->label();
      } else {
         return 0;
      }
   }

   void edge::setDestination(SP<node> n, int p) {
      cerr << "My dest is set to node " << n->label() << endl;
      mDestination = n;
      mDestinationPort = p;
   }

   void edge::setSource(SP<node> n, int p) {
      cerr << "My source is set to node " << n->label() << endl;
      mSource = n;
      mSourcePort = p;
   }

   void edge::setType(INFO I) {
      mType = I;
   }
}
