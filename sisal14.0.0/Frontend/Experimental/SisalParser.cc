#include "SisalParser.hh"

#define yyFlexLexer ssFlexLexer
#include <FlexLexer.h>

#include "SisalLexer.hh"

#include "token.hh"
using sisalc::token;

#include "PublicFunction.hh"
using sisalc::PublicFunction;

#include "module.hh"
using sisalc::module;
using sisalc::MODULE;

#include "stamp.hh"
using sisalc::stamp;

#include "literal.hh"
using sisalc::literal;
using sisalc::LITERAL;

#include "info.hh"
using sisalc::info;
using sisalc::INFO;

#include "basicInfo.hh"
using sisalc::basicInfo;

#include "arrayInfo.hh"
using sisalc::arrayInfo;

#include "tupleInfo.hh"
using sisalc::tupleInfo;

#include "functionInfo.hh"
using sisalc::functionInfo;

#include "ModuleBody.hh"
#include "Expression.hh"
#include "Type.hh"

SisalParser::SisalParser(SisalLexer* lexer) 
   : yylval(0), mLexer(lexer), mFile("?"), mLine(0) {
}

int SisalParser::lex() {
   mLexer->yylex(this);
}

void SisalParser::generalError(const char* location, const char* msg) {
   if ( mLine > 0 ) {
      cerr << mFile << ": " << mLine << ": " << msg << endl;
   } else {
      cerr << "non-localized error: " << msg << endl;
   }
}
   
void SisalParser::value( semanticBase* tok ) {
   yylval = tok;
   mFile = tok->file();
   mLine = tok->lineno();
}

/*
#     #
##   ##  ######   #####  #    #   ####   #####    ####
# # # #  #          #    #    #  #    #  #    #  #
#  #  #  #####      #    ######  #    #  #    #   ####
#     #  #          #    #    #  #    #  #    #       #
#     #  #          #    #    #  #    #  #    #  #    #
#     #  ######     #    #    #   ####   #####    ####  
*/

/**************************************************************************/
/* GLOBAL **************         moduleBody        ************************/
/**************************************************************************/
/* Add definition to body (parameters, functions, type)                   */
/**************************************************************************/
semanticBase* SisalParser::moduleBody() {
   // Refresh the basic types
   mBooleanType = basicInfo::ctor(basicInfo::BOOL);
   mCharacterType = basicInfo::ctor(basicInfo::CHAR);
   mDoubleRealType = basicInfo::ctor(basicInfo::DOUBLE);
   mIntegerType = basicInfo::ctor(basicInfo::INTEGER);
   mNullType = basicInfo::ctor(basicInfo::NIL);
   mRealType = basicInfo::ctor(basicInfo::REAL);
   mStringType = arrayInfo::ctor(mCharacterType);

   return new ModuleBody();
}

semanticBase* SisalParser::moduleBody(semanticBase* MODULE_BODY, semanticBase* DEFINITION) {
   ModuleBody* module_body = dynamic_cast<ModuleBody*>(MODULE_BODY);    assert(module_body);
   Definition* definition = dynamic_cast<Definition*>(DEFINITION);    assert(!DEFINITION || definition);

   if ( definition ) module_body->push_back(definition);

   module_body->setLocation();

   return module_body;
}

