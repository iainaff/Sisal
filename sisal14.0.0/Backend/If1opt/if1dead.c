/* if1dead.c,v
 * Revision 12.7  1992/11/04  22:04:56  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:32  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

int ikcnt  = 0;          /* COUNT OF COMBINED K IMPORTS            */
int unnec  = 0;          /* COUNT OF UNNECESSARY EDGES OR LITERALS */
int Tunnec  = 0;          /* COUNT OF UNNECESSARY EDGES OR LITERALS */
int unused = 0;          /* COUNT OF UNUSED VALUES                 */
int Tunused = 0;          /* COUNT OF UNUSED VALUES                 */
int dscnt  = 0;          /* COUNT OF DEAD SIMPLE NODES             */
int Tdscnt  = 0;          /* COUNT OF DEAD SIMPLE NODES             */
int dccnt  = 0;          /* COUNT OF DEAD COMPOUND NODES           */
int Tdccnt  = 0;          /* COUNT OF DEAD COMPOUND NODES           */
int agcnt  = 0;          /* COUNT OF DEAD AGather NODES            */
int Tagcnt  = 0;          /* COUNT OF DEAD AGather NODES            */


/**************************************************************************/
/* LOCAL  **************   FastRemoveUnnecEdges    ************************/
/**************************************************************************/
/* PURPOSE: REMOVE UNNECESSARY EDGES AND LITERALS FROM FORALL, LOOPB, AND */
/*          LOOPA SUBGRAPHS.  THE FOLLOWING ARE CONSIDERED UNNECESSARY:   */
/*                                                                        */
/*          1. "T := constant, M, OR K" (FORALL BODY)                     */
/*          2. "L := constant OR K", L NOT REDEFINED IN BODY              */
/*          3. "L := OLD L"                                               */
/*          4. "T := constant, K, OR NOT REDEFINED L (LOOPA BODY)         */
/*          5. "L := constant OR K" and "L := SAME constant OR K"         */
/*                                                                        */
/*          WARNING: IF A CONSTANT OF TYPE t IS PROPAGATED TO A RETURNS   */
/*                   NODE ORIGINALLY EXPECTING A MULTIPLE OF t, IT  WILL  */
/*                   LOSE THE MULTIPLE DATA TYPE---REPLACED BY t ITSELF.  */
/*                   THIS VILOLATES THE DEFINITION OF IF1, BUT LLNL IF1   */
/*                   SOFTWARE APPEARS TO ACCEPT IT. FURTHER, NEGATIVE     */
/*                   ARITHMETIC CONSTANTS ARE NOT PROPAGATED UNLESS sgnok */
/*                   IS ENABLED (FOR DI COMPATABILITY).                   */ 
/**************************************************************************/

