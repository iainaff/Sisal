#include "sisalc.h"

/**************************************************************************/
/* GLOBAL **************           if1opt          ************************/
/**************************************************************************/
/* Normal case: if1opt foo.mono foo.opt -R -M -1 -l -e -U2                */
/**************************************************************************/
void if1opt(char* file,
            char* result,
            char* bindir,
            int suppressWarnings, /* suppress warnings? */
            int profiling, /* profiling */
            charStarQueue** inlineProtectedFunctions, /* functions NOT to inline */
            int eliminateDeadCode, /* dead code elimination */
            int optimizeInlined, /* optimize inlined calls */
            int associativeReductions, /* do non-associative reductions for speed? */
            int aggressiveCSE, /* Agressively move operations out of loops */
            int aggressiveVectors, /* Aggressively detect vectors */
            int nodeStripping, /* strip out nodes */
            int concurrent, /* Is target parallel? */
            int inlining, /* Inline function calls? */
            int normalizedArrayIndexing, /* normalized array indexing */
            int crayStyleVectors, /* Use Cray styled vectors? */
            int FORTRANIntrinsicBitFunctions,
            int optimize /* Perform aggressive optimization */
            ) {
   char optimizer[MAXPATHLEN];
   charStarQueue* argv = 0;
   char* entry = 0;

   sprintf(optimizer,"%s/if1opt",bindir);
   enqueue(&argv,optimizer);

   enqueue(&argv,file);
   enqueue(&argv,result);

   /* ----------------------------------------------- */
   /* Protect certain functions from inlining         */
   /* ----------------------------------------------- */
   while(inlineProtectedFunctions && *inlineProtectedFunctions) {
      entry = dequeue(inlineProtectedFunctions);
      enqueue(&argv,"-#");
      enqueue(&argv,entry);
   }

   if ( suppressWarnings ) enqueue(&argv,"-w");
   if ( profiling ) enqueue(&argv,"-W");
   if ( !eliminateDeadCode ) enqueue(&argv,"-d");
   if ( optimizeInlined ) enqueue(&argv,"-8");
   if ( !associativeReductions ) enqueue(&argv,"-Y");
   if ( aggressiveCSE ) enqueue(&argv,"-G");
   if ( aggressiveVectors ) enqueue(&argv,"-AggV");
   if ( !nodeStripping ) enqueue(&argv,"-6");
   if ( concurrent ) enqueue(&argv,"-R");
   if ( !inlining ) enqueue(&argv,"-x");
   if ( crayStyleVectors ) {
      enqueue(&argv,"-X");
      enqueue(&argv,"-C5");
   }
   if ( FORTRANIntrinsicBitFunctions ) enqueue(&argv,"-M");
   if ( !optimize ) {
      enqueue(&argv,"-r"); /* no recf */
      enqueue(&argv,"-v"); /* no invar */
      enqueue(&argv,"-c"); /* no cse */
      enqueue(&argv,"-u"); /* no ifuse */
      enqueue(&argv,"-j"); /* no sfuse */
      enqueue(&argv,"-S"); /* no split */
      enqueue(&argv,"-Z"); /* no invert */
      enqueue(&argv,"-z"); /* no dfuse */
      enqueue(&argv,"-D"); /* no dope */
      enqueue(&argv,"-9"); /* no amove */
      enqueue(&argv,"-g"); /* no gcse */
      enqueue(&argv,"-f"); /* no fold */
      enqueue(&argv,"-a"); /* no scalar */
   }
   enqueue(&argv,"-1"); /* no tgcse */
   enqueue(&argv,"-l"); /* setL removal */
   enqueue(&argv,"-e"); /* no tgcse */
   enqueue(&argv,"-U2"); /* Unrolling */

   if ( Submit(&argv) != 0 ) {
      compilerError("if1 optimizer failure");
   }
}
