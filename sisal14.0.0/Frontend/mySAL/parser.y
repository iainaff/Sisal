/*-*-c-*-*/
%{
#include <stdio.h>
#include "../../Include/sisalInfo.h"
#include "ParserControl.h"

#define YYSTYPE void*

  static YYSTYPE lastReduction = 0;
#define ACTION \
   yyval = yyReductionAction(yyMasterObject, \
   	yytname[yyr1[yyn]],yyn,yyvsp-yylen+1,yylen); \
   lastReduction = yyval;

#define ERROR \
   yyval = yyErrorAction(yyMasterObject, "FIXME", yytname[yyr1[yyn]], yyn, yyvsp[yylen-1]); \
   lastReduction = yyval;

  staticforward  int yyerror(char*);

  void* yyMasterObject = 0;
  void* (*yyMakeToken)(void*,const char*,const char*,unsigned long,unsigned long,unsigned long,unsigned long) = 0;
  void* (*yyReductionAction)(void*,const char*, unsigned int, void**, unsigned int) = 0;
  void* (*yyErrorAction)(void*,const char*,const char*,unsigned int, void*) = 0;
  int (*yyErrorHandler)(void*,const char*) = 0;
  int (*yyInputFunction)(void*,char*,int) = 0;
  const char* yyFileName = 0;

%}

%token_table
%token main_
%token id
%token defines
%token integerLiteral
%token doubleLiteral
%token function_
%token returns_
%token integer_
%token double_
%token if_ then_ else_
%token let_ in_
%token end_
%token for_
%token of_
%token array_
%token matrix_
%token slab_
%token value_
%token initial_
%token while_
%token repeat_
%token equals
%token notequals
%token less
%token lessequals
%token greater
%token greaterequals


%left ','                        
%left '+' '-'
%left '*' '/' div_ mod_
%nonassoc equals notequals less lessequals greater greaterequals
%left dot_
%left cross_

%%
program : prologue functionList { ACTION } ;

prologue : { ACTION } ;

functionList : function { ACTION }
	| functionList function { ACTION }
;

/* ----------------------------------------------- */
/* Functions					   */
/* ----------------------------------------------- */
function :
		function_ id '(' openScope optParameters returns_ typeList ')' expression closeScope end_ function_ id { ACTION }
	|	function_ id '(' openScope optParameters returns_ typeList ')' semi closeScope { ACTION }
	|	main_ '(' openScope optParameters returns_ typeList ')' expression closeScope end_ main_ { ACTION }
	| 	error { ERROR }
;

openScope : { ACTION } ;
closeScope : { ACTION } ;


optParameters :
		{ ACTION }
	|	parameters { ACTION }
;

parameters :
		parameter { ACTION }
	|	parameters semi parameter { ACTION }
;

parameter : idList ':' type { ACTION }
;
		

typeList :
		type { ACTION }
	| 	typeList ',' type { ACTION }
;

type :
		integer_ { ACTION }
	|	double_ { ACTION }
;

/* ----------------------------------------------- */
/* Expressions                                     */
/* ----------------------------------------------- */
expression :
	 	singleton { ACTION }
	|	expression ',' singleton { ACTION }
	;

singleton		:
		idExpr { ACTION }
	|	integerConst { ACTION }
	|	doubleConst { ACTION }
	|	ifExpr { ACTION }
	|	letExpr { ACTION }
	|	forExpr { ACTION }
	|	forInitialExpr { ACTION }
	|	infix { ACTION }
	|	paren { ACTION }
	| 	functionCall { ACTION }
	| 	error { ERROR }
;

infix :		singleton '+' singleton { ACTION }
	|	singleton '-' singleton { ACTION }
	|	singleton '*' singleton { ACTION }
	|	singleton '/' singleton { ACTION }
	|	singleton equals singleton { ACTION }
	|	singleton less singleton { ACTION }
	|	singleton lessequals singleton { ACTION }
	|	singleton greater singleton { ACTION }
	|	singleton greaterequals singleton { ACTION }
	|	singleton div_ singleton { ACTION }
	|	singleton mod_ singleton { ACTION }
;

paren : '(' expression ')' { ACTION } ;
functionCall : id '(' optExpression ')' { ACTION } ;
optExpression : { ACTION } | expression { ACTION } ;
idExpr : id { ACTION } ;
integerConst :	integerLiteral { ACTION } ;
doubleConst :	doubleLiteral { ACTION } ;

/* ----------------------------------------------- */
/* If statement					   */
/* ----------------------------------------------- */
ifExpr : if_ boolExpr then_ expression else_ expression end_ if_ { ACTION } ;

boolExpr : singleton { ACTION } ;

/* ----------------------------------------------- */
/* Let statement				   */
/* ----------------------------------------------- */
letExpr : let_ openScope
		nameList
          in_
		expression
	  closeScope end_ let_ { ACTION }
;

optNameList :   { ACTION }
	|	nameList { ACTION }
