#include <string>
#include <algorithm>
#include "module.hh"

namespace sisalc {
   module::module()
      : mName("")
   {}
   module::module(const string& name)
      : mName(name)
   {}
   module::module(const char* name)
      : mName(name)
   {}

   void module::addGraph(GRAPH G) {
      mGraph.push_back(G);
   }

   void module::addInfo(INFO I) {
      // See if we have it already
      vector<INFO>::iterator x = find(mInfo.begin(), mInfo.end(), I);
                    
      if ( x == mInfo.end() ) {
         I->setParent(self());
         mInfo.push_back(I);
      }
   }

   void module::writeSelf(ostream& os) const {
         for(int i=0; i < mInfo.size(); ++i) os << *mInfo[i];
         for(int i=0; i < mStamp.size(); ++i) os << mStamp[i];
         for(int i=0; i < mGraph.size(); ++i) os << *mGraph[i];
      }

   ostream& operator<<(ostream& os, const module& object) {
      object.writeSelf(os);
      return os;
   }

   void module::addStamp(stamp s) {
      mStamp.push_back(s);
   }

   void module::self(MODULE p) {
      assert(p.get()==this);
      mSelf = p;
   }
   MODULE module::self() const {
      assert(mSelf.get() == this);
      return mSelf;
   }

   int module::offset(const INFO I) const {
      for(vector< INFO >::const_iterator itr = mInfo.begin();
          itr != mInfo.end();
          ++itr) {
         if  ( I.get() == (*itr).get() ) return itr - mInfo.begin() + 1;
      }
      return 0;
   }
}
