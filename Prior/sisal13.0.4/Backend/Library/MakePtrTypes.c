#include "world.h"


/**************************************************************************/
/* GLOBAL **************       MakePtrTypes        ************************/
/**************************************************************************/
/* PURPOSE: MAKE POINTER SYMBOL TABLE ENTRIES.                            */
/**************************************************************************/

void MakePtrTypes()
{
  ptr_real = FindInfo( ++maxint, IF_PTR_REAL );
  ptr_real->tname = "float*";

  ptr_double = FindInfo( ++maxint, IF_PTR_DOUBLE );
  ptr_double->tname = "double*";

  ptr_integer = FindInfo( ++maxint, IF_PTR_INTEGER );
  ptr_integer->tname = "int*";

  ptr = FindInfo( ++maxint, IF_PTR );
  ptr->tname = "POINTER";
}

/* $Log$
 * Revision 1.1  1993/01/21  23:29:45  miller
 * Initial version of the IFX library.  It replaces the if[12]build.c
 * read.c timer.c util.c and write.c and if[12].h files from the
 * backend phases.
 * */
