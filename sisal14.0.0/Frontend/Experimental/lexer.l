%{

#include <stdio.h>
   
#define YY_USER_INIT 
#define YY_USER_ACTION columnLineAdjust();

#define SSTABSTOP 8

#include "semanticBase.hh"
#include "token.hh"

   using sisalc::token;

   
#include "SisalParser.hh"
#include "SisalLexer.hh"
#include "parser.h"

%}

%option c++
%option prefix="ss"
%option yyclass="sisalc::SisalLexer"
%option yymore

whitespace [ \t\n]
digit    [0-9]
id       [A-Za-z][A-Za-z0-9_]*
quote	 ['"]

%x comment
%x linePragma
%x character
%x endCharacter
%x escCharacter
%x insideString

%%

{whitespace}

{digit}+ {
    parser->value( new token(yytext,mFilename,mLine,mColumn,mEndLine,mEndColumn) );
    return integerLiteral;
}

{digit}+"."{digit}* {
    parser->value( new token(yytext,mFilename,mLine,mColumn,mEndLine,mEndColumn) );
    return integerLiteral;
}

"'\\"[0-7][0-7][0-7]"'" {
   parser->value( new token(yytext,mFilename,mLine,mColumn,mEndLine,mEndColumn) );
   return characterLiteral;
}

{id} {
    parser->value( new token(yytext,mFilename,mLine,mColumn,mEndLine,mEndColumn) );
    return parser->checkForKeyword(yytext,yyleng);
}

"//"[^\n]* { /* Comment */ }

^#[ \t]*{digit}+[ \t] {
    int line;
    sscanf(yytext,"# %d",&line);
    mLine = line-1; // There is a newline following this token
    BEGIN(linePragma);
}
^#line[ \t]*{digit}+[ \t] {
    int line;
    sscanf(yytext,"#line %d",&line);
    mLine = line-1; // There is a newline following this token
    BEGIN(linePragma);
}
<linePragma>.* {
    yytext[yyleng-1] = 0;
    mFilename = yytext+1;
    BEGIN(INITIAL);
}

"'"  {
   mEndColumn = mColumn; yymore();
   BEGIN(character);
}
<character>\\ {
   mEndColumn = mColumn; yymore();
   BEGIN(escCharacter);
}
<character>' {
   parser->value( new token("'\\0'",mFilename,mLine,mColumn,mEndLine,mEndColumn) );
   BEGIN(INITIAL);
   return characterLiteral;
}
<character>. {
   mEndColumn = mColumn; yymore();
   BEGIN(endCharacter);
}
<escCharacter>[bcfnrt'\\] {
   mEndColumn = mColumn; yymore();
   BEGIN(endCharacter);
}
<escCharacter>0[0-7]?[0-7]?[0-7]? {
   mEndColumn = mColumn; yymore();
   BEGIN(endCharacter);
}
<escCharacter>.|\n {
   mEndColumn = mColumn; yymore();
   cerr << mFilename << ": " << mLine << ": Invalid escape in character constant " << yytext << endl;
   BEGIN(endCharacter);
}
<endCharacter>' {
   parser->value( new token(yytext,mFilename,mLine,mColumn,mEndLine,mEndColumn) );
   BEGIN(INITIAL);
   return characterLiteral;
}
<endCharacter>. {
   mEndColumn = mColumn; yymore();
   cerr << mFilename << ": " << mLine << ": multiple characters in character constant " << yytext << endl;
}
<endCharacter>\n {
   cerr << mFilename << ": " << mLine << ": unterminated character constant " << yytext;
   yytext[yyleng-1] = '\'';
   parser->value( new token(yytext,mFilename,mLine,mColumn,mEndLine,mEndColumn) );
   BEGIN(INITIAL);
   return characterLiteral;
}

\" {
   mEndColumn = mColumn; yymore();
   BEGIN(insideString);
}

<insideString>\\\" {
   mEndColumn = mColumn; yymore();
}
<insideString>\" {
   parser->value( new token(yytext,mFilename,mLine,mColumn,mEndLine,mEndColumn) );
   BEGIN(INITIAL);
   return stringLiteral;
}
<insideString>. {
   mEndColumn = mColumn; yymore();
}
<insideString>\n {
   cerr << mFilename << ": " << mLine << ": unterminated string " << yytext;
   yytext[yyleng-1] = '"';
   parser->value( new token(yytext,mFilename,mLine,mColumn,mEndLine,mEndColumn) );
   BEGIN(INITIAL);
   return stringLiteral;
}

"/*"			BEGIN(comment);
<comment>[^*\n]*	
<comment>"*"+[^*/\n]*	
<comment>\n		
<comment>"*"+"/"	BEGIN(INITIAL);

.  {
    parser->value( new token(yytext,mFilename,mLine,mColumn,mEndLine,mEndColumn) );
    return yytext[0];
}

{quote}<EOF> {
    cerr << "Unterminated quote" << endl;
    yyterminate();
}

%%
int sswrap() {return 1; }

string makePrintable(const string& s) {
    string result;
    for(int i = 0; i < s.size(); ++i) {
        if ( isgraph(s[i]) || s[i] == ' ' ) {
            result = result + s[i];
        } else {
            char buf[8];

            switch ( s[i] ) {
            case '\n':
                result += "\\n";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                sprintf(buf,"\\%03o",s[i]);
                result =+ buf;
            }
        }
    }
    return result;
}
sisalc::SisalLexer::SisalLexer(const char* name,istream* arg_yyin, ostream* arg_yyout )
    : ssFlexLexer(arg_yyin,arg_yyout), parser(0), mFilename(name),
    mLine(1), mColumn(1), mEndLine(1), mEndColumn(0)
{
}

void sisalc::SisalLexer::columnLineAdjust() {
   mLine = mEndLine;
   mColumn = mEndColumn;

   //fprintf(stderr,"%s:%d: @ col %d '%s'\n", mFilename.c_str(),mLine,mColumn,makePrintable(yytext).c_str());
   for(int i=0;i<yyleng; ++i) {
      if ( yytext[i] == '\n' ) {
         mEndLine++;
         mEndColumn = 1;
      } else if ( yytext[i] == '\t' ) {
         mEndColumn = (mEndColumn+SSTABSTOP)/SSTABSTOP*SSTABSTOP;
      } else {
         ++mEndColumn;
      }
   }
}
