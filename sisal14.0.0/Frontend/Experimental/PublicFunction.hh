#ifndef PUBLICFUNCTION_HH
#define PUBLICFUNCTION_HH

#include <string>
#include "graph.hh"

namespace sisalc {
   using sisalc::graph;

   class PublicFunction : public graph {
   public:
      // -----------------------------------------------
      // Constructors with SP<> shadows
      // -----------------------------------------------
      PublicFunction(const char* name) : mName(name), graph(1003) {}
      static SP<graph> ctor(const char* name) { return SP<graph>(new PublicFunction(name)); }
      PublicFunction(const string& name) : mName(name), graph(1003) {}
      static SP<graph> ctor(const string& name) { return SP<graph>(new PublicFunction(name)); }

   protected:
      virtual char letter() const { return 'X'; }
      virtual string value() const { string s("\""); s += mName; s+="\""; return s; }
   protected:
      string mName;
   };
}
#endif

