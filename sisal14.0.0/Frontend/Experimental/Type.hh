#ifndef TYPE_HH
#define TYPE_HH

#include <string>
#include <algorithm>

#include "semanticBase.hh"
using sisalc::semanticBase;

#include "info.hh"
using sisalc::INFO;

class Type : public semanticBase {
public:
   Type() {}
   Type(INFO result) {value(result);}

   virtual string self() const { return "Type"; }

   void mergeIn(const Type* T) {
      if ( T ) {
         copy( T->begin(), T->end(), 
               inserter(mSupport,mSupport.end()) );
      }
   }
   
   INFO value() {
      assert( mSupport.size() > 0 );
      return mSupport[0];
   }

   void value(INFO x) {
      if ( mSupport.size() == 0 ) {
         mSupport.push_back(x);
      } else {
         mSupport[0] = x;
      }
   }

   void support(INFO x) {
      if ( mSupport.size() == 0 ) {
         mSupport.push_back(info::null);
      }
      mSupport.push_back(x);
   }

   typedef vector<INFO>::iterator iterator;
   typedef vector<INFO>::const_iterator const_iterator;
   iterator begin() { return mSupport.begin(); }
   iterator end() { return mSupport.end(); }
   const_iterator begin() const { return mSupport.begin(); }
   const_iterator end() const{ return mSupport.end(); }
      

protected:
   vector<INFO> mSupport;
};

#endif
