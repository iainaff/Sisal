#include "semanticBase.hh"
using sisalc::semanticBase;
#include "Definition.hh"

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
