/**************************************************************************/
/* FILE   **************          linker.c         ************************/
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

void linker(char* loader,
            char* target,
            char* libdir,
            charStarQueue** objectFiles,
            charStarQueue** loadOptions,
            char* LDFLAGS,
            char* linkWithF77,
            int forFORTRAN,
            int forC
            ) {
   charStarQueue* argv = 0;
   char* entry = 0;
   char srt0[MAXPATHLEN];
   char dashL[MAXPATHLEN];
   
   explodeEnqueue(&argv,loader);
   enqueue(&argv,"-o");
   enqueue(&argv,target);

   if ( ! ( forFORTRAN || forC ) ) {
      sprintf(srt0,"%s/srt0.o",libdir);
      enqueue(&argv,srt0);
   }

   while(objectFiles && *objectFiles) {
      entry = dequeue(objectFiles);
      enqueue(&argv,entry);
   }
   while(loadOptions && *loadOptions) {
      entry = dequeue(loadOptions);
      enqueue(&argv,entry);
   }

   sprintf(dashL,"-L%s",libdir);
   enqueue(&argv,dashL);
   
   enqueue(&argv,"-lsisal");
   
   explodeEnqueue(&argv,LDFLAGS);
   explodeEnqueue(&argv,linkWithF77);

   if ( Submit(&argv) != 0 ) {
      compilerError("linker failure");
   }
}
