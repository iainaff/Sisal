#ifndef LITERAL_HH
#define LITERAL_HH

#include <string>

#include "SP.hh"
#include "IFObject.hh"
#include "edge.hh"
#include "node.hh"
#include "info.hh"

namespace sisalc {

   class sisalc::edge;
   class sisalc::node;
   class sisalc::info;

   class literal : public sisalc::edge {
   public:
      // -----------------------------------------------
      // Constructors with SP<> shadows
      // -----------------------------------------------
      literal();
      static SP<literal> ctor() {return SP<literal>(new literal);}

      literal(SP<sisalc::info>, const string&);
      static SP<literal> ctor(SP<sisalc::info> i , const string& s) {return SP<literal>(new literal(i,s));}

      literal(SP<sisalc::info>, const char*);
      static SP<literal> ctor(SP<sisalc::info> i , const char* s) {return SP<literal>(new literal(i,s));}

      // -----------------------------------------------
      // Reference counted registration
      // -----------------------------------------------
   protected:
      SP<literal> mSelf;
   public:
      void self(SP<literal>);
      SP<literal> self() const;
      static SP<literal> null;

      // -----------------------------------------------
      // Output
      // -----------------------------------------------
      // Inherits from edge
      virtual bool valid() const;

      // -----------------------------------------------
      // IF Labeling
      // -----------------------------------------------
   protected:
      virtual char letter() const { return 'L'; }
      virtual int i1() const;
      virtual int i2() const;
      virtual string value() const;

      // -----------------------------------------------
      // Interconnect
      // ----------------------------------------------
   public:
      // Inherits from edge

   protected:
      string mValue;
      SP<sisalc::node> mSource;
      SP<sisalc::node> mDestination;
   };

   typedef SP<literal> LITERAL;
}
#endif
