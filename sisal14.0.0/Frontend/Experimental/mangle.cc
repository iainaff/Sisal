#include <stdio.h>
#include "mangle.hh"

// -----------------------------------------------
// The mangler function takes a base name and
// privacy info and builds the mangled string
// that uniquely represents the signature
// -----------------------------------------------
namespace sisalc {
   string mangle(string module, string name, INFO i) {
      char buf[1024];
      string result;
      result = name;
      result += "__Q25sisal";
      sprintf(buf,"%d",module.size());
      result += buf;
      result += module;

      if ( i ) { 
         result += i->mangle();
      } else {
         result += "v";
      }

      cerr << module << "::" << name << endl;
      return result;
   }
}
