#include "sisalc.h"

/* if2gen foo.part foo.c -m -U -G -O -Y3 -B -a */

void if2gen(char* file,
             char* result,
             char* bindir,
             int suppressWarnings, /* suppress warnings? */
             int profiling, /* profiling */

            int sliceThrottle,
            int showSource,
            int bindProcessors,
            int associativeReductions,
            int FORTRANIntrinsicBitFunctions,
            int vector,
            int bounds
             ) {
   char generator[MAXPATHLEN];
   charStarQueue* argv = 0;

   sprintf(generator,"%s/if2gen",bindir);
   enqueue(&argv,generator);

   enqueue(&argv,file);
   enqueue(&argv,result);

   if ( !sliceThrottle ) enqueue(&argv,"-m");

   if ( suppressWarnings ) enqueue(&argv,"-w");
   if ( profiling ) enqueue(&argv,"-W");

   if ( showSource ) enqueue(&argv,"-%");
   if ( FORTRANIntrinsicBitFunctions ) enqueue(&argv,"-U");
   if ( bounds ) enqueue(&argv,"-B");

   enqueue(&argv,"-G"); /* if ( !nogshared ) av[avcnt++] = "-G"; */
   enqueue(&argv,"-O"); /* if ( useORTS ) av[avcnt++] = "-O"; */
   enqueue(&argv,"-a"); /* if ( noaimp ) av[avcnt++] = "-a"; */
   enqueue(&argv,"-Y3"); /* Prebuild */

   if ( Submit(&argv) != 0 ) {
      compilerError("if2 generator failure");
   }

}
