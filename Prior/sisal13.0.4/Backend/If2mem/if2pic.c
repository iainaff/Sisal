/* if2pic.c,v
 * Revision 12.7  1992/11/04  22:05:06  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:22  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


static void GatherATNodeInfo( n )
PNODE n;
{
  register PEDGE e;

  if ( !IsAtNode( n ) )
    return;

  inatns++;

  switch ( n->type ) {
    case IFAAddHAT:
    case IFAAddLAT:
      convatns++;

      for ( e = n->exp; e != NULL; e = e->esucc )
	if ( e->mark == 'P' ) {
	  patns++;
	  break;
	  }

      if ( n->imp->mark == 'P' )
	syncatns++;
      else
	fsyncatns++;

      if ( IsSGraph( n->imp->src ) )
	if ( IsLoop( n->imp->src->G_DAD ) )
	  for ( e = n->exp; e != NULL; e = e->esucc )
	    if ( IsSGraph( e->dst ) && ( e->iport == n->imp->eport ) ) {
              incratns++;

	      if ( !(n->imp->mark == 'P' && e->mark == 'P') )
		fincratns++;

              break;
	      }

      break;

    case IFACatenateAT:
      convatns++;

      for ( e = n->exp; e != NULL; e = e->esucc )
	if ( e->mark == 'P' ) {
	  patns++;
	  break;
	  }

      if ( n->imp->isucc->mark == 'P' && n->imp->mark == 'P' )
	syncatns++;
      else
	fsyncatns++;

      if ( IsSGraph( n->imp->src ) )
	if ( IsLoop( n->imp->src->G_DAD ) )
	  for ( e = n->exp; e != NULL; e = e->esucc )
	    if ( IsSGraph( e->dst ) && ( e->iport == n->imp->eport ) ) {
              incratns++;

	      if ( !(n->imp->mark == 'P' && e->mark == 'P') )
		fincratns++;

              break;
	      }

      if ( IsSGraph( n->imp->isucc->src ) )
	if ( IsLoop( n->imp->isucc->src->G_DAD ) )
	  for ( e = n->exp; e != NULL; e = e->esucc )
	    if ( IsSGraph( e->dst ) && ( e->iport == n->imp->isucc->eport ) ) {
              incratns++;

	      if ( !(n->imp->isucc->mark == 'P' && e->mark == 'P') )
		fincratns++;

              break;
	      }
      break;

    case IFABuildAT:
    case IFAFillAT:
      convatns++;
      syncatns++;

      for ( e = n->exp; e != NULL; e = e->esucc )
	if ( e->mark == 'P' ) {
	  patns++;
	  break;
	  }

      break;

    case IFAGatherAT:
    case IFFinalValueAT:
    case IFReduceAT:
    case IFRedTreeAT:
    case IFRedLeftAT:
    case IFRedRightAT:
      convatns++;
      syncatns++;

      for ( e = n->exp->dst->G_DAD->exp; e != NULL; e = e->esucc )
	if ( e->eport == n->exp->iport )
	  if ( e->mark == 'P' ) {
	    patns++;
	    break;
	    }

      break;


    default:
      break;
    }
}


static void PrintATNodeInfo()
{
  FPRINTF( infoptr, "\n **** INSERTED AT-NODE INFORMATION\n\n" );
  FPRINTF( infoptr, " Total Number of AT-NODES:                         %d\n", inatns );
  FPRINTF( infoptr, " Total Number of CONVERTED AT-NODES:               %d\n", convatns );
  FPRINTF( infoptr, " Total Number of SYNC AT-NODES:                    %d\n", syncatns );
  FPRINTF( infoptr, " Total Number of FAILED SYNC AT-NODES:             %d\n", fsyncatns );
  FPRINTF( infoptr, " Total Number of PARENT AT-NODES:                  %d\n", patns );
  FPRINTF( infoptr, " Total Number of Incremental-Build AT-NODES:       %d\n", 
	   incratns );
  FPRINTF( infoptr, " Total Number of FAILED Incremtnal-Build AT-NODES: %d\n",
		     fincratns );
}


/**************************************************************************/
/* LOCAL  **************       WriteMemCountInfo      ************************/
/**************************************************************************/
/* PURPOSE: WRITE COUNT VALUES TO stderr: TOTALS AND A SUMMARY BY LEVEL.  */
/**************************************************************************/

