/**************************************************************************/
/* FILE   **************          if2up.c          ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "sisalc.h"

/* if2up foo.mem foo.up -I */

void if2up(char* file,
           char* result,
           char* bindir,
           int suppressWarnings, /* suppress warnings? */
           int profiling /* profiling */
           ) {
   char updater[MAXPATHLEN];
   charStarQueue* argv = 0;

   sprintf(updater,"%s/if2up",bindir);
   enqueue(&argv,updater);

   enqueue(&argv,file);
   enqueue(&argv,result);

   if ( suppressWarnings ) enqueue(&argv,"-w");
   if ( profiling ) enqueue(&argv,"-W");

   enqueue(&argv,"-I"); /* Suppress Sequent code improvement migration */

   if ( Submit(&argv) != 0 ) {
      compilerError("if2 update in place failure");
   }

}
