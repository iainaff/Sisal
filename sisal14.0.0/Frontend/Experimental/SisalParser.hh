/**************************************************************************/
/* FILE   **************       SisalParser.hh      ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#ifndef SISALPARSER_HH
#define SISALPARSER_HH

#include <string>
#include "semanticBase.hh"
#include "IFCore.hh"

namespace sisalc {
   class SisalLexer;
   class SisalParser {
   public:
      SisalParser(SisalLexer* lexer);
      int checkForKeyword(const char*,int);

      cluster Modules;

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
      virtual semanticBase* functionDefine(semanticBase* OPTPUBLIC, semanticBase* FUNCTION_, semanticBase* ID, semanticBase* LPAREN_, semanticBase* OPTARG_TYPE, semanticBase* RETURNS_, semanticBase* RETURNS_TYPE, semanticBase* RPAREN_, semanticBase* EXPRESSION, semanticBase* END_, semanticBase* FUNCTION_2, semanticBase* OPTID, semanticBase* OPTINTERFACE_LIST);
      virtual semanticBase* functionError(semanticBase* OPTPUBLIC, semanticBase* FUNCTION_, semanticBase* ID, semanticBase* LPAREN_, semanticBase* ERROR, semanticBase* RPAREN_, semanticBase* EXPRESSION, semanticBase* END_, semanticBase* FUNCTION_2, semanticBase* OPTID, semanticBase* OPTINTERFACE_LIST, const char* msg);
      virtual semanticBase* mergeArguments(semanticBase* LABELED_TYPE, semanticBase* SEMI_, semanticBase* ARG_TYPE);
      virtual semanticBase* labelType(semanticBase* IDLIST, semanticBase* COLON_, semanticBase* TYPE);
      virtual semanticBase* idList(semanticBase* ID);
      virtual semanticBase* idList(semanticBase* IDLIST, semanticBase* COMMA_, semanticBase* ID);
      virtual semanticBase* idListError(semanticBase* ERROR);
      virtual semanticBase* returnsType(semanticBase* TYPE);
      virtual semanticBase* returnsType(semanticBase* TYPE, semanticBase* COMMA_, semanticBase* RETURNS_TYPE);
      virtual semanticBase* basicType(semanticBase* BASIC_TYPE);
      virtual semanticBase* simpleExpression(semanticBase* ID);
      virtual semanticBase* functionCall(semanticBase* ID, semanticBase* LPAREN_, semanticBase* OPTEXPRESSION, semanticBase* RPAREN_);
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
      info* mBooleanType;
      info* mCharacterType;
      info* mDoubleRealType;
      info* mIntegerType;
      info* mNullType;
      info* mRealType;
      info* mStringType;
   };
}

#endif
