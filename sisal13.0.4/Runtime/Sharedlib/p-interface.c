#include "world.h"


static void ConfigureExecution( lsValue, gssValue, bValue, xftValue, axValue )
int lsValue;
int gssValue;
int bValue;
int xftValue;
int axValue;
{
  if ( lsValue > 0 ) {
    if ( gssValue == 1 )
      SisalError( "PARAMETER CONFLICT IN sconfig", "-gss AND -ls" );

    LoopSlices = lsValue;
    }

  if ( gssValue == 1 ) {
    if ( LoopSlices > 0 )
      SisalError( "PARAMETER CONFLICT IN sconfig", "-gss AND -ls" );

    DefaultLoopStyle = 'G';	/* GSS */
    }

  if ( bValue == 1 )
    BindParallelWork = TRUE;

  if ( axValue >= 0 )
    ArrayExpansion = axValue;

  if ( xftValue >= 0 )
    XftThreshold = xftValue;
}

static void ParseInterfaceArguments( wValue, dsValue, rValue )
int wValue;
int dsValue;
int rValue;
{
  if ( dsValue > 0 )
    DsaSize = dsValue;
  else
    SisalError( "ILLEGAL VALUE SPECIFIED IN sstart", "-ds" );

  if ( wValue > 0 && wValue <= MAX_PROCS )
    NumWorkers = wValue;
  else
    SisalError( "ILLEGAL VALUE SPECIFIED IN sstart", "-w" );

  if ( rValue == TRUE ) {
    GatherPerfInfo = TRUE;
    OPEN( PerfFd, SINFOFile, "a" );
    }

  if ( LoopSlices == -1 )
    LoopSlices = NumWorkers;
}

#define FIBREIN		1
#define FIBREOUT	2
#define SINFO		3

static void ParseFileParameters(filename)
    char        *filename;
{
    FILE        *optfile;
    char        arg[512];
    int         argc;
    char        *argv[100];

    /*** Read the file into argc and argv. ***/

    argv[0] = '\0';		/* no command was used */
    argc = 1;
    optfile = fopen(filename, "r");
    if (optfile==NULL) {
        SisalError("UNABLE TO READ STARTUP FILE SPECIFIED IN sstartf", 
                   filename);
        return;
    }
    while (fscanf(optfile, " %s", arg)==1 && argc<100) {
        argv[argc++] = strcpy((char*)malloc(strlen(arg)+1), arg);
    }
    fclose(optfile);

    ParseCommandLine(argc, argv);
    return;
}

#define SCONFIG_FUNCTION(x)                                                \
void x( lsValue, gssValue, bValue, xftValue, axValue )                     \
int *lsValue;                                                              \
int *gssValue;                                                             \
int *bValue;                                                               \
int *xftValue;                                                             \
int *axValue;                                                              \
{                                                                          \
  ConfigureExecution( *lsValue, *gssValue, *bValue, *xftValue, *axValue ); \
}

#define SSTART_FUNCTION(x)                               \
void x( wValue, dsValue, rValue )                        \
int *wValue;                                             \
int *dsValue;                                            \
int *rValue;                                             \
{                                                        \
  ParseInterfaceArguments( *wValue, *dsValue, *rValue ); \
  InitSisalRunTime();                                    \
  StartWorkers();                                        \
}

#define SPCALL_FUNCTION(x)				\
void x ( filename, entry )				\
char *filename;						\
void (*entry)();					\
{							\
  ParseFileParameters( filename );                      \
  InitSisalRunTime();                                   \
  StartWorkersWithEntry(entry);                         \
}


#define SSTARTF_FUNCTION(x)                              \
void x( filename )                                       \
char *filename;                                          \
{                                                        \
  ParseFileParameters( filename );                       \
  InitSisalRunTime();                                    \
  StartWorkers();                                        \
}

#define SSTOP_FUNCTION(x) \
void x()                  \
{                         \
  StopWorkers();          \
  ShutDownDsa();          \
  ReleaseSharedMemory();  \
  if ( GatherPerfInfo )   \
    DumpRunTimeInfo();    \
}


/* C VERSIONS */
SSTART_FUNCTION( sstart )
SSTARTF_FUNCTION( sstartf )
SSTOP_FUNCTION( sstop )
SCONFIG_FUNCTION( sconfig )

