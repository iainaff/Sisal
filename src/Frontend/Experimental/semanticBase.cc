#include "semanticBase.hh"
#include <fstream>

namespace sisalc {
#define SSMAXERRORS 3
void semanticBase::error(const char* msg,bool showSelf) const {
   static int errors = 0;

   // -----------------------------------------------
   // Display in Emacs parsable format with a ^ to
   // indicate start of error
   // -----------------------------------------------
   cerr << mFileName << ':' << mStartLine << ": " << msg;
   if ( showSelf ) cerr << ' ' << self();
   cerr << endl;
   cerr << mFileName << ":\t" << fetchLine() << endl;
   cerr << mFileName << ":\t";
   for(int i=0; i < mStartColumn-1; ++i) cerr << '.';
   cerr << '^' << endl;

   // -----------------------------------------------
   // Don't let parser get out of control
   // -----------------------------------------------
   if ( ++errors > SSMAXERRORS ) {
      cerr << mFileName << ':' << mStartLine << ": Too many errors" << endl;
      exit(1);
   }
}

void semanticBase::errorEcho(const char* msg) const {
   error(msg,true);
}

string semanticBase::fetchLine() const {
   char buffer[10240];
   ifstream file;
   file.open(mFileName.c_str());
   for(int i = 0; i < mStartLine; ++i) {
      file.getline(buffer,sizeof(buffer));
   }

   return string(buffer);
}
   


}
