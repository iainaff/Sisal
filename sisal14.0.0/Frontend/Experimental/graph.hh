#ifndef GRAPH_HH
#define GRAPH_HH

#include "SP.hh"
#include <vector>
using std::vector;

#include "node.hh"
#include "info.hh"

namespace sisalc {

   // -----------------------------------------------
   // forward dependencies
   // -----------------------------------------------
   class node;
   class info;

   class graph : public node {
   public:
      // -----------------------------------------------
      // Constructors with SP<> shadows
      // -----------------------------------------------
      graph();
      static SP<graph> ctor() { return SP<graph>(new graph); }

      graph(unsigned);
      static SP<graph> ctor(unsigned x) { return SP<graph>(new graph(x)); }

      graph(const char*);
      static SP<graph> ctor(const char* x) { return SP<graph>(new graph(x)); }

      // -----------------------------------------------
      // Reference counted registration
      // -----------------------------------------------
   protected:
      SP<graph> mSelf;
   public:
      void self(SP<graph> p);
      SP<graph> self() const;
      static SP<graph> null;

      // -----------------------------------------------
      // Output
      // -----------------------------------------------
      virtual void writeSelf(ostream&) const;
      virtual bool valid() const;

      // -----------------------------------------------
      // IF Labeling
      // -----------------------------------------------
      int offset(const SP<node>) const;
      virtual unsigned int label() const { return 0; }

   protected:
      virtual char letter() const { return 'G'; }
      virtual int i1() const;
      virtual int i2() const { return -1; }

      // -----------------------------------------------
      // Interconnect
      // -----------------------------------------------
   public:
      virtual void addNode(SP<node>);
      void setType(SP<info>);

   protected:
      SP<info> mType;
      vector< SP<node> > mNodes;

      virtual const operation_t* getTable() const;
   };

   typedef SP<graph> GRAPH;
}
#endif

