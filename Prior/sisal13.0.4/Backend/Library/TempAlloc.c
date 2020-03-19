#include "world.h"


/**************************************************************************/
/* GLOBAL **************         TempAlloc         ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE, INITIALIZE, AND RETURN A TEMPORARY                  */
/**************************************************************************/

PTEMP TempAlloc( name, tempinfo, mod, fld )
char  *name;
PINFO  tempinfo;
int    mod;
int    fld;
{
  register PTEMP   t;
  register PBBLOCK b;
  char    buf[100];

  /* t = (PTEMP) MyAlloc( sizeof(TEMP) ); */
  b = MyBBlockAlloc();
  t = &(b->t);

  t->DeAl    = FALSE;
  t->status  = UNKNOWN;
  t->next    = NULL;
  t->info    = tempinfo;
  t->mod     = mod;
  t->fld     = fld;
  t->pop     = 0;
  t->dist    = 0;
  t->fid     = -1;
  t->ccost   = 0.0;

  SPRINTF( buf, "%s%d", name, ++tmpid );
  t->name = CopyString( buf );

  return( t );
}

/* $Log$
 * Revision 1.1  1993/01/21  23:30:29  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
