#ifndef EXPRESSION_HH
#define EXPRESSION_HH

#include "semanticBase.hh"
using sisalc::semanticBase;

#include "edge.hh"
using sisalc::EDGE;
#include "node.hh"
using sisalc::NODE;

class Expression: public semanticBase {
public:
   Expression() {}
   typedef vector<EDGE>::iterator outputIterator;
   typedef vector<NODE>::iterator nodeIterator;

   void mergeIn(Expression* E) {
      if ( E ) {
         copy(E->outputs().begin(),E->outputs().end(),
              inserter(mOutputs,mOutputs.end()));
         copy(E->unresolved().begin(),E->unresolved().end(),
              inserter(mUnresolved,mUnresolved.end()));
         copy(E->nodes().begin(),E->nodes().end(),
              inserter(mNodes,mNodes.end()));
      }
   }

   vector<EDGE>& outputs() { return mOutputs; }
   vector<EDGE>& unresolved() { return mUnresolved; }
   vector<NODE>& nodes() { return mNodes; }

protected:
   vector<EDGE> mOutputs;
   vector<EDGE> mUnresolved;
   vector<NODE> mNodes;
};

#endif
