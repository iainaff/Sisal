#ifndef SISAL_PARSER_HH
#define SISAL_PARSER_HH
#include <string>
#include "semanticBase.hh"
using sisalc::semanticBase;

#include "module.hh"
using sisalc::MODULE;

#include "info.hh"
using sisalc::INFO;


class SisalLexer;
class SisalParser {
public:
   SisalParser(SisalLexer* lexer);
   int checkForKeyword(const char*,int);

   vector<MODULE> modules;

   int lex();

   void generalError(const char* location, const char* msg);
   
   void value( semanticBase* tok );

   semanticBase* value() { return yylval; }

   // PARSER RULES
   virtual semanticBase* sisalModule(semanticBase* SISAL_MODULE, semanticBase* MODULE_BODY);
   virtual semanticBase* createSisalModule(semanticBase* MODULE_, semanticBase* ID, semanticBase* SEMI_);
   virtual semanticBase* moduleBody();
   virtual semanticBase* moduleBody(semanticBase* MODULE_BODY, semanticBase* DEFINITION);
   virtual semanticBase* badDefinition(semanticBase* ERROR, const char* msg);
   virtual semanticBase* functionDefine(semanticBase* OPTPUBLIC, semanticBase* FUNCTION_, semanticBase* ID, semanticBase* LPAREN_, semanticBase* ARG_TYPE, semanticBase* RETURNS_, semanticBase* RETURNS_TYPE, semanticBase* RPAREN_, semanticBase* EXPRESSION, semanticBase* END_, semanticBase* FUNCTION_2, semanticBase* OPTID, semanticBase* OPTINTERFACE_LIST);
   virtual semanticBase* returnsType(semanticBase* TYPE);
   virtual semanticBase* returnsType(semanticBase* TYPE, semanticBase* COMMA_, semanticBase* RETURNS_TYPE);
   virtual semanticBase* basicType(semanticBase* BASIC_TYPE);
   virtual semanticBase* commaOperator(semanticBase* EXPRESSION, semanticBase* COMMA_, semanticBase* EXPRESSION2);
   virtual semanticBase* errorExpr(semanticBase* ERROR);
   virtual semanticBase* literalInteger(semanticBase* INTEGERLITERAL);
   virtual semanticBase* literalCharacter(semanticBase* CHARACTERLITERAL);
   virtual semanticBase* literalString(semanticBase* STRINGLITERAL);

   // Blank line above is required!!!
protected:
   SisalLexer* mLexer;
   semanticBase* yylval;
   string mFile;
   int mLine;
   INFO mBooleanType;
   INFO mCharacterType;
   INFO mDoubleRealType;
   INFO mIntegerType;
   INFO mNullType;
   INFO mRealType;
   INFO mStringType;
};

#endif
