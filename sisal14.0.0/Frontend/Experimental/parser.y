%{

#include "semanticBase.hh"
#include "SisalParser.hh"

using namespace sisalc;

// Leave this defined to get yytname
#define YYDEBUG 1

#define YYPARSE_PARAM myParserAsVoid
#define YYLEX_PARAM myParserAsVoid
#define mParser reinterpret_cast<SisalParser*>(myParserAsVoid)
#define YYSTYPE semanticBase*

static void yyerror(const char*) { return; }
static int yylex(semanticBase** yylval_ptr,void* myParser_ptr) {
   SisalParser* parser = reinterpret_cast<SisalParser*>(myParser_ptr);
   int x = parser->lex();
   assert(!x || parser->value());
   *yylval_ptr = parser->value();
   return x;
}

%}

%pure_parser

%token module_
%token type_
%token function_
%token forward_
%token parameter_
%token array_
%token use_
%token public_
%token returns_
%token end_
%token in_
%token for_
%token boolean_
%token character_
%token double_
%token integer_
%token null_
%token real_
%token string_

%token integerLiteral
%token characterLiteral
%token stringLiteral
%token id

%left ','                        
%left '+' '-'
%left '*' '/' div_ mod_

%%
PROGRAM : MODULE
	| PROGRAM MODULE
	;

MODULE : SISAL_MODULE MODULE_BODY { $$ = mParser->sisalModule($1,$2); }
	;

SISAL_MODULE : module_ id ';' { $$ = mParser->createSisalModule($1,$2,$3); }
	;

/*-----------------------------------------------*/
/* Modules                                       */
/*-----------------------------------------------*/
MODULE_BODY : { $$ = mParser->moduleBody(); }
        |       MODULE_BODY DEFINITION { $$ = mParser->moduleBody($1,$2); }
	;

DEFINITION :	PARAMETER_DEFINE ';'
        |       TYPE_DEFINE ';'
        |       USE_DEFINE ';'
        |       FUNCTION_FORWARD ';'
        |       FUNCTION_DEFINE ';'
        |       error ';' { $$ = mParser->badDefinition($2,"mangled definition"); yyclearin; }
        |       error { $$ = mParser->badDefinition($1,"missing semicolon near:");  }
	;

PARAMETER_DEFINE : optPUBLIC parameter_ id '=' EXPRESSION 
	;

TYPE_DEFINE : optPUBLIC type_ id '=' array_ '[' id ']' 
	;

USE_DEFINE : use_ id
	;

/*-----------------------------------------------*/
/* Function                                      */
/*-----------------------------------------------*/
FUNCTION_FORWARD : forward_ function_ id 
	;

FUNCTION_DEFINE :
	optPUBLIC function_ id '(' ARG_TYPE returns_ RETURNS_TYPE ')' EXPRESSION end_ function_ optID optINTERFACE_LIST
{
   $$ = mParser->functionDefine($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13);
}
                ;

ARG_TYPE : { $$ = 0; }
		;

RETURNS_TYPE : TYPE { $$ = mParser->returnsType($1); ; }
        |       TYPE ',' RETURNS_TYPE { $$ = mParser->returnsType($1,$2,$3); }
                ;

TYPE : id { $$ = $1; }
        |       BASIC_TYPE  { $$ = mParser->basicType($1); }
		;

BASIC_TYPE :    boolean_ 
        |       character_ 
        |       double_ 
        |       integer_
        |       null_
        |       real_
        |       string_
                ;

INTERFACE_LIST: INTERFACE_BODY
                ;

INTERFACE_BODY: INTERFACE
	|       INTERFACE_BODY INTERFACE
                ;

INTERFACE :	'=' stringLiteral in_ stringLiteral
        |       '=' stringLiteral for_ stringLiteral
                ;


/*-----------------------------------------------*/
/* Expressions                                   */
/*-----------------------------------------------*/
EXPRESSION : 	LITERAL { $$ = $1; }
        |       EXPRESSION ',' EXPRESSION { $$ = mParser->commaOperator($1,$2,$3); }
        |       EXPRESSION '+' EXPRESSION
        |       EXPRESSION '-' EXPRESSION
        |       EXPRESSION '*' EXPRESSION
        |       EXPRESSION '/' EXPRESSION
        |       EXPRESSION div_ EXPRESSION
        |       EXPRESSION mod_ EXPRESSION
	|       error { $$ = mParser->errorExpr($1); yyclearin; }
                ;

LITERAL :	integerLiteral { $$ = mParser->literalInteger($1); }
        |       characterLiteral { $$ = mParser->literalCharacter($1); }
        |       stringLiteral { $$ = mParser->literalString($1); }
                ;

/*-----------------------------------------------*/
/* Optional tokens                               */
/*-----------------------------------------------*/
optPUBLIC: 	{ $$ = 0; }
        |       public_ 
                ;

optID:  	{ $$ = 0; }
	|       id
                ;

optINTERFACE_LIST : { $$ = 0; }
	|       INTERFACE_LIST
                ;

%%

namespace sisalc {

/**************************************************************************/
/* GLOBAL **************      checkForKeyword      ************************/
/**************************************************************************/
/*  We have to implement here where yytname is defined                    */
/**************************************************************************/
    int SisalParser::checkForKeyword(const char* x,int n) {
        for(int i=0; yytname[i] && yytname[i][0]; ++i) {
            if ( strncmp(x,yytname[i],n) == 0 && yytname[i][n] == '_' ) {
                return 255+i;
            }
        }
        return id;
    }


}
