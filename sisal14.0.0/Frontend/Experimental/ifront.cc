#include "sisalInfo.h"

#include <iostream>
#include <fstream>

#define yyFlexLexer ssFlexLexer
#include <FlexLexer.h>

#include "SisalLexer.hh"
#include "SisalParser.hh"
#include "ifx.hh"

#include "node.hh"
#include "module.hh"
#include "PublicFunction.hh"

#include "SP.hh"

using namespace sisalc;

extern int ssparse(void*);

int main(int argc, char** argv) {
   try {
      ifstream in;
      in.open(argv[1]);
      SisalLexer lexer(argv[1],&in);
      SisalParser parser(&lexer);
   
      cerr << "-- Start parsing" << endl;
      int stat = ssparse(&parser);
      cerr << "-- End parsing " << stat << endl;

      cerr << "Detected " << parser.modules.size() << " modules" << endl;
      for(int i=0; i < parser.modules.size(); ++i) {
         cerr << "module " << i << " is " << endl;
         cout << *(parser.modules[0]);
      }
      return stat;

   } catch( const char* msg ) {
      cerr << "ABORT: " << msg << ' ' << __LINE__ << endl;
   }
}

#if 0
   
   MODULE m = module::ctor("foobar");
   m->addStamp(stamp('F',"Open source frontend"));

   INFO I = info::ctor();
   m->addInfo(I);

   INFO I2 = info::ctor();
   m->addInfo(I2);

   GRAPH G = PublicFunction::ctor("foobar");
   m->addGraph(G);
   G->setType(I);

   NODE N = node::ctor(100);
   G->addNode(N);

   EDGE E = edge::ctor(I);
   N->attachInput(E,1);
   G->attachOutput(E,1);

   EDGE E2 = edge::ctor(I);
   G->attachInput(E2,1);
   N->attachOutput(E2,1);

   cout << *m;

#endif
