/**************************************************************************/
/* FILE   **************       if2refcntopt.c      ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log$
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:03  patmiller
 * Well, here is the first set of big changes in the distribution
 * in 5 years!  Right now, I did a lot of work on configuration/
 * setup (now all autoconf), breaking out the machine dependent
 * #ifdef's (with a central acconfig.h driven config file), changed
 * the installation directories to be more gnu style /usr/local
 * (putting data in the /share/sisal14 dir for instance), and
 * reduced the footprint in the top level /usr/local/xxx hierarchy.
 *
 * I also wrote a new compiler tool (sisalc) to replace osc.  I
 * found that the old logic was too convoluted.  This does NOT
 * replace the full functionality, but then again, it doesn't have
 * 300 options on it either.
 *
 * Big change is making the code more portably correct.  It now
 * compiles under gcc -ansi -Wall mostly.  Some functions are
 * not prototyped yet.
 *
 * Next up: Full prototypes (little) checking out the old FLI (medium)
 * and a new Frontend for simpler extension and a new FLI (with clean
 * C, C++, F77, and Python! support).
 *
 * Pat
 *
 *
 * Revision 1.4  1993/02/24  18:39:00  miller
 * Changed the fix to the recursive memory problem (921002) to a dynamic
 * patch.
 *
 * Revision 1.3  1993/01/07  00:39:44  miller
 * Make changes for LINT and combined files.
 *
 * Revision 1.2  1992/11/06  19:34:10  miller
 * Patch for #921002 -- The backend was over aggressively consuming a refcnt
 * before entering a recursive function call.  This was killing the code
 * because the heap value was being scavenged and overwritten!
 *
 * Revision 12.8  1992/11/05  23:30:37  miller
 * Patch for #921002 -- The backend was over aggressively consuming a refcnt
 * before entering a recursive function call.  This was killing the code
 * because the heap value was being scavenged and overwritten!
 *
 * Revision 12.8  1992/10/23  20:41:47  miller
 * Patch for #921002 -- The backend was over aggressively consuming a refcnt
 * before entering a recursive function call.  This was killing the code
 * because the heap value was being scavenged and overwritten!
 *
 * Revision 12.7  1992/10/21  18:10:03  miller
 * Initial RCS Version by Cann
 *
 */
/**************************************************************************/

#include "world.h"


/**************************************************************************/
/* LOCAL  **************     TryAndHoistNoOp       ************************/
/**************************************************************************/
/* PURPOSE: TRY AND MOVE NoOp NODE n OUT OF ANY LOOP IN WHICH IT MIGHT    */
/*          RESIDE AND LEAVE IN ITS PLACE A TRUE NoOp NODE.               */
/**************************************************************************/