static void WriteMemCountInfo( msg )
char *msg;
{
    register struct level *l;
    register int           i;

    FPRINTF( stderr, "\n   * OCCURRENCE COUNTS %s\n\n", msg );

    FPRINTF( stderr, " Lits  %4d Edges %4d Smpls %4d Comps %4d Grphs %4d",
		       lits, edges, simples, comps, graphs              );

    FPRINTF( stderr, " AtNds %4d", ats );

    for ( i = 0; i < IF1GraphNodes; i++ ) {
	if ( (i % 7) == 0 )
	    FPRINTF( stderr, "\n" );

        FPRINTF( stderr, " %-5.5s %4d", GraphName(i), gnodes[i] );
	}

    for ( i = 0; i < IF1CompoundNodes; i++ ) {
	if ( (i % 7) == 0 )
	    FPRINTF( stderr, "\n" );

        FPRINTF( stderr, " %-5.5s %4d", CompoundName(i), cnodes[i] );
	}

    for ( i = 0; i < IF1SimpleNodes; i++ ) {
	if ( (i % 7) == 0 )
	    FPRINTF( stderr, "\n" );

        FPRINTF( stderr, " %-5.5s %4d", SimpleName(i), snodes[i] );
	}

    FPRINTF( stderr, "\n RSum  %4d RProd %4d RLst  %4d RGrt  %4d RCat  %4d",
		     rsum, rprod, rleast, rgreat, rcat                    );

    for ( i = 0; i < IF2AtNodes; i++ ) {
	if ( (i % 7) == 0 )
	    FPRINTF( stderr, "\n" );

        FPRINTF( stderr, " %-6.6s %3d", AtName(i), atnodes[i] );
	}

    FPRINTF( stderr, "\n" );

    FPRINTF( stderr, "\n   * SUMMARY BY LEVEL\n\n" );

    for ( i = 0; i <= maxl; i++ ) {
	l = &(levels[i]);

        FPRINTF( stderr, " Level %4d Lits  %4d Edges %4d Smpls %4d", i, 
			 l->lits, l->edges, l->simples               );

	FPRINTF( stderr, " AtNds %4d Comps %4d Grphs %4d\n",
		         l->ats, l->comps, l->graphs      );
        }

}


/**************************************************************************/
/* GLOBAL **************     CountNodesAndEdges    ************************/
/**************************************************************************/
/* PURPOSE: COUNT OCCURRENCES OF GRAPH NODES, SUBGRAPH NODES, SIMPLE      */
/*          NODES, COMPOUND NODES, LITERALS, AND EDGES IN ALL FUNCTION    */
/*          GRAPHS.  THE COUNTS ARE PRINTED TO stderr.                    */
/**************************************************************************/

void CountNodesAndEdges( msg )
char *msg;
{
    register struct level *l;
    register PNODE         f;
    register int           i;

    lits = edges  = simples = graphs = comps = ats = 0;
    rsum = rgreat = rleast  = rcat   = rprod = 0;


    topl = maxl =  -1;

    for ( i = 0; i < MaxLevel; i++ ) {
	l = &(levels[i]);

	l->lits  = l->edges  = l->simples = 0;
	l->comps = l->graphs = l->ats     = 0;
	}

    for ( i = 0; i < IF1GraphNodes; i++ )
	gnodes[i] = 0;

    for ( i = 0; i < IF1SimpleNodes; i++ )
	snodes[i] = 0;

    for ( i = 0; i < IF1CompoundNodes; i++ )
	cnodes[i] = 0;

    for ( i = 0; i < IF2AtNodes; i++ )
	atnodes[i] = 0;

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
	Count( cfunct = f );

    WriteMemCountInfo( msg );
}


