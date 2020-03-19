/**************************************************************************/
/* FILE   **************       ModuleBody.hh       ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef MODULEBODY_HH
#define MODULEBODY_HH

#include "semanticBase.hh"
#include "Definition.hh"

namespace sisalc {

   class ModuleBody: public semanticBase {
   public:
      ModuleBody() {}
      virtual string self() const { return "Body"; }
      void push_back(Definition* D) { definitions.push_back(D); }
      typedef vector<Definition*>::iterator iterator;
      vector<Definition*>::iterator begin() { return definitions.begin(); }
      vector<Definition*>::iterator end() { return definitions.end(); }
   protected:
      vector<Definition*> definitions;
   };
}
#endif