/* FORTRAN VERSIONS: CRAY, OTHERS */
SSTART_FUNCTION( SSTART )
SSTART_FUNCTION( sstart_ )
SSTARTF_FUNCTION( SSTARTF )
SSTARTF_FUNCTION( sstartf_ )
SSTOP_FUNCTION( SSTOP )
SSTOP_FUNCTION( sstop_ )
SCONFIG_FUNCTION( SCONFIG )
SCONFIG_FUNCTION( sconfig_ )
SPCALL_FUNCTION( SPCALL )
SPCALL_FUNCTION( spcall_ )


#define IDInfo( x, y, z, w ) \
{ \
  register int   i; \
  register int   DimInc; \
  register int   InfoInc; \
  register int   DSize   = 1; \
  register int   Major   = z[0]; \
  register int   Mode    = z[1]; \
  register int   Mutable = w; \
  z += 3; \
  switch ( Major ) { \
    case ROW_MAJOR: \
      InfoInc = -5; \
      z   += ((x-1)*5); \
      if ( Mode == PRESERVE ) { \
        DimInc = -1; \
        y += (x-1); \
        } \
      else \
        DimInc = 1; \
      break; \
    case COL_MAJOR: \
      InfoInc = 5; \
      if ( Mode == PRESERVE ) \
	DimInc = 1; \
      else { \
	DimInc = -1; \
        y += (x-1); \
	} \
      break; \
    default: \
      InfoInc = -5; \
      DimInc = 1; \
      SisalError( "ILLEGAL ARRAY DESCRIPTOR", \
                  "mixed language interface row major" );  \
    } \
  if ( DimInc == 1 && x != 1 ) \
    Mutable = FALSE; \
  for ( i = 0; i < x; i++ ) { \
    y->LSize   = z[LHI]-z[LLO]+1; \
    y->Offset  = z[LLO]-z[PLO]; \
    y->SLow    = z[SLO]; \
    y->DSize   = DSize; \
    y->Mutable = Mutable; \
    DSize *= (z[PHI]-z[PLO]+1); \
    y += DimInc; \
    z    += InfoInc; \
    } \
}


void InitDimInfo( ronly, Dim, DimInfo, Info )
int       ronly;
int       Dim;
DIMINFOP  DimInfo;
int      *Info;
{
#if SUNIX || ALLIANT || CRAY || SUN || SGI || RS6000
  IDInfo( Dim, DimInfo, Info, Info[2] || ronly );
#else
  IDInfo( Dim, DimInfo, Info, FALSE );
#endif
}

void IDescriptorCheck( Dim, Info )
int  Dim;
int *Info;
{
  register int CurrentDim;
  register int Plo,Phi, Llo,Lhi;
  register int Major;

  Plo = Phi = Llo = Lhi = CurrentDim = 0;

  switch ( (Major = *Info) ) {
    case ROW_MAJOR:
    case COL_MAJOR:
      break;

    default:
      goto InfoError;
    }

  Info = &(Info[3]);

  for ( CurrentDim = 1; CurrentDim <= Dim; CurrentDim++ ) {
    Plo = Info[PLO];
    Phi = Info[PHI];
    Llo = Info[LLO];
    Lhi = Info[LHI];

    if ( Phi-Plo+1 == 0 ) {
      if ( Lhi-Llo+1 != 0 ) goto InfoError;
      continue;
      }

    if ( Lhi-Llo+1 == 0 ) {
      if ( Phi-Plo+1 < 0 ) goto InfoError;
      continue;
      }

    if ( Lhi-Llo+1 < 0 ) goto InfoError;
    if ( Phi-Plo+1 < 0 ) goto InfoError;

    if ( Llo < Plo || Llo > Phi ) goto InfoError;
    if ( Lhi < Plo || Lhi > Phi ) goto InfoError;

    Info = Info+5;
    }

  return;

InfoError:
  if ( UsingSdbx )
    SdbxMonitor( SDBX_IERR );

  FPRINTF( stderr, 
	   "Descriptor Info: Major=%d Dimensions=%d Current Dimmension = %d\n", 
	   Major, Dim, CurrentDim );

  FPRINTF( stderr, "Descriptor Info: Plo=%d Phi=%d Llo=%d Lhi=%d\n",
		    Plo,Phi,Llo,Lhi );

  SisalError( "ILLEGAL ARRAY DESCRIPTOR", 
              "mixed language interface IDescriptorCheck" );
}
