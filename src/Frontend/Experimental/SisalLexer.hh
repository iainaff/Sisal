/**************************************************************************/
/* FILE   **************       SisalLexer.hh       ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include <string>
class SisalParser;

#ifndef SISALLEXER_HH
#define SISALLEXER_HH

namespace sisalc {
   class SisalParser;
   class SisalLexer: public ssFlexLexer {
   public:
      SisalLexer(const char* name="<stdin>", istream* arg_yyin = 0, ostream* arg_yyout = 0 );
      int yylex();
      int yylex(SisalParser* p) { parser=p; return yylex(); }
   
   protected:
      SisalParser* parser;
      string mFilename;
      unsigned int mColumn;
      unsigned int mLine;
      unsigned int mEndLine;
      unsigned int mEndColumn;
      void columnLineAdjust();
   };
}
#endif
