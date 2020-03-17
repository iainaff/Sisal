/**************************************************************************/
/* FILE   **************         if1fold.c         ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log:
 */
/**************************************************************************/

#include "world.h"

static PNODE    FoldNode;

/**************************************************************************/
/* MACRO  **************      BooleanToInteger     ************************/
/**************************************************************************/
/* PURPOSE: CONVERT IF1 BOOLEAN CONSTANT b TO ITS INTEGER REPRESENTATION. */
/**************************************************************************/

#define BooleanToInteger(b)   ( ((*(b) == 'T') || (*(b) == 't'))? 1 : 0 )


/**************************************************************************/
/* MACRO  **************      IntegerToBoolean     ************************/
/**************************************************************************/
/* PURPOSE: CONVERT INTEGER i INTO ITS IF1 BOOLEAN REPRESENTATION.        */
/**************************************************************************/

#define IntegerToBoolean( i ) ( ((i) == 1)? "TRUE" : "FALSE" )


/**************************************************************************/
/* LOCAL  **************      FindLowerBound       ************************/
/**************************************************************************/
/* PURPOSE: ATTEMPT TO FIND AND RETURN THE EDGE DEFINING THE LOWER BOUND  */
/*          OF EDGE e.                                                    */
/**************************************************************************/

static PEDGE FindLowerBound( e )
PEDGE e;
{
  register PNODE src;
  register PNODE n;
  register PEDGE ee;

  if ( e == NULL )
    return( NULL );

  if ( IsConst( e ) )
    return( NULL );

  src = e->src;

  if ( UsageCount( src, e->eport ) != 1 )
    return( NULL );

  switch ( src->type ) {
    case IFAAddH:
    case IFACatenate:
      return( FindLowerBound( src->imp ) );

    case IFLoopA:
    case IFLoopB:
      if ( (ee = FindImport( src->L_RET, e->eport )) == NULL )
        return( NULL );
      goto MoveOn;
    case IFForall:
      if ( (ee = FindImport( src->F_RET, e->eport )) == NULL )
        return( NULL );
MoveOn:

      if ( ee->esucc != NULL )
        return( NULL );

      n = ee->src;

      switch ( n->type ) {
        case IFAGather:
          return( n->imp );
        
        case IFRedLeft:
        case IFRedRight:
        case IFRedTree:
        case IFReduce:
          if ( n->imp->CoNsT[0] != REDUCE_CATENATE )
            return( NULL );

          /* IS THE SECOND IMPORT THE LOWER BOUND? */
          if ( !native )
            return( NULL );

          return( n->imp->isucc );

        default:
          ;
        }
      return( NULL );

    default:
      break;
    }

  return( NULL );
}


/**************************************************************************/
/* LOCAL  **************       WasReduced          ************************/
/**************************************************************************/
/* PURPOSE: ATTEMPT TO ELIMINATE OR REDUCE NODE n, AND RETURN THE STATUS. */
/**************************************************************************/

