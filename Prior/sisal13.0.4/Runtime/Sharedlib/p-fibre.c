#include "world.h"


#define BASE10        10
#define BASE8         8

#define TEXT_SIZE     251
#undef MAX_CHAR
#define MAX_CHAR      0x7f

char	*ArgumentP = ArgumentString;
static	int	InputLine	= 1;
static	int	InputColumn	= 0;
static  char    Buffer		= '\0';
        char    LookAhead	= FALSE;
        int     LookAheadToken;

char    FibreChar;
char    FibreBool;
char    FibreNil;
int     FibreInt;
float   FibreFlt;
double  FibreDbl;

int     Indent = 0;

static char    Text[TEXT_SIZE];
static int     Length;
static int     Token;
static int     CharMode = FALSE;

#define	GET_NEXT_TOKEN	(InputColumn++,Buffer = (*ArgumentP)?(*ArgumentP++):getc(FibreInFd), InputLine=(Buffer=='\n')?(InputColumn=0,InputLine+1):(InputLine),Buffer)
#define ISDIGIT(X)	(((X) >= '0') && ((X) <= '9'))
#define LENGTH_OK	(Length < TEXT_SIZE)
#define ADD_TO_TEXT	if (LENGTH_OK) Text[Length++] = Buffer

#define GET_DIGITS        while (ISDIGIT(Buffer)) {           \
                               ADD_TO_TEXT;                      \
                               GET_NEXT_TOKEN;                   \
                               }

void FibreError(s)
     char   *s;
{
  char		buf[1024],charname[10];


  if ( Buffer == (char)EOF ) {
    strcpy(charname,"<EOF>");
  } else if ( isprint(Buffer) ) {
    sprintf(charname,"%c",Buffer);
  } else {
    sprintf(charname,"\\%03o",Buffer);
  }

  sprintf(buf,"%s at line %d, column %d = '%s'\n",
	  s,InputLine,InputColumn,charname);
  SisalError("FIBRE",buf);
}

static long GetLong( Base )
register int Base;
{
  register long Value;

  Value = 0;

  for ( ;; ) {
    if ( ISDIGIT( Buffer ) ) {
      if ( (Buffer - '0') >= Base )
        FibreError( "ILLEGAL DIGIT" );

      Value = (Value * Base) + (Buffer - '0');
      }
    else
      break;

    GET_NEXT_TOKEN;
    }

  UNGETC( Buffer, FibreInFd );
  return( Value );
}


static void GetExponent()
{
  Buffer = 'e';

  ADD_TO_TEXT;
  GET_NEXT_TOKEN;

  if ((Buffer == '-') || (Buffer == '+')) {
    ADD_TO_TEXT;
    GET_NEXT_TOKEN;
    }

  if ( ISDIGIT( Buffer ) )
    { GET_DIGITS; }
  else    
    FibreError( "ILLEGAL EXPONENT" );

  UNGETC( Buffer ,FibreInFd );
}


static void GetFraction( FractionOptional )
int FractionOptional;
{
  ADD_TO_TEXT;
  GET_NEXT_TOKEN;

  if ( !FractionOptional && !ISDIGIT( Buffer ) )
    FibreError( "ILLEGAL FLOATING POINT NUMBER" );

  GET_DIGITS;

  switch ( Buffer ) {
    case 'D' :
    case 'd' :
    case 'e' :
    case 'E' :
      GetExponent();
      break;

    default:
      UNGETC( Buffer, FibreInFd );
      break;
    }
}


static int PackageNumber()
{

  Text[Length] = '\0';

  switch ( Token ) {
    case INT_ :
      FibreInt = atoi( Text );
      break;

    case DOUBLE_ :
      FibreDbl = atof( Text ); 
      break;
    }

  return( Token );
}


static char GetChar()
{
  register char Value;
  register long Escape;  

  if ( Buffer == '\\' ) {
    GET_NEXT_TOKEN;

    if ( ISDIGIT( Buffer ) ) {
      Escape = GetLong( BASE8 );
      Value = (char) Escape;

      if ((Escape < 0) || (Escape > MAX_CHAR)) 
        FibreError( "PRECEEDING OCTAL ESCAPE OUT OF RANGE" );
      }
    else
      switch ( Buffer ) {
        case 'n' :
        case 'N' :
          Value = '\n';
          break;

        case 'f' :
        case 'F' :
          Value = '\f';
          break;

        case 'b' :
        case 'B' :
          Value = '\b';
          break;

        case 'r' :
        case 'R' :
          Value = '\r';
          break;

        case 't' :
        case 'T' :
          Value = '\t';
          break;

        default  :
          Value = Buffer;
          break;
        }
    } else  {
      if ( (Buffer < ' ') || (Buffer > '~') )
        FibreError( "PRINTABLE CHARACTER EXPECTED" );

      Value = (long) Buffer;
      }

  return( Value );
}


