#include "literal.hh"

namespace sisalc {

   literal::literal()
      : mValue("")
   {}
   
   literal::literal(const string& s)
      : mValue(s)
   {}
   
   literal::literal(const info* type, const string& value)
      : edge(type), mValue(value)
   {}
   
   literal::literal(const info* type, const char* value)
      : edge(type), mValue(value)
   {}
   
   bool literal::valid() const {
      return true;
   }

   int literal::i1() const {
      return -1;
   }
   
   int literal::i2() const {
      return -1;
   }

   string literal::sValue() const {
      string result;
      result += "\"";
      result += mValue;
      result += "\"";
      return result;
   }

}
