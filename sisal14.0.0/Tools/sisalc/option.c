/**************************************************************************/
/* FILE   **************          option.c         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "sisalInfo.h"
#include "option.h"

typedef enum { SETDEFAULT=0, CHECKARG=1 } actions;

/**************************************************************************/
/* LOCAL  **************          matches          ************************/
/**************************************************************************/
/* See if an argument matches the option name or alternate.  Allow -no to */
/* reverse the sense of an argument if present                            */
/**************************************************************************/
static int matches(char* arg, option_t* option, int* senseP) {
   char* base = arg;
   char* name = option->name;
   char* alternate = option->alternate;
   int baseDash, nameDash, alternateDash;

   /* ----------------------------------------------- */
   /* Look for an immediate match                     */
   /* ----------------------------------------------- */
   if ( strcmp(base,name) == 0 ) return 1;
   if ( alternate && strcmp(base,alternate) == 0 ) return 1;

   /* ----------------------------------------------- */
   /* Strip leading -/-- off of name, base, alternate */
   /* ----------------------------------------------- */
   for(baseDash=0; base && *base == '-'; ++baseDash, ++base);
   for(nameDash=0; name && *name == '-'; ++nameDash, ++name);
   for(alternateDash=0; alternate && *alternate == '-'; ++alternateDash, ++alternate);
   
   
   /* ----------------------------------------------- */
   /* See if this is a multi-sense form (-<no->xxx)   */
   /* ----------------------------------------------- */
   if ( nameDash == baseDash ) {
      if ( strcmp(name,base) == 0 ) return 1;
      if ( strncmp(name,"<no->",5) == 0 ) {
         if ( strcmp(name+5,base) == 0 ) return 1;
         if ( strncmp(base,"no-",3) == 0 ) {
            *senseP = !*senseP;
            return 1;
         }
      }
   }

   /* ----------------------------------------------- */
   /* Same for the alternate name                     */
   /* ----------------------------------------------- */
   if ( alternate && alternateDash == baseDash ) {
      if ( strcmp(alternate,base) == 0 ) return 1;
      if ( strncmp(alternate,"<no->",5) == 0 ) {
         if ( strcmp(alternate+5,base) == 0 ) return 1;
         if ( strncmp(base,"no-",3) == 0 ) {
            *senseP = !*senseP;
            return 1;
         }
      }
   }

   return 0;
}

/**************************************************************************/
/* LOCAL  **************         matchEqual        ************************/
/**************************************************************************/
/* Return string if the name or alternate matches the argument            */
/**************************************************************************/
static char* matchEqual(char* arg, option_t* option) {
   char* equal = index(arg,'=');
   if ( !equal ) return 0; /* No = in argument */
   if ( strncmp(option->name,arg,equal-arg) == 0 ) return equal+1;
   if ( option->alternate && strncmp(option->alternate,arg,equal-arg) == 0 ) return equal+1;
   return 0;
}

/**************************************************************************/
/* LOCAL  **************        booleanFlag        ************************/
/**************************************************************************/
/* Set or reset a boolean flag.  An extra input is the correct default    */
/* sense the operation (0 for false, 1 for true)                          */
/**************************************************************************/
static int booleanFlag(int action,char*** argP, option_t* option,int normalSense) {
   char* arg = (argP)?(**argP):0;
   int sense = normalSense;
   switch (action) {
   case SETDEFAULT:
      *(option->param0) = normalSense;
      break;
   case CHECKARG:
      if ( matches(arg,option,&sense) ) {
         *(option->param0) = !sense;
         (*argP)++;
         return 1;
      }
      break;
   default:
      exit(1);
   }
   return 0;
}

/**************************************************************************/
/* GLOBAL **************        defaultTrue        ************************/
/**************************************************************************/
/* For default True flags                                                 */
/**************************************************************************/
int defaultTrue(int action,char*** argP, option_t* option) {
   return booleanFlag(action,argP,option,1);
}

/**************************************************************************/
/* GLOBAL **************        defaultFalse       ************************/
/**************************************************************************/
/* For default False flags                                                */
/**************************************************************************/
int defaultFalse(int action,char*** argP, option_t* option) {
   return booleanFlag(action,argP,option,0);
}

/**************************************************************************/
/* GLOBAL **************     defaultInitialized    ************************/
/**************************************************************************/
/* For non-defaulted integer flags (using atoi())                         */

/**************************************************************************/
int defaultInitialized(int action,char*** argP, option_t* option) {
   char* arg = (argP)?(**argP):0;
   char* equal = 0;
   switch (action) {
   case SETDEFAULT:
      break;
   case CHECKARG:
      equal = matchEqual(arg,option);
      if ( equal ) {
         *(option->param0) = atoi(equal);
         (*argP)++;
         return 1;
      }
      break;
   default:
      exit(1);
   }
   return 0;
}

/**************************************************************************/
/* GLOBAL **************      fetchStringEqual     ************************/
/**************************************************************************/
/* For flags in the form -foo=xxx where we only want one value            */
/**************************************************************************/
int fetchStringEqual(int action,char*** argP, option_t* option) {
   char* arg = (argP)?(**argP):0;
   char* equal = 0;
   switch (action) {
   case SETDEFAULT:
      break;
   case CHECKARG:
      equal = matchEqual(arg,option);
      if ( equal ) {
         *(option->param2) = equal;
         (*argP)++;
         return 1;
      }
      break;
   default:
      exit(1);
   }
   return 0;
}

