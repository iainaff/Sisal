#include "sisalInfo.h"

#include <iostream>
#include <fstream>

#define yyFlexLexer ssFlexLexer
#include <FlexLexer.h>

#include "SisalLexer.hh"
#include "SisalParser.hh"
#include "IFCore.hh"

using namespace sisalc;

extern int ssparse(void*);

int main(int argc, char** argv) {
   try {
      assert(argc > 1);
      ifstream in;
      in.open(argv[1]);
      if ( !in ) {
         cerr << "error: File " << argv[1] << endl;
         perror("error: ");
         exit(1);
      }

      sisalc::SisalLexer lexer(argv[1],&in);
      SisalParser parser(&lexer);
   
      cerr << "-- Start parsing" << endl;
      int stat = ssparse(&parser);
      cerr << "-- End parsing " << stat << endl;
      if ( stat ) exit(stat);

      // -----------------------------------------------
      // Find the occurrance of the main program
      // -----------------------------------------------
      Function* main = 0;
      for( cluster::moduleIterator mPtr = parser.Modules.begin();
           mPtr != parser.Modules.end();
           ++mPtr) {
         main = (*mPtr)->findFunction("main");
         if ( main ) {
            cerr << "Found main in module " << (*mPtr)->name() << endl;
            break;
         }
      }
      if ( !main ) {
         cerr << "Error: no main program found" << endl;
         exit(1);
      }

      // -----------------------------------------------
      // Starting with that function, type bind and
      // mark as used all necessary functions
      // -----------------------------------------------
      main->typeBinding();


      // -----------------------------------------------
      // Dump the active parts of the cluster
      // -----------------------------------------------
      cout << parser.Modules;

      return stat;

   } catch( const char* msg ) {
      cerr << "ABORT: " << msg << ' ' << __LINE__ << endl;
   }
}

