#include "SisalParser.hh"

#define yyFlexLexer ssFlexLexer
#include <FlexLexer.h>

#include "SisalLexer.hh"

#include "token.hh"

#include "IFCore.hh"
#include "literal.hh"
#include "BasicInfo.hh"
#include "ArrayInfo.hh"
#include "TupleInfo.hh"
using sisalc::TupleInfo;
#include "FunctionInfo.hh"
#include "Function.hh"

#include "ModuleBody.hh"
#include "Expression.hh"
#include "Type.hh"
#include "IDList.hh"
#include "FatEdge.hh"

namespace sisalc {
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
      mBooleanType = new BasicInfo(BasicInfo::BOOL);
      mCharacterType = new BasicInfo(BasicInfo::CHAR);
      mDoubleRealType = new BasicInfo(BasicInfo::DOUBLE);
      mIntegerType = new BasicInfo(BasicInfo::INTEGER);
      mNullType = new BasicInfo(BasicInfo::NIL);
      mRealType = new BasicInfo(BasicInfo::REAL);
      mStringType = new ArrayInfo(mCharacterType);

      return new ModuleBody();
   }

   semanticBase* SisalParser::moduleBody(semanticBase* MODULE_BODY, semanticBase* DEFINITION) {
      assert(MODULE_BODY);	ModuleBody* module_body = dynamic_cast<ModuleBody*>(MODULE_BODY);    assert(module_body);
      assert(DEFINITION);	Definition* definition = dynamic_cast<Definition*>(DEFINITION);    assert(!DEFINITION || definition);

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
   semanticBase* SisalParser::functionDefine(semanticBase* OPTPUBLIC, semanticBase* FUNCTION_, semanticBase* ID, semanticBase* LPAREN_, semanticBase* OPTARG_TYPE, semanticBase* RETURNS_, semanticBase* RETURNS_TYPE, semanticBase* RPAREN_, semanticBase* EXPRESSION, semanticBase* END_, semanticBase* FUNCTION_2, semanticBase* OPTID, semanticBase* OPTINTERFACE_LIST) {
      token* optpublic = dynamic_cast<token*>(OPTPUBLIC);    assert(!OPTPUBLIC || optpublic);
      assert(FUNCTION_);	token* function_ = dynamic_cast<token*>(FUNCTION_);    assert(function_);
      assert(ID);		token* functionName = dynamic_cast<token*>(ID);    assert(functionName);
      assert(LPAREN_);	token* lparen_ = dynamic_cast<token*>(LPAREN_);    assert(lparen_);
      Type* optarg_type = dynamic_cast<Type*>(OPTARG_TYPE);  assert(!OPTARG_TYPE || optarg_type);
      assert(RETURNS_);	token* returns_ = dynamic_cast<token*>(RETURNS_);    assert(returns_);
      assert(RETURNS_TYPE); Type* returns_type = dynamic_cast<Type*>(RETURNS_TYPE);    assert(returns_type);
      assert(RPAREN_);	token* rparen_ = dynamic_cast<token*>(RPAREN_);    assert(rparen_);
      assert(EXPRESSION);	Expression* expression = dynamic_cast<Expression*>(EXPRESSION);    assert(expression);
      assert(END_);	token* end_ = dynamic_cast<token*>(END_);    assert(end_);
      assert(FUNCTION_2);	token* function_2 = dynamic_cast<token*>(FUNCTION_2);    assert(function_2);
      token* tailID = dynamic_cast<token*>(OPTID);    assert(!OPTID || tailID);
      InterfaceList* optinterface_list = dynamic_cast<InterfaceList*>(OPTINTERFACE_LIST);    assert(!OPTINTERFACE_LIST || optinterface_list);

      // -----------------------------------------------
      // If a tail ID exists, it must match
      // -----------------------------------------------
      if ( tailID ) {
         if ( functionName->self() != tailID->self() ) {
            tailID->errorEcho("Mismatched function tail name:");
            functionName->errorEcho("For function:");
         }
      }

      // -----------------------------------------------
      // Build the function type
      // -----------------------------------------------
      FunctionInfo* F = new FunctionInfo(optarg_type?optarg_type->value():0,
                             returns_type->value());
      Type* FType = new Type(F);
      FType->mergeIn(optarg_type);
      FType->mergeIn(returns_type);

      // -----------------------------------------------
      // Build either an XGraph or LGraph depending on
      // the ``public'' value.
      // -----------------------------------------------
      Function* G = new Function(functionName->self(),
                                 optpublic?("XGraph"):("LGraph"));
      G->setType(F);
      G->pragma("sf",functionName->file());
      G->pragma("sl",functionName->lineno());

      // -----------------------------------------------
      // Add the nodes to the graph
      // -----------------------------------------------
      for( Expression::nodeIterator p = expression->nodeBegin();
           p != expression->nodeEnd();
           ++p ) {
         G->addNode(*p);
      }

      // -----------------------------------------------
      // Wire the dangling outputs into the graph
      // -----------------------------------------------
      unsigned int port = 1;
      for( Expression::edge_iterator eptr = expression->outputBegin();
           eptr != expression->outputEnd();
           ++eptr, ++port) {
         G->attachInput(*eptr,port);
      }

      // -----------------------------------------------
      // Attach the graph and types to the definition
      // -----------------------------------------------
      Definition *Def = new Definition;
      Def->mFunctions.push_back(G);
      copy(FType->begin(),FType->end(),
           inserter(Def->mTypes,Def->mTypes.end()));

      // -----------------------------------------------
      // We have some unresolved identifiers
      // -----------------------------------------------
      for( Expression::edge_iterator eptr = expression->unresolvedBegin();
           eptr != expression->unresolvedEnd();
           ++eptr) {
         edge* E = *eptr;

         string name = E->spragma("na");
         cerr << "Unresolved name " << name << endl;
         // See if we can get it from arg_type
         if ( optarg_type ) {
            TupleInfo* tuple = dynamic_cast<TupleInfo*>(optarg_type->value());
            assert(tuple);
            vector<const info*> typeList = tuple->list();
            vector<string> names = tuple->names();
            assert(names.size() == typeList.size());
            for(unsigned int port=1; port <= names.size(); ++port) {
               string& argName = names[port-1];
               if ( argName == name ) { 
                  cerr << name << " is found at port " << port << endl;
                  E->setSource(G,port);
                  E->setType(typeList[port-1]);
                  break;
               }
            }
         }
      }

      return Def;
   }

   /**************************************************************************/
   /* GLOBAL **************       functionError       ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   semanticBase* SisalParser::functionError(semanticBase* OPTPUBLIC, semanticBase* FUNCTION_, semanticBase* ID, semanticBase* LPAREN_, semanticBase* ERROR, semanticBase* RPAREN_, semanticBase* EXPRESSION, semanticBase* END_, semanticBase* FUNCTION_2, semanticBase* OPTID, semanticBase* OPTINTERFACE_LIST, const char* msg) {
      token* optpublic = dynamic_cast<token*>(OPTPUBLIC);    assert(!OPTPUBLIC || optpublic);
      assert(FUNCTION_);	token* function_ = dynamic_cast<token*>(FUNCTION_);    assert(function_);
      assert(ID);		token* id = dynamic_cast<token*>(ID);    assert(id);
      assert(LPAREN_);	token* lparen_ = dynamic_cast<token*>(LPAREN_);    assert(lparen_);
      assert(RPAREN_);	token* rparen_ = dynamic_cast<token*>(RPAREN_);    assert(rparen_);
      assert(EXPRESSION);	Expression* expression = dynamic_cast<Expression*>(EXPRESSION);    assert(expression);
      assert(END_);	token* end_ = dynamic_cast<token*>(END_);    assert(end_);
      assert(FUNCTION_2);	token* function_2 = dynamic_cast<token*>(FUNCTION_2);    assert(function_2);
      token* optid = dynamic_cast<token*>(OPTID);    assert(!OPTID || optid);
      InterfaceList* optinterface_list = dynamic_cast<InterfaceList*>(OPTINTERFACE_LIST);    assert(!OPTINTERFACE_LIST || optinterface_list);

      lparen_->error(msg);

      return new Definition();
   }

   /**************************************************************************/
   /* GLOBAL **************       badDefinition       ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   semanticBase* SisalParser::badDefinition(semanticBase* ERROR, const char* msg) {
      if ( ERROR ) {
         ERROR->errorEcho(msg);
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
      assert(SISAL_MODULE);	token* sisal_module = dynamic_cast<token*>(SISAL_MODULE);    assert(sisal_module);
      assert(MODULE_BODY);	ModuleBody* module_body = dynamic_cast<ModuleBody*>(MODULE_BODY);    assert(module_body);


      // -----------------------------------------------
      // Build the module body
      // -----------------------------------------------
      module* m = new module(sisal_module->self());
      m->addStamp('M',sisal_module->self());
      m->addStamp('C',"Faked IF1CHECK");
      m->addStamp('D',"Nodes are DFOrdered");
      m->addStamp('F',"Open source frontend Version0.1");
      Modules.addModule(m);

      // -----------------------------------------------
      // Add in the basic types
      // -----------------------------------------------
      m->addInfo(mBooleanType);
      m->addInfo(mCharacterType);
      m->addInfo(mDoubleRealType);
      m->addInfo(mIntegerType);
      m->addInfo(mNullType);
      m->addInfo(mRealType);
      m->addInfo(mStringType);

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
      assert(MODULE_);	token* module_ = dynamic_cast<token*>(MODULE_);    assert(module_);
      assert(ID);		token* id = dynamic_cast<token*>(ID);    assert(id);
      assert(SEMI_);	token* semi_ = dynamic_cast<token*>(SEMI_);    assert(semi_);
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
      info* tuple = new TupleInfo(type->value());
      Type* tupleType = new Type(tuple);
      tupleType->mergeIn(type);

      return tupleType;
   }

   semanticBase* SisalParser::returnsType(semanticBase* TYPE, semanticBase* COMMA_, semanticBase* RETURNS_TYPE) {
      assert(TYPE);	Type* type = dynamic_cast<Type*>(TYPE);    assert(type);
      assert(COMMA_);	token* comma_ = dynamic_cast<token*>(COMMA_);    assert(comma_);
      assert(RETURNS_TYPE);	Type* returns_type = dynamic_cast<Type*>(RETURNS_TYPE);    assert(returns_type);

      // -----------------------------------------------
      // Extend the tuple with the new value
      // -----------------------------------------------
      info* tuple = new TupleInfo(type->value(),returns_type->value());
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
      assert(INTEGERLITERAL);	token* integerliteral = dynamic_cast<token*>(INTEGERLITERAL);    assert(integerliteral);

      // -----------------------------------------------
      // Build a new literal value
      // -----------------------------------------------
      literal* L = new literal(mIntegerType, integerliteral->self());

      // -----------------------------------------------
      // Attach to an expression
      // -----------------------------------------------
      Expression* Expr = new Expression();
      Expr->addOutput(L);
      return Expr;
   }

   /**************************************************************************/
   /* GLOBAL **************      literalCharacter     ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   semanticBase* SisalParser::literalCharacter(semanticBase* CHARACTERLITERAL) {
      assert(CHARACTERLITERAL);	token* characterliteral = dynamic_cast<token*>(CHARACTERLITERAL);    assert(characterliteral);

      // -----------------------------------------------
      // Build a new literal value
      // -----------------------------------------------
      literal* L = new literal(mCharacterType, characterliteral->self());

      // -----------------------------------------------
      // Attach to an expression
      // -----------------------------------------------
      Expression* Expr = new Expression();
      Expr->addOutput(L);
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
      literal* L = new literal(mStringType, stringliteral->self());

      // -----------------------------------------------
      // Attach to an expression
      // -----------------------------------------------
      Expression* Expr = new Expression();
      Expr->addOutput(L);
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
      assert(BASIC_TYPE);	token* basic_type = dynamic_cast<token*>(BASIC_TYPE);    assert(basic_type);

      if ( basic_type->self() == "boolean" ) {
         return new Type(mBooleanType);
      } else if ( basic_type->self() == "character" ) {
         return new Type(mCharacterType);
      } else if ( basic_type->self() == "doublereal" ) {
         return new Type(mDoubleRealType);
      } else if ( basic_type->self() == "integer" ) {
         return new Type(mIntegerType);
      } else if ( basic_type->self() == "null" ) {
         return new Type(mNullType);
      } else if ( basic_type->self() == "real" ) {
         return new Type(mRealType);
      } else if ( basic_type->self() == "string" ) {
         return new Type(mStringType);
      } else {
         basic_type->errorEcho("unknown built-in type:");
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
      assert(EXPRESSION);	Expression* expression = dynamic_cast<Expression*>(EXPRESSION);    assert(expression);
      assert(COMMA_);	token* comma_ = dynamic_cast<token*>(COMMA_);    assert(comma_);
      assert(EXPRESSION2);	Expression* expression2 = dynamic_cast<Expression*>(EXPRESSION2);    assert(expression2);

      // -----------------------------------------------
      // Merge expressions together
      // -----------------------------------------------
      expression->mergeIn(expression2);
      return expression;
   }

   /**************************************************************************/
   /* GLOBAL **************       mergeArguments      ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   semanticBase* SisalParser::mergeArguments(semanticBase* LABELED_TYPE, semanticBase* SEMI_, semanticBase* ARG_TYPE) {
      assert(LABELED_TYPE); Type* labeled_type = dynamic_cast<Type*>(LABELED_TYPE);  assert(labeled_type);
      assert(SEMI_);	 token* semi_ = dynamic_cast<token*>(SEMI_);  assert(semi_);
      assert(ARG_TYPE);	 Type* arg_type = dynamic_cast<Type*>(ARG_TYPE);  assert(arg_type);
      return 0;
   }

   /**************************************************************************/
   /* GLOBAL **************         labelType         ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   semanticBase* SisalParser::labelType(semanticBase* IDLIST, semanticBase* COLON_, semanticBase* TYPE) {
      assert(IDLIST);	 IDList* idlist = dynamic_cast<IDList*>(IDLIST);  assert(idlist);
      assert(COLON_);	 token* colon_ = dynamic_cast<token*>(COLON_);  assert(colon_);
      assert(TYPE);	 Type* type = dynamic_cast<Type*>(TYPE);  assert(type);

      Type* tupleType = new Type;
      tupleType->mergeIn(type);

      info* chain = 0;
      for(IDList::iterator id = idlist->end()-1;
          id != idlist->begin()-1;
          --id) {
         info* tuple = new TupleInfo(type->value(),chain);
         tuple->pragma("na",(*id)->self());
         tupleType->support(tuple);
         chain = tuple;
      }
      tupleType->value(chain);
      return tupleType;
   }

   /**************************************************************************/
   /* GLOBAL **************           idList          ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   semanticBase* SisalParser::idList(semanticBase* ID) {
      assert(ID);	 token* id = dynamic_cast<token*>(ID);  assert(id);
      return new IDList(id);
   }

   /**************************************************************************/
   /* GLOBAL **************           idList          ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   semanticBase* SisalParser::idList(semanticBase* IDLIST, semanticBase* COMMA_, semanticBase* ID) {
      assert(IDLIST);	 IDList* idlist = dynamic_cast<IDList*>(IDLIST);  assert(idlist);
      assert(COMMA_);	 token* comma_ = dynamic_cast<token*>(COMMA_);  assert(comma_);
      assert(ID);		 token* id = dynamic_cast<token*>(ID);  assert(id);

      idlist->append(id);
      return idlist;
   }

   /**************************************************************************/
   /* GLOBAL **************        idListError        ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
   semanticBase* SisalParser::idListError(semanticBase* ERROR) {
      return new IDList;
   }

   /**************************************************************************/
   /* GLOBAL **************      simpleExpression     ************************/
   /**************************************************************************/
   /* Build an expression from an identifier                                 */
   /**************************************************************************/
   semanticBase* SisalParser::simpleExpression(semanticBase* ID) {
      assert(ID);	 token* id = dynamic_cast<token*>(ID);  assert(id);

      // -----------------------------------------------
      // We build an edge with dangling inputs and
      // outputs (but with this name).  Type will
      // be inserted later
      // -----------------------------------------------
      edge* E = new edge();
      E->pragma("na",id->self());
      E->pragma("sf",ID->file());
      E->pragma("sl",ID->lineno());

      // -----------------------------------------------
      // Get an expression node and put in a deferred
      // lookup
      // -----------------------------------------------
      Expression* Expr = new Expression();
      Expr->addUnresolved(E);
      Expr->addOutput(E);

      return Expr;
   }

   /**************************************************************************/
   /* GLOBAL **************        functionCall       ************************/
   /************************************************************************ **/
   /*  */
   /**************************************************************************/
    semanticBase* SisalParser::functionCall(semanticBase* ID, semanticBase* LPAREN_, semanticBase* OPTEXPRESSION, semanticBase* RPAREN_) {
        assert(ID);	 token* id = dynamic_cast<token*>(ID);  assert(id);
        assert(LPAREN_);	 token* lparen_ = dynamic_cast<token*>(LPAREN_);  assert(lparen_);
        Expression* optexpression = dynamic_cast<Expression*>(OPTEXPRESSION);  assert(!OPTEXPRESSION || optexpression);
        assert(RPAREN_);	 token* rparen_ = dynamic_cast<token*>(RPAREN_);  assert(rparen_);

      // -----------------------------------------------
      // Function of no arguments
      // -----------------------------------------------
      node* call = new node("Call",ID->file(), ID->lineno());
      literal* L = new literal(id->self());
      call->attachInput(L,1);

      Expression* Expr = new Expression();
      Expr->addNode(call);

      // -----------------------------------------------
      // Wire in outputs of expression into the node
      // -----------------------------------------------
      if ( optexpression ) {
         unsigned int port = 2;
         for( Expression::edge_iterator eptr = optexpression->outputBegin();
              eptr != optexpression->outputEnd();
              ++eptr,++port) {
            call->attachInput(*eptr,port);
         }
      }

      // -----------------------------------------------
      // Result is a "Fat" edge of unknown arity
      // -----------------------------------------------
      edge* E = new FatEdge();
      call->attachOutput(E,1);
      Expr->addOutput(E);

      return Expr;
   }
}
