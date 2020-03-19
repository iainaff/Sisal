/**************************************************************************/
/* FILE   **************          Type.hh          ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef TYPE_HH
#define TYPE_HH

#include <string>
#include <algorithm>

#include "semanticBase.hh"
using sisalc::semanticBase;

#include "IFCore.hh"
using sisalc::info;

class Type : public semanticBase {
public:
   Type() {}
   Type(info* result) {value(result);}

   virtual string self() const { return "Type"; }

   void mergeIn(const Type* T) {
      if ( T ) {
         copy( T->begin(), T->end(), 
               inserter(mSupport,mSupport.end()) );
      }
   }
   
   info* value() {
      assert( mSupport.size() > 0 );
      return mSupport[0];
   }

   void value(info* x) {
      if ( mSupport.size() == 0 ) {
         mSupport.push_back(x);
      } else {
         mSupport[0] = x;
      }
   }

   void support(info* x) {
      if ( mSupport.size() == 0 ) {
         mSupport.push_back(0);
      }
      mSupport.push_back(x);
   }

   typedef vector<info*>::iterator iterator;
   typedef vector<info*>::const_iterator const_iterator;
   iterator begin() { return mSupport.begin(); }
   iterator end() { return mSupport.end(); }
   const_iterator begin() const { return mSupport.begin(); }
   const_iterator end() const{ return mSupport.end(); }
      

protected:
   vector<info*> mSupport;
};

#endif
