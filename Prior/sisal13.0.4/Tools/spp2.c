#include "../config.h"

#define MAX_LENGTH  20000
#define EXTRA       10      /* NEUTRAL ZONE!!! */

#define IsDigit(c) ( (c >= '0') && (c <= '9') )

#define OK    0
#define ERROR 1

char *program = "spp2";

char *bfile = "BaDNaMe";
char *rfile = "RePlAcEmEnT";

static char *line  = NULL;
static char *file  = NULL;
static int   lnum  = 0;


char *MyAlloc( sz )
int sz;
{
  char *p;

  if ( (p = (char*)malloc(sz)) == NULL ) {
    fprintf( stderr, "%s: MyAlloc FAILED!!!\n", program );
    exit(ERROR);
    }
  
  return( p );
}


static int GetLine()
{
   register int c;
   register int i;
   register int hist;
   register char *s;
   register char *nb;
   register char *b;

   /* FREE PREVIOUS LINE */
   if ( line != NULL )
     free( line );

   hist = 1;

   for (;;) {

     if ( hist > 1 ) {
       nb = s = MyAlloc( MAX_LENGTH*hist+EXTRA );

       for ( i = 0 ; i < MAX_LENGTH-1; i++ ) {
	 *s = b[i];
	 s++;
         }

       free( b );
       b = nb;
     } else {
       b = s = MyAlloc( MAX_LENGTH+EXTRA );
       }

     for ( i = 0 ; i < MAX_LENGTH-1; i++ ) {
       if ( (c = getc(stdin)) == EOF )
         return( EOF );

       if ( c == '\n' ) {
         s[i]   = '\n';
         s[i+1] = '\0';
  
	 line = b;
         return( ~EOF );
         }

       s[i] = c;
       }

     hist++;
     }
}


static char *SkipWhites( s )
char *s;
{
  while ( *s == ' ' || *s == '\t' ) 
    s++;

  return( s );
}

static char *SkipUpToDoubleQuote( s )
char *s;
{
  while ( !(*s == '"' || *s == '\n') )
    s++;

  return( s );
}


static int IsCppInfoLine()
{
  register char *s;
  register char *lnstart;
  register char *lnstop;
  register char *fnstart;
  register char *fnstop;

  /* # line line_number "file_name" */
  /* #      line_number "file_name" */

  s = SkipWhites(line);

  if ( *s++ != '#' ) return( FALSE );

  s = SkipWhites( s );

  if ( *s == 'l' ) {
    if ( *s++ != 'l' ) return( FALSE );
    if ( *s++ != 'i' ) return( FALSE );
    if ( *s++ != 'n' ) return( FALSE );
    if ( *s++ != 'e' ) return( FALSE );

    s = SkipWhites( s );
    }

  if ( !IsDigit(*s) ) return( FALSE );

  lnstart = s;

  while ( IsDigit(*s) ) 
    s++;

  lnstop = s;

  s = SkipWhites( s );

  if ( *s++ != '"' ) return( FALSE );

  fnstart = s;

  s = SkipUpToDoubleQuote( s );

  if ( *s != '"' ) return( FALSE );

  fnstop = s;

  s = SkipWhites( s+1 );

  if ( *s != '\n' )
    return( FALSE );

  *lnstop = '\0';
  *fnstop = '\0';

  lnum = atoi( lnstart );
  file = fnstart;

  /* GET RID OF THE root.c NAMES INTRODUCED BY SPPRUN!!! */
  if ( strcmp( file, bfile ) == 0 )
    file = rfile;

  return( TRUE );
}

void
main( argc, argv )
int    argc;
char **argv;
{
  register char *c;
  register int   idx;

  for ( idx = 1; idx < argc; ++idx )
    if ( *(c = argv[ idx ]) == '-' )
      switch ( *( ++c ) ) {
	case 'R':
	  rfile = c+1;
	  break;

	case 'B':
	  bfile = c+1;
	  break;

        default:
	  break;
        }

  while ( GetLine() != EOF ) {
    if ( IsCppInfoLine() ) {
      printf( "%%$UNINCLUDE(%s,%d)\n", file, lnum-1 );
      continue;
      }

    printf( "%s", line );
    }

  exit( OK );
}
