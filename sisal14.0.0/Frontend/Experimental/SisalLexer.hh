#ifndef SISAL_LEXER_HH
#define SISAL_LEXER_HH
#include <string>

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

#endif
