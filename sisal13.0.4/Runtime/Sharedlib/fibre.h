extern FILE *FibreInFd;
extern FILE *FibreOutFd;
extern FILE *PerfFd;

extern void    FibreError();
extern int     FibreParse();
extern POINTER SisalMainArgs; 
extern POINTER ReadFibreInputs();
extern void    WriteFibreOutputs();

#define PrintIndent \
{                                                  \
  register int Counter;                            \
  for ( Counter = Indent; Counter > 0; Counter-- ) \
    fprintf( FibreOutFd, " " );                    \
}

extern char *iformat,*fformat,*dformat,*nformat,*cformat,*cformat2,*bformat;
#define WriteInt(x)  {PrintIndent; fprintf( FibreOutFd, iformat, x );   }
#define WriteFlt(x)  {PrintIndent; fprintf( FibreOutFd, fformat, x ); }
#define WriteDbl(x)  {PrintIndent; fprintf( FibreOutFd, dformat, x );}
#define WriteNil(x)  {PrintIndent; fprintf( FibreOutFd, nformat, x );  }

#define WriteChar(x) \
{                                              \
  PrintIndent;                                 \
  if ( ((x) < ' ') || ((x) > '~') )            \
    fprintf( FibreOutFd, cformat2, (x) & 0xff);\
  else if ( (x) == '\\' )                      \
    fprintf( FibreOutFd, "'\\\\'\n" );         \
  else if ( (x) == '\'' )                      \
    fprintf( FibreOutFd, "'\\''\n" );          \
  else                                         \
    fprintf( FibreOutFd, cformat, (x) & 0xff );\
}

#define WriteBool(x) \
  {PrintIndent; fprintf( FibreOutFd, bformat, (x)? 'T' : 'F' );}

#define ReadInt(x)  {FibreParse( INT_ );    x = FibreInt; }
#define ReadFlt(x)  {FibreParse( FLOAT_ );  x = FibreFlt; }
#define ReadDbl(x)  {FibreParse( DOUBLE_ ); x = FibreDbl; }
#define ReadNil(x)  {FibreParse( NIL_ );    x = FibreNil; }
#define ReadBool(x) {FibreParse( BOOL_ );   x = FibreBool;}
#define ReadChar(x) {FibreParse( CHAR_ );   x = FibreChar;}

#define IF_BOOL   12
#define IF_CHAR   13
#define IF_DOUBLE 14
#define IF_INT    15
#define IF_NULL   16
#define IF_REAL   17

#define IF_NON    18

#define IF_RCPX   19
#define IF_DCPX   20

#define IF_ARRAY   0
#define IF_RECORD  5
#define IF_UNION   9
#define IF_STREAM  6

extern char    FibreChar;
extern char    FibreBool;
extern char    FibreNil;
extern int     FibreInt;
extern float   FibreFlt;
extern double  FibreDbl;

extern int     FibreStrings;

#define BASE_         300
#define STRING_TERM_  300
#define STRING_START_ 301
#define STRING_CHAR_  302
#define INT_          303
#define CHAR_         304
#define DOUBLE_       305
#define NIL_          306
#define BOOL_         307
#define EOF_          308
#define RECORDE_      309
#define RECORDB_      310
#define STREAME_      311
#define STREAMB_      312
#define UNIONE_       313
#define UNIONB_       314
#define COLON_        315
#define ARRAYB_       316
#define ARRAYE_       317
#define SEMI_COLON_   318
#define FLOAT_        319
#define COMMA_        320
#define ANY_          321

extern char    LookAhead;
extern int     LookAheadToken;
extern int     Indent;

#define GET_LOOKAHEAD  LookAheadToken = FibreParse( ANY_ ); \
		       if ( LookAheadToken != ARRAYE_ &&    \
			    LookAheadToken != STREAME_ )    \
                         LookAhead = TRUE

extern char ArgumentString[];