static void TryAndHoistNoOp( n )
PNODE n;
{
  register PEDGE li;
  register PEDGE ii;
  register PEDGE bi;
  register PEDGE ri;
  register PEDGE e;
  register PNODE l;
  register PNODE nn;

  /* CHECK IF n IS A CANDIDATE FOR HOISTING */
  if ( !(n->imp->rmark1 == rMARK && n->imp->omark1) ) /* rO? */
    return;

  if ( IsConst( n->imp ) )
    return;

  if ( !IsSGraph( n->imp->src ) )
    return;

  l = n->imp->src->G_DAD;

  if ( !IsLoop( l ) )
    return;

  if ( n->imp->src != l->L_BODY )
    return;

  if ( n->imp->pm > 0 )
    return;

  /* SEE IF THE ARRAY IS LOOP CARRIED AND CAN TOLERATE THE HOISTING */
  if ( (bi = FindImport( l->L_BODY, n->imp->eport )) == NULL )
    return;

  if ( !(bi->rmark1 == RMARK && bi->omark1) ) /* RO? */
    return;

  if ( bi->cm < 0 || bi->pm > 0 )
    return;

  /* SEE IF THE INITIAL ARRAY CAN TOLERATE THE HOISTING */
  if ( (ii = FindImport( l->L_INIT, n->imp->eport )) == NULL )
    return;

  if ( IsConst( ii ) )
    return;

  if ( ii->pm > 0 )
    return;

  if ( UsageCount( l->L_INIT, ii->eport ) != 1 )
    return;

  /* SEE IF THE RETURN ARRAY CAN TOLERATE THE HOISTING */
  if ( (ri = FindExport( l->L_RET, bi->iport )) == NULL )
    return;

  if ( UsageCount( l->L_RET, ri->eport ) != 1 )
    return;

  if ( ri->dst->type != IFFinalValue )
    return;

  if ( ri->pm > 0 || ri->cm < 0 )
    return;

  if ( !(ri->dst->lmark) )
    return;

  /* CHECK THAT THE INPUT ARRAY TO THE LOOP IS TOLERABLE TO HOISTING */
  if ( (li = FindImport( l, ii->eport )) == NULL )
    Error2( "TryAndHoistNoOp", "FindImport TO LOOP FAILED" );

  if ( li->cm < 0 )
    return;

  /* OK, EVERYTHING CHECKS OUT, SO MOVE n */
  nn = NodeAlloc( ++maxint, IFNoOp );
  nn->file  = n->file;
  nn->funct = n->funct;
  nn->line  = n->line;

  LinkNode( l->npred, nn );

  e = EdgeAlloc( nn, 1, l, li->iport );
  e->info   = li->info;
  e->sr     = 1;
  e->rmark1 = RMARK;
  e->omark1 = TRUE;
  LinkExport( nn, e );
  LinkImport( l, e );

  UnlinkImport( li );
  li->iport  = 1;
  li->cm     = -1;

  LinkImport( nn, li );

  ii->rmark1 = RMARK;
  ii->omark1 = TRUE;

  n->imp->cm     = 0;

  if ( !IsConst( n->imp ) )
    for ( e = n->imp->src->exp; e != NULL; e = e->esucc )
      if ( e->eport == n->imp->eport ) {
        e->rmark1 = RMARK;
        e->omark1 = TRUE;
        }

  n->imp->rmark1 = RMARK;
  n->imp->omark1 = TRUE;

  hnoops++;
}


/**************************************************************************/
/* GLOBAL **************     CleanNoOpImports      ************************/
/**************************************************************************/
/* PURPOSE: CHANGE THE cm VALUE OF ALL NoOp NODE IMPORTS THAT HAVE mk=R   */
/*          PRAGMAS TO 0.                                                 */
/**************************************************************************/

void CleanNoOpImports()
{
    register PNODE n;

    for ( n = nohead; n != NULL; n = n->usucc ) {
     if ( n->imp->rmark1 == RMARK )
       n->imp->cm = 0;

     TryAndHoistNoOp( n );
     }
}


/**************************************************************************/
/* LOCAL  **************    BindProducerPragmas    ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN pm AND sr TO THE ASSOCIATED PRAGMAS OF ALL REFERENCES  */
/*          TO EDGE ee.                                                   */
/**************************************************************************/

static void BindProducerPragmas( ee, pm, sr )
PEDGE ee;
int   pm;
int   sr;
{
    register PEDGE e;

    for ( e = ee->src->exp; e != NULL; e = e->esucc )
        if ( e->eport == ee->eport ) {
            e->pm = pm;
            e->sr = sr;
            }
}


/**************************************************************************/
/* LOCAL  **************       BeforeAWrite        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF WRITE SET wset CONTAINS AN GRAPH IMPORT OR AN  */
/*          ENTERY WHOSE DESTINATION IS THE DESTINATION OF AN ADE WHOSE   */
/*          SOURCE IS e->dst, ELSE RETURN FALSE.                          */
/**************************************************************************/

