#include "sisalc.h"

void sisal(char* sis,
           char* if1,
           char* bindir,
           char* datadir) {
   char frontend[MAXPATHLEN];
   char fileArg[MAXPATHLEN];
   charStarQueue* argv = 0;

   sprintf(frontend,"%s/sisal",bindir);
   enqueue(&argv,frontend);
   enqueue(&argv,"-dir");
   enqueue(&argv,datadir);
   enqueue(&argv,sis);
   enqueue(&argv,"-O");
   enqueue(&argv,if1);
   sprintf(fileArg,"-F%s",sis);
   enqueue(&argv,fileArg);

   if ( Submit(&argv) != 0 ) {
      compilerError("frontend failure");
   }
}
