#ifndef TOKEN_HH
#define TOKEN_HH

#include "semanticBase.hh"
#include <iostream>
#include <string>
namespace sisalc {
   class token : public semanticBase {
   public:
      token(const char* tok, string& fileName, int startLine, int startColumn, int endLine, int endColumn)
         : mToken(tok), semanticBase(fileName,startLine,startColumn,endLine,endColumn) {
      }
      virtual string self() const { return mToken; }
   private:
      string mToken;
   };
}
#endif
