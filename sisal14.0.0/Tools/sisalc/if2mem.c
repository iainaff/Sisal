#include "sisalc.h"

/* if2mem foo.opt foo.mem */

void if2mem(char* file,
                   char* result,
                   char* bindir,
                   int suppressWarnings, /* suppress warnings? */
                   int profiling, /* profiling */
                   int optimize, /* Perform aggressive optimization */
                   int optimizeInlined /* optimize inlined calls */
            ) {
   char manager[MAXPATHLEN];
   charStarQueue* argv = 0;

   sprintf(manager,"%s/if2mem",bindir);
   enqueue(&argv,manager);

   enqueue(&argv,file);
   enqueue(&argv,result);

   if ( !optimize ) {
      enqueue(&argv,"-v"); /* no invar */
      enqueue(&argv,"-V"); /* no Oinvar */
      enqueue(&argv,"-c"); /* no cse */
      enqueue(&argv,"-g"); /* no gcse */
      enqueue(&argv,"-f"); /* no fold */
   }

   if ( suppressWarnings ) enqueue(&argv,"-w");
   if ( profiling ) enqueue(&argv,"-W");

   if ( Submit(&argv) != 0 ) {
      compilerError("if2 memory manager failure");
   }

}
