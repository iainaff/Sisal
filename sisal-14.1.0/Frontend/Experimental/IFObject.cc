#include "assert.h"
#include "IFCore.hh"

namespace sisalc {

   ostream& operator<<(ostream& os, const IFObject& object) {
      assert(object.valid());
      object.writeSelf(os);
      return os;
   }

   IFObject::IFObject() {
   }

   IFObject::IFObject(const string& sourceFile, unsigned int sourceLine) {
      pragma("sf",sourceFile);
      pragma("ln",sourceLine);
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
      string val = sValue(); if ( val.size() > 0 ) os << val;

      // Pragmas
      for(map<const char*,string,ltcharP>::const_iterator i = mStringPragmas.begin();
          i != mStringPragmas.end();
          ++i) {
         pair<const char *,string> x = *i;
         os << " %" << x.first << '=' << x.second;
      }

      for(map<const char*,int,ltcharP>::const_iterator i = mIntegerPragmas.begin();
          i != mIntegerPragmas.end();
          ++i) {
         pair<const char *,int> x = *i;
         os << " %" << x.first << '=' << x.second;
      }

      os << endl;
          
   }

   void IFObject::pragma(const char* xx,const string& value) {
      mStringPragmas[xx] = value;
   }
   void IFObject::pragma(const char* xx,const int value) {
      mIntegerPragmas[xx] = value;
   }
   string IFObject::spragma(const char* xx,const string& def) const {
      map<const char*,string,ltcharP>::const_iterator idx = mStringPragmas.find(xx);
      if ( idx == mStringPragmas.end() ) {
         return def;
      } else {
         return (*idx).second;
      }
   }
   int IFObject::ipragma(const char* xx,int def) const {
      map<const char*,int>::const_iterator idx = mIntegerPragmas.find(xx);
      if ( idx == mIntegerPragmas.end() ) {
         return def;
      } else {
         return (*idx).second;
      }
   }
   bool IFObject::bpragma(const char* xx,bool def) const {
      return ipragma(xx,(int)def);
   }

   void IFObject::deletePragma(const char* xx) {
      map<const char*,string,ltcharP>::iterator sIdx = mStringPragmas.find(xx);
      if ( sIdx != mStringPragmas.end() ) mStringPragmas.erase(sIdx);
      map<const char*,int>::iterator idx = mIntegerPragmas.find(xx);
      if ( idx != mIntegerPragmas.end() ) mIntegerPragmas.erase(idx);
   }
   
   void IFObject::displayError(const char* kind, const char* msg) const {
      cerr << kind << ':' << endl;

      // Look for file and line info...
      string file = spragma("sf");
      if ( file != "" ) {
         cerr << file << ':';
         int line = ipragma("sl");
         if ( line ) {
            cerr << line << ':';
         }
      }
      cerr << ' ' << msg << endl;
   }

   void IFObject::information(const char* msg) const {
      displayError("information",msg);
   }

   void IFObject::warning(const char* msg) const {
      displayError("warning",msg);
   }
   void IFObject::error(const char* msg) const {
      displayError("error",msg);
   }
   void IFObject::fatal(const char* msg) const {
      displayError("fatal",msg);
      exit(1);
   }



   
      

}