static int WasReduced( n )
PNODE n;
{
  register PNODE dst;
  register PEDGE e;
  register PEDGE i;
  register PNODE l;
  register PEDGE ee;
  register PEDGE lo1;
  register PEDGE lo2;
  register int   port;
  register PEDGE se;

  switch ( n->type ) {
    case IFNeg:
      if ( n->exp == NULL )
        return( FALSE );

      if ( n->exp->esucc != NULL )
        return( FALSE );

      dst = n->exp->dst;

      switch ( dst->type ) {
        case IFPlus:
          if ( n->exp->iport == 1 )
            ImportSwap( dst );
    
          dst->type = IFMinus;
          break;

        case IFMinus:
          if ( n->exp->iport == 1 )
            return( FALSE );

          dst->type = IFPlus;
          break;

        default:
          return( FALSE );
        }

      i = n->imp;
      e = n->exp;
    
      UnlinkImport( e );
      UnlinkImport( i );

      i->iport = e->iport;
      LinkImport( dst, i );

      UnlinkNode( n );
      negcnt++;

      return( TRUE );

    case IFASetL:
      lo1 = n->imp->isucc;

      if ( !IsConst( lo1 ) )
        if ( !IsGraph( lo1->src ) )
          return( FALSE );

      if ( (lo2 = FindLowerBound( n->imp )) == NULL )
        return( FALSE );

      l = lo2->dst->exp->dst->G_DAD;

      if ( !IsConst( lo1 ) ) {
        port = ++maxint;

        if ( IsConst( lo2 ) ) {
          lo2->CoNsT = NULL;
          lo2->eport = port;
          LinkExport( lo2->dst->exp->dst, lo2 ); 
        } else {
          UnlinkExport( lo2 );
          lo2->eport = port;
          LinkExport( lo2->dst->exp->dst, lo2 );
          }

        ee = EdgeAlloc( lo1->src, lo1->eport, l, port );
        ee->info = lo1->info;
        LinkExport( lo1->src, ee );
        LinkImport( l, ee );
        }
      else
        ChangeEdgeToConst( lo2, lo1 );

      for ( e = n->exp; e != NULL; e = se ) {
        se = e->esucc;

        UnlinkExport( e );
        e->eport = n->imp->eport;
        LinkExport( n->imp->src, e );
        }

      UnlinkExport( n->imp );
      UnlinkNode( n );
      /* free( n ); */

      setlcnt++;
      return( TRUE );

    default:
      ;
    }

  return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************   IsLogicalShortCircuit   ************************/
/**************************************************************************/
/* PURPOSE: FOLD THE FOLLOWING LOGICAL SUBEXPRESSIONS: TRUE|x, x|TRUE,    */
/*             TRUE|x -> TRUE, x|TRUE -> TRUE, FALSE&x -> FALSE,          */
/*             x&FALSE -> FALSE, x|FALSE -> x, FALSE|x -> x,              */
/*             x&TRUE -> x, TRUE&x -> x                                   */
/**************************************************************************/

static int IsLogicalShortCircuit( n )
PNODE n;
{
  register int  tv;
  register int  v;
  register int  itv;
           EDGE c;

  switch ( n->type ) {
    case IFPlus:  /* | */
    case IFTimes: /* & */
      if ( n->imp->info->type != IF_BOOL )
        break;

      tv  = (n->type == IFTimes)? 0 : 1;
      itv = (n->type == IFTimes)? 1 : 0;

      c.info  = n->imp->info;
      c.src   = NULL;
      c.eport = CONST_PORT;
      c.CoNsT = CopyString( IntegerToBoolean( tv ) );

      if ( IsConst( n->imp ) ) {
        v = BooleanToInteger(n->imp->CoNsT);

        /* T|?->T, F&?->F */
        if ( v == tv ) {
          ChangeExportsToConst( n, 1, &c );
          OptRemoveDeadNode( n );
          lfcnt++;
          return( TRUE );
          }

        if ( v == itv )
          if ( !IsConst( n->imp->isucc ) ) {
            ChangeExportPorts( n, 1, n->imp->isucc->eport );
            LinkExportLists( n->imp->isucc->src, n );
            OptRemoveDeadNode( n );
            lfcnt++;
            return( TRUE );
            }
        }

      if ( IsConst( n->imp->isucc ) ) {
        v = BooleanToInteger(n->imp->isucc->CoNsT);

        /* ?|T->T, ?&F->F */
        if ( v == tv ) {
          ChangeExportsToConst( n, 1, &c );
          OptRemoveDeadNode( n );
          lfcnt++;
          return( TRUE );
          }

        if ( v == itv )
          if ( !IsConst( n->imp ) ) {
            ChangeExportPorts( n, 1, n->imp->eport );
            LinkExportLists( n->imp->src, n );
            OptRemoveDeadNode( n );
            lfcnt++;
            return( TRUE );
            }
        }

      break;

    default: 
      break;
    }

  return( FALSE );
}


/**************************************************************************/
/* GLOBAL **************     OptNormalizeNode      ************************/
/**************************************************************************/
/* PURPOSE: IF NODE n IMPORTS A CONSTANT AND n IS A Plus OR Times NODE,   */
/*          THEN MAKE THE CONSTANT THE SECOND IMPORT.                     */
/**************************************************************************/

void OptNormalizeNode( n )
PNODE n;
{
  switch ( n->type ) {
    case IFPlus:
    case IFTimes:
      break;

    default:
      return;
    }

  if ( IsConst( n->imp ) ) {
    ImportSwap( n );
    return;
    }

  if ( OptIsEdgeInvariant( n->imp ) )
    ImportSwap( n );
}


/**************************************************************************/
/* LOCAL  **************    NormalizeIndexing      ************************/
/**************************************************************************/
/* PURPOSE: IF e's SOURCE IS A PLUS OR MINUS NODE, CHECK IF ITS FIRST     */
/*          IMPORT IS ALSO A PLUS OR MINUS NODE WITH MORE THAN ONE EXPORT.*/
/*          IF THE TWO NODES ARE CANDIDATE FOR CHAIN FOLDING, THEN        */
/*          BREAK THE DEPENDENCE BETWEEN THE TWO NODES TO FACILITATE      */
/*          OPTIMIZATIONS DONE BY ImproveIndexing IN if2gen.              */
/**************************************************************************/

static void NormalizeIndexing( e )
PEDGE e;
{
  register int   v0;
  register int   v1;
  register PNODE n;
  register PNODE nn;
           char  buf[100];

  if ( !normidx )
    return;

  if ( IsConst( e ) )
    return;

  n = e->src;

  if ( !(IsPlus(n) || IsMinus(n)) )
    return;

  OptNormalizeNode( n );

  if ( !IsConst( n->imp->isucc ) )
    return;

  if ( IsConst( n->imp ) )
    return;

  nn = n->imp->src;

  if ( !(IsPlus(nn) || IsMinus(nn)) )
    return;

  OptNormalizeNode( nn );

  if ( nn->exp->esucc == NULL )
    return;

  if ( !IsConst( nn->imp->isucc ) )
    return;

  if ( IsConst( nn->imp ) )
    return;

  UnlinkExport( n->imp );
  n->imp->eport = nn->imp->eport;
  LinkExport( nn->imp->src, n->imp );

  v0 = atoi( n->imp->isucc->CoNsT );

  if ( IsMinus(n) ) {
    v0 = -v0;
    n->type = IFPlus;
    }

  v1 = atoi( nn->imp->isucc->CoNsT );

  if ( IsMinus(nn) )
    v1 = -v1;

  SPRINTF( buf, "%d", v0 + v1 );
  n->imp->isucc->CoNsT = CopyString( buf );
  idxm++;
}

/**************************************************************************/
/* STATIC **************        AsciiToLong        ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE POSSIBLY SIGNED INTEGER REPRESENTED BY STRING c.   */
/**************************************************************************/

static long AsciiToLong( c, b )
char *c;
int   b;
{
    register long v = 0;
    register long s = 1;

    if ( ( *c == '-' ) || ( *c == '+' ) ) {
        if ( *c == '-' )
            s = -1;

        c++;
        }

    for ( ; *c != '\0'; c++ )
        v = (v * b) + (*c - '0');

    return( v * s );
}


/**************************************************************************/
/* LOCAL  **************         FoldExp           ************************/
/**************************************************************************/
/* PURPOSE: STRENGTH REDUCE SELECTIVE IFExp OPERATIONS: exp(n,2) -> n*n.  */
/**************************************************************************/

static void FoldExp( n )
PNODE n;
{
  register PEDGE  i2;
  register PEDGE  i1;
  register PNODE  pr;
  register PNODE  m;
  register PEDGE  ExpNode;
  register PEDGE  e;
  register PNODE  n0;
  register PNODE  n1;
  register PNODE  n2;
  register PNODE  n3;
  register int    v;
  register double vf;

  i1 = n->imp;

  if ( IsConst( i1 ) )
    return;

  i2 = i1->isucc;

  if ( !IsConst( i2 ) )
    return;

  switch( n->exp->info->type ) {
    case IF_REAL:
      vf = (double) atof( i2->CoNsT );
      v  = (int)vf;

      if ( ((double)v) - vf != 0 )
        return;

      break;

    case IF_DOUBLE:
      vf = (double) atof( DoubleToReal( i2->CoNsT ) );
      v  = (int)vf;

      if ( ((double)v) - vf != 0 )
        return;

      break;

    case IF_INTEGER:
      v = AsciiToLong( i2->CoNsT, BASE10 );
      break;

    default:
      return;
    }

  if ( v < 2 || v > 64 )
    return;

  /* FIX NODE n TO BE N*N */
  n->type = IFTimes;
  i2->eport = i1->eport;
  i2->CoNsT = NULL;
  /* ------------------------------------------------------------ */
  /* IFExp unfolding introduced an INTEGER edge where there should */
  /* have been a REAL edge.  921006 -- PJM */
  i2->info = i1->info;
  /* ------------------------------------------------------------ */
  LinkExport( i1->src, i2 );

  /* LINK IN THE OTHER MULTIPLIES AND REASSIGN n'S EXPORTS */

  ExpNode = n->exp;
  n->exp = NULL;

  m = n;
  pr = n;

  for ( v--; v > 1; v-- ) {
    m = NodeAlloc( maxint++, IFTimes );
    CopyPragmas( n, m );
    LinkNode( pr, m );

    e = EdgeAlloc( pr, 1, m, 1 );
    e->info = i1->info;
    LinkExport( pr, e );
    LinkImport( m, e );

    /* i2->eport IS ALREADY 2 (i2 is i1 at this point!) */
    CopyEdgeAndReset( i2, i2->src, m );

    pr = m;
    }

  m->exp = ExpNode;

  for ( e = m->exp; e != NULL; e = e->esucc )
    e->src = m;

  expcnt++;

  /* ONLY ONE TIMES IN THE CHAIN!!! */
  if ( n == pr )
    return;

  /* NOW NORMALIZE THE SEQUENCE SO CSE CAN REDUCE IT! */

  /* AT THIS POINT, pr IS THE LAST TIMES IN THE CHAIN! */
  n0 = n;
  n1 = n0;

  for ( ;; ) {
    /* GRAB THREE IFTimes NODES; THE FIRST AND THIRD CAN HAVE FANOUT */
    if ( !(n1->type == IFTimes) )
      goto MoveOn;

    if ( n1 == pr )
      goto MoveOn;

    n2 = n1->exp->dst;

    /* SHOULD NOT BE NEEDED, BUT... */
    if ( n2->type != IFTimes )
      goto MoveOn;

    if ( n2 == pr )
      goto MoveOn;

    if ( n2->exp->esucc != NULL )
      Error2( "FoldExp", "IFExp CHAIN CORRUPTION (FANOUT)!" );

    n3 = n2->exp->dst;

    if ( n3->type != IFTimes )
      goto MoveOn;

    /* OK, NOW DO A REDUCTION STEP */

    /* DETACH THE EDGE LINKING n2 and n3 */
    ExpNode = n2->exp;
    n2->exp = NULL;
    UnlinkImport( ExpNode );

    /* NOW MOVE n3 AND ITS EXPORT LIST */
    UnlinkNode( n3 );
    LinkNode( n1, n3 );
    n2->exp = n3->exp;
    n3->exp = NULL;

    for ( e = n2->exp; e != NULL; e = e->esucc )
      e->src = n2;

    /* DID WE JUST MOVE THE LAST NODE?  IF SO, FIX pr TO POINT TO THE */
    /* NEW LAST */
    if ( n3 == pr )
      pr = n2;

    /* MOVE n2'S SECOND IMPORT TO n3 */
    e = n2->imp->isucc;
    UnlinkImport( e );
    e->iport = 1;
    LinkImport( n3, e );

    /* LINK n3 TO n2 USING ExpNode */
    ExpNode->src   = n3;
    ExpNode->eport = 1;
    ExpNode->dst   = n2;
    ExpNode->iport = 2;
    LinkExport( n3, ExpNode );
    LinkImport( n2, ExpNode );

MoveOn:
    if ( n1 == pr || n2 == pr || n3 == pr ) {
      n0 = n0->exp->dst;
      n1 = n0;

      if ( n1 == pr )
        break;
      }
    else
      n1 = n1->exp->dst;
    }
}

/**************************************************************************/
/* LOCAL  **************         FoldDiv           ************************/
/**************************************************************************/
/* PURPOSE: Fold SELECTIVE IFDiv OPERATIONS: n/k -> n*(1/k)               */
/**************************************************************************/

static void FoldDiv( n )
PNODE n;
{
  register PEDGE  i2;
  register PEDGE  i1;
  register double vf;

  i1 = n->imp;

  if ( IsConst( i1 ) ) return;

  i2 = i1->isucc;

  if ( !IsConst( i2 ) ) return;

  switch( n->exp->info->type ) {
    case IF_REAL:
      vf = (double) atof( i2->CoNsT );
      break;

    case IF_DOUBLE:
      vf = (double) atof( DoubleToReal( i2->CoNsT ) );
      break;

    default:
      return;
    }

  /* ------------------------------------------------------------ */
  /* Don't divide by zero!                                        */
  /* ------------------------------------------------------------ */
  if ( vf == 0.0 ) return;

  /* ------------------------------------------------------------ */
  /* Invert the divisor and put back into the CoNsT field         */
  /* ------------------------------------------------------------ */
  vf = 1.0 / vf;
  i2->CoNsT = MyAlloc(32);
  sprintf(i2->CoNsT,"%.20e",vf);

  /* ------------------------------------------------------------ */
  /* Convert divide into a multiply                               */
  /* ------------------------------------------------------------ */
  n->type = IFTimes;
}


static double GetSpecialOperand( e )
PEDGE  e;
{
  register double val;

  switch ( e->info->type ) {
    case IF_INTEGER:
      val = (double) atoi( e->CoNsT );
      break;

    case IF_REAL:
      val = (double) atof( e->CoNsT );
      break;

    case IF_DOUBLE:
      val = atof( DoubleToReal( e->CoNsT ) );
      break;

    default:
    FPRINTF( infoptr, "type=%d if1line=%d\n", e->info->type, e->if1line );
      Error2( "GetSpecialOperand", "EDGE TYPE ERROR" );
    }

  return( val );
}


static void StoreSpecialConst( e, val )
PEDGE  e;
double val;
{
  char a[ANSWER_SIZE];

  switch ( e->info->type ) {
    case IF_INTEGER:
      SPRINTF( a, "%d", (int) val );
      break;

    case IF_DOUBLE:
    case IF_REAL:
      SPRINTF( a, "%.16e", val );
      break;

    default:
      Error2( "StoreSpecialConst", "EDGE TYPE ERROR" );
    }

  e->CoNsT = CopyString( a );
}


static int IsSpecialCandidate( n )
PNODE n;
{
  switch( n->type ) {
    case IFPlus:
    case IFTimes:
      if ( IsBoolean( n->exp->info ) )
        return( FALSE );

    case IFMinus:
    case IFDiv:
    case IFNeg:
      return( TRUE );

    default:
      return( FALSE );
    }
}


/**************************************************************************/
/* LOCAL  **************     HandleUselessNode     ************************/
/**************************************************************************/
/* PURPOSE: NODE n HAS BEEN FOLDED INTO CONSTANT val AND IS NOT LONGER    */
/*          NEEDED.                                                       */
/**************************************************************************/

static void HandleUselessNode ( n, val )
PNODE  n;
double val;
{
    register PEDGE e;
    register PEDGE se;

    for ( e = n->exp ; e != NULL ; e = se ) {
        se = e->esucc;

        StoreSpecialConst( e, val );
        e->eport = CONST_PORT;
        e->esucc = e->epred = NULL;
        e->src   = NULL;
        }

    /* MAKE NODE n A DEAD NODE                                            */
    n->exp = NULL;
    OptRemoveDeadNode( n );
}


/**************************************************************************/
/* LOCAL  **************   OptSkipIdentityNode     ************************/
/**************************************************************************/
/* PURPOSE: THE DESTINATION OF EDGE i IS AN IDENTITY NODE OF THE FORM     */
/*          A - 0, 0 + A, A + 0, 1 * A, A * 1, OR A / 1 WHERE A IS THE    */
/*          NON-CONSTANT EDGE i. THE EXPORTS OF NODE n ARE CONVERTED INTO */
/*          REFERENCES TO i, CONVERTING n INTO A DEAD NODE.               */
/**************************************************************************/

static void OptSkipIdentityNode( i )
PEDGE i;
{
    register PEDGE e;
    register PEDGE se;

    /* MOVE i->dst EXPORT REFERENCES TO i->src                            */

    for ( e = i->dst->exp ; e != NULL ; e = se ) {
        se = e->esucc;

        UnlinkExport( e );
        e->eport = i->eport;
        LinkExport( i->src, e);
        }

    /* PREVIOUS LOOP WILL SET i->dst->exp TO NULL                         */
    OptRemoveDeadNode( i->dst );
    ident_cnt++;
}


/**************************************************************************/
/* LOCAL  **************      IsSpecialChain       ************************/
/**************************************************************************/
/* PURPOSE: CHECK IF NODE n HEADS A SINGLE THREADED CHAIN OF PLUS OR      */
/*          MINUS NODES.  CONSTANT c1 IS THE SECOND IMPORT TO n, ITSELF   */ 
/*          A PLUS OR MINUS NODE.  IF THE CHAIN CONTAINS ANOTHER NODE     */
/*          WITH A CONSTANT SECOND IMPORT, THE NODE IS REMOVED.  FOR      */
/*          EXAMPLE, (A + c1 - B + c2) BECOMES (A + c3 - B) WHERE         */
/*          c3 = c1 +c2. IF A CHAIN IS FOLDED, TRUE IS RETURNED. NOTE:    */
/*          e->CoNsT DEFINES c1.                                          */
/**************************************************************************/


static int IsSpecialChain( c1, n, e )
double  c1;
PNODE   n;
PEDGE   e;
{
    register PNODE  prevn;
    register double op1;

    if ( (!IsOneExport( n )) || (n->exp->iport != 1) )
        return( FALSE );

    /* BUG FIX: 5/28/91 */
    if ( IsBoolean( n->exp->info ) )
        return( FALSE );

    for ( ;; ) {
        prevn = n;
        n     = n->exp->dst;

        if ( !(IsPlus( n ) || IsMinus( n )) )
            return( FALSE );

        if ( !IsConst( n->imp->isucc ) ) {
            if ( (!IsOneExport( n )) || (n->exp->iport != 1) )
                return( FALSE );

            continue;
            }

        break;
        }

    /* A FOLDABLE CHAIN EXISTS; HENCE, FOLD IT                            */
    chain_cnt++;

    op1 = GetSpecialOperand( n->imp->isucc );

    if ( IsMinus( n ) )
        op1 = -op1;

    StoreSpecialConst( e, op1 + c1 );

    /* APPEND THE EXPORT LIST OF n TO prevn, MAKING n A DEAD NODE         */

    LinkExportLists( prevn, n );
    n->exp = NULL;
    OptRemoveDeadNode( n );

    return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************    FoldSpecialNodes       ************************/
/**************************************************************************/
/* PURPOSE: THE FOLLOWING PATTERNS ARE FOLDED:                            */
/*                                                                        */
/*       1) A + 0, 0 + A, A * 0, 0 * A, A - 0, AND A / 1                  */
/*       2) (A [+/-] Const1) [+/-]... [[+/-] Const2] WHERE EACH EXPORT IS */
/*          REFERENCED ONCE                                               */
/*       3) A - (-Const)                                                  */
/*       4) A + (-Const)                                                  */
/*       5) -(A - B)                                                      */
/*       6) A / -1                                                        */
/*                                                                        */
/**************************************************************************/


static void FoldSpecialNodes( g )
PNODE g;
{
    register PNODE  n;
    register PEDGE  i1;
    register PEDGE  i2;
    register int    const1;
    register int    const2;
    register double op1;
    register double op2;
    register PNODE  sn;
    register PNODE  sg;

    for ( n = g->G_NODES ; n != NULL ; n = sn ) {
        sn = n->nsucc;

        if ( IsCompound( n ) ) {
            PropagateConst( n );

            for ( sg = n->C_SUBS ; sg != NULL ; sg = sg->gsucc )
                FoldSpecialNodes( sg );

            continue;
            }

        if ( !IsSpecialCandidate( n ) )
            continue;

        OptNormalizeNode( n );

        i1 = n->imp;
        i2 = i1->isucc;
    
        const1 = const2 = FALSE;

        /* FETCH OPERANDS: CAN'T BE ERROR VALUE CONSTANTS!                */

        if ( IsConst( i1 ) ) {
            const1 = TRUE;
            op1 = GetSpecialOperand( i1 );
            }

        if ( i2 != NULL )
          if ( IsConst( i2 ) ) {
              const2 = TRUE;
              op2 = GetSpecialOperand( i2 );
              }

        /* ONE OR THE OTHER BUT NOT BOTH */
        if ( !IsNeg( n ) )
            if ( !(const1 ^ const2) )
                continue;

        switch ( n->type ) {
            case IFMinus:
                if ( const1 ) {
                  if ( op1 == 0.0 ) {             /* ZERO MINUS NON-CONSTANT */
                    UnlinkImport( i1 );
                    /* free( i1 ); */
                    n->exp->iport = 1;
                    n->type = IFNeg;
                    ident_cnt++;
                    }

                  break;
                  }
    
                if ( op2 == 0.0 ) {              /* NON-CONSTANT MINUS ZERO */
                    OptSkipIdentityNode( i1 );
                    break;
                    }

                if ( IsSpecialChain( -op2, n, i2 ) ) {
                    n->type = IFPlus;
                    sn = n;                             /* PROCESS n AGAIN */
                    break;
                    }

                if ( op2 < 0.0 ) {        /* A - (Const) WHERE Const < 0  */
                    n->type = IFPlus; pncnt++;
                    i2->CoNsT++;
                    }

                break;

            case IFPlus:
                if ( const1 && (op1 == 0.0) ) {   /* ZERO PLUS NON-CONSTANT */
                    OptSkipIdentityNode( i2 );
                    break;
                    }

                if ( const2 && (op2 == 0.0) ) {   /* NON-CONSTANT PLUS ZERO */
                    OptSkipIdentityNode( i1 );
                    break;
                    }

                if ( const1 )
                    break;

                if ( IsSpecialChain( op2, n, i2 ) ) {
                    sn = n;                           /* PROCESS n AGAIN  */
                    break;
                    }

                if ( op2 < 0.0 ) {
                    n->type = IFMinus; pncnt++;
                    i2->CoNsT++;
                    }

                break;

            case IFTimes:
                if ( const1 ) {
                    if ( op1 == 1.0 ) {           /* ONE TIMES NON-CONSTANT */
                        OptSkipIdentityNode( i2 );
                        break;
                        }
    
                    if ( op1 == 0.0 ) {          /* ZERO TIMES NON-CONSTANT */
                        zero_cnt++;
                        HandleUselessNode( n, 0.0 );
                        break;
                        }
    
                    break;
                    }
    
                if ( const2 ) {
                    if ( op2 == 1.0 ) {           /* NON-CONSTANT TIMES ONE */
                        OptSkipIdentityNode( i1 );
                        break;
                        }
    
                    if ( op2 == 0.0 ) {          /* NON-CONSTANT TIMES ZERO */
                        zero_cnt++;
                        HandleUselessNode( n, 0.0 );
                        }
                    }
    
                break;
            
            case IFDiv:
                if ( const2 ) {
                    if ( op2 == 0.0 ) break;

                    if (op2 == 1.0 ) {              /* NON-CONSTANT DIV ONE */
                        OptSkipIdentityNode( i1 );
                        break;
                        }

                    if (op2 == -1.0) {                  /* NON-CONST DIV -1 */
                        dncnt++;
                        UnlinkImport( i2 );
                        n->type = IFNeg;
                        sn = n;                     /* START AGAIN WITH n */
                        }
                    }

                break;

            case IFNeg:
                if ( const1 )
                    break;
        
                /* CHECK FOR -(A-B)                                       */

                if ( IsMinus( i1->src ) && IsOneExport( i1->src ) ) {
                    pncnt++ ;

                    /* SWAP IMPORTS TO THE MINUS NODE AND MAKE n DEAD     */
                    ImportSwap( i1->src );
    
                    LinkExportLists( i1->src, n );
                    n->exp = NULL;
                    OptRemoveDeadNode( n );
                    }

                break;
         

            default:
                break;
            } 
        }
}


/**************************************************************************/
/* LOCAL  **************    FoldExceptionHandler   ************************/
/**************************************************************************/
/* PURPOSE: HANDLES ARITHMETIC EXCEPTIONS FOR CONSTANT FOLDER.            */
/**************************************************************************/

static void FoldExceptionHandler( sig )
  int   sig;
{
  char  ebuf[256];

  if ( !FoldNode ) Error1("ARITHMETIC EXCEPTION DURING CONSTANT FOLDING");

  SPRINTF(ebuf,"ARITHMETIC EXCEPTION DURING CONSTANT FOLD %s:%d (%d)\n",
          (FoldNode->file)?(FoldNode->file):"???",
          (FoldNode->line>0)?(FoldNode->line):0,
          (FoldNode->if1line>0)?(FoldNode->if1line):0
          );
  Error1(ebuf);
}


/**************************************************************************/
/* LOCAL  **************        RemoveBranch       ************************/
/**************************************************************************/
/* PURPOSE: REPLACE SELECT NODE s WITH THE NODES FROM ITS LIVE BRANCH AND */
/*          APPROPRIATELY WIRE ALL EXPORT AND IMPORT REFERENCES. THE DEAD */
/*          BRANCH AND s ARE DESTROYED.                                   */
/**************************************************************************/

static PNODE RemoveBranch( s )
PNODE s;
{
    register PNODE sg;
    register PEDGE e;
    register PEDGE i;
    register PEDGE se;
    register PEDGE si;
    register PNODE ln;
    register PNODE sn;

    if ( !native )
        return( s->nsucc );

    /* WHICH BRANCH IS LIVE?                                              */
    sg = ( strcmp( s->S_TEST->imp->CoNsT, "FALSE" ) == 0 )? 
                                      s->S_ALT : s->S_CONS;

    /* LINK THE LIVE BRANCH'S EXPORTS TO THEIR SOURCES OUTSIDE s; NOTE: A */
    /* SOURCE MAY BE A CONSTANT.                                          */

    for ( e = sg->exp; e != NULL; e = se ) {
        se = e->esucc;

        i = FindImport( s, e->eport );

        if ( !IsConst( i ) ) {
            e->eport = i->eport;
            UnlinkExport( e );
            LinkExport( i->src, e );
            }
        else
            ChangeEdgeToConst( e, i );
        }

    /* LINK s'S EXPORTS TO THEIR SOURCES WITHIN THE LIVE BRANCH           */

    for ( e = s->exp; e != NULL; e = se ) {
        se = e->esucc;

        i = FindImport( sg, e->eport );

        if ( !IsConst( i ) ) {
            e->eport = i->eport;
            UnlinkExport( e );
            LinkExport( i->src, e );
            }
        else
            ChangeEdgeToConst( e, i );
        }

    /* REMOVE sg's IMPORTS AS THEY ARE NOW DEAD REFERENCES                */

    for ( i = sg->imp; i != NULL; i = si ) {
        si = i->isucc;
        RemoveDeadEdge( i );
        }

    /* MOVE THE LIVE BRANCH'S NODE LIST OUTSIDE s AND DESTROY s           */
    if ( sg->G_NODES != NULL ) {
        ln = s->nsucc;

        if ( sg->G_NODES != NULL ) {
            s->nsucc        = sg->G_NODES;
            s->nsucc->npred = s;
            }

        if ( ln != NULL ) {
            ln->npred = FindLastNode( sg->G_NODES );
            ln->npred->nsucc = ln;
            }

        sg->G_NODES = NULL;
        sg->imp = sg->exp = NULL;
        }

    sn = s->nsucc;
    OptRemoveDeadNode( s );

    sfcnt++;

    return( sn );
}


/**************************************************************************/
/* STATIC **************       CharToInteger       ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE INTEGER REPRESENTING CHARACTER LITERAL c.  THE     */
/*          FOLLOWING CHARACTER FORMATS ARE ASSUMED: \OctalDigits,        */
/*          \SpecialCharacter, \NonSpecialCharacter, AND Character.       */
/**************************************************************************/

static long CharToInteger( c )
char *c;
{
    register int v;

    if ( *c == '\\' ) {
        c++;

        if ((*c >= '0') && (*c <= '7'))
            v = AsciiToLong( c, BASE8 );
        else
            switch ( *c ) {
                case 'n': case 'N': v = '\n'; break;
                case 'f': case 'F': v = '\f'; break;
                case 'b': case 'B': v = '\b'; break;
                case 'r': case 'R': v = '\r'; break;
                case 't': case 'T': v = '\t'; break;
                default:            v = *c;   break;
                }
        }
    else
        v = (long) *c;

    return( v );
}


/**************************************************************************/
/* GLOBAL **************       DoubleToReal        ************************/
/**************************************************************************/
/* PURPOSE: CONVERT IF1 DOUBLE CONSTANT d TO AN IF1 REAL CONSTANT--A FORM */
/*          MATH FUNCTION atof CAN HANDLE.                                */
/**************************************************************************/

char *DoubleToReal( d )
char *d;
{
    register char *p;

    for ( p = d; *p != '\0'; p++ )
        if ( (*p == 'D') || (*p == 'd') )
           *p = 'e';

    return( d );
}


/**************************************************************************/
/* LOCAL  **************        GetOperands        ************************/
/**************************************************************************/
/* PURPOSE: FORM THE INTERNAL REPRESENTATION OF EACH LITERAL IMPORTED TO  */
/*          NODE n (ASSUMED TO BE EITHER MONADIC OR DYADIC).  IF n IS     */
/*          MONADIC THE ONLY IMPORTED LITERAL IS PLACED IN EITHER iop1 OR */
/*          dop1. IF n IS DYADIC, THE FIRST LITERAL IS PLACED IN EITHER   */
/*          iop1 OR dop1 AND THE SECOND IS PLACED IN EITHER iop2 OR dop2. */
/*          INTEGER, CHARACTER, AND BOOLEAN LITERALS ARE REPRESENTED BY   */
/*          iop1 AND iop2.  REAL AND DOUBLE_REAL LITERALS ARE REPRESETNED */
/*          BY dop1 AND dop2. AS A SIDE EFFECT, ALL DOUBLE_REAL CONSTANTS */
/*          ARE COERCED TO REAL (D or d EXPONENT SPECIFIERS CHANGED TO e);*/
/*          IT IS ASSUMED THAT if1write WILL CORRECT THIS.                */
/**************************************************************************/
static int  GetConstantEdge(ival,dval,E)
     long       *ival;
     double     *dval;
     PEDGE      E;
{
  if ( !E || !(E->CoNsT) ) return ERROR;

  switch ( E->info->type ) {
  case IF_FUNCTION:
    return OK;

  case IF_INTEGER:
    *ival = AsciiToLong( E->CoNsT, BASE10 );
    return OK;

  case IF_BOOL:
    *ival =  BooleanToInteger( E->CoNsT );
    return OK;

  case IF_CHAR:
    *ival = CharToInteger( E->CoNsT );
    return OK;

  case IF_REAL:
    *dval = atof( E->CoNsT );
    return OK;

  case IF_DOUBLE:
    *dval = atof( DoubleToReal( E->CoNsT ) );
    return OK;
  }

  return( ERROR );
}

static int GetOperands( n )
PNODE n;
{
  int   stat;

  stat = GetConstantEdge(&iop1,&dop1,n->imp);
  if ( stat == ERROR ) return ERROR;

  stat = GetConstantEdge(&iop2,&dop2,n->imp->isucc);
  if ( stat == ERROR ) return ERROR;

  return( OK );
}


/**************************************************************************/
/* STATIC **************           Fold            ************************/
/**************************************************************************/
/* PURPOSE: FOLD NODE n IF IT IS AN ARITHMETIC, COERSION, OR COMPARISION  */
/*          OPERATION WITH CONSTANT IMPORTS.  AS A SIDE EFFECT, SIGNED    */
/*          LITERALS MAY BE PRODUCED. SIGNED LITERALS ARE NOT SUPPORTED   */
/*          BY LLNL SOFTWARE; IT IS ASSUMED THAT IF1Write WILL CORRECT    */
/*          THIS BY ADDING IFNeg NODES AND REMOVING MINUS SIGNES. NODES   */
/*          IMPORTING ERROR CONSTANTS ARE NOT FOLDED.                     */
/**************************************************************************/

static void Fold( n )
PNODE n;
{
    register PEDGE  i;
    register PEDGE  e;
    register PEDGE  se;
    register char  *r;
    register int    cnt;
    register int    b;
             char   a[ANSWER_SIZE];

    FoldNode = n;               /* In case we trap! */

    /* ? = control -> control = ? FOR LOOP SPLITTING (ALSO IN If1Normal) */
    if ( n->type == IFEqual ) 
      if ( !IsConst( n->imp->isucc ) )
        if ( IsSGraph( n->imp->isucc->src ) )
          if ( !IsImport( n->imp->isucc->src->G_DAD, n->imp->isucc->eport ) )
            ImportSwap( n );

    /* NORMALIZE FORM OF BOOLEAN CONSTANTS */
    for ( i = n->imp; i != NULL; i = i->isucc )
      if ( IsConst( i ) && IsBoolean( i->info ) ) {
        b = BooleanToInteger( i->CoNsT );
        i->CoNsT = CopyString( IntegerToBoolean( b ) );
        }

    /* ------------------------------------------------------------ */
    /* Convert exponentiation of v ^ constant to multiplies         */
    /* ------------------------------------------------------------ */
    if ( IsExp( n ) ) FoldExp( n );

    /* ------------------------------------------------------------ */
    /* Convert division by a constant to multiplication by inverse  */
    /* ------------------------------------------------------------ */
    if ( n->type == IFDiv ) FoldDiv( n );

    if ( IsLogicalShortCircuit( n ) )
      return;

    for ( cnt = 0, i = n->imp; i != NULL; i = i->isucc )
        if ( !IsConst( i ) || (++cnt > 2) )
            return;

    if ( cnt == 0 ) /* NO OPERANDS! */
      return;

    if ( GetOperands( n ) == ERROR )
        return;

    switch ( n->type ) {
        case IFPlus:
            switch ( n->imp->info->type ) {
                case IF_INTEGER:
                    SPRINTF( a, "%ld", iop1 + iop2 );
                    break;

                case IF_DOUBLE:
                case IF_REAL:
                    SPRINTF( a, "%.16e", dop1 + dop2 );
                    break;

                default:
                    SPRINTF( a, "%s", IntegerToBoolean( iop1 || iop2 ) );
                    break;
                }

            break;

        case IFTimes:
            switch ( n->imp->info->type ) {
                case IF_INTEGER:
                    SPRINTF( a, "%ld", iop1 * iop2 );
                    break;

                case IF_DOUBLE:
                case IF_REAL:
                    SPRINTF( a, "%.16e", dop1 * dop2 );
                    break;

                default:
                    SPRINTF( a, "%s", IntegerToBoolean( iop1 && iop2 ) );
                    break;
                }

            break;

        case IFMinus:
            if ( IsInteger( n->imp->info ) )
                SPRINTF( a, "%ld", iop1 - iop2 );
            else
                SPRINTF( a, "%.16e", dop1 - dop2 );

            break;

        case IFDiv:
            if ( IsInteger( n->imp->info ) ) {
                if ( iop2 == 0 ) return;
                SPRINTF( a, "%ld", iop1 / iop2 );
            } else {
                if ( dop2 == 0.0 ) return;
                SPRINTF( a, "%.16e", dop1 / dop2 );
                }

            break;

        case IFMod:
            if ( !IsInteger( n->imp->info ) )
                return;

            if ( iop2 == 0 ) return;
            SPRINTF( a, "%ld", iop1 % iop2 );
            break;

        case IFExp:
            if ( IsInteger( n->imp->info ) )
                SPRINTF( a, "%d", IntegerPower(iop1,iop2) );
            else if ( IsInteger( n->imp->isucc->info ) )
                SPRINTF( a, "%.16e", pow( dop1, ((double) iop2) ) );
            else
                SPRINTF( a, "%.16e", pow( dop1, dop2 ) );

            break;

        case IFMax:
            switch ( n->imp->info->type )  {
                case IF_INTEGER:
                    SPRINTF( a, "%ld", ( iop1 > iop2 )? iop1 : iop2 );
                    break;

                case IF_REAL:
                case IF_DOUBLE:
                    SPRINTF( a, "%.16e", ( dop1 > dop2 )? dop1 : dop2 );
                    break;

                default:
                    return;
                }

            break;

        case IFMin:
            switch ( n->imp->info->type )  {
                case IF_INTEGER:
                    SPRINTF( a, "%ld", ( iop1 < iop2 )? iop1 : iop2 );
                    break;

                case IF_REAL:
                case IF_DOUBLE:
                    SPRINTF( a, "%.16e", ( dop1 < dop2 )? dop1 : dop2 );
                    break;

                default:
                    return;
                }

            break;

        case IFAbs:
            if ( IsInteger( n->imp->info ) )
                SPRINTF( a, "%d", abs( (int)iop1 ) );
            else
                SPRINTF( a, "%.16e", fabs( dop1 ) );

            break;

        case IFNeg:
            if ( IsInteger( n->imp->info ) )
                SPRINTF( a, "%ld", -iop1 );
            else
                SPRINTF( a, "%.16e", -dop1 );

            break;

        case IFFloor:
            SPRINTF( a, "%d", (int) floor( dop1 ) );
            break;

        case IFInt:
            switch ( n->imp->info->type ) {
                case IF_BOOL:
                    SPRINTF( a, "%d", (int) iop1 );
                    break;

                case IF_CHAR:
                    SPRINTF( a, "%d", (int) iop1 );
                    break;

                case IF_DOUBLE:
                case IF_REAL:
                    SPRINTF( a, "%d", (int) floor( dop1 + 0.5 ) );
                    break;

                default:
                    return;
                }

            break;

        case IFTrunc:
            SPRINTF( a, "%d", (int) dop1 );
            break;

        case IFSingle:
        case IFDouble:
            if ( IsInteger( n->imp->info ) )
                SPRINTF( a, "%.16e", (double) iop1 );
            else
                SPRINTF( a, "%.16e", dop1 );

            break;

        case IFChar:
            if ( (iop1 < ' ') || (iop1 > '~') )
                SPRINTF( a, "\\0%lo", iop1 );
            else if ( iop1 == '\\' )
                SPRINTF( a, "\\\\" );
            else if ( iop1 == '\'' )
                SPRINTF( a, "\\'" );
            else
                SPRINTF( a, "%lc", iop1 );

            break;

        case IFNot:
            SPRINTF( a, "%s", IntegerToBoolean( (iop1 == 1)? 0 : 1 ) );
            break;

        case IFLess:
            switch ( n->imp->info->type ) {
                case IF_CHAR:
                case IF_BOOL:
                case IF_INTEGER:
                    SPRINTF( a, "%s", IntegerToBoolean( iop1 < iop2 ) );
                    break;

                default:
                    SPRINTF( a, "%s", IntegerToBoolean( dop1 < dop2 ) );
                    break;
                }

            break;

        case IFLessEqual:
            switch ( n->imp->info->type ) {
                case IF_CHAR:
                case IF_BOOL:
                case IF_INTEGER:
                    SPRINTF( a, "%s", IntegerToBoolean( iop1 <= iop2 ) );
                    break;

                default:
                    SPRINTF( a, "%s", IntegerToBoolean( dop1 <= dop2 ) );
                    break;
                }

            break;

        case IFEqual:
            switch ( n->imp->info->type ) {
                case IF_CHAR:
                case IF_BOOL:
                case IF_INTEGER:
                    SPRINTF( a, "%s", IntegerToBoolean( iop1 == iop2 ) );
                    break;

                default:
                    SPRINTF( a, "%s", IntegerToBoolean( dop1 == dop2 ) );
                    break;
                }

            break;

        case IFNotEqual:
            switch ( n->imp->info->type ) {
                case IF_CHAR:
                case IF_BOOL:
                case IF_INTEGER:
                    SPRINTF( a, "%s", IntegerToBoolean( iop1 != iop2 ) );
                    break;

                default:
                    SPRINTF( a, "%s", IntegerToBoolean( dop1 != dop2 ) );
                    break;
                }

            break;

        case IFCall:
#define FoldMathIntrins(icheck,dcheck,f)\
                switch(n->imp->isucc->info->type){\
                case IF_INTEGER:\
                  if ( icheck ) return;\
                  SPRINTF(a,"%d",(int)(f((double)iop2)));\
                  break;\
                case IF_DOUBLE:\
                case IF_REAL:\
                  if ( dcheck ) return;\
                  SPRINTF(a,"%.16e",f(dop2));\
                  break;\
                default:\
                  return;\
                }
            /* ------------------------------------------------------------ */
            /* Make sure that the intrinsics are turned on AND that the     */
            /* call has precisely ONE argument                              */
            /* ------------------------------------------------------------ */
              if ( !intrinsics ) return;
              if ( !n->imp->isucc ) return;
              if ( n->imp->isucc->isucc ) return;

              if ( CaseCmp( n->imp->CoNsT,"log" ) == 0 ) {
                FoldMathIntrins(iop2<0,dop2<0.0,log);

              } else if ( CaseCmp( n->imp->CoNsT,"log10" ) == 0 ) {
                FoldMathIntrins(iop2<0,dop2<0.0,log10);

              } else if ( CaseCmp( n->imp->CoNsT,"etothe" ) == 0 ) {
                FoldMathIntrins(0,0,exp);

              } else if ( CaseCmp( n->imp->CoNsT,"sin" ) == 0 ) {
                FoldMathIntrins(0,0,sin);

              } else if ( CaseCmp( n->imp->CoNsT,"cos" ) == 0 ) {
                FoldMathIntrins(0,0,cos);

              } else if ( CaseCmp( n->imp->CoNsT,"tan" ) == 0 ) {
                FoldMathIntrins(0,0,tan);

              } else if ( CaseCmp( n->imp->CoNsT,"asin" ) == 0 ) {
                FoldMathIntrins(0,0,asin);

              } else if ( CaseCmp( n->imp->CoNsT,"acos" ) == 0 ) {
                FoldMathIntrins(0,0,acos);

              } else if ( CaseCmp( n->imp->CoNsT,"atan" ) == 0 ) {
                FoldMathIntrins(0,0,atan);

              } else if ( CaseCmp( n->imp->CoNsT,"sqrt" ) == 0 ) {
                FoldMathIntrins(iop2<0,dop2<0.0,sqrt);

              } else {
                return;
              }
#undef FoldMathIntrins
            break;

        default:
            return;
        }       

    /* ------------------------------------------------------------ */
    /* Make sure that the folded value is a proper value (could be  */
    /* NaN or Infinity!)                                            */
    /* ------------------------------------------------------------ */
    if ( CaseCmp(a,"NaN") == 0 || CaseCmp(a,"Infinity") == 0 ) return;

    
    r = CopyString( a );

    for ( e = n->exp; e != NULL; e = se ) {
        se = e->esucc;

        e->CoNsT = r;
        e->eport = CONST_PORT;
        e->esucc = NULL;
        e->epred = NULL;
        e->src   = NULL;
        }

    n->exp = NULL;
    OptRemoveDeadNode( n ); fcnt++;
}


/**************************************************************************/
/* LOCAL  **************         FoldNodes         ************************/
/**************************************************************************/
/* PURPOSE: REMOVE CONSTANT NODES FROM THE  NODE LIST DEFINED  BY g AND   */
/*          PROPAGATE  CONSTANTS INTO  ALL COMPOUND NODES.                */
/**************************************************************************/

static void FoldNodes( g )
PNODE g;
{
    register PNODE n;
    register PNODE sn;
    register PEDGE i;
    register PEDGE ii;
    register PEDGE e;
    register PEDGE se;

    if ( IsIGraph( g ) )
        return;

    for ( n = g->G_NODES; n != NULL; n = sn ) {
        sn = n->nsucc;

        if ( WasReduced( n ) )
          continue;

        if ( IsSimple( n ) ) {
            OptNormalizeNode( n );

            switch( n->type ) {
              case IFAElement:
              case IFAReplace:
                if ( IsArray( n->imp->info ) )
                  NormalizeIndexing( n->imp->isucc );

                break;

              default:
                break;
              }

            Fold( n );
            continue;
            }

        PropagateConst( n );

        if ( IsSelect( n ) )
            if ( IsConst( n->S_TEST->imp ) ) {
              /* BUG FIX 10/24/89 CANN */
              /* sn = n->npred; */ /* n WILL BE REPLACED BY ZERO OR   */
              sn = RemoveBranch( n ); /* MORE NODES FROM THE LIVE BRANCH*/
              continue;
              }

        for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
            FoldNodes( g );

        /* PROPAGATE IDENTICAL, IDENTICAL CONSTANTS OUT OF BRANCHES */
        if ( !IsSelect( n ) )
          continue;

        for ( i = n->S_ALT->imp; i != NULL; i = i->isucc ) {
          if ( !IsConst( i ) )
            continue;

          ii = FindImport( n->S_CONS, i->iport );

          if ( !IsConst( ii ) )
            continue;
          
          /* IDENTICAL? */
          if ( strcmp( i->CoNsT, ii->CoNsT ) != 0 ) {
            switch( i->info->type ) {
              case IF_INTEGER:
                if ( atoi(i->CoNsT) == atoi(ii->CoNsT) )
                  break;

                continue;

              case IF_REAL:
              case IF_DOUBLE:
                if ( atof( DoubleToReal( i->CoNsT ) ) ==
                     atof( DoubleToReal( ii->CoNsT ) ) )
                  break;

                continue;

              default:
                continue;
              }
            }

          esccnt++;
          /* CHANGE SELECT EXPORTS */
          for ( e = n->exp; e != NULL; e = se ) {
            se = e->esucc;

            if ( e->eport != i->iport )
              continue;
            
            ChangeEdgeToConst( e, i );
            }

          OptRemoveDeadNode( n );
          }
        }
}


/**************************************************************************/
/* GLOBAL **************        WriteFoldInfo      ************************/
/**************************************************************************/
/* PURPOSE: PRINT INFORMATION GATHERED DURING CONSTANT FOLDING TO stderr. */
/**************************************************************************/

void WriteFoldInfo()
{
    FPRINTF( infoptr, "\n **** CONSTANT NODE FOLDING\n\n"   );
    FPRINTF( infoptr, " Folded Simple Nodes:          %d\n", fcnt  );
    FPRINTF( infoptr, " Folded Logical Nodes:         %d\n", lfcnt );
    FPRINTF( infoptr, " Folded Select Nodes:          %d\n", sfcnt );
    FPRINTF( infoptr, " Propagated Constants:         %d\n", pcnt  );
    FPRINTF( infoptr, " Exported Select Constants:    %d\n", esccnt);
    FPRINTF( infoptr, " EXP Strength Reductions:      %d\n", expcnt);
    FPRINTF( infoptr, "\n **** SPECIAL CONSTANT FOLDING\n\n" );
    FPRINTF( infoptr, " A * 0 OR 0 * A Nodes Folded:  %d\n", zero_cnt  );
    FPRINTF( infoptr, " Identity Nodes Folded:        %d\n", ident_cnt );
    FPRINTF( infoptr, " +/- Chains Folded:            %d\n", chain_cnt );
    FPRINTF( infoptr, " DIV-TO-NEG Conversions        %d\n", dncnt     );
    FPRINTF( infoptr, " Propagated Negations          %d\n", pncnt     );
    FPRINTF( infoptr, " Array Indexing Modifications: %d\n", idxm      );
    FPRINTF( infoptr, " Negation Reductions:          %d\n", negcnt    );
    FPRINTF( infoptr, " Array SetL Reductions:        %d\n", setlcnt   );
}


/**************************************************************************/
/* GLOBAL **************         If1Fold           ************************/
/**************************************************************************/
/* PURPOSE: FOLD THE CONSTANT NODES IN EACH FUNCTION GRAPH. ONLY SPECIFIC */
/*          MONADIC AND DYADIC NODES ARE FOLDED. THE ARITHMETIC EXCEPTION */
/*          HANDLER FoldExceptionHandler IS INSTALLED BEFORE FOLDING AND  */
/*          THE OLD HANDLER IS REINSTALLED AFTER FOLDING. SELECT NODES    */
/*          WITH UNREACHABLE BRANCHES ARE FOLDED.                         */
/**************************************************************************/

void If1Fold()
{
  register PNODE f;

  signal( SIGFPE, FoldExceptionHandler );

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    FoldNodes( cfunct = f );
    FoldSpecialNodes( f );
  }

  signal( SIGFPE, SIG_DFL );
}
/*
 * $Log$
 * Revision 1.3  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.2  2001/01/01 05:46:22  patmiller
 * Adding prototypes and header info -- all will be broken
 *
 * Revision 1.1.1.1  2000/12/31 17:56:23  patmiller
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
 * Revision 1.14  1994/07/01  23:30:10  denton
 * int -> long argument as referenced.
 *
 * Revision 1.13  1994/06/16  21:29:23  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.12  1994/04/15  15:50:32  denton
 * Added config.h to centralize machine specific header files.
 *
 * Revision 1.11  1994/04/05  21:10:01  denton
 * Remove signal warning.
 *
 * Revision 1.10  1994/03/22  00:40:11  miller
 * Now uses new integer power routine (in Backend/Library) to fold integer
 * powers.
 *
 * Revision 1.9  1994/02/02  20:09:01  miller
 * Better fix for constant folded integer powers.
 *
 * Revision 1.8  1993/11/30  00:26:04  miller
 * Constant folding for intrinsics.
 *
 * Revision 1.7  1993/06/11  00:49:17  miller
 * Fold counters multiply defined in FoldItems.c in Library
 *
 * Revision 1.6  1993/06/10  23:19:01  miller
 * Fixes required to make this run on RS6000
 *
 * Revision 1.5  1993/01/07  00:37:36  miller
 * Make changes for LINT and combined files.
 *
 * Revision 1.4  1992/12/23  22:27:10  miller
 * Removed essentially duplicate function that are in the library and
 * the Ifx..../ subdirectories.
 *
 * Revision 1.3  1992/12/10  22:57:46  miller
 * Makefile, world.h changed to reflect new IFX.h
 * if1fold.c if1fusion.c if1inline.c opt.c: Modified to allow -trace function
 *
 * Revision 1.2  1992/11/06  19:31:00  miller
 * Fix for  #921006 where the exp(v,constant) was being inproperly
 * unfolded.  An integer edge was introduced to one the first multiply
 * even if the edge should be real or double real.
 *
 * Revision 12.8  1992/11/05  23:29:50  miller
 * Fix for  #921006 where the exp(v,constant) was being inproperly
 * unfolded.  An integer edge was introduced to one the first multiply
 * even if the edge should be real or double real.
 *
 * Revision 12.8  1992/10/23  20:51:32  miller
 * Fix for  #921006 where the exp(v,constant) was being inproperly
 * unfolded.  An integer edge was introduced to one the first multiply
 * even if the edge should be real or double real.
 *
 * Revision 12.7  1992/10/21  18:08:33  miller
 * Initial RCS Version by Cann
 * */

