#include <stdio.h>
#include "sisalrt.h"

#undef _READ
void         _READ();
int RecompileTheModuleDefining_READ = 0;
#undef _PIPE
void         _PIPE();
int RecompileTheModuleDefining_PIPE = 0;
#undef _STDIN
void         _STDIN();
int RecompileTheModuleDefining_STDIN = 0;
#undef _ARGV
void         _ARGV();
int RecompileTheModuleDefining_ARGV = 0;

struct Args12 {   
struct ActRec *FirstAR; int Count;   
POINTER In1;    POINTER Out1;   
  };

struct Args13 {   
struct ActRec *FirstAR; int Count;   
POINTER Out1;   
  };

struct Args14 {   
struct ActRec *FirstAR; int Count;   
  POINTER In1;    POINTER Out1;   int Out2;
  };

extern char** sisal_save_argv;
void _ARGV( void* args )
{
  POINTER val;
  POINTER arg;
  int i;
  int j;
  char** p;
  char* q;

  ABld(val,1,1); /* Empty array */
  ((struct Args13*)args)->Out1 = val;

  for(p=sisal_save_argv;p && *p;++p) {
    ABld(arg,1,1);
    for(q=*p; q && *q; ++q) {
      AGather(arg,*q,char);
    }
    AGather(val,arg,POINTER);
  }
}

void _READ( void* args )
{
  FILE *fp = 0;
  char buf[4096];
  POINTER val;
  ARRAYP filename = (ARRAYP)(((struct Args12*)args)->In1);
  int i;
  int c;
  int size;

  size = filename->Size;
  if ( size > 4095 ) size = 4095;

  strncpy(buf,(char*)(filename->Base+filename->LoBound),size);
  buf[size] = 0;

  fp = fopen(buf,"r");
  if ( !fp ) { 
    fprintf(stderr,"File error: %s\n",buf);
    perror(buf);
    exit(1);
  }

  ABld(val,1,1);

  while( (c=fgetc(fp)) != EOF ) {
    AGather(val,c,char);
  }
  fclose(fp);
  
  ((struct Args12*)args)->Out1 = val;
}

void _PIPE( void* args )
{
  FILE *fp = 0;
  char buf[4096];
  POINTER val;
  ARRAYP filename = (ARRAYP)(((struct Args14*)args)->In1);
  int i;
  int c;
  int size;
  int status;

  size = filename->Size;
  if ( size > 4095 ) size = 4095;

  strncpy(buf,(char*)(filename->Base+filename->LoBound),size);
  buf[size] = 0;

  fp = popen(buf,"r");
  if ( !fp ) { 
    fprintf(stderr,"Pipe error: %s\n",buf);
    perror(buf);
    exit(1);
  }

  ABld(val,1,1);

  while( (c=fgetc(fp)) != EOF ) {
    AGather(val,c,char);
  }
  status = pclose(fp);
  
  ((struct Args14*)args)->Out1 = val;
  ((struct Args14*)args)->Out2 = status;
}


void _STDIN( void* args )
{
  POINTER val;
  int c;

  ABld(val,1,1);

  while( (c=fgetc(stdin)) != EOF ) {
    AGather(val,c,char);
  }
  
  ((struct Args13*)args)->Out1 = val;
}
