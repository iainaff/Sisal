#ifndef EDGE_HH
#define EDGE_HH

#include "SP.hh"
#include "IFObject.hh"
#include "node.hh"
#include "info.hh"

namespace sisalc {

   // -----------------------------------------------
   // forward dependencies
   // -----------------------------------------------
   class node;
   class info;

   class edge : public IFObject {
   public:
      // -----------------------------------------------
      // Constructors with SP<> shadows
      // -----------------------------------------------
      edge();
      static SP<edge> ctor() { return SP<edge>(new edge); }

      edge(INFO);
      static SP<edge> ctor(INFO x) { return SP<edge>(new edge(x)); }

      // -----------------------------------------------
      // Reference counted registration
      // -----------------------------------------------
   protected:
      SP<edge> mSelf;
   public:
      void self(SP<edge>);
      SP<edge> self() const;
      static SP<edge> null;

      // -----------------------------------------------
      // Output
      // -----------------------------------------------
      //virtual void writeSelf(ostream&) const;
      virtual bool valid() const;

      // -----------------------------------------------
      // IF Labeling
      // -----------------------------------------------
   protected:
      virtual char letter() const { return 'E'; }
      virtual int i1() const;
      virtual int i2() const;
      virtual int i3() const;
      virtual int i4() const;
      virtual int i5() const;

      // -----------------------------------------------
      // Interconnect
      // -----------------------------------------------
   public:
      void setDestination(SP<node>,int);
      void setSource(SP<node>,int);
      void setType(SP<info>);

   protected:
      SP<node> mSource;
      int mSourcePort;
      SP<node> mDestination;
      int mDestinationPort;
      SP<info> mType;

   };

   typedef SP<edge> EDGE;
}
#endif
