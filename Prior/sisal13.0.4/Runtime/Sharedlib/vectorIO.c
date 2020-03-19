#include "world.h"

int	FibreStrings = TRUE;

#define GenericReadArray(scalartype,reader,term)\
{ \
  POINTER	val0; \
  register scalartype val6; \
  register int lob; \
 \
  FibreParse( INT_ ); \
  lob = FibreInt; \
  GET_LOOKAHEAD; \
  if ( LookAheadToken == COMMA_ ) { \
    FibreParse( COMMA_ ); \
    FibreParse( INT_ ); \
    OptABld( val0, lob, 1, FibreInt, scalartype ); \
    ((ARRAYP)val0)->Phys->Size = 0; \
  } else { \
    ABld( val0, lob, 1 ); \
  } \
  FibreParse( COLON_ ); \
 \
  GET_LOOKAHEAD; \
  while ( LookAheadToken != term ) { \
    if ( LookAheadToken == SEMI_COLON_ ) \
      FibreError( "REPETITION FACILITY NOT IMPLEMENTED" ); \
    reader( val6 ); \
    AGather( val0, val6, scalartype ); \
    GET_LOOKAHEAD; \
  } \
  return val0; \
}

/* ------------------------------------------------------------ */
/* B O O L E A N						*/
/* ------------------------------------------------------------ */
POINTER ReadBoolVector()
{
  switch ( FibreParse( ANY_ ) ) {
  case ARRAYB_:
  GenericReadArray(char,ReadBool,ARRAYE_);

  case STREAMB_:
  GenericReadArray(char,ReadBool,STREAME_);

  case STRING_START_:
    FibreError( "STRING DELIMITER WAS NOT EXPECTED" );
    break;

  default:
    FibreError( "ARRAY DELIMITER EXPECTED" );
  }
  return (POINTER)(NULL);
}
void WriteBoolVector( val )
POINTER val;
{
  register POINTER Base2;
  register int     HiBound;
  register int     Lo2;
  register ARRAYP arr = (ARRAYP) val;

  PrintIndent;
  Lo2 = arr->LoBound;
  fprintf( FibreOutFd, "[ %d,%d:", Lo2, Lo2+(arr->Size)-1 );
  fprintf( FibreOutFd, " # DRC=%d PRC=%d\n", arr->RefCount, arr->Phys->RefCount );

  Indent++;
  Base2 = arr->Base;
  HiBound = Lo2 + arr->Size - 1;
  for (	; Lo2 <= HiBound; Lo2++ ) {
    WriteBool( (((char*)Base2)[Lo2]) );
  }

  Indent--;
  PrintIndent;
  fprintf( FibreOutFd, "]\n" );
}


/* ------------------------------------------------------------ */
/* C H A R A C T E R						*/
/* ------------------------------------------------------------ */
POINTER ReadCharVector()
{
  POINTER	val0;

  switch ( FibreParse( ANY_ ) ) {
  case ARRAYB_:
    GenericReadArray(char,ReadChar,ARRAYE_);

  case STREAMB_:
    GenericReadArray(char,ReadChar,STREAME_);

  case STRING_START_:
    ABld( val0, 1, 1 );
    while ( FibreParse( ANY_ ) != STRING_TERM_ ) {
      AGather( val0, FibreChar, char );
    }
    return val0;

  default:
    FibreError( "ARRAY DELIMITER EXPECTED" );
  }
  return (POINTER)(NULL);
}

