/**************************************************************************/
/* FILE   **************       Definition.hh       ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef DEFINITION_HH
#define DEFINITION_HH

#include "semanticBase.hh"
#include "IFCore.hh"
#include "Function.hh"

namespace sisalc {
   class Definition : public semanticBase {
   public:
      Definition() {}
      virtual string self() const { return "definition"; }

      vector<info*> mTypes;
      vector<Function*> mFunctions;
      vector<stamp> mStamps;

      virtual void registration(module* m) {
         for(int i=0; i<mTypes.size(); ++i) {
            m->addInfo(mTypes[i]);
         }
         for(int i=0; i<mFunctions.size(); ++i) {
            m->addFunction(mFunctions[i]);
         }
         for(int i=0; i<mStamps.size(); ++i) {
            m->addStamp(mStamps[i]);
         }
      }
   };
}
#endif