;

nameList : nameListBody semi  { ACTION } ;

nameListBody :
		nameEntry { ACTION }
	|	nameListBody semi nameEntry { ACTION }
;

nameEntry : idList defines expression  { ACTION } ;

semi : ';' { ACTION }
	| error { ERROR }
;

optSemi : semi { ACTION } | { ACTION } ;

idList :
		id { ACTION }
	|	idList ',' id { ACTION }
;

/* ----------------------------------------------- */
/* Forall statement				   */
/* ----------------------------------------------- */
forExpr : for_ openScope rangeExpr optNameList returns_ arrayReturns end_ for_ { ACTION } ;

rangeExpr : 	rangeClause { ACTION }
	|	rangeExpr dot_ rangeExpr { ACTION }
	| 	rangeExpr cross_ rangeExpr { ACTION }
;

rangeClause : id in_ expression { ACTION } ;

arrayReturns : arrayReturnBody optSemi { ACTION } ;

arrayReturnBody : arrayReturnClause { ACTION }
	| arrayReturnBody semi arrayReturnClause { ACTION }
;

arrayReturnClause : arrayReturn of_ singleton { ACTION } ;

arrayReturn : array_  { ACTION } | matrix_ { ACTION } | slab_ { ACTION }
;

/* ----------------------------------------------- */
/* for initial loop				   */
/* ----------------------------------------------- */
forInitialExpr : for_ initial_ openScope optNameList while_ boolExpr closeScope repeat_ openScope nameList returns_ valueReturns end_ for_ { ACTION } ;

valueReturns : valueReturnBody semi { ACTION } ;

valueReturnBody : valueReturnClause { ACTION }
	| valueReturnBody semi valueReturnClause { ACTION }
;

valueReturnClause : value_ of_ singleton { ACTION } ;

%%
#if 0
#endif

/**************************************************************************/
/* GLOBAL **************         yyIDCheck         ************************/
/************************************************************************ **/
/* This searchs the BISON table for keyword matches			  */
/**************************************************************************/
int yyIDCheck(char* text) {
    int idOffset;
    const char* const * p = 0;
    int len;

    /* ----------------------------------------------- */
    /* Search through the list of tokens for this ID   */
    /* Yes, this is really slow, and I'll accellerate  */
    /* the lookup later.			       */
    /* ----------------------------------------------- */
    for(p = yytname+3; *p; ++p) {
	/* In case all fail */
	if ( strcmp(*p,"id") == 0 ) idOffset = yytoknum[ p-yytname ];

	/* Match only if last char of token is _ (i.e. end_ matches end)  */
	len = strlen(*p);
	if ( (*p)[len-1] == '_' && strncmp(text,*p,len-1) == 0 ) {
	    return yytoknum[ p-yytname ];
	}
    }
    return idOffset;
}

int yyLookup(char* text) {
    const char* const * p = 0;

    /* ----------------------------------------------- */
    /* Search through the list of tokens for this ID   */
    /* Yes, this is really slow, and I'll accellerate  */
    /* the lookup later.			       */
    /* ----------------------------------------------- */
    for(p = yytname+3; *p; ++p) {
	/* Skip $, error, ...  */
	if ( strcmp(text,*p) == 0 ) {
	    return yytoknum[ p-yytname ];
	}
    }

    return 0;
}

static int yyerror(char* msg) {
  if ( yyErrorHandler ) {
    return yyErrorHandler(yyMasterObject,msg);
  } else {
    printf("Error: %s\n",msg);
    return 0;
  }
}


  int yyProgrammedParse(
		       void* myMasterObject,
		       const char* myInitialFile,
		       void* (*myMakeToken)(void*,const char*,const char*,unsigned long,unsigned long,unsigned long,unsigned long),
		       void* (*myReductionAction)(void*,const char*, unsigned int, void**, unsigned int),
		       void* (*myErrorAction)(void*,const char*,const char*,unsigned int, void*),
		       int (*myErrorHandler)(void*,const char*),
		       int (*myInputFunction)(void*,char*,int),
		       void* result
		       ) {
    int status;

    if ( yyMakeToken ) {
      fprintf(stderr,"Invalid reentrance\n");
      exit(1);
    }

    yyMasterObject = myMasterObject;
    yyFileName = myInitialFile;
    yyMakeToken = myMakeToken;
    yyReductionAction = myReductionAction;
    yyErrorAction = myErrorAction;
    yyErrorHandler = myErrorHandler;
    yyInputFunction = myInputFunction;

    fprintf(stderr,"Ready to parse\n");
    status = yyparse();
    fprintf(stderr,"Done with parse, status = %d\n",status);

    *((void**)(result)) = lastReduction;

    yyMasterObject = 0;
    yyFileName = 0;
    yyMakeToken = 0;
    yyReductionAction = 0;
    yyErrorAction = 0;
    yyErrorHandler = 0;
    yyInputFunction = 0;

    return status;
  }