static void FastRemoveUnnecEdges( n )
PNODE n;
{
    register PEDGE i;
    register PEDGE si;
    register PEDGE ii;

    ASSERT( IsAnyLoop( n ), "Not a loop");
    switch ( n->type ) {
        case IFForall:
            /* T := constant, M, OR K */

            for ( i = n->F_BODY->imp; i != NULL; i = si ) { 
                si = i->isucc;

                ++Tunnec;

                if ( IsConst( i ) ) {
                    if ( !CanPropagateConst( i ) )
                        continue;

                    ChangeExportsToConst( n->F_RET, i->iport, i );
		    UnlinkImport( i );
		    UnlinkExport( i );
                    unnec++;
                    }
                else if ( IsSGraph( i->src ) ) {
                    ChangeExportPorts( n->F_RET, i->iport, i->eport );
		    UnlinkImport( i );
		    UnlinkExport( i );
                    unnec++;
                    }
                }

            break;

        case IFLoopA:
        case IFLoopB:
            /* LOOP BODY: L := OLD L */

            for ( i = n->L_BODY->imp; i != NULL; i = si ) {
                si  = i->isucc;

                ++Tunnec;

                if ( !IsConst( i ) )
                    if ( IsSGraph( i->src ) )
                        if ( i->iport == i->eport ) {
		            UnlinkImport( i );
		            UnlinkExport( i );
                            unnec++;
                            }
                }

            /* L NOT REDEFINED: L := constant OR K */

            for ( i = n->L_INIT->imp; i != NULL; i = si ) {
                si = i->isucc;


                if ( !IsImport( n->L_BODY, i->iport ) ) {
                ++Tunnec;
                    if ( IsConst( i ) ) {
                        if ( !CanPropagateConst( i ) )
                            continue;

                        ChangeExportsToConst( n->L_TEST, i->iport, i );
                        ChangeExportsToConst( n->L_BODY, i->iport, i );
                        ChangeExportsToConst( n->L_RET,  i->iport, i );

		        UnlinkImport( i );
		        UnlinkExport( i );
                        unnec++;
                        }
                    else if ( IsSGraph( i->src ) ) {
                        ChangeExportPorts( n->L_TEST, i->iport, i->eport );
                        ChangeExportPorts( n->L_BODY, i->iport, i->eport );
                        ChangeExportPorts( n->L_RET,  i->iport, i->eport );

		        UnlinkImport( i );
		        UnlinkExport( i );
                        unnec++;
                        }
                    }
                }

            /* INIT (L := constant OR K) and BODY (L := SAME constant OR K) */

            for ( i = n->L_INIT->imp; i != NULL; i = si ) {
                si = i->isucc;


                if ( (ii = FindImport( n->L_BODY, i->iport ) ) == NULL )
                    continue;
                ++Tunnec;
                ++Tunnec;

                if ( IsConst( i ) ) {
                    if ( !IsConst( ii ) )
                        continue;

                    if ( !AreConstsEqual( i, ii ) )
                        continue;

                    ChangeExportsToConst( n->L_TEST, i->iport, i );
                    ChangeExportsToConst( n->L_BODY, i->iport, i );
                    ChangeExportsToConst( n->L_RET,  i->iport, i );

		    UnlinkImport( i );
		    UnlinkExport( i );
		    UnlinkImport( ii );
		    UnlinkExport( ii );
                    unnec += 2;
                    }
                else if ( IsSGraph( i->src ) ) {
                    if ( IsConst( ii ) ) /* BUG FIX: 8/2/90 */
                        continue;

                    if ( !IsSGraph( ii->src ) )
                        continue;

                    if ( i->eport != ii->eport )
                        continue;

                    ChangeExportPorts( n->L_TEST, i->iport, i->eport );
                    ChangeExportPorts( n->L_BODY, i->iport, i->eport );
                    ChangeExportPorts( n->L_RET,  i->iport, i->eport );

		    UnlinkImport( i );
		    UnlinkExport( i );
		    UnlinkImport( ii );
		    UnlinkExport( ii );
                    unnec += 2;
                    }
                }

            /* LOOPA BODY: T := constant, K, OR L NOT REDEFINED */

            if ( IsLoopA( n ) )
                for ( i = n->L_BODY->imp; i != NULL; i = si ) {
                    si = i->isucc;

                    if ( IsImport( n->L_INIT, i->iport ) )
                        continue;
                ++Tunnec;
                        
                    if ( IsConst( i ) ) {
                        if ( !CanPropagateConst( i ) )
                            continue;

                        ChangeExportsToConst( n->L_TEST, i->iport, i );
		        UnlinkImport( i );
		        UnlinkExport( i );
                        unnec++;
                        }
                    else if ( IsSGraph( i->src ) )
                        if ( !IsImport( n->L_BODY, i->eport ) ) {
                            ChangeExportPorts( n->L_TEST, i->iport, i->eport );
		            UnlinkImport( i );
		            UnlinkExport( i );
                            unnec++;
                            }
                    }

            break;

        default:
            UNEXPECTED("Unknown loop");
    }
}


/**************************************************************************/
/* GLOBAL **************       CombineKports       ************************/
/**************************************************************************/
/* PURPOSE: COMBINE REDUNDANT IMPORTS TO COMPOUND NODE c AND ADJUST ALL   */
/*          REFERENCES. NOTE, THE FIRST IMPORT TO TAGCASE NODES IS NEVER  */
/*          COMBINED WITH OTHER IMPORTS.                                  */
/**************************************************************************/