/**************************************************************************/
/* LOCAL  **************     WriteNotConverted     ************************/
/**************************************************************************/
/* PURPOSE: IF IF1 NODE n EXPORTS AN ARRAY THEN PRINT A FILURE TO CONVERT */
/*          TO AT-NODE MESSAGE TO stderr.  STRING op DEFINES THE SYMBOLIC */
/*          NAME FOR NODE n.  THE ORIGINAL SOURCE FILE POSITION OF n IS   */
/*          PRINTED.                                                      */
/**************************************************************************/

static void WriteNotConverted( n, op )
PNODE  n;
char  *op;
{
    if ( !IsArray( n->exp->info ) )
	return;

    FPRINTF( infoptr, " AT-NODE CONVERSION FAILURE: %s: %s, %s, %d\n",
		        op, n->file, n->funct, n->line              );
}


/**************************************************************************/
/* LOCAL  **************        TraceBuffer        ************************/
/**************************************************************************/
/* PURPOSE: TRACE THE BUFFER EXPORTED FROM NODE n WITH EXPORT PORT eport  */
/*          TO ITS FINAL AT-NODE DESTINATIONS, PRINTING THEIR SYMBOLIC    */
/*          NAMES AND ORIGINAL SOURCE FILE LOCATIONS. MARK PRAGMAS ON     */
/*          RELAVENT IMPORTS ARE PRINTED.                                 */
/**************************************************************************/

