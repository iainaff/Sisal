/**************************************************************************/
/* FILE   **************         compile.c         ************************/
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

void compile(char* file,
             char* result,
             char* includedir,
	     int profiling,
             char* compiler,
             char* flags
             ) {

   char dashI[MAXPATHLEN];
   charStarQueue* argv = 0;

   explodeEnqueue(&argv,compiler);
   enqueue(&argv,file);
   enqueue(&argv,"-c");
   enqueue(&argv,"-o");
   if ( profiling ) enqueue(&argv,"-pg");
   enqueue(&argv,result);
   sprintf(dashI,"-I%s",includedir);
   enqueue(&argv,dashI);
   explodeEnqueue(&argv,flags);

   if ( Submit(&argv) != 0 ) {
      compilerError("C compiler failure");
   }

}