static int GetNil()
{
  GET_NEXT_TOKEN;

  switch ( Buffer ) {
    case 'I' :
    case 'i' :
      break;

    default  :
      FibreError( "ILLEGAL CHARACTER FOLLOWING N" );
    }

  GET_NEXT_TOKEN;

  switch ( Buffer ) {
    case 'L' :
    case 'l' :
      break;

    default  :
      FibreError( "ILLEGAL CHARACTER FOLLOWING NI" );
    }

  return( 0 );
}


static int FibreLex()
{
  Length = 0;

  GET_NEXT_TOKEN;


  if ( CharMode ) {
    if ( Buffer == '"' ) {
      CharMode = FALSE;
      return( STRING_TERM_ );
      }

    FibreChar = GetChar();
    return( STRING_CHAR_ );
    }

TopOfWorld:

  Token = DOUBLE_;

  if ( ISDIGIT( Buffer ) ) {
    GET_DIGITS;

    switch ( Buffer ) {
      case '.' :
        GetFraction( TRUE );
        break;

      case 'D' :
      case 'd' :
      case 'e' :
      case 'E' :
        GetExponent();
        break;

      default  :
        Token = INT_;
        UNGETC( Buffer, FibreInFd );
	break;
      }

    return( PackageNumber() );
    }

  switch ( Buffer ) {
    case '.':
      GetFraction( FALSE );
      return( PackageNumber() );

    case '-':
    case '+':
      ADD_TO_TEXT;
      GET_NEXT_TOKEN;

      if ( ISDIGIT( Buffer ) || Buffer == '.' ) 
        goto TopOfWorld;

      FibreError( "ILLEGAL NUMBER" );
      
    case 't' :
    case 'T' :
      FibreBool = TRUE;
      return( BOOL_ );

    case 'f' :
    case 'F' :
      FibreBool = FALSE;
      return( BOOL_ );

    case 'N' :
    case 'n' :
      FibreNil = GetNil();
      return( NIL_ );

    case '"' :
      CharMode = TRUE;
      return( STRING_START_ );

    case '#' :
      while ( Buffer != '\n' )
        GET_NEXT_TOKEN;

      goto TopOfWorld;

    case '\'':
      GET_NEXT_TOKEN;

      FibreChar = GetChar();

      GET_NEXT_TOKEN;

      if ( Buffer != '\'' )
        FibreError( "CHARACTER DELIMITER EXPECTED: (')" );

      return( CHAR_ );

    case ',' :
      return( COMMA_ );

    case '>' :
      return( RECORDE_ );

    case '<' :
      return( RECORDB_ );

    case '}' :
      return( STREAME_ );

    case '{' :
      return( STREAMB_ );

    case ')' :
      return( UNIONE_ );

    case '(' :
      return( UNIONB_ );

    case ':' :
      return( COLON_ );

    case '[' :
      return( ARRAYB_ );

    case ']' :
      return( ARRAYE_ );

    case ';' :
      return( SEMI_COLON_ );


    case ' ' :
    case '\t' :
    case '\b' :
    case '\r' :
    case 013 : /* VT */
    case 014 : /* FF */
      GET_NEXT_TOKEN;

      goto TopOfWorld;

    case '\n' :
      GET_NEXT_TOKEN;

      goto TopOfWorld;

    case EOF :
      return( EOF_ );

    default  :
      FibreError( "ILLEGAL TOKEN START CHARACTER" );
    }

  return 0;
}


static char *Messages[] = {
      "STRING TERMINATOR EXPECTED: (\")",
      "STRING INITIATOR EXPECTED: (\")",
      "STRING CHARACTER EXPECTED",
      "INTEGER EXPECTED",
      "CHARACTER CONSTANT EXPECTED",
      "DOUBLE EXPECTED",
      "NIL EXPECTED",
      "BOOLEAN EXPECTED",
      "EOF EXPECTED",
      "> EXPECTED",
      "< EXPECTED",
      "} EXPECTED",
      "{ EXPECTED",
      ") EXPECTED",
      "( EXPECTED",
      ": EXPECTED",
      "[ EXPECTED",
      "] EXPECTED",
      "; EXPECTED",
      "FLOAT EXPECTED",
      ", EXPECTED"};


int FibreParse( Expected ) 
int Expected;
{
  int FibreToken;

  if ( LookAhead ) {
    FibreToken     = LookAheadToken;
    LookAhead = FALSE;
    }
  else 
    FibreToken = FibreLex();

  if ( Expected == ANY_ ) {
    if ( FibreToken == EOF_ )
      FibreError( "UNEXPECTED EOF" );

    return( FibreToken );
    }

  if ( Expected != FibreToken ) {
    if ( Expected == FLOAT_ && FibreToken == DOUBLE_ )
      FibreFlt  = FibreDbl;
    else
      FibreError(  Messages[ Expected - BASE_ ] );
    }

  return( Expected );
}
