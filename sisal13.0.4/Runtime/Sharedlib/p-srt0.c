/************************************************************************\
 * p-srt0.c - SISAL runtime system main
\************************************************************************/

#include "world.h"

static char RCSVERSION[] = "$State$";
static char VERSION[SIZEOF(RCSVERSION)] = "?.?";

/************************************************************************\
 * MAIN
\************************************************************************/

#if !defined(POWER4)

int main( argc, argv )
     int argc;
     char **argv;
{
  int		i;

  FibreInFd = stdin;
  FibreOutFd = stdout;
  PerfFd = stderr;

  ParseCommandLine( argc, argv );
  InitSisalRunTime();

  /* Form the version number from the RCS checkout revision number */
  if ( RCSVERSION[6] == ':' ) {
    strcpy(VERSION,RCSVERSION+8);
    for(i=0; VERSION[i]; i++) {
      if ( VERSION[i] == '$' ) {
	VERSION[i] = '\0';
	break;
      }
    }
  }

  FPRINTF( stderr, "%s %s\n", BANNER, VERSION );

  SisalMainArgs = ReadFibreInputs();

  StartWorkers();
  SisalMain( SisalMainArgs );
  StopWorkers();

  if ( !NoFibreOutput ) {
    WriteFibreOutputs( SisalMainArgs );
    fputc( '\n', FibreOutFd );
  }

  if ( GatherPerfInfo )
    DumpRunTimeInfo();

  ShutDownDsa();

  return 0;
}

/************************************************************************\
 * IBM Power4 MAIN - requires a hideous startup to go parallel
\************************************************************************/

#else /* POWER4 */

extern struct shared_s LSR;

char bbbuf[64*1024];

int main(int argc, char *argv[])
{
	extern	p6k_fmain();
	extern	p6k_fmaster();
	extern	p6k_envdef();
	int	i;
	char *outstr;
	char me[255];
	char *dbx;

	SRp = &LSR;

  	if(ParseCommandLine( argc, argv ))
	{
		outstr = malloc(255);
		sprintf(outstr,"GEN_NCPUS=%d",NumWorkers);
		putenv(outstr);
		outstr = malloc(255);
		sprintf(outstr,"GEN_CPU1=junior2");
		putenv(outstr);
		outstr = malloc(255);
		sprintf(outstr,"GEN_CPU2=junior3");
		putenv(outstr);
		outstr = malloc(255);
		sprintf(outstr,"GEN_CPU3=junior4");
		putenv(outstr);

	}

	/*
	 * if we are running under DBX, set flag so we can ignore
	 * SIGINT
	 */
        dbx = getenv("GEN_DBX");
	if(dbx != NULL)
		UnderDBX = 1;
	else
		UnderDBX = 0;

	i = p6k_main(argc,argv,p6k_fmain,p6k_fmaster);
	return(i);
}


p6k_fmaster( argc, argv )
int argc;
char **argv;
{
  int		i;

  InitSisalRunTime();

 /*
  * here, we are have called p6k_main.  Copy into shared
  * region
  */
  memmove(&SR,&LSR,sizeof(LSR));


  /* Form the version number from the RCS checkout revision number */
  if ( RCSVERSION[6] == ':' ) {
    strcpy(VERSION,RCSVERSION+8);
    for(i=0; VERSION[i]; i++) {
      if ( VERSION[i] == '$' ) {
	VERSION[i] = '\0';
	break;
      }
    }
  }

}

p6k_fmain( argc, argv )
int argc;
char **argv;
{

  SisalMainArgs = ReadFibreInputs();

  StartWorkers();

  if ( !NoFibreOutput ) {
    WriteFibreOutputs( SisalMainArgs );
    fputc( '\n', FibreOutFd );
  }

  if ( GatherPerfInfo )
    DumpRunTimeInfo();

  ShutDownDsa();

  exit(0);
}

#endif
