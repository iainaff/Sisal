#ifndef LITERAL_HH
#define LITERAL_HH

#include <string>

#include "IFCore.hh"

namespace sisalc {

   class literal : public edge {
   public:
      // -----------------------------------------------
      // Constructors
      // -----------------------------------------------
      literal();
      literal(const string&);
      literal(const info*, const string&);
      literal(const info*, const char*);

      // -----------------------------------------------
      // Output
      // -----------------------------------------------
      // Inherits from edge
      virtual bool valid() const;

      // -----------------------------------------------
      // Accessors
      // -----------------------------------------------
      string value() const { return mValue; }

   protected:
      virtual char letter() const { return 'L'; }
      virtual int i1() const;
      virtual int i2() const;
      virtual string sValue() const;

      // -----------------------------------------------
      // Interconnect
      // ----------------------------------------------
   public:
      // Inherits from edge

   protected:
      string mValue;
   };
}
#endif
