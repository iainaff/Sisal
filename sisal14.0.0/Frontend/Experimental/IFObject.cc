#include "assert.h"
#include "ifx.hh"

namespace sisalc {

ostream& operator<<(ostream& os, const IFObject& object) {
   assert(object.valid());
   object.writeSelf(os);
   return os;
}

IFObject::IFObject(string& sourceFile, unsigned int sourceLine)
   : mSourceFile(sourceFile), mSourceLine(sourceLine) {
}

IFObject::IFObject()
   : mSourceLine(0) {
}

void IFObject::writeSelf(ostream& os) const {
   assert(valid());

   os << letter() << '\t';
   int v1 = i1(); if ( v1 >= 0 ) os << v1;
   os << ' ';
   int v2 = i2(); if ( v2 >= 0 ) os << v2;
   os << '\t';
   int v3 = i3(); if ( v3 >= 0 ) os << v3;
   os << ' ';
   int v4 = i4(); if ( v4 >= 0 ) os << v4;
   os << '\t';
   int v5 = i5(); if ( v5 >= 0 ) os << v5;
   os << '\t';
   string val = value(); if ( val.size() > 0 ) os << val;
   os << endl;
}

}