/**************************************************************************/
/* GLOBAL **************       functionDefine      ************************/
/**************************************************************************/
/* Define a public or private function                                    */
/**************************************************************************/
typedef semanticBase InterfaceList;
semanticBase* SisalParser::functionDefine(semanticBase* OPTPUBLIC, semanticBase* FUNCTION_, semanticBase* ID, semanticBase* LPAREN_, semanticBase* ARG_TYPE, semanticBase* RETURNS_, semanticBase* RETURNS_TYPE, semanticBase* RPAREN_, semanticBase* EXPRESSION, semanticBase* END_, semanticBase* FUNCTION_2, semanticBase* OPTID, semanticBase* OPTINTERFACE_LIST) {
   token* optpublic = dynamic_cast<token*>(OPTPUBLIC);    assert(!OPTPUBLIC || optpublic);
   token* function_ = dynamic_cast<token*>(FUNCTION_);    assert(function_);
   token* functionName = dynamic_cast<token*>(ID);    assert(functionName);
   token* lparen_ = dynamic_cast<token*>(LPAREN_);    assert(lparen_);
   Type* arg_type = dynamic_cast<Type*>(ARG_TYPE);    assert(!ARG_TYPE || arg_type);
   token* returns_ = dynamic_cast<token*>(RETURNS_);    assert(returns_);
   Type* returns_type = dynamic_cast<Type*>(RETURNS_TYPE);    assert(returns_type);
   token* rparen_ = dynamic_cast<token*>(RPAREN_);    assert(rparen_);
   Expression* expression = dynamic_cast<Expression*>(EXPRESSION);    assert(expression);
   token* end_ = dynamic_cast<token*>(END_);    assert(end_);
   token* function_2 = dynamic_cast<token*>(FUNCTION_2);    assert(function_2);
   token* tailID = dynamic_cast<token*>(OPTID);    assert(!OPTID || tailID);
   InterfaceList* optinterface_list = dynamic_cast<InterfaceList*>(OPTINTERFACE_LIST);    assert(!OPTINTERFACE_LIST || optinterface_list);

   // -----------------------------------------------
   // If a tail ID exists, it must match
   // -----------------------------------------------
   if ( tailID ) {
      if ( functionName->self() != tailID->self() ) {
         tailID->error("Mismatched function tail name:");
         functionName->error("For function:");
      }
   }

   // -----------------------------------------------
   // Build the function type
   // -----------------------------------------------
   INFO F = functionInfo::ctor(arg_type?arg_type->value():info::null,
                               returns_type->value());
   Type* FType = new Type(F);
   FType->mergeIn(arg_type);
   FType->mergeIn(returns_type);

   // -----------------------------------------------
   // Build either an XGraph or LGraph depending on
   // the ``public'' value.
   // -----------------------------------------------
   GRAPH G;
   if ( optpublic == 0 ) {
      G = PublicFunction::ctor(functionName->self());
   } else {
      G = PublicFunction::ctor(functionName->self());
   }
   G->setType(F);

   // -----------------------------------------------
   // Add the nodes to the graph
   // -----------------------------------------------
   for( Expression::nodeIterator p = expression->nodes().begin();
        p != expression->nodes().end();
        ++p ) {
   }

   // -----------------------------------------------
   // Wire the dangling outputs into the graph
   // -----------------------------------------------
   for( int port = 1; port <= expression->outputs().size(); ++port ) {
      G->attachInput(expression->outputs()[port-1],port);
   }

   // -----------------------------------------------
   // Attach the graph and types to the definition
   // -----------------------------------------------
   Definition *Def = new Definition;
   Def->mGraphs.push_back(G);
   copy(FType->begin(),FType->end(),
        inserter(Def->mTypes,Def->mTypes.end()));
   return Def;
}

/**************************************************************************/
/* GLOBAL **************       badDefinition       ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
semanticBase* SisalParser::badDefinition(semanticBase* ERROR, const char* msg) {
   if ( ERROR ) {
      ERROR->error(msg);
   } else {
      generalError("EOF",msg);
   }
   return new Definition();
}

/**************************************************************************/
/* GLOBAL **************        sisalModule        ************************/
/**************************************************************************/
/* A module has a label statement and a body                              */
/**************************************************************************/
semanticBase* SisalParser::sisalModule(semanticBase* SISAL_MODULE, semanticBase* MODULE_BODY) {
   token* sisal_module = dynamic_cast<token*>(SISAL_MODULE);    assert(sisal_module);
   ModuleBody* module_body = dynamic_cast<ModuleBody*>(MODULE_BODY);    assert(module_body);


   // -----------------------------------------------
   // Build the module body
   // -----------------------------------------------
   MODULE m = module::ctor(sisal_module->self());
   m->addStamp('M',sisal_module->self());
   m->addStamp('C',"Faked IF1CHECK");
   m->addStamp('D',"Nodes are DFOrdered");
   m->addStamp('F',"Open source frontend Version0.1");
   modules.push_back(m);

   // -----------------------------------------------
   // Add in the basic types
   // -----------------------------------------------
   m->addInfo(mBooleanType);
   m->addInfo(mCharacterType);
   m->addInfo(mDoubleRealType);
   m->addInfo(mIntegerType);
   m->addInfo(mNullType);
   m->addInfo(mRealType);
   //m->addInfo(mStringType);

   // -----------------------------------------------
   // Apply definitions
   // -----------------------------------------------
   for( ModuleBody::iterator def = module_body->begin();
        def != module_body->end();
        ++def ) {
      (*def)->registration(m);
   }

   return sisal_module;
}

/**************************************************************************/
/* GLOBAL **************     createSisalModule     ************************/
/**************************************************************************/
/* We need to remember the name                                           */
/**************************************************************************/
semanticBase* SisalParser::createSisalModule(semanticBase* MODULE_, semanticBase* ID, semanticBase* SEMI_) {
   token* module_ = dynamic_cast<token*>(MODULE_);    assert(module_);
   token* id = dynamic_cast<token*>(ID);    assert(id);
   token* semi_ = dynamic_cast<token*>(SEMI_);    assert(semi_);
   return id;
}

