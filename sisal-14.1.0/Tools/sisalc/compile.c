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
	     int gprof,
             char* compiler,
             char* flags
             ) {

   char dashI[MAXPATHLEN];
   charStarQueue* argv = 0;

   explodeEnqueue(&argv,compiler);
   enqueue(&argv,file);
   enqueue(&argv,"-c");
   enqueue(&argv,"-o");
   enqueue(&argv,result);
   if ( gprof ) enqueue(&argv,"-pg");
   sprintf(dashI,"-I%s",includedir);
   enqueue(&argv,dashI);
   explodeEnqueue(&argv,flags);

   if ( Submit(&argv) != 0 ) {
      compilerError("C compiler failure");
   }

}