void CombineKports( c )
PNODE c;
{
    register PEDGE i1;
    register PEDGE i2;
    register PNODE g;
    register PEDGE si;

    i1 = (IsTagCase( c ))? c->imp->isucc : c->imp;

    for ( ; i1 != NULL; i1 = i1->isucc ) 
        for ( i2 = i1->isucc; i2 != NULL; i2 = si ) {
            si = i2->isucc;

            if ( AreEdgesEqual( i1, i2 ) ) {
                for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
                    ChangeExportPorts( g, i2->iport, i1->iport );
                
                UnlinkImport( i2 );
                UnlinkExport( i2 );
                }
            }
}


/**************************************************************************/
/* LOCAL  **************  FastRemoveUnusedLTports  ************************/
/**************************************************************************/
/* PURPOSE: REMOVE AND DESTROY ALL IMPORTS OF LOOP l'S BODY AND INITIAL   */
/*          SUBGRAPHS NOT REFERENCED IN l. REMOVAL OF A BODY IMPORT MAY   */
/*          CREATE OTHER UNUSED IMPORTS; HENCE, MULTIPLE PASSES OVER THE  */
/*          BODY ARE REQUIRED.                                            */
/*          THIS ROUTINE HAS BEEN CONFIGURED FOR NON-DYNAMIC TRAVERSAL    */
/*          (SEE FastCleanForalls).                                       */ 
/**************************************************************************/

static void FastRemoveUnusedLTports( l )
PNODE l;
{
    register PEDGE i;
    register PEDGE si;
    register int   chng = TRUE;

    while ( chng ) {
        chng = FALSE;

        for ( i = l->L_BODY->imp; i != NULL; i = si ) {
            si = i->isucc;

            ++Tunused;

            if ( IsExport( l->L_TEST, i->iport ) || 
                 IsExport( l->L_BODY, i->iport ) ||
                 IsExport( l->L_RET,  i->iport )   )
                continue;

	    UnlinkImport( i );
	    UnlinkExport( i );
            unused++; chng = TRUE;
            }
        }

    for ( i = l->L_INIT->imp; i != NULL; i = si ) {
        si = i->isucc;

            ++Tunused;

        if ( IsExport( l->L_TEST, i->iport ) || 
             IsExport( l->L_BODY, i->iport ) ||
             IsExport( l->L_RET,  i->iport )   )
            continue;

	UnlinkImport( i );
	UnlinkExport( i );
        unused++; chng = TRUE;
        }
}


/**************************************************************************/
/* LOCAL  **************   FastRemoveUnusedKports  ************************/
/**************************************************************************/
/* PURPOSE: REMOVE AND DESTROY ALL IMPORTS OF COMPOUND NODE c NOT USED    */
/*          WITHIN c.  THE FIRST IMPORT OF A TAGCASE NODE IS ALWAYS USED. */
/*          THIS ROUTINE HAS BEEN CONFIGURED FOR NON-DYNAMIC TRAVERSAL    */
/*          (SEE FastCleanForalls).                                       */ 
/**************************************************************************/

static void FastRemoveUnusedKports( c )
PNODE c;
{
    register PEDGE i;
    register PEDGE si;
    register PNODE g;

    for ( i = (IsTagCase(c))? c->imp->isucc : c->imp; i != NULL; i = si ) {
        si = i->isucc;

        for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
            if ( IsExport( g, i->iport ) )
              break;

            ++Tunused;

        if ( g == NULL ) {
          UnlinkImport( i ); 
          UnlinkExport( i );
          unused++;
          }
        }
}


/**************************************************************************/
/* LOCAL  **************  FastRemoveUnusedRports   ************************/
/**************************************************************************/
/* PURPOSE: REMOVE AND DESTROY ALL IMPORTS OF SUBGRAPH g NOT EXPORTED     */
/*          FROM THE COMPOUND NODE FOR WHICH g BELONGS.                   */
/*          THIS ROUTINE HAS BEEN CONFIGURED FOR NON-DYNAMIC TRAVERSAL    */
/*          (SEE FastCleanForalls).                                       */ 
/**************************************************************************/