void TraceBuffer( n, eport )
PNODE n;
int   eport;
{
    register PEDGE e;
    register PEDGE ee;
    register PNODE a;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
	if ( e->eport != eport )
	    continue;

	a = e->dst;
        switch ( a->type ) {
	    case IFAAddHAT:
	        if ( IsPMarked( a, 1 ) && (UsageCount( a, 1 ) > 1) )
	          FPRINTF( infoptr, "[P_FANOUT]\n" );

		FPRINTF( infoptr, "   array_addh(%%mk=%c,%%mk=V): %s, %s, %d\n", 
				 a->imp->mark, a->file, a->funct, a->line    );

                break;

	    case IFAAddLAT:
	        if ( IsPMarked( a, 1 ) && (UsageCount( a, 1 ) > 1) )
	          FPRINTF( stderr, "[P_FANOUT]\n" );

		FPRINTF( stderr, "   array_addl(%%mk=%c,%%mk=V): %s, %s, %d\n", 
				 a->imp->mark, a->file, a->funct, a->line    );

                break;

	    case IFACatenateAT:
	        if ( IsPMarked( a, 1 ) && (UsageCount( a, 1 ) > 1) )
	          FPRINTF( stderr, "[P_FANOUT]\n" );

		FPRINTF( stderr, "   %%mk=%c || %%mk=%c: %s, %s, %d\n",
				 a->imp->mark, a->imp->isucc->mark, a->file,
				 a->funct, a->line                        );

                break;

	    case IFAFillAT:
	        if ( IsPMarked( a, 1 ) && (UsageCount( a, 1 ) > 1) )
	          FPRINTF( stderr, "[P_FANOUT]\n" );

		FPRINTF( stderr, "   array_fill(low,hi,%%mk=V): %s, %s, %d\n", 
				 a->file, a->funct, a->line                 );

                break;

	    case IFABuildAT:
	        if ( IsPMarked( a, 1 ) && (UsageCount( a, 1 ) > 1) )
	          FPRINTF( stderr, "[P_FANOUT]\n" );

		FPRINTF( stderr, "   array [low,%%mk=V...]: %s, %s, %d\n", 
				 a->file, a->funct, a->line             );

                break;

	    case IFAGatherAT:
	        if ( IsPMarked( a->exp->dst->G_DAD, a->exp->iport ) && 
		     (UsageCount( a->exp->dst->G_DAD, a->exp->iport ) > 1) )
	          FPRINTF( stderr, "[P_FANOUT]\n" );

		FPRINTF( stderr, "   returns array of %%mk=V: %s, %s, %d", 
				 a->file, a->funct, a->line             );

                if ( a->imp->isucc->isucc->iport == 3 )
		    FPRINTF( stderr, " FILTER OVERRIDE\n" );
                else
		    FPRINTF( stderr, "\n" );

                break;

	    case IFFinalValueAT:
	        if ( IsPMarked( a->exp->dst->G_DAD, a->exp->iport ) && 
		     (UsageCount( a->exp->dst->G_DAD, a->exp->iport ) > 1) )
	          FPRINTF( stderr, "[P_FANOUT]\n" );

		FPRINTF( stderr, "   returns value of %%mk=P: %s, %s, %d\n", 
				 a->file, a->funct, a->line               );

                break;

	    case IFReduceAT:
	        if ( IsPMarked( a->exp->dst->G_DAD, a->exp->iport ) && 
		     (UsageCount( a->exp->dst->G_DAD, a->exp->iport ) > 1) )
	          FPRINTF( stderr, "[P_FANOUT]\n" );

		FPRINTF( stderr, "   returns value of catenate " );
                goto ReduceFinish;

	    case IFRedLeftAT:
	        if ( IsPMarked( a->exp->dst->G_DAD, a->exp->iport ) && 
		     (UsageCount( a->exp->dst->G_DAD, a->exp->iport ) > 1) )
	          FPRINTF( stderr, "[P_FANOUT]\n" );

		FPRINTF( stderr, "   returns value of left catenate " );
		goto ReduceFinish;

	    case IFRedRightAT:
	        if ( IsPMarked( a->exp->dst->G_DAD, a->exp->iport ) && 
		     (UsageCount( a->exp->dst->G_DAD, a->exp->iport ) > 1) )
	          FPRINTF( stderr, "[P_FANOUT]\n" );

		FPRINTF( stderr, "   returns value of right catenate " );
		goto ReduceFinish;

	    case IFRedTreeAT:
	        if ( IsPMarked( a->exp->dst->G_DAD, a->exp->iport ) && 
		     (UsageCount( a->exp->dst->G_DAD, a->exp->iport ) > 1) )
	          FPRINTF( stderr, "[P_FANOUT]\n" );

		FPRINTF( stderr, "   returns value of tree catenate " );

ReduceFinish:

		FPRINTF( stderr, "%%mk=%c: %s, %s, %d\n", 
				 a->imp->isucc->isucc->mark, a->file, a->funct,
				 a->line                                     );

                break;


	    case IFShiftBuffer:
		TraceBuffer( a, 1 );
		break;

	    case IFForall:
		/* DOES THE GENERATE SUBGRAPH SCATTER PARTITIONS OF THE   */
		/* BUFFER INTO THE BODY SUBGRAPH?  IF SO, TRACE THEM INTO */
		/* THE BODY SUBGRAPH!                                     */

		if ( (ee = FindExport( a->F_GEN, e->iport )) != NULL )
		    TraceBuffer( a->F_BODY, ee->dst->exp->iport );

                TraceBuffer( a->F_RET, e->iport );
		break;

	    case IFLoopA:
	    case IFLoopB:
		/* IS THE BUFFER REFERENCED IN THE INITIAL OR BODY        */
		/* SUBGRAPH? IF SO, FOLLOW IT!                            */

		if ( (ee = FindExport( a->L_INIT, e->iport )) != NULL )
		    TraceBuffer( a->L_BODY, ee->iport );
                else if ( IsExport( a->L_BODY, e->iport ) )
		    TraceBuffer( a->L_BODY, e->iport );

                TraceBuffer( a->L_RET, e->iport );
                break;

	    case IFSGraph:
		break;                             /* LOOP CARRIED BUFFER */

	    case IFSelect:        /* SPECIAL BUFFER REMOVAL AND COMBINING */
		if ( IsExport( a->S_CONS, e->iport ) ) {
		  FPRINTF( stderr, "[S_CONS]\n" );
		  TraceBuffer( a->S_CONS, e->iport );
		  }

		if ( IsExport( a->S_ALT, e->iport ) ) {
		  FPRINTF( stderr, "[S_ALT]\n" );
		  TraceBuffer( a->S_ALT, e->iport );
		  }

		break;

	    default:
		Error1( "TraceBuffer: ILLEGAL BUFFER DESTINATION NODE" );
		break;
            }
        }
}