/**************************************************************************/
/* GLOBAL **************        returnsType       ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
semanticBase* SisalParser::returnsType(semanticBase* TYPE) {
   Type* type = dynamic_cast<Type*>(TYPE);    assert(type);

   // -----------------------------------------------
   // Convert this to a tuple type
   // -----------------------------------------------
   cerr << "Bulid a vanilla guple from " << type->value().get() << endl;
   INFO tuple = tupleInfo::ctor(type->value());
   Type* tupleType = new Type(tuple);
   tupleType->mergeIn(type);

   return tupleType;
}

semanticBase* SisalParser::returnsType(semanticBase* TYPE, semanticBase* COMMA_, semanticBase* RETURNS_TYPE) {
   Type* type = dynamic_cast<Type*>(TYPE);    assert(type);
   token* comma_ = dynamic_cast<token*>(COMMA_);    assert(comma_);
   Type* returns_type = dynamic_cast<Type*>(RETURNS_TYPE);    assert(returns_type);

   // -----------------------------------------------
   // Extend the tuple with the new value
   // -----------------------------------------------
   cerr << "make an extended tuple" << endl;
   INFO tuple = tupleInfo::ctor(type->value(),returns_type->value());
   Type* tupleType = new Type(tuple);
   tupleType->mergeIn(returns_type);
   tupleType->mergeIn(type);

   return tupleType;
}

/**************************************************************************/
/* GLOBAL **************       literalInteger      ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
semanticBase* SisalParser::literalInteger(semanticBase* INTEGERLITERAL) {
   token* integerliteral = dynamic_cast<token*>(INTEGERLITERAL);    assert(integerliteral);

   // -----------------------------------------------
   // Build a new literal value
   // -----------------------------------------------
   LITERAL L = literal::ctor(mIntegerType, integerliteral->self());

   // -----------------------------------------------
   // Attach to an expression
   // -----------------------------------------------
   Expression* Expr = new Expression();
   Expr->outputs().push_back(L);
   return Expr;
}

/**************************************************************************/
/* GLOBAL **************      literalCharacter     ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
semanticBase* SisalParser::literalCharacter(semanticBase* CHARACTERLITERAL) {
   token* characterliteral = dynamic_cast<token*>(CHARACTERLITERAL);    assert(characterliteral);

   // -----------------------------------------------
   // Build a new literal value
   // -----------------------------------------------
   LITERAL L = literal::ctor(mCharacterType, characterliteral->self());

   // -----------------------------------------------
   // Attach to an expression
   // -----------------------------------------------
   Expression* Expr = new Expression();
   Expr->outputs().push_back(L);
   return Expr;
}

/**************************************************************************/
/* GLOBAL **************       literalString       ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
semanticBase* SisalParser::literalString(semanticBase* STRINGLITERAL) {
   token* stringliteral = dynamic_cast<token*>(STRINGLITERAL);    assert(stringliteral);

   // -----------------------------------------------
   // Build a new literal value
   // -----------------------------------------------
   cerr << "Literal is " << stringliteral->self() << endl;
   LITERAL L = literal::ctor(mStringType, stringliteral->self());

   // -----------------------------------------------
   // Attach to an expression
   // -----------------------------------------------
   Expression* Expr = new Expression();
   Expr->outputs().push_back(L);
   return Expr;
}

/**************************************************************************/
/* GLOBAL **************         errorExpr         ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
semanticBase* SisalParser::errorExpr(semanticBase* ERROR) {
   assert(ERROR);
   ERROR->error("Invalid expression");
   return new Expression();
}

/**************************************************************************/
/* GLOBAL **************         basicType         ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
semanticBase* SisalParser::basicType(semanticBase* BASIC_TYPE) {
   token* basic_type = dynamic_cast<token*>(BASIC_TYPE);    assert(basic_type);

   if ( basic_type->self() == "boolean" ) {
      cerr << "Make a bool" << endl;
      return new Type(mBooleanType);
   } else if ( basic_type->self() == "character" ) {
      cerr << "Make a char" << endl;
      return new Type(mCharacterType);
   } else if ( basic_type->self() == "doublereal" ) {
      return new Type(mDoubleRealType);
   } else if ( basic_type->self() == "integer" ) {
      cerr << "Make a int" << endl;
      return new Type(mIntegerType);
   } else if ( basic_type->self() == "null" ) {
      return new Type(mNullType);
   } else if ( basic_type->self() == "real" ) {
      return new Type(mRealType);
   } else if ( basic_type->self() == "string" ) {
      return new Type(mStringType);
   } else {
      basic_type->error("unknown built-in type:");
      return new Type(mIntegerType);
   }
   return 0;
}

/**************************************************************************/
/* GLOBAL **************       commaOperator       ************************/
/************************************************************************ **/
/*  */
/**************************************************************************/
semanticBase* SisalParser::commaOperator(semanticBase* EXPRESSION, semanticBase* COMMA_, semanticBase* EXPRESSION2) {
   Expression* expression = dynamic_cast<Expression*>(EXPRESSION);    assert(expression);
   token* comma_ = dynamic_cast<token*>(COMMA_);    assert(comma_);
   Expression* expression2 = dynamic_cast<Expression*>(EXPRESSION2);    assert(expression2);

   // -----------------------------------------------
   // Merge expressions together
   // -----------------------------------------------
   cerr << "Merging" << endl;
   expression->mergeIn(expression2);
   return expression;
}
