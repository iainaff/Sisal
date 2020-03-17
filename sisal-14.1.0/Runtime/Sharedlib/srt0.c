/**************************************************************************/
/* FILE   **************           srt0.c          ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

/************************************************************************\
 * p-srt0.c - SISAL runtime system main
\************************************************************************/

#include "sisalrt.h"

static char RCSVERSION[] = "$State$";
static char RCS_REVISION[SIZEOF(RCSVERSION)] = "?.?";

/************************************************************************\
 * MAIN
\************************************************************************/

int main( argc, argv )
     int argc;
     char **argv;
{
  int           i;

  FibreInFd = stdin;
  FibreOutFd = stdout;
  PerfFd = stderr;

  ParseCommandLine( argc, argv );
  InitSisalRunTime();

  /* Form the version number from the RCS checkout revision number */
  if ( RCSVERSION[6] == ':' ) {
    strcpy(RCS_REVISION,RCSVERSION+8);
    for(i=0; RCS_REVISION[i]; i++) {
      if ( RCS_REVISION[i] == '$' ) {
        RCS_REVISION[i] = '\0';
        break;
      }
    }
  }

  if (!NoFibreOutput) FPRINTF( stderr, "%s %s\n", SISAL_BANNER, RCS_REVISION );

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
