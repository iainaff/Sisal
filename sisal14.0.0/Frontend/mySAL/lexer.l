%{
/**************************************************************************/
/* FILE   **************          lexer.l          ************************/
/************************************************************************ **/
/* Author: Patrick Miller August 19 2001				  */
/* Copyright (C) 2001 Patrick J. Miller					  */
/**************************************************************************/
/*  */
/**************************************************************************/
#include "ParserControl.h"

#define YY_USER_ACTION { advanceText(yytext); }
#define YY_NO_UNPUT
#define YY_INPUT(buf,result,max_size) \
  result = yyInputFunction(yyMasterObject,buf,max_size)

  static long startLine = 1;
  static long startColumn = 1;
  static long endLine = 1;
  static long endColumn = 1;

  static void advanceText(char* text) {
    for(;text && *text;++text) {
      if ( *text == '\n' ) {
	endColumn = 1;
	endLine++;
      } else {
	endColumn++;
      }
    }
  }

#define DONOTRETURN() { \
  startLine = endLine; \
  startColumn = endColumn; \
}

  extern void* yylval;
#define RETURNS(x) { \
  yylval = yyMakeToken(yyMasterObject,yytext,yyFileName,startLine,startColumn,endLine,endColumn-1); \
  DONOTRETURN(); \
  return (x); \
}


%}
 
whitespace [ \t\n]
digit    [0-9]
id       [A-Za-z][A-Za-z0-9_]*
quote	 ['\"]

%option noyywrap
%x comment
%x linePragma
%x character
%x endCharacter
%x escCharacter
%x insideString

%%

{id}	{ RETURNS( yyIDCheck(yytext) ); }

":="	{ RETURNS( yyLookup("defines") ); }

"=="	{ RETURNS( yyLookup("equals") ); }
"!="	{ RETURNS( yyLookup("notequals") ); }
"<"	{ RETURNS( yyLookup("less") ); }
"<="	{ RETURNS( yyLookup("lessequals") ); }
">"	{ RETURNS( yyLookup("greater") ); }
">="	{ RETURNS( yyLookup("greaterequals") ); }

{digit}+ { RETURNS( yyLookup("integerLiteral") ); }

{digit}+"."{digit}* { RETURNS( yyLookup("doubleLiteral") ); }

{whitespace} { DONOTRETURN(); }

"//"[^\n]* { DONOTRETURN(); }


. {
  RETURNS( yytext[0] );
}


%%


static int IDcheck(char* text) {
    if ( strcmp(text,"keyword") == 0 ) {
	return 200;
    } else if ( strcmp(text,"foobar") == 0 ) {
	return 201;
    }
    return 257;
}

