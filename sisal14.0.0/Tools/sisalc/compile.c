#include "sisalc.h"

void compile(char* file,
             char* result,
             char* includedir,
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
   sprintf(dashI,"-I%s",includedir);
   enqueue(&argv,dashI);
   explodeEnqueue(&argv,flags);

   if ( Submit(&argv) != 0 ) {
      compilerError("C compiler failure");
   }

}
