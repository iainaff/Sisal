/**************************************************************************/
/* FILE   **************          if1ld.c          ************************/
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

/* if1ld -o foo.mono -FUR foo.if1 goo.if1 */

void if1ld(charStarQueue** if1Files,
           char* mono,
           char* bindir,
           int warning,
           int profile,

           int forFortran,
           int forC,
	   int separateCompilation,

           charStarQueue** entries,
           charStarQueue** entriesForC,
           charStarQueue** entriesForFORTRAN,
           charStarQueue** reductionFunctions,


           char* QStamps
           ) {
   char loader[MAXPATHLEN];
   charStarQueue* argv = 0;
   char* entry = 0;

   sprintf(loader,"%s/if1ld",bindir);
   enqueue(&argv,loader);
   enqueue(&argv,"-o");
   enqueue(&argv,mono);

   if ( forFortran ) {
      enqueue(&argv,"-F");
   } else if ( forC ) {
      enqueue(&argv,"-C");
   } else {
      /* enqueue(&argv,"-S"); */
   }
   if ( !warning ) enqueue(&argv,"-w");
   if ( profile ) enqueue(&argv,"-W");
   if ( forFortran ) enqueue(&argv,"-F");
   if ( forC ) enqueue(&argv,"-C");
   if ( separateCompilation ) enqueue(&argv,"-S");
   while(entries && *entries) {
      entry = dequeue(entries);
      enqueue(&argv,"-e");
      enqueue(&argv,entry);
   }
   while(entriesForC && *entriesForC) {
      entry = dequeue(entriesForC);
      enqueue(&argv,"-c");
      enqueue(&argv,entry);
   }
   while(entriesForFORTRAN && *entriesForFORTRAN) {
      entry = dequeue(entriesForC);
      enqueue(&argv,"-f");
      enqueue(&argv,entry);
   }

   while(reductionFunctions && *reductionFunctions) {
      entry = dequeue(reductionFunctions);
      enqueue(&argv,"-r");
      enqueue(&argv,entry);
   }

   enqueue(&argv,QStamps);

   while(if1Files && *if1Files) {
      entry = dequeue(if1Files);
      enqueue(&argv,entry);
   }

   if ( Submit(&argv) != 0 ) {
      compilerError("if1 loader failure");
   }
}
