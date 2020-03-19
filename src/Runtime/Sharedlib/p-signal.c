/**************************************************************************/
/* FILE   **************         p-signal.c        ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "sisalrt.h"
#include <signal.h>

static void HandleSig( SigCode )
int SigCode;
{
  extern void InitSignalSystem();

  if ( UsingSdbx )
    switch ( SigCode ) {
      case SIGINT:
        SdbxMonitor( SDBX_INT );
        InitSignalSystem();
        return;

      case SIGFPE:
        SdbxMonitor( SDBX_FPE );
        break;

      default:
        break;
      }

#ifdef HAVE_PSIGNAL
  psignal(SigCode,"\nERROR");
#endif
  AbortParallel();
}

void InitSignalSystem()
{
  signal( SIGINT, HandleSig );
  signal( SIGFPE, HandleSig );
}
