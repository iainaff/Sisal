#include "sisalc.h"

/* if2part s.costs foo.up foo.part - - -SR -P1 */

void if2part(char* file,
             char* result,
             char* bindir,
             char* datadir,
             int suppressWarnings, /* suppress warnings? */
             int profiling, /* profiling */

             int associativeReductions,
             int parallelismLoopNesting,
             int vectors,
             int processors,
             int crayStyleVectors,
             int minimumParallelLoopCost,
             int assumedIterationCount
             ) {
   char partitioner[MAXPATHLEN];
   char costs[MAXPATHLEN];
   charStarQueue* argv = 0;

   sprintf(partitioner,"%s/if2part",bindir);
   enqueue(&argv,partitioner);
   sprintf(costs,"%s/s.costs",datadir);
   enqueue(&argv,costs);

   enqueue(&argv,file);
   enqueue(&argv,result);

   enqueue(&argv,"-"); /* Loop report in */
   enqueue(&argv,"-"); /* Loop report out */

   if ( suppressWarnings ) enqueue(&argv,"-w");
   if ( profiling ) enqueue(&argv,"-W");

   if ( Submit(&argv) != 0 ) {
      compilerError("if2 partitioner failure");
   }

}
