#include "literal.hh"

namespace sisalc {

   void literal::self(SP<literal> p) {
      edge::self(p);
      assert(p.get()==this);
      mSelf = p;
   }
   SP<literal> literal::self() const {
      assert(mSelf.get() == this);
      return mSelf;
   }

   literal::literal()
      : edge(info::null), mValue("")
   {}
   
   literal::literal(SP<info> type, const string& value)
      : edge(type), mValue(value)
   {}
   
   literal::literal(SP<info> type, const char* value)
      : edge(type), mValue(value)
   {}
   
   bool literal::valid() const {
      return mSelf.get() == this;
   }

   int literal::i1() const {
      return -1;
   }
   
   int literal::i2() const {
      return -1;
   }

   string literal::value() const {
      string result;
      result += "\"";
      result += mValue;
      result += "\"";
      return result;
   }

}
