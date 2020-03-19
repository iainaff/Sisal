#include "world.h"

/**************************************************************************/
/*                                                                        */
/* NAME                                                                   */
/*    NewType                                                             */
/*                                                                        */
/* DESCRIPTION                                                            */
/*    Creates a new PINFO structure, initializes, and returns a pointer   */
/*    to it.                                                              */
/*                                                                        */
/* PARAMETERS                                                             */
/*    type         Special type code as defined in IFX.h (i.e.:IF_ARRAY)  */
/*    ref1,ref2    Two integers that represent different things dependin  */
/*                 on the type code (see file:doc section Making Edges).  */
/*    name         char * to set the name pragma.                         */
/*                                                                        */
/* RETURNS                                                                */
/*    itail        itail is a pointer to the last PINFO structure in a    */
/*                 list which is also the new PINFO structure constructe  */
/*                                                                        */
/**************************************************************************/

PINFO NewType(type,ref1,ref2,name)
int type;
int ref1;
int ref2;
char *name;
{
  pragmas.name = name;
  MakeInfo(++LargestTypeLabelSoFar,type,ref1,ref2);
  StandardPragmas(&pragmas);

  return itail;
}
