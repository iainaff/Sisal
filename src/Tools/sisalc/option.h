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
typedef struct sisalc_option {
   char* name;
   char* alternate;
   int (*matcher)(int,char*,char***,struct sisalc_option*);
   char* oneLineDoc;
   char* manDoc;
   int* param0;
   int* param1;
   char** param2;
   charStarQueue** queue;
} option_t;

extern int defaultTrue PROTO((int,char*,char***, option_t*));
extern int defaultFalse PROTO((int,char*,char***, option_t*));
extern int defaultInitialized PROTO((int,char*,char***, option_t*));
extern int suffixedFile PROTO((int,char*,char***, option_t*));
extern int fetchStringEqual PROTO((int,char*,char***, option_t*));
extern int fetchStringNext PROTO((int,char*,char***, option_t*));
extern int appendQueue PROTO((int,char*,char***, option_t*));
extern int prefixedOption PROTO((int,char*,char***, option_t*));
extern int catchAll PROTO((int,char*,char***, option_t*));

extern int optionHelp PROTO((int,char*,char***, option_t*));
extern int optionHTML PROTO((int,char*,char***, option_t*));
extern int optionMAN PROTO((int,char*,char***, option_t*));

extern int overviewOption PROTO((int action,char*,char*** argP, option_t* option));
extern void setOptionDefaults PROTO((option_t* options));

extern void exitIfNotFound PROTO((char* bad));
extern void optionScan PROTO((char* program, int argc, char** argv, option_t* options, void (*handler)(char*)));

#endif