void WriteCharVector( val )
POINTER val;
{
  register POINTER Base2;
  register int     HiBound;
  register int     Lo2;
  register ARRAYP  arr = (ARRAYP) val;
  int		   c;

  PrintIndent;
  Lo2 = arr->LoBound;
  Base2 = arr->Base;
  HiBound = Lo2 + arr->Size - 1;
  if ( FibreStrings && Lo2 == 1 ) {
    fputc( '"', FibreOutFd );
    for (	; Lo2 <= HiBound; Lo2++ ) {
      c = *((char*)Base2+Lo2);
      if ( isascii(c) && isprint(c) ) {
	if ( c == '"' ) {
	  fputs( "\\\"",FibreOutFd);
	} else if ( c == '\\' ) {
	  fputs( "\\\\",FibreOutFd);
	} else { 
	  fputc( c, FibreOutFd );
	}
      } else {
	switch ( c ) {
	case '\b': fputs( "\\b", FibreOutFd ); break;
	case '\n': fputs( "\\n", FibreOutFd ); break;
	case '\f': fputs( "\\f", FibreOutFd ); break;
	case '\r': fputs( "\\r", FibreOutFd ); break;
	case '\t': fputs( "\\t", FibreOutFd ); break;
	default:
	  fprintf( FibreOutFd,"\\%03o", c & 0xff);
	}
      }
    }
    fputc( '"', FibreOutFd );
  } else {
    fprintf( FibreOutFd, "[ %d,%d:", Lo2, Lo2+(arr->Size)-1 );
    fprintf( FibreOutFd, " # DRC=%d PRC=%d\n", arr->RefCount, arr->Phys->RefCount );

    Indent++;
    for (	; Lo2 <= HiBound; Lo2++ ) {
      WriteChar( *((char*)Base2+Lo2) );
    }

    Indent--;
    PrintIndent;
    fputc( ']', FibreOutFd );
  }
  fputc( '\n', FibreOutFd );
}

/* ------------------------------------------------------------ */
/*  D O U B L E							*/
/* ------------------------------------------------------------ */
POINTER ReadDoubleVector()
{
  switch ( FibreParse( ANY_ ) ) {
  case ARRAYB_:
    GenericReadArray(double,ReadDbl,ARRAYE_);

  case STREAMB_:
    GenericReadArray(double,ReadDbl,STREAME_);

  case STRING_START_:
    FibreError( "STRING DELIMITER WAS NOT EXPECTED" );
    break;

  default:
    FibreError( "ARRAY DELIMITER EXPECTED" );
  }
  return (POINTER)(NULL);
}

void WriteDoubleVector( val )
POINTER val;
{
  register POINTER Base2;
  register int     HiBound;
  register int     Lo2;
  register ARRAYP arr = (ARRAYP) val;

  PrintIndent;
  Lo2 = arr->LoBound;
  fprintf( FibreOutFd, "[ %d,%d:", Lo2, Lo2+(arr->Size)-1 );
  fprintf( FibreOutFd, " # DRC=%d PRC=%d\n", arr->RefCount, arr->Phys->RefCount );

  Indent++;
  Base2 = arr->Base;
  HiBound = Lo2 + arr->Size - 1;
  for ( ; Lo2 <= HiBound; Lo2++ ) {
    WriteDbl( (((double*)Base2)[Lo2]) );
  }

  Indent--;
  PrintIndent;
  fprintf( FibreOutFd, "]\n" );
}

/* ------------------------------------------------------------ */
/* I N T E G E R						*/
/* ------------------------------------------------------------ */
POINTER ReadIntegerVector()
{
  switch ( FibreParse( ANY_ ) ) {
  case ARRAYB_:
    GenericReadArray(int,ReadInt,ARRAYE_);

  case STREAMB_:
    GenericReadArray(int,ReadInt,STREAME_);

  case STRING_START_:
    FibreError( "STRING DELIMITER WAS NOT EXPECTED" );
    break;

  default:
    FibreError( "ARRAY DELIMITER EXPECTED" );
  }
  return (POINTER)(NULL);
}

void WriteIntegerVector( val )
POINTER val;
{
    register POINTER Base2;
    register int     HiBound;
    register int     Lo2;
    register ARRAYP arr = (ARRAYP) val;

    PrintIndent;
  Lo2 = arr->LoBound;
    fprintf( FibreOutFd, "[ %d,%d:", Lo2, Lo2+(arr->Size)-1 );
    fprintf( FibreOutFd, " # DRC=%d PRC=%d\n", arr->RefCount, arr->Phys->RefCount );

    Indent++;
    Base2 = arr->Base;
    HiBound = Lo2 + arr->Size - 1;
    for ( ; Lo2 <= HiBound; Lo2++ ) {
      WriteInt( (((int*)Base2)[Lo2]) );
      }

    Indent--;
    PrintIndent;
    fprintf( FibreOutFd, "]\n" );
}

