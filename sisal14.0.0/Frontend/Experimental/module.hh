#ifndef MODULE_HH
#define MODULE_HH

#include <string>
#include <vector>
#include "info.hh"
#include "stamp.hh"
#include "node.hh"
#include "graph.hh"

namespace sisalc {

   class info;
   class graph;
   class node;

   class module {
   public:
      // -----------------------------------------------
      // Constructors with SP<> shadows
      // -----------------------------------------------
      module();
      static SP<module> ctor() { return SP<module>(new module); }

      module(const string&);
      static SP<module> ctor(const string& s) { return SP<module>(new module(s)); }

      module(const char*);
      static SP<module> ctor(const char* s) { return SP<module>(new module(s)); }

      // -----------------------------------------------
      // Reference counted registration
      // -----------------------------------------------
   protected:
      SP<module> mSelf;
   public:
      void self(SP<module> p);
      SP<module> self() const;
      static SP<node> null;

      // -----------------------------------------------
      // Output
      // -----------------------------------------------
      void writeSelf(ostream& os) const;
      bool valid() const;

      // -----------------------------------------------
      // IF Labeling
      // -----------------------------------------------
      int offset(const SP<info>) const;
      int offset(const SP<graph>) const;
      int label() const;

      // -----------------------------------------------
      // Interconnect
      // -----------------------------------------------
      void addInfo(SP<info>);
      void addStamp(stamp);
      void addStamp(char x,const char* msg) { addStamp(stamp(x,msg)); }
      void addStamp(char x,const string& msg) { addStamp(stamp(x,msg)); }
      void addGraph(SP<graph>);

   protected:
      string mName;
      vector<SP<info> > mInfo;
      vector<stamp> mStamp;
      vector<SP<graph> > mGraph;
   };

   ostream& operator<<(ostream& os, const module& object);
   typedef SP<module> MODULE;
}
#endif
