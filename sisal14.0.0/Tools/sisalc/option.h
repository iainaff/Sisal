#ifndef _OPTION_H
#define _OPTION_H

#include "charStarQueue.h"

/* ----------------------------------------------- */
/* Option descriptions                             */
/* ----------------------------------------------- */
typedef struct option {
   char* name;
   char* alternate;
   int (*matcher)(int,char***,struct option*);
   char* oneLineDoc;
   char* manDoc;
   int* param0;
   int* param1;
   char** param2;
   charStarQueue** queue;
} option_t;

extern int defaultTrue(int,char***, option_t*);
extern int defaultFalse(int,char***, option_t*);
extern int defaultInitialized(int,char***, option_t*);
extern int suffixedFile(int,char***, option_t*);
extern int fetchStringEqual(int,char***, option_t*);
extern int fetchStringNext(int,char***, option_t*);
extern int appendQueue(int,char***, option_t*);
extern int prefixedOption(int,char***, option_t*);
extern int catchAll(int,char***, option_t*);

extern void setOptionDefaults(option_t* options);

extern void exitIfNotFound(char* bad);
extern void optionScan(int argc, char** argv, option_t* options, void (*handler)(char*));

extern void optionUsage(FILE* outf);
#endif