static int BeforeAWrite( e, wset )
PEDGE e;
PSET  wset;
{
    register PEDGE ee;
    register int   w;

    for ( w = 0; w <= wset->last; w++ ) {
        ee = wset->set[w];

        if ( IsGraph( ee->dst ) )
            return( TRUE );

        if ( IsAdePresent( e->dst, ee->dst ) )
            return( TRUE );
        }

    return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************      BeforeAllWrites      ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF AN ADE EXIST FROM e->dst TO THE DESTINATION OF */
/*          ALL MEMBERS OF WRITE SET wset. AN IMPLICIT ADE BETWEEN e->dst */
/*          AND ITS SCOPE DEFINING GRAPH NODE IS ASSUMED TO EXIST.        */
/**************************************************************************/

static int BeforeAllWrites( e, wset )
PEDGE e;
PSET  wset;
{
    register PEDGE ee;
    register int   w;

    for ( w = 0; w <= wset->last; w++ ) {
        ee = wset->set[w];

        if ( !IsGraph( ee->dst ) )
            if ( !IsAdePresent( e->dst, ee->dst ) )
                return( FALSE );
        }

    return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************      NeutralizeEdge       ************************/
/**************************************************************************/
/* PURPOSE: IF e'S cm VALUE IS EQUAL TO -1 (NOT ALREADY OPTIMIZED IF A    */
/*          COMPOUND IMPORT) THEN DECREMENT THE APPROPRAITE PRODUCER      */
/*          PRAGMA OF ALL EDGES REPRESENTING REFERENCES TO EDGE e. E'S    */
/*          cm VALUE IS SET TO 0; HENCE, NO REFERENCE COUNTING NEED BE    */
/*          DONE, WITH RESPECT TO e,  BY e->dst AND POSSIBLY e->src.      */
/**************************************************************************/

static void NeutralizeEdge( e )
PEDGE e;
{
    register PEDGE ee;

    if ( e->cm != -1 )
        return;

    /* ------------------------------------------------------------ */
    /* Sometimes we get memory bombs with recursive programs.  This */
    /* fixed the problem, but introduces a nasty memory leak.  We   */
    /* allow the user to turn the patch on, but it defaults to off. */
    /* ------------------------------------------------------------ */
    if ( UsingPatch(1) ) {
      /* ------------------------------------------------------------ */
      /* The Function body will consume one reference for these edges */
      /* This introduces a leak, but it will fix 921002 for now!      */
      /* ------------------------------------------------------------ */
      if ( e->dst->type == IFCall ) {
        e->cm += 1;
        return;
      }
    }

    e->cm = 0;

    for ( ee = e->src->exp; ee != NULL; ee = ee->esucc )
        if ( ee->eport == e->eport ) {
            if ( ee->sr != -2 )
                ee->sr--;
            else
                ee->pm--;
            }
}


/**************************************************************************/
/* LOCAL  **************     ChangeToGenerator     ************************/
/**************************************************************************/
/* PURPOSE: MARK GRAPH g AS A GENERATOR OF THE EXPORT WITH PORT NUMBER    */
/*          eport; THAT IS, GRAPH g IS RESPONSIBLE FOR ATTEMPTING TO      */
/*          RECYCLE THE EXPORT.  TRUE IS RETURNED IF THE CHANGE IS MADE.  */
/**************************************************************************/

static int ChangeToGenerator( g, eport )
PNODE g;
int   eport;
{
    register PEDGE e;
    register int   chg = FALSE;

    for ( e = g->exp; e != NULL; e = e->esucc )
        if ( e->eport == eport ) {
            chg = TRUE;

            e->pm--;
            e->grset->gen = e;
            e->gwset->gen = e;
            }

    return( chg );
}


/**************************************************************************/
/* LOCAL  **************         AddGround         ************************/
/**************************************************************************/
/* PURPOSE: CREATE, INITIALIZE, AND INSERT A DATA GROUND, A PORT ZERO     */
/*          IMPORT, BETWEEN NODE src (EXPORT PORT NUMBER eport) AND GRAPH */
/*          dst.  THE cm VALUE OF THE GROUND IS -1 AND THE PRODUCER       */
/*          VALUES ARE TAKEN FROM THE ARGUMENT LIST. Info IS THE ASSIGNED */
/*          type. THE ALLOCATED EDGE IS APPENDED TO THE DATA GOUND EDGE   */
/*          LIST FOR LATER MARK OPTIMIZATION. THE GROUND IS RETURNED.     */
/**************************************************************************/

static PEDGE AddGround( src, eport, dst, EdgeInfo, pm, sr )
PNODE src;
int   eport;
PNODE dst;
PINFO EdgeInfo;
int   pm;
int   sr;
{
    register PEDGE e;

    e = EdgeAlloc( src, eport, dst, 0 );

    e->info  = EdgeInfo;
    e->cm    = -1;
    e->sr    = sr;
    e->pm    = pm;
    e->pl    = -2;
    e->wmark = TRUE;

    LinkExport( src, e );
    LinkImport( dst, e );

    AppendToUtilityList( dghead, dgtail, e );

    return( e );
}


/**************************************************************************/
/* LOCAL  **************     DecrementPmValues     ************************/
/**************************************************************************/
/* PURPOSE: DECREMENT THE pm VALUE OF ALL REFERENCES TO THE EXPORT WITH   */
/*          PORT VALUE eport.                                             */
/**************************************************************************/

static void DecrementPmValues( n, eport )
PNODE n;
int   eport;
{
    register PEDGE e;

    for ( e = n->exp;  e != NULL; e = e->esucc )
        if ( e->eport == eport )
            e->pm--;
}


/**************************************************************************/
/* LOCAL  **************           NotRef          ************************/
/**************************************************************************/
/* PURPOSE: APPEND L VALUE DEFINITION EDGE i AND ITS REDEFINING EDGE TO   */
/*          THE DATA GROUND LIST FOR FUTURE OPTIMIZATION.                 */
/**************************************************************************/

static void NotRef( c, i )
PNODE c;
PEDGE i;
{
    AppendToUtilityList( dghead, dgtail, i );
    AppendToUtilityList( dghead, dgtail, FindImport( c->L_BODY, i->iport ) );
}


/**************************************************************************/
/* LOCAL  **************        RefBodyOnly        ************************/
/**************************************************************************/
/* PURPOSE: MARK THE BODY SUBGRAPH OF LOOP c AS A GENERATOR OF L VALUE i  */
/*          AND CHANGE THE cm VALUE OF i'S REDEFINING BODY IMPORT TO 0.   */
/*          EDGE i IS APPENDED TO THE DATA GROUND LIST.                   */
/**************************************************************************/

static void RefBodyOnly( c, i )
PNODE c;
PEDGE i;
{
    register PEDGE ii;

    ChangeToGenerator( c->L_BODY, i->iport );

    ii = FindImport( c->L_BODY, i->iport );
    ii->cm = 0;

    AppendToUtilityList( dghead, dgtail, i );
}


/**************************************************************************/
/* LOCAL  **************       RefBodyAndRet       ************************/
/**************************************************************************/
/* PURPOSE: CHANGE THE cm VALUE OF THE EDGE REDEFINING L PORT VALUE i IN  */
/*          THE BODY OF LOOP c TO 0 AND MARK THE BODY SUBGRAPH AS THE     */
/*          GENERATOR OF i.  THEN COUNT THE NUMBER OF LAST ITERATION      */
/*          NODES IN THE RETURN SUBGRAPH OF c AND CORRESPONDINGLY ASSIGN  */
/*          VALUES TO THE pl and pm PRAGMAS OF EACH REFERENCE TO i IN THE */
/*          RETURN SUBGRAPH.  IF THE COUNT IS NOT ZERO, i'S cm VALUE IS   */
/*          CHANGED TO 0, ELSE i IS APPENDED TO THE DATA GROUND LIST.     */
/**************************************************************************/

static void RefBodyAndRet( c, i )
PNODE c;
PEDGE i;
{
    register PEDGE e;
    register PNODE n;
    register int   cnt;
             PEDGE ii;

    ii = FindImport( c->L_BODY, i->iport );
    ii->cm = 0;

    ChangeToGenerator( c->L_BODY, i->iport );

    cnt = 0;

    for ( n = c->L_RET->G_NODES; n != NULL; n = n->nsucc )
        switch ( n->type ) {
            case IFFinalValue:
                if ( n->imp->isucc != NULL )
                    break;

            case IFFinalValueAT:
                if ( n->imp->eport != i->iport )
                    break;

                n->imp->cm = 0;
                n->lmark   = TRUE;
                cnt++;
                break;

            default:
                break;
            }

    if ( cnt == 0 ) {
        AppendToUtilityList( dghead, dgtail, i );
    } else {
        i->cm = 0;

        for ( e = c->L_RET->exp; e != NULL; e = e->esucc )
            if ( e->eport == i->iport ) {
                e->pm -= cnt;
                e->pl  = cnt - 1;
                }
        }
}


/**************************************************************************/
/* LOCAL  **************         RefRetOnly        ************************/
/**************************************************************************/
/* PURPOSE: CHANGE THE cm VALUE OF L PORT VALUE i AND THE EDGE REDEFINING */
/*          i IN THE BODY OF LOOP c TO 0 AND CHANGE THE RETURN SUBGRAPH   */
/*          INTO A GENERATOR OF i.                                        */
/**************************************************************************/

static void RefRetOnly( c, i )
PNODE c;
PEDGE i;
{
    PEDGE ii;

    i->cm = 0;

    ii = FindImport( c->L_BODY, i->iport );
    ii->cm = 0;

    ChangeToGenerator( c->L_RET, i->iport );
}


/**************************************************************************/
/* LOCAL  ************** OptBoundaryReferenceCounts ***********************/
/**************************************************************************/
/* PURPOSE: ELIMINATE UNNECESSARY REFERENCE COUNTING AS A RESULT OF GRAPH */
/*          BOUNDARIES. THE FOLLOWING CHANGES ARE MADE:                   */
/*                                                                        */
/*          Call:         BEFORE: ---> (W) cm = -1 (N)CALL                */
/*                                CALLEE FG(N) pm=F   (W) --->            */
/*                        AFTER:  ---> (W) cm =  0 (N)CALL                */
/*                                CALLEE FG(N) pm=F-1 (W) --->            */
/*                                                                        */
/*                        NOW THE CALLEE IS A GENERATOR OF IMPORT N.      */
/*                                                                        */
/*          Select:                                                       */
/*          TagCase:      BEFORE: ---> (W) cm = -1 (N)SELECT OR TAGCASE   */
/*                                SG(N) pm = F   (?) --->                 */
/*                                SG(NO N REFERENCE)                      */
/*                        AFTER:  ---> (W) cm =  0 (N)SELECT OR TAGCASE   */
/*                                SG(N) pm = F-1 (?) --->                 */
/*                                SG(NOW REF N) pm = 0 ---> cm = -1 (0)SG */
/*                                                                        */
/*                        NOW ALL SUBGRAPHS ARE GENERATORS OF IMPORT N.   */
/*                        NOTE: THE FIRST TagCase IMPORT IS ALWAYS (R).   */
/*                                                                        */
/*          Forall:       BEFORE: ---> (W) cm = -1 (N)FORALL              */
/*                                GEN(N) (W) pm = F ---> ONLY REF IN GEN  */ 
/*                                ---> cm = -1 (M)BODY                    */
/*                                RET(N) pm = F --->                      */
/*                        AFTER:  ---> (W) cm =  0 (N)FORALL              */
/*                                GEN(N) (W) pm = F-1 -> ONLY REF IN GEN  */
/*                                ---> cm =  0 (M)BODY                    */
/*                                RET(M) pm = F-1 ->                      */
/*                                                                        */
/*                        NOW THE RETURN SUBGRAPH IS A GENERATOR OF ALL   */
/*                        T PORT VALUES; HENCE RESPONSIBLE FOR RECYCLING  */
/*                        THEIR STORAGE IF NECESSARY. THE SAME HOLDS FOR  */
/*                        ALL WRITE CLASSIFIED K IMPORTS ONLY REFERENCED  */
/*                        IN THE GENERATE SUBGRAPH.                       */
/*                                                                        */
/*          Loop[AB]:     BEFORE: ---> (?) cm = -1 (N)LOOP                */
/*                                INIT(N) pm = F   ---> ONLY REF IN INIT  */
/*                        AFTER:  ---? (?) cm =  0 (N)LOOP                */
/*                                INIT(N) pm = F-1 ---> ONLY REF IN INIT  */
/*                                                                        */
/*                        REFER TO RefBodyAndRet, RefRetOnly, AND         */
/*                        RefBodyOnly FOR OTHER LOOP OPTIMIZATIONS.       */
/*                                                                        */
/*          THE FOLLOWING SPECIAL ACTION IS TAKEN TO ELIMINATE REDUNDANT  */
/*          REFERENCE COUNTING: THE pm PRAGMAS OF ALL COMPOUND NODE       */
/*          AGGREGATE EXPORTS ARE DECREMENTED AND THE ASSOCIATED SUBGRAPH */
/*          IMPORT cm PRAGMAS ARE SET TO 0. AScatter pm PRAGMAS ARE SET   */
/*          TO 0.                                                         */
/**************************************************************************/

static void OptBoundaryReferenceCounts()
{
    register PNODE c;
    register PEDGE i;
    register PEDGE e;
    register PNODE g;

    for ( c = chead; c != NULL; c = c->usucc ) {
        if ( IsCompound( c ) ) {
            for ( e = c->exp; e != NULL; e = e->esucc )
                if ( IsAggregate( e->info ) )
                    e->pm--;

            switch ( c->type ) {
                case IFSelect:
                case IFTagCase:
                    for ( g = c->C_SUBS; g != NULL; g = g->gsucc )
                        AssignCMPragmas( g, 0 );

                    break;

                case IFForall:
                    AssignCMPragmas( c->F_GEN, 0 );
                    AssignCMPragmas( c->F_RET, 0 );

                    for ( i = c->F_GEN->imp; i != NULL; i = i->isucc )
                        if ( IsAggregate( i->info ) )
                            if ( IsAScatter( i->src ) )
                                i->pm = 0;
                    
                    break;

                case IFLoopA:
                case IFLoopB:
                    AssignCMPragmas( c->L_RET, 0 );
                    break;
                }
            }

        switch ( c->type ) {
            case IFCall:
                /* if ( c->bmark ) break; */
                /* NEW CANN 2/92 ASSUME ALL ARE OPTIMIZED WRITES */
                if ( c->bmark ) {
                  for ( i = c->imp->isucc; i != NULL; i = i->isucc ) {
                    if ( !IsAggregate( i->info ) )
                      continue;

                    i->cm = 0;
                    }

                  break;
                  }

                g = FindFunction( c->imp->CoNsT );

                for ( i = c->imp->isucc; i != NULL; i = i->isucc ) {
                    if ( !IsAggregate( i->info ) )
                        continue;

                    /* USED IN THE FUNCTION?                              */
                    if ( (e = FindExport( g, i->iport - 1 )) == NULL )
                        continue;
                        
                    if ( !IsEmptySet( e->lwset ) )
                        i->cm = 0;
                    }

                break;

            case IFLGraph:
            case IFXGraph:
                /* if ( c->bmark ) break; */
                /* NEW CANN 2/92 ASSUME ALL ARE OPTIMIZED WRITES */
                if ( c->bmark ) {
                  for ( e = c->exp; e != NULL; e = e->esucc ) {
                    if ( !IsAggregate( e->info ) )
                        continue;

                    e->pm--;
                    e->grset->gen = e;
                    e->gwset->gen = e;
                    }

                  break;
                  }

                for ( e = c->exp; e != NULL; e = e->esucc ) {
                    if ( !IsAggregate( e->info ) )
                        continue;

                    if ( !IsEmptySet( e->lwset ) ) {
                        e->pm--;
                        e->grset->gen = e;
                        e->gwset->gen = e;
                        }
                    }

                break;

            case IFSelect:
            case IFTagCase:
                for ( i = c->imp; i != NULL; i = i->isucc ) {
                    if ( !IsAggregate( i->info ) )
                        continue;

                    if ( !(i->wmark) )
                        continue;

                    i->cm = 0;

                    if ( IsSelect( c ) )
                        g = c->C_SUBS->gsucc;
                    else
                        g = c->C_SUBS;

                    for ( /* NOTHING */; g != NULL; g = g->gsucc )
                        if ( !ChangeToGenerator( g, i->iport ) ) {
                            e = AddGround( g, i->iport, g, i->info, 0, -2 );
                            e->lwset = SetAlloc( NULL_SET, g );

                            EnterInSet( e->lwset, e );
                            }
                    }

                break;

            case IFForall:
                for ( i = c->imp; i != NULL; i = i->isucc ) {
                    if ( !IsAggregate( i->info ) )
                        continue;

                    if ( IsExport( c->F_BODY, i->iport ) || 
                         IsExport( c->F_RET,  i->iport )  )
                        continue;

                    if ( !i->wmark )
                        continue;

                    i->cm = 0;
                    DecrementPmValues( c->F_GEN, i->iport );
                    }

                for ( i = c->F_BODY->imp; i != NULL; i = i->isucc ) {
                    if ( !IsAggregate( i->info ) )
                        continue;

                    ChangeToGenerator( c->F_RET, i->iport );
                    i->cm = 0;
                    }

                break;

            case IFLoopA:
            case IFLoopB:
                for ( i = c->imp; i != NULL; i = i->isucc ) {
                    if ( !IsAggregate( i->info ) )
                        continue;

                    if ( IsExport( c->L_BODY, i->iport ) || 
                         IsExport( c->L_RET,  i->iport )  )
                        continue;

                    i->cm = 0;
                    DecrementPmValues( c->L_INIT, i->iport );
                    }

                for ( i = c->L_INIT->imp; i != NULL; i = i->isucc ) {
                    if ( !IsAggregate( i->info ) )
                        continue;

                    if ( IsExport( c->L_BODY, i->iport ) ) {
                        if ( IsExport( c->L_RET, i->iport ) )
                            RefBodyAndRet( c, i );
                        else
                            RefBodyOnly( c, i );
                        }
                    else if ( IsExport( c->L_RET, i->iport ) )
                        RefRetOnly( c, i );
                    else
                        NotRef( c, i );
                    }

                break;

            default:
                break;
            }
        }
}


/**************************************************************************/
/* LOCAL  **************  OptNodeReferenceCounts   ************************/
/**************************************************************************/
/* PURPOSE: EXAMINING ALL GLOBAL READ/WRITE SETS ELIMINATE ALL REFERENCE  */
/*          COUNTING NOT REQUIRED BECAUSE OF EXECUTION ORDERING (ENFORCED */
/*          BY ADES).  IF A GLOBAL WRITE SET IS EMPTY THEN THE ASSOCIATED */
/*          AGGREGATE IS READ ONLY AND REQUIRES NO REFERENCE COUNTING. IF */
/*          AN AGGREGATES GLOBAL WRITE SET IS NOT EMPTY THEN AN A READ    */
/*          REFERENCE NEEDN'T BE REFERENCE COUNTED IF IT EXECUTES BEFORE  */
/*          A WRITE OPTERATION DEFINED IN ITS LOCAL WRITE SET OR BEFORE   */
/*          ALL WRITES IN THE GLOBAL WRITE SET.  IF THE SOURCE NODE OF AN */
/*          AGGREGATE IS CLASSIFIED AS ITS GENERATOR (ITS ABSOLUTE ORIGIN,*/
/*          FAKED FOR SOME GRAPH'S EXPORTS) AND THE AGGREGATE'S LOCAL     */
/*          WRITE SET IS EMPTY, THEN A DATA GROUND REFERENCE IS INSERTED. */
/**************************************************************************/

static void OptNodeReferenceCounts()
{
    register PSET  grset;
    register PSET  gwset;
    register PEDGE e;
    register int   r;
    register PEDGE ee;

    for ( grset = gshead; grset != NULL; grset = gwset->ssucc ) {
        gwset = grset->ssucc;

        if ( IsEmptySet( gwset ) )
            for ( r = 0; r <= grset->last; r++ )
                NeutralizeEdge( grset->set[r] );
        else
            for ( r = 0; r <= grset->last; r++ ) {
                e = grset->set[r];

                if ( BeforeAWrite( e, e->lwset ) )
                    NeutralizeEdge( e );
                else if ( BeforeAllWrites( e, gwset ) )
                    NeutralizeEdge( e );
                }

        if ( (e = gwset->gen) == NULL )
            continue;

        if ( e->sr != -2 ) {
            if ( e->sr == 0 ) {
                ee = AddGround( e->src, e->eport, gwset->graph, e->info, -2, 1);
                ee->lwset = e->lwset;

                EnterInSet( e->lwset, ee );
                BindProducerPragmas( e, -2, 1 );
                }

            continue;
            }

        if ( e->pm == -1 ) {
            ee = AddGround( e->src, e->eport, gwset->graph, e->info, 0, -2 ); 
            ee->lwset = e->lwset;

            EnterInSet( e->lwset, ee );
            BindProducerPragmas( e, 0, -2 );
            }
        }
}


/**************************************************************************/
/* GLOBAL **************   If2ReferenceCountOpt    ************************/
/**************************************************************************/
/* PURPOSE: ELIMINATE ALL UNNECESSARY REFERENCE COUNT OPERATIONS IN ALL   */
/*          FUNCTIONS.  FIRST UNNECESSARY REFERENCE COUNTING BECAUSE OF   */
/*          FUNCTION GRAPH AND SUBGRAPH BOUNDARIES IS ELIMINATED. THEN    */
/*          UNNECESSAY REFERENCE COUNTING BECAUSE OF ADE ORDERINGS IS     */
/*          ELIMINATED.                                                   */
/**************************************************************************/

void If2ReferenceCountOpt()
{
    OptBoundaryReferenceCounts();
    OptNodeReferenceCounts();
}