static void FastRemoveUnusedRports( g )
PNODE g;
{
    register PEDGE i;
    register PEDGE si;

    for ( i = g->imp; i != NULL; i = si ) {
        si = i->isucc;

            ++Tunused;

        if ( !IsExport( g->G_DAD, i->iport ) ) {
            UnlinkImport( i ); 
            UnlinkExport( i );
            unused++;
            }
        }
}


/**************************************************************************/
/* LOCAL  **************   FastRemoveUnusedTports  ************************/
/**************************************************************************/
/* PURPOSE: REMOVE AND DESTROY ALL IMPORTS OF FORALL f'S BODY SUBGRAPH    */
/*          NOT REFERENCED IN f'S RETURN SUBGRAPH.                        */
/*          THIS ROUTINE HAS BEEN CONFIGURED FOR NON-DYNAMIC TRAVERSAL    */
/*          (SEE FastCleanForalls).                                       */ 
/**************************************************************************/

static void FastRemoveUnusedTports( f )
PNODE f;
{
    register PEDGE i;
    register PEDGE si;

    for ( i = f->F_BODY->imp; i != NULL; i = si ) {
        si = i->isucc;

            ++Tunused;

        if ( !IsExport( f->F_RET, i->iport ) ) {
            UnlinkImport( i );
            UnlinkExport( i );
            unused++;
            }
        }
}


/**************************************************************************/
/* GLOBAL **************       FastCleanGraph      ************************/
/**************************************************************************/
/* PURPOSE: SELECTIVELY CLEAN THE NODES IN GRAPH g USING A NON-DYNAMIC    */
/*          TRAVERSAL ROUTINE.                                            */
/**************************************************************************/

void FastCleanGraph( g )
PNODE g;
{
  register PNODE n;
  register PEDGE i;
  register PNODE sn;
  register PEDGE si;
  register PNODE sg;

  /* SEE CleanDependentPaths */
  for ( i = g->imp; i != NULL; i = si ) {
    si = i->isucc;

    if ( i->iport == 0 ) {
      UnlinkImport( i );
      UnlinkExport( i );
      }
    }

  for ( n = FindLastNode( g ); n != g; n = n->npred ) {
    switch( n->type ) {
      case IFSelect:
        CombineKports( n );
        /* FastRemoveUnnecEdges( n ); */
        FastRemoveUnusedRports( n->S_ALT  );
	FastCleanGraph( n->S_ALT );
        FastRemoveUnusedRports( n->S_CONS );
	FastCleanGraph( n->S_CONS );
        FastRemoveUnusedKports( n );
        break;

      case IFTagCase:
        CombineKports( n );
        /* FastRemoveUnnecEdges( n ); */

        for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc ) {
          FastRemoveUnusedRports( sg );
	  FastCleanGraph( sg );
	  }

        FastRemoveUnusedKports( n );
        break;

      case IFForall:
        CombineKports( n );
        FastRemoveUnnecEdges( n );

        FastRemoveUnusedRports( n->F_RET );
	FastCleanGraph( n->F_RET );
        FastRemoveUnusedTports( n );
	FastCleanGraph( n->F_BODY );
        FastRemoveUnusedKports( n );
        break;

      case IFLoopA:
      case IFLoopB:
        CombineKports( n );
        FastRemoveUnnecEdges( n );

        FastRemoveUnusedRports( n->L_RET );
	FastCleanGraph( n->L_RET );
        FastRemoveUnusedLTports( n );
	FastCleanGraph( n->L_BODY );
        FastRemoveUnusedKports( n );
        break;

      default:
	break;
      }

    if ( n->exp == NULL )  {
      if ( IsPeek( n ) )
        continue;

      if ( glue && IsCall( n ) )
	continue;

      for ( i = n->imp; i != NULL; i = i->isucc )
        UnlinkExport( i );
      }
    }

  for ( n = g->G_NODES; n != NULL; n = sn ) {
    sn = n->nsucc;

    if ( n->exp == NULL ) {
      if ( IsPeek( n ) )
        continue;
      if ( glue && IsCall( n ) )
	continue;

      UnlinkNode( n );
      }
    }
}


