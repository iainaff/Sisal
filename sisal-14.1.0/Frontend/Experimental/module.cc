/**************************************************************************/
/* FILE   **************         module.cc         ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 18 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include <algorithm>
#include "IFCore.hh"
#include "FunctionInfo.hh"

namespace sisalc {
   /**************************************************************************/
   /* GLOBAL **************           module          ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   module::module() : mCluster(0) {
   }
   module::module(const string& name) : mCluster(0), mName(name) {
   }
   module::module(const char* name) : mCluster(0), mName(name) {
   }


   /**************************************************************************/
   /* GLOBAL **************         writeSelf         ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   void module::writeSelf(ostream& os) const {
      // -----------------------------------------------
      // Write out types
      // -----------------------------------------------
      for(vector<info*>::const_iterator iPtr = mInfos.begin();
          iPtr != mInfos.end();
          ++iPtr) {
         os << **iPtr;
      }
      // -----------------------------------------------
      // Write out stamps
      // -----------------------------------------------
      for(vector<stamp>::const_iterator sPtr = mStamps.begin();
          sPtr != mStamps.end();
          ++sPtr) {
         os << *sPtr;
      }

      // -----------------------------------------------
      // Write out functions
      // -----------------------------------------------
      for(vector<Function*>::const_iterator fPtr = mFunctions.begin();
          fPtr != mFunctions.end();
          ++fPtr) {
         os << **fPtr;
      }
   }

   /**************************************************************************/
   /* GLOBAL **************           valid           ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   bool module::valid() const {
      return true;
   }

   /**************************************************************************/
   /* GLOBAL **************           offset          ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   int module::offset(const info* I) const {
      vector<info*>::const_iterator position = 
         find(mInfos.begin(), mInfos.end(), I);
      assert( position != mInfos.end() );

      return position - mInfos.begin() + 1; // 1 based index
   }

   /**************************************************************************/
   /* GLOBAL **************          addInfo          ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   void module::addInfo(info* I) {
      assert(I);

      // -----------------------------------------------
      // If we don't have this type already, link it in
      // -----------------------------------------------
      if ( find(mInfos.begin(), mInfos.end(), I) == mInfos.end() ) {
         mInfos.push_back(I);
         I->setParent(this);
      }
   }
   
   /**************************************************************************/
   /* GLOBAL **************        addFunction        ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   void module::addFunction(Function *F) {
      if ( F ) {
         mFunctions.push_back(F);
         F->setModule(this);
      }
   }

   /**************************************************************************/
   /* GLOBAL **************          addStamp         ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   void module::addStamp(const stamp& S) {
      mStamps.push_back(S);
   }
   void module::addStamp(char C, const string& s) {
      stamp aStamp(C,s);
      addStamp(aStamp);
   }
   void module::addStamp(char C, const char* s) {
      stamp aStamp(C,s);
      addStamp(aStamp);
   }

   Function* module::findFunction(const char* name) {
      string S(name);
      return findFunction(S);
   }

   Function* module::findFunction(const string& name) {
      for(vector<Function*>::iterator fPtr = mFunctions.begin();
          fPtr != mFunctions.end();
          ++fPtr) {
         if ( name == (*fPtr)->name() ) return (*fPtr);
      }
      return 0;
   }

   Function* module::findFunction(const string& name, vector<const info*> signature) {
      for(vector<Function*>::iterator fPtr = mFunctions.begin();
          fPtr != mFunctions.end();
          ++fPtr) {
         cerr << "Check function " << (*fPtr)->name() << endl;
         if ( name == (*fPtr)->name() ) {
            cerr << "I have found a function " << name << endl;
            const info* T = (*fPtr)->type();
            assert(T);
            const FunctionInfo* fType = dynamic_cast<const FunctionInfo*>(T);
            assert(fType);
            // -----------------------------------------------
            // Figure out the signature of this function
            // -----------------------------------------------
            const info* thisSignature = fType->car();

            // -----------------------------------------------
            // Compare the types
            // -----------------------------------------------
            cerr << "Signature pointer is " << thisSignature
                 << " to match a call sig of length " << signature.size() << endl;
            int i;
            for(i=0; thisSignature && i < signature.size(); ++i) {
               cerr << "Type element " << i << endl;
               cerr << "  " << signature[i] << ' ' << thisSignature->car() << endl;
               if ( signature[i] != thisSignature->car() ) {
                  break;
               }
               thisSignature = thisSignature->cdr();
            }
            if ( thisSignature == 0 && i == signature.size() ) return (*fPtr);
         }
      }
      return 0;
   }

   /**************************************************************************/
   /* GLOBAL **************          integer          ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   info* module::integer() {
       return mInfos[3];
   }
}
