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
static char VERSION[SIZEOF(RCSVERSION)] = "?.?";

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
    strcpy(VERSION,RCSVERSION+8);
    for(i=0; VERSION[i]; i++) {
      if ( VERSION[i] == '$' ) {
        VERSION[i] = '\0';
        break;
      }
    }
  }

  FPRINTF( stderr, "%s %s\n", SISAL_BANNER, VERSION );

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
