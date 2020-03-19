#include "world.h"

/**************************************************************************/
/* GLOBAL **************     CopyEdgeAndReset      ************************/
/**************************************************************************/
/* PURPOSE: COPY EDGE e AND LINK INTO THE GRAPH, BUT USING src AND dst AS */
/*          THE SOURCE AND DESTINATION NODES.                             */
/**************************************************************************/

void CopyEdgeAndReset( e, src, dst )
PEDGE e;
PNODE src;
PNODE dst;
{
  register PEDGE ee;

  ee = CopyEdge( e, (IsConst(e))? NULL : src, dst );

  if ( !IsConst( e ) )
    LinkExport( src, ee );

  LinkImport( dst, ee );
}