/**************************************************************************/
/* GLOBAL **************      fetchStringNext      ************************/
/**************************************************************************/
/* For flags of the form -xxx zooper                                      */
/**************************************************************************/
int fetchStringNext(int action,char*** argP, option_t* option) {
   char* arg = (argP)?(**argP):0;
   switch (action) {
   case SETDEFAULT:
      break;
   case CHECKARG:
      if ( !(*argP)[1] ) break;
      if ( strcmp(arg,option->name) == 0 ||
           ( option->alternate && strcmp(arg,option->alternate) == 0)
           ) {
         *(option->param2) = (*argP)[1];
         (*argP) += 2;
         return 1;
      }
      break;
   default:
      exit(1);
   }
   return 0;
}

/**************************************************************************/
/* GLOBAL **************        appendQueue        ************************/
/**************************************************************************/
/* For flags in the form -foo=xxx where we only want all values           */
/**************************************************************************/
int appendQueue(int action,char*** argP, option_t* option) {
   char* arg = (argP)?(**argP):0;
   char* equal = 0;
   switch (action) {
   case SETDEFAULT:
      break;
   case CHECKARG:
      equal = matchEqual(arg,option);
      if ( equal ) {
         enqueue(option->queue,equal);
         (*argP)++;
         return 1;
      }
      break;
   default:
      exit(1);
   }
   return 0;
}

/**************************************************************************/
/* GLOBAL **************        suffixedFile       ************************/
/**************************************************************************/
/* For matching particular file suffixes (e.g. *.c)                       */
/**************************************************************************/
int suffixedFile(int action,char*** argP, option_t* option) {
   char* arg = (argP)?(**argP):0;
   char* suffix = 0;
   char* argSuffix = 0;
   switch (action) {
   case SETDEFAULT:
      break;
   case CHECKARG:
      suffix = option->name+1; /* Skip the * */
      argSuffix = arg+strlen(arg)-strlen(suffix);
      if ( argSuffix < arg ) return 0; /* Too short to match */
      if ( strcmp(suffix,argSuffix) == 0 ) {
         enqueue(option->queue,arg);
         (*argP)++;
         return 1;
      }
      break;
   default:
      exit(1);
   }
   return 0;
}

/**************************************************************************/
/* GLOBAL **************       prefixedOption      ************************/
/**************************************************************************/
/* For matching when only the first few letters must match (e.g. -Ifoo)   */
/**************************************************************************/
int prefixedOption(int action,char*** argP, option_t* option) {
   char* arg = (argP)?(**argP):0;
   int prefixLen = 0;
   switch (action) {
   case SETDEFAULT:
      break;
   case CHECKARG:
      prefixLen = strlen(option->name)-1;
      if ( strncmp(option->name,arg,prefixLen) == 0 ) {
         enqueue(option->queue,arg);
         (*argP)++;
         return 1;
      }
      break;
   default:
      exit(1);
   }
   return 0;
}

/**************************************************************************/
/* GLOBAL **************          catchAll         ************************/
/**************************************************************************/
/* Just match anything                                                    */
/**************************************************************************/
int catchAll(int action,char*** argP, option_t* option) {
   char* arg = (argP)?(**argP):0;
   switch (action) {
   case SETDEFAULT:
      break;
   case CHECKARG:
      enqueue(option->queue,arg);
      (*argP)++;
      return 1;
   default:
      exit(1);
   }
   return 0;
}

/**************************************************************************/
/* GLOBAL **************     setOptionDefaults     ************************/
/**************************************************************************/
/* Runs through option list and applies defaults as needed                */
/**************************************************************************/
void setOptionDefaults(option_t* options) {
   /* ----------------------------------------------- */
   /* Holy OO programming in C... Invoke matcher      */
   /* member on the option object                     */
   /* ----------------------------------------------- */
   for(; options->name; ++options) {
      options->matcher(SETDEFAULT,0,options);
   }
}

/**************************************************************************/
/* GLOBAL **************       exitIfNotFound      ************************/
/**************************************************************************/
/* Can be used to force optionScan() to bomb out                          */
/**************************************************************************/
void exitIfNotFound(char* bad) {
   fprintf(stderr,"Unknown option: %s\n",bad);
   exit(1);
}

/**************************************************************************/
/* GLOBAL **************         optionScan        ************************/
/**************************************************************************/
/* Scan through the argv list and look for options.  If not found, apply  */
/* a handler function (if provided).                                      */
/**************************************************************************/
void optionScan(int argc, char** argv, option_t* options, void (*handler)(char*) ) {
   int i;
   option_t* opt;
   char** last = argv+argc;
   while(argv < last) {
      for(opt=options; opt->name; ++opt) {
         if ( opt->matcher(CHECKARG,&argv, opt) ) goto Found;
      }
      if (handler) handler(argv[i]);
   Found:
      ;
   }
}

void optionUsage(FILE* out) {
   fprintf(out,"Not done\n");
}
