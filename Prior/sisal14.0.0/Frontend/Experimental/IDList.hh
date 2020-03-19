/**************************************************************************/
/* FILE   **************         IDList.hh         ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef IDLIST_HH
#define IDLIST_HH

#include <string>
#include <algorithm>

#include "semanticBase.hh"
#include "token.hh"

namespace sisalc {
   class IDList : public semanticBase {
   public:
      IDList() {}
      IDList(token* id) { mIDS.push_back(id); }

      virtual string self() const { return "IDList"; }

      void mergeIn(const IDList* T) {
         if ( T ) {
            copy( T->begin(), T->end(), 
                  inserter(mIDS,mIDS.end()) );
         }
      }

      void append(token* id) { mIDS.push_back(id); }

      typedef vector<token*>::iterator iterator;
      typedef vector<token*>::const_iterator const_iterator;
      iterator begin() { return mIDS.begin(); }
      const_iterator begin() const { return mIDS.begin(); }
      iterator end() { return mIDS.end(); }
      const_iterator end() const { return mIDS.end(); }
   protected:
      vector<token*> mIDS;
   };
}
#endif