/**************************************************************************/
/* LOCAL  **************     WriteGraphPicture     ************************/
/**************************************************************************/
/* PURPOSE: WRITE INFORMATION CONCERNING BUFFER USAGE IN GRAPH g TO       */
/*          stderr.  IF1 TO IF2 AT-NODE CONVERSION FAILURES ARE REPORTED  */
/*          IN THIS ROUTINE.                                              */
/**************************************************************************/

static void WriteGraphPicture( g )
PNODE g;
{
    register PNODE n;

    for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
	GatherATNodeInfo( n );

	if ( IsCompound( n ) ) {
	    for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		WriteGraphPicture( g );

            continue;
	    }

	switch ( n->type ) {
            case IFAAddH: 
	        if ( !IsArray( n->exp->info ) )
	            break;

		WriteNotConverted( n, "array_addh(...)" );
		break;

	    case IFAAddL:
	        if ( !IsArray( n->exp->info ) )
	            break;

		WriteNotConverted( n, "array_addl(...)" );
		break;

	    case IFACatenate:
	        if ( !IsArray( n->exp->info ) )
	            break;

		WriteNotConverted( n, "||" );
		break;

	    case IFAFill:
	        if ( !IsArray( n->exp->info ) )
	            break;

		WriteNotConverted( n, "array_fill(...)" );
		break;

	    case IFABuild:
	        if ( !IsArray( n->exp->info ) )
	            break;

		WriteNotConverted( n, "array [...]" );
		break;

	    case IFAGather:
	        if ( !IsArray( n->exp->info ) )
	            break;

		WriteNotConverted( n, "returns array of" );
		break;

	    case IFFinalValue:
	        if ( !IsArray( n->exp->info ) )
	            break;

		WriteNotConverted( n, "returns value of" );
		break;

	    case IFReduce:
	        if ( !IsArray( n->exp->info ) )
	            break;

		if ( n->imp->CoNsT[0] != REDUCE_CATENATE )
		    break;

		WriteNotConverted( n, "returns value of catenate" );
		break;

	    case IFRedLeft:
	        if ( !IsArray( n->exp->info ) )
	            break;

		if ( n->imp->CoNsT[0] != REDUCE_CATENATE )
		    break;

		WriteNotConverted( n, "returns value of left catenate" );
		break;

	    case IFRedRight:
	        if ( !IsArray( n->exp->info ) )
	            break;

		if ( n->imp->CoNsT[0] != REDUCE_CATENATE )
		    break;

		WriteNotConverted( n, "returns value of right catenate" );
		break;

	    case IFRedTree:
	        if ( !IsArray( n->exp->info ) )
	            break;

		if ( n->imp->CoNsT[0] != REDUCE_CATENATE )
		    break;

		WriteNotConverted( n, "returns value of tree catenate" );
		break;

	    case IFMemAlloc:
		FPRINTF( stderr, "  MEMORY BUFFER USAGE TRACE:\n" );
		TraceBuffer( n, 1 );
		break;

	    default:
		break;
	    }
        }
}


/**************************************************************************/
/* GLOBAL **************     WriteIf2memPicture    ************************/
/**************************************************************************/
/* PURPOSE: WRITE INFORMATION CONCERNING BUFFER USAGE IN ALL FUNCTIONS    */
/*          TO stderr; INCLUDING IF1 TO IF2 AT-NODE CONVERSION FAILURES.  */
/**************************************************************************/