/* ------------------------------------------------------------ */
/* N U L L							*/
/* ------------------------------------------------------------ */
POINTER ReadNullVector()
{
  switch ( FibreParse( ANY_ ) ) {
  case ARRAYB_:
    GenericReadArray(int,ReadNil,ARRAYE_);

  case STREAMB_:
    GenericReadArray(int,ReadNil,STREAME_);

  case STRING_START_:
    FibreError( "STRING DELIMITER WAS NOT EXPECTED" );
    break;

  default:
    FibreError( "ARRAY DELIMITER EXPECTED" );
  }
  return (POINTER)(NULL);
}

void WriteNullVector( val )
POINTER val;
{
  register POINTER Base2;
  register int     HiBound;
  register int     Lo2;
  register ARRAYP arr = (ARRAYP) val;

  PrintIndent;
  Lo2 = arr->LoBound;
  fprintf( FibreOutFd, "[ %d,%d:", Lo2, Lo2+(arr->Size)-1 );
  fprintf( FibreOutFd, " # DRC=%d PRC=%d\n", arr->RefCount, arr->Phys->RefCount );

  Indent++;
  Base2 = arr->Base;
  HiBound = Lo2 + arr->Size - 1;
  for ( ; Lo2 <= HiBound; Lo2++ ) {
    WriteNil( (((char*)Base2)[Lo2]) );
  }

  Indent--;
  PrintIndent;
  fprintf( FibreOutFd, "]\n" );
}

/* ------------------------------------------------------------ */
/* R E A L							*/
/* ------------------------------------------------------------ */
POINTER ReadRealVector()
{
  switch ( FibreParse( ANY_ ) ) {
  case ARRAYB_:
    GenericReadArray(float,ReadFlt,ARRAYE_);

  case STREAMB_:
    GenericReadArray(float,ReadFlt,STREAME_);

  case STRING_START_:
    FibreError( "STRING DELIMITER WAS NOT EXPECTED" );
    break;

  default:
    FibreError( "ARRAY DELIMITER EXPECTED" );
  }
  return (POINTER)(NULL);
}

void WriteRealVector( val )
POINTER val;
{
  register POINTER Base2;
  register int     HiBound;
  register int     Lo2;
  register ARRAYP arr = (ARRAYP) val;

  PrintIndent;
  Lo2 = arr->LoBound;
  fprintf( FibreOutFd, "[ %d,%d:", Lo2, Lo2+(arr->Size)-1 );
  fprintf( FibreOutFd, " # DRC=%d PRC=%d\n", arr->RefCount, arr->Phys->RefCount );

  Indent++;
  Base2 = arr->Base;
  HiBound = Lo2 + arr->Size - 1;
  for ( ; Lo2 <= HiBound; Lo2++ ) {
    WriteFlt( (((float*)Base2)[Lo2]) );
  }

  Indent--;
  PrintIndent;
  fprintf( FibreOutFd, "]\n" );
}
/* ------------------------------------------------------------ */
#if 0  
"  register %s val%d;\n", i->A_ELEM->tname, indent );

  
"  FibreParse( INT_ );"

  
"{"
  
"register int lob;"
  
"lob = FibreInt;"
  
"GET_LOOKAHEAD;"
  
"if ( LookAheadToken == COMMA_ ) {"
  
"  FibreParse( COMMA_ );"
  
"  FibreParse( INT_ );"

  
"  OptABld( %s, lob, 1, FibreInt, %s );\n", dst, i->A_ELEM->tname );

 "  ((ARRAYP)%s)->Phys->Size = 0;\n", dst ); 

  
"} else {"
  
"  ABld( %s, lob, 1 );\n", dst );
  
" }"
  
"FibreParse( COLON_ );"
  
"}"

  
"  GET_LOOKAHEAD;"

  
"  while ( LookAheadToken != %s ) {\n", del );

  
"    if ( LookAheadToken == SEMI_COLON_ )"

  
  
 "      FibreError( \"REPETITION FACILITY NOT IMPLEMENTED\" );"

    PrintReadOp( indent+4, buf, i->A_ELEM );

  
"    AGather( %s, %s, %s );\n", dst, buf, i->A_ELEM->tname                );
 
  
"    GET_LOOKAHEAD;"

  
"    }"

  
" }"
/* ------------------------------------------------------------ */
#endif
