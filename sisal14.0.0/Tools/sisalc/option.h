#ifndef OPTION_H
#define OPTION_H

/**************************************************************************/
/* FILE   **************          option.h         ************************/
/**************************************************************************/
/* Author: Patrick Miller December 31 2000                                */
/* Copyright (C) 2000 Patrick Miller                                      */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

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

extern int defaultTrue PROTO((int,char***, option_t*));
extern int defaultFalse PROTO((int,char***, option_t*));
extern int defaultInitialized PROTO((int,char***, option_t*));
extern int suffixedFile PROTO((int,char***, option_t*));
extern int fetchStringEqual PROTO((int,char***, option_t*));
extern int fetchStringNext PROTO((int,char***, option_t*));
extern int appendQueue PROTO((int,char***, option_t*));
extern int prefixedOption PROTO((int,char***, option_t*));
extern int catchAll PROTO((int,char***, option_t*));

extern void setOptionDefaults PROTO((option_t* options));

extern void exitIfNotFound PROTO((char* bad));
extern void optionScan PROTO((int argc, char** argv, option_t* options, void (*handler)(char*)));

extern void optionUsage PROTO((FILE* outf));
#endif