void WriteIf2memPicture()
{
   /* PNODE f;

    FPRINTF( stderr, "\n   * ARRAY MEMORY MAP\n" );

    for ( f = glstop->gsucc ; f != NULL ; f = f->gsucc ) {
	if ( IsIGraph( f ) )
	    continue;

	FPRINTF( stderr, "\n FUNCTION %s(...)\n", f->G_NAME );

	WriteGraphPicture( f );
	}
   */ 
    PrintATNodeInfo();
    (void)WriteAggregateInfo();
}


/**************************************************************************/
/* LOCAL  **************     MemWriteGraphWarnings     ************************/
/**************************************************************************/
/* PURPOSE: WRITE BUILD COPY WARNINGS FOR GRAPH g TO stderr.              */
/**************************************************************************/

static void MemWriteGraphWarnings( g )
PNODE g;
{
  register PNODE n;
  register PNODE sg;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) ) {
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
        MemWriteGraphWarnings( sg );

      continue;
      }

    switch ( n->type ) {
      case IFAAddHAT:
        if ( n->imp->mark == 'P' )
          break;

        PrintWarning( "array_addh", n->line, n->funct, n->file, TRUE );
	break;

      case IFAAddLAT:
        if ( n->imp->mark == 'P' )
          break;

        PrintWarning( "array_addl", n->line, n->funct, n->file, TRUE );
	break;

      case IFACatenateAT:
        if ( n->imp->isucc->mark == 'P' && n->imp->mark == 'P' )
	  break;

        PrintWarning( "||", n->line, n->funct, n->file, TRUE );
        break;

      case IFAFillAT:
      case IFAGatherAT:
      case IFFinalValueAT:
        break;

      case IFReduceAT:
      case IFRedTreeAT:
      case IFRedLeftAT:
      case IFRedRightAT:
        if ( n->imp->isucc->isucc->mark == 'P' )
          break;

        PrintWarning( "catenate", n->line, n->funct, n->file, TRUE );
        break;

      case IFAAddH: 
        if ( !IsArray( n->exp->info ) )
          break;

        PrintWarning("array_addh", n->line, n->funct, n->file, FALSE );
        break;

      case IFAAddL:
        if ( !IsArray( n->exp->info ) )
          break;

        PrintWarning("array_addl", n->line, n->funct, n->file, FALSE );
        break;

      case IFACatenate:
        if ( !IsArray( n->exp->info ) )
          break;

        PrintWarning( "||", n->line, n->funct, n->file, FALSE );
        break;

      case IFAFill:
        if ( !IsArray( n->exp->info ) )
          break;

        PrintWarning( "array_fill", n->line, n->funct, n->file, FALSE );
        break;

      case IFABuild:
        if ( !IsArray( n->exp->info ) )
          break;

        PrintWarning( "array [..]", n->line, n->funct, n->file, FALSE );
        break;

      case IFAGather:
        if ( !IsArray( n->exp->info ) )
          break;

        PrintWarning( "array of", n->line, n->funct, n->file, FALSE );
        break;

      case IFRedLeft:
      case IFRedRight:
      case IFRedTree:
      case IFReduce:
        if ( !IsArray( n->exp->info ) )
          break;

        if ( n->imp->CoNsT[0] != REDUCE_CATENATE )
          break;

        PrintWarning( "catenate", n->line, n->funct, n->file, FALSE );
	break;

      default:
        break;
        }
      }
}


/**************************************************************************/
/* GLOBAL **************    WriteIf2memWarnings    ************************/
/**************************************************************************/
/* PURPOSE: WRITE AGGREGATE CONSTRUCTION COPY WARNINGS TO stderr.         */
/**************************************************************************/

void WriteIf2memWarnings()
{
  PNODE f;

  for ( f = glstop->gsucc ; f != NULL ; f = f->gsucc ) {
    if ( IsIGraph( f ) )
      continue;

    MemWriteGraphWarnings( f );
    }
}
