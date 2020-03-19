#include "world.h"


/**************************************************************************/
/* GLOBAL **************      FindLargestNode      ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE NODE IN THE NODE LIST BEGINNING WITH NODE n; that  */
/*          has the largest label.                                        */
/**************************************************************************/

PNODE FindLargestNode( n )
register PNODE n;
{
  register PNODE m;

  if ( n == NULL )
    return( NULL );

  m = n;
  for (n = n->nsucc; n; n = n->nsucc)
    {
      if (n->label > m->label)
	m = n;
    }

  return( m );
}