/**************************************************************************/
/* LOCAL  **************        RecycleNode        ************************/
/**************************************************************************/
/* PURPOSE: RECYCLE NODE n AND ITS IMPORTS. IF n IS A COMPOUND NODE, THEN */
/*          ALL NODES OF ITS SUBGRAPHS ARE RECYCLED. BEFORE BEING FREED,  */
/*          n IS UNLINKED FROM ITS NODE LIST.                             */
/**************************************************************************/

static void RecycleNode( n )
PNODE n;
{
    if ( IsCompound( n ) )
      dccnt++;
    else
      dscnt++;

    if ( n->type == IFAGather ) agcnt++;

    UnlinkNode( n );
}


/**************************************************************************/
/* GLOBAL **************      OptRemoveDeadNode       ************************/
/**************************************************************************/
/* PURPOSE: REMOVE AND DESTROY NODE n IF IT IS DEAD---A NODE IS DEAD IF   */
/*          ITS EXPORT LIST IS EMPTY. THE IMPORTS OF A DEAD NODE ARE ALSO */
/*          DESTROYED, POSSIBLY CAUSING THE DESTRUCTION OF OTHER NODES.   */
/*          IF n IS NOT DEAD AND IS A COMPOUND NODE AN ATTEMPT IS MADE TO */
/*          CLEAN IT.  GRAPH NODES ARE IGNORED.                           */
/**************************************************************************/

void OptRemoveDeadNode( n )
PNODE n;
{
    register PEDGE i;
    register PEDGE si;

    if ( n == NULL )
        return;

    if ( IsGraph( n ) )
        return;

    if ( IsPeek( n ) )
      return;
    if ( glue && IsCall( n ) )
      return;

    if (IsCompound(n))
	++Tdccnt;
    else
	++Tdscnt;

    if (n->type == IFAGather)
	++Tagcnt;

    if ( n->exp == NULL ) {
        for ( i = n->imp; i != NULL; i = si ) {
            si = i->isucc;
            RemoveDeadEdge( i );
            }

        RecycleNode( n );
        }
}


/**************************************************************************/
/* GLOBAL **************      RemoveDeadEdge       ************************/
/**************************************************************************/
/* PURPOSE: REMOVE AND DESTROY EDGE d AND CHECK IF ITS SOURCE NODE IS     */
/*          DEAD OR REQUIRES CLEANING.  EDGE d MAY DEFINE A CONSTANT.     */
/**************************************************************************/

void RemoveDeadEdge( d )
PEDGE d;
{
    UnlinkImport( d );
    UnlinkExport( d );

    OptRemoveDeadNode( d->src );
}


/**************************************************************************/
/* GLOBAL **************       WriteCleanInfo      ************************/
/**************************************************************************/
/* PURPOSE: PRINT INFORMATION GATHERED DURING CLEANING AND RECYCLING TO   */
/*          stderr.                                                       */
/**************************************************************************/

void WriteCleanInfo()
{
    FPRINTF( infoptr, "\n **** GRAPH CLEANUP SUMMARY\n\n" );
    FPRINTF( infoptr, " Unnecessary Edges And Literals:  %d of %d\n", unnec,Tunnec  );
    FPRINTF( infoptr, " Unused Values:                   %d of %d\n", unused,Tunused );
    FPRINTF( infoptr, " Removed Simple Nodes             %d of %d\n", dscnt,Tdscnt  );
    FPRINTF( infoptr, " Removed Compound Nodes           %d of %d\n", dccnt,Tdccnt  );
    FPRINTF( infoptr, " Removed AGather Nodes            %d of %d\n", agcnt,Tagcnt  );
}


/**************************************************************************/
/* GLOBAL **************          If1Clean         ************************/
/**************************************************************************/
/* PURPOSE: CLEAN ALL FUNCTION GRAPHS: REMOVING ALL DEAD AND UNECESSARY   */
/*          NODES AND EDGES.                                              */
/**************************************************************************/

void If1Clean()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
    FastCleanGraph( cfunct = f );
}
