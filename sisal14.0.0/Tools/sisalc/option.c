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

typedef enum { SETDEFAULT=0, CHECKARG=1, DOC=2 } actions;
typedef enum { TEXT = 0, HTML=1, MAN=2 } textType;

static textType helpKind = TEXT;
static char* oneLineDoc = 0;
static char* manDoc = 0;

static void escapedString(char* s) {
   for(;s && *s;++s) {
      switch (helpKind) {
      case MAN:
         switch(*s) {
         case '-': fputs("\\-",stdout); break;
         default: fputc(*s,stdout);
         }
         break;
      case HTML:
         switch(*s) {
         case '<': fputs("&lt;",stdout); break;
         case '>': fputs("&gt;",stdout); break;
         default: fputc(*s,stdout);
         }
         break;
      default: fputc(*s,stdout);
      }
   }
}

static void escapedLine(char* s) {
   escapedString(s);
   fputc('\n',stdout);
}

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
static int booleanFlag(int action,char* program,char*** argP, option_t* option,int normalSense) {
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
   case DOC:
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
int defaultTrue(int action,char* program,char*** argP, option_t* option) {
   return booleanFlag(action,program,argP,option,1);
}

/**************************************************************************/
/* GLOBAL **************        defaultFalse       ************************/
/**************************************************************************/
/* For default False flags                                                */
/**************************************************************************/
int defaultFalse(int action,char* program,char*** argP, option_t* option) {
   return booleanFlag(action,program,argP,option,0);
}

/**************************************************************************/
/* GLOBAL **************     defaultInitialized    ************************/
/**************************************************************************/
/* For non-defaulted integer flags (using atoi())                         */

/**************************************************************************/
int defaultInitialized(int action,char* program,char*** argP, option_t* option) {
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
   case DOC:
      escapedString(" <number>");
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
int fetchStringEqual(int action,char* program,char*** argP, option_t* option) {
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
   case DOC:
      escapedString("=<string>");
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
int fetchStringNext(int action,char* program,char*** argP, option_t* option) {
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
   case DOC:
      escapedString(" <string>");
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
int appendQueue(int action,char* program,char*** argP, option_t* option) {
   char* arg = (argP)?(**argP):0;
   char* equal = 0;
   int sense = 0;

   switch (action) {
   case SETDEFAULT:
      break;
   case CHECKARG:
      /* Check if it is the form -e=xxxx */
      equal = matchEqual(arg,option);
      if ( equal ) {
         enqueue(option->queue,equal);
         (*argP)++;
         return 1;
      }

      /* Check if it is the form -e xxxx */
      else if ( matches(arg,option,&sense) ) {
        (*argP)++;
        if (*argP) {
          enqueue(option->queue,**argP);
          (*argP)++;
          return 1;
        }
      }
      break;
   case DOC:
      escapedString("=<string>");
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
int suffixedFile(int action,char* program,char*** argP, option_t* option) {
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
   case DOC:
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
int prefixedOption(int action,char* program,char*** argP, option_t* option) {
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
   case DOC:
      escapedString("<string>");
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
int catchAll(int action,char* program,char*** argP, option_t* option) {
   char* arg = (argP)?(**argP):0;
   switch (action) {
   case SETDEFAULT:
      break;
   case CHECKARG:
      enqueue(option->queue,arg);
      (*argP)++;
      return 1;
   case DOC:
      break;
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
      options->matcher(SETDEFAULT,0,0,options);
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
void optionScan(char* program, int argc, char** argv, option_t* options, void (*handler)(char*) ) {
   int i;
   option_t* opt;
   char** last = argv+argc;
   while(argv < last) {
      for(opt=options; opt->name; ++opt) {
         if ( opt->matcher(CHECKARG,program,&argv, opt) ) goto Found;
      }
      if (handler) handler(argv[i]);
   Found:
      ;
   }
}


static void header(char* program, char* doc) {
   switch (helpKind) {
   case TEXT:
      printf("%s -- %s\n",program,doc);
      break;
   case HTML:
      printf("<HEAD>\n");
      printf("<HTML>\n");
      printf("<HEAD>\n");
      printf("<TITLE>");
      escapedString(program);
      escapedString(" -- ");
      escapedString(doc);
      printf("</TITLE>\n");
      printf("</HEAD>\n");
      printf("<BODY>\n");
      printf("<H1>");
      escapedString(program);
      escapedString(" -- ");
      escapedString(doc);
      printf("</H1>\n");
      break;
   case MAN:
      printf(".TH %s 1\n",program);
      printf(".SH NAME\n");
      escapedString(program);
      escapedString(" -- ");
      escapedLine(doc);
      break;
   default:
      exit(1);
   }
}

static void synopsis(char* program) {
   switch (helpKind) {
   case TEXT:
      printf("usage: %s ",program);
      break;
   case HTML:
      break;
   case MAN:
      printf(".SH SYNOPSIS\n");
      printf(".B ");
      escapedLine(program);
      break;
   default:
      exit(2);
   }
}

static void synopsisOf(option_t* option) {
   switch (helpKind) {
   case TEXT:
   case MAN:
      printf("[ %s",option->name);
      option->matcher(DOC,0,0,option);
      fputs(" ]",stdout);
      break;
   case HTML:
      break;
   default:
      exit(3);
   }
}

static void commandLineOptions(char* program) {
   switch (helpKind) {
   case TEXT:
      fputc('\n',stdout);
      break;
   case HTML:
      printf("<TABLE BORDER NOSAVE>\n");
      break;
   case MAN:
      fputc('\n',stdout);
      printf(".SH DESCRIPTION\n");
      escapedLine(manDoc);
      fputc('\n',stdout);
      fputc('\n',stdout);
      printf("Options:\n\n");
      break;
   default:
      exit(4);
   }
}

static void commandLineOf(option_t* option) {
   char buf[1024];
   switch (helpKind) {
   case TEXT:
      if ( option->alternate ) {
         sprintf(buf,"%s,%s",option->name,option->alternate);
      } else {
         sprintf(buf,"%s",option->name);
      }
      while ( strlen(buf) < 20) strcat(buf," ");
      printf("  %s %s\n",buf,option->oneLineDoc);
      break;
   case HTML:
      printf("<TR><TD>");
      escapedString(option->name);
      option->matcher(DOC,0,0,option);
      if (option->alternate) {
         printf("<BR>");
         escapedString(option->alternate);
         option->matcher(DOC,0,0,option);
      }
      printf("</TD><TD>");
      escapedString(option->manDoc);
      printf("</TD></TR>\n");
      break;
   case MAN:
      printf(".TP 13\n");
      printf(".B ");
      escapedString(option->name);
      option->matcher(DOC,0,0,option);
      if (option->alternate) {
         fputc(' ',stdout);
         escapedString(option->alternate);
         option->matcher(DOC,0,0,option);
      }
      fputc('\n',stdout);
      fputc('\n',stdout);
      escapedLine(option->manDoc);
      fputc('\n',stdout);
      break;
      
   default:
      exit(5);
   }
}

static void description(char* doc) {
   switch (helpKind) {
   case TEXT:
      break;
   case HTML:
      break;
   case MAN:
      break;
   default:
      exit(6);
   }
}


static void body(option_t* opt) {
   switch (helpKind) {
   default:
      opt->matcher(DOC,0,0,opt);
   }
}

static void trailer() {
   switch (helpKind) {
   case HTML:
      printf("</TABLE>\n");
      printf("</BODY>\n");
      break;
   default:
      ;
   }
}

static int helpText(int action,char* program,char*** argP, option_t* option,textType kind) {
   option_t* opt;
   char* arg = (argP)?(**argP):0;

   switch (action) {
   case SETDEFAULT:
      break;
   case CHECKARG:
      if ( strcmp(arg,option->name) == 0 ||
           ( option->alternate && strcmp(arg,option->alternate) == 0 )
           ) {
         helpKind = kind;
         header(program,oneLineDoc);

         synopsis(program);
         for(opt=option; opt->name; ++opt) {
            synopsisOf(opt);
         }
         description(manDoc);

         commandLineOptions(program);
         for(opt=option; opt->name; ++opt) {
            commandLineOf(opt);
         }
      
         trailer();
         exit(0);
      }
      break;
   case DOC:
      break;
   default:
      exit(1);
   }
   return 0;
}

int overviewOption(int action,char* program,char*** argP, option_t* option) {
   switch (action) {
   case SETDEFAULT:
      oneLineDoc = option->oneLineDoc;
      manDoc = option->manDoc;
      break;
   case CHECKARG:
      break;
   default:
      exit(1);
   }
   return 0;

}
int optionHelp(int action,char* program,char*** argP, option_t* option) {
   return helpText(action,program,argP,option,TEXT);
}
int optionHTML(int action,char* program,char*** argP, option_t* option) {
   return helpText(action,program,argP,option,HTML);
}
int optionMAN(int action,char* program,char*** argP, option_t* option) {
   return helpText(action,program,argP,option,MAN);
}
