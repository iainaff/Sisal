#ifndef DEFINITION_HH
#define DEFINITION_HH

#include "semanticBase.hh"
using sisalc::semanticBase;

#include "module.hh"
using sisalc::MODULE;

#include "info.hh"
using sisalc::INFO;

#include "graph.hh"
using sisalc::GRAPH;

#include "stamp.hh"
using sisalc::stamp;

class Definition : public semanticBase {
public:
   Definition() {}
   virtual string self() const { return "definition"; }

   vector<INFO> mTypes;
   vector<GRAPH> mGraphs;
   vector<stamp> mStamps;

   virtual void registration(MODULE m) {
      for(int i=0; i<mTypes.size(); ++i) {
         m->addInfo(mTypes[i]);
      }
      for(int i=0; i<mGraphs.size(); ++i) {
         m->addGraph(mGraphs[i]);
      }
      for(int i=0; i<mStamps.size(); ++i) {
         m->addStamp(mStamps[i]);
      }
   }
};

#endif
