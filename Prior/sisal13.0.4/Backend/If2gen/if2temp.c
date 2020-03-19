/* if2temp.c,v
 * Revision 12.7  1992/11/04  22:05:03  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:04  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"

#define IsLastUse(x)        ((x)->rc == 1) 
#define IncPopularity(x,y)  if ( x->temp != NULL ) x->temp->pop += y

/* USED TO HELP THE fxc ON THE ALLIANT USE THE VECTOR REGS!! */
#define MAX_VEC_REGS        4

static PTEMP  thead  = NULL;                               /* LEVEL STACK */
static double ccost  = 0.0;
static PNODE  critb;

int    tmpid;                                 /* TEMPORTARY NAME STAMP    */
int    nmid   = 0;                            /* GENERAL NAME STAMP       */

int    vimax;                     /* MAXIMUM NUMBER OF VECTOR TEMPORARIES */ 
int    vfmax;
int    vdmax;

int    vicnt;    /* CURRENT NUMBER OF VECTOR TEMPORARIES FOR A GIVEN LOOP */
int    vfcnt;
int    vdcnt;

int    cpsb = -1;                         /* POINTER SWAP BUFFER NUMBERS */
int    psb;

int    rags;             /* RAGGED MEMORY ALLOCATION MANAGEMENT VARIABLES */


#define MAX_FRAMES 200

static int   frametop = 1;             /* NUMBER OF ALLOCATE LOOP POOL FRAMES */


static void AssignVectorTemps( l )
PNODE l;
{
    register PEDGE i;

    /* PROPAGATE K PORT TEMPORARIES TO ALL SUBGRAPHS                      */

    for ( i = l->imp; i != NULL; i = i->isucc ) {
	if ( IsLastUse( i ) && (i->cm != -1) ) {
	    if ( !(IsExport( l->L_TEST, i->iport ) ||
		   IsExport( l->L_BODY, i->iport ) ||
		   IsExport( l->L_RET,  i->iport )) )
		PropagateTemp( l->L_INIT, i->iport, FALSE, i->temp );
            else
		PropagateTemp( l->L_INIT, i->iport, TRUE, i->temp );
	    }
        else
            PropagateTemp( l->L_INIT, i->iport, TRUE, i->temp );

	PropagateTemp( l->L_TEST, i->iport, TRUE, i->temp );
	PropagateTemp( l->L_BODY, i->iport, TRUE, i->temp );

	/* if ( IsExport( l->L_BODY, i->iport ) )
	    if ( i->temp != NULL )
		if ( i->temp->ccost < ccost )
		    i->temp->ccost = ccost; */

	PropagateTemp( l->L_RET,  i->iport, TRUE, i->temp );
	}

    /* ASSIGN TEMPORARIES IN THE RETURN SUBGRAPHS                */ 

    AssignTemps( l->L_RET );

    /* FREE TEMPORARIES ALLOCATED IN l OR IMPORTED TO l THAT ARE DEAD     */
    /* WHEN IT COMPLETES.                                                 */

    for ( i = l->imp; i != NULL; i = i->isucc )
	if ( !IsTempExported( l, i->temp ) )
	    FreeTemp( i );
}


/**************************************************************************/
/* LOCAL  **************   DecImportPopularity     ************************/
/**************************************************************************/
/* PURPOSE: DECREMENT THE POPULARITY VALUE FOR ALL IMPORT OF SUBGRAPH g.  */
/**************************************************************************/

static void DecImportPopularity( g )
PNODE g;
{
    register PEDGE i;

    switch ( g->G_DAD->type ) {
	case IFSelect:
	case IFTagCase:
	    break;

	case IFForall:
	    if ( g->G_DAD->F_BODY == g )
		return;
	    
	    break;

	default:
	    if ( g->G_DAD->L_BODY == g || g->G_DAD->L_TEST == g )
		return;

	    break;
	}

    for ( i = g->imp; i != NULL; i = i->isucc )
	if ( i->temp != NULL )
	    if ( !IsGraph( i->src ) )
	        i->temp->pop--;
}


/**************************************************************************/
/* LOCAL  **************   DecExportPopularity     ************************/
/**************************************************************************/
/* PURPOSE: DECREMENT THE POPULARITY VALUE FOR ALL EXPORTS OF SUBGRAPH g. */
/**************************************************************************/

static void DecExportPopularity( g )
PNODE g;
{
    register PEDGE e;

    for ( e = g->exp; e != NULL; e = e->esucc )
	if ( e->temp != NULL )
	    if ( IsCompound( e->dst ) )
	        e->temp->pop--;

    switch ( g->G_DAD->type ) {
	case IFSelect:
	case IFTagCase:
	    return;

	case IFForall:
	    if ( g->G_DAD->F_BODY == g || g->G_DAD->F_RET == g )
		return;

            break;

	case IFLoopA:
	case IFLoopB:
	    if ( g->G_DAD->L_BODY == g || g->G_DAD->L_RET == g )
		return;

	    break;

        case IFUReduce:
	    UNIMPLEMENTED( "IFUReduce" );
	    if ( g->G_DAD->L_BODY == g || g->G_DAD->L_RET == g )
		return;

            break;

        default:
            UNEXPECTED( "Unknown compound" );
        }

    for ( e = g->exp; e != NULL; e = e->esucc )
	if ( e->temp != NULL )
	    if ( !IsCompound( e->dst ) )
	        e->temp->pop--;
}


/**************************************************************************/
/* LOCAL  **************      IsDerefCandidate     ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF iport IS REFERENCED IN LOOP BODY b OR IN LOOP  */
/*          IN RETURN SUBGRAPH r AND IS REFERENCED EACH ITERATION.        */
/**************************************************************************/

static int IsDerefCandidate( iport, r, b )
int   iport;
PNODE r;
PNODE b;
{
    register PEDGE e;

    if ( IsExport( b, iport ) )
	return( TRUE );

    for ( e = r->exp; e != NULL; e = e->esucc ) {
	if ( e->eport != iport )
	    continue;

	switch ( e->dst->type ) {
	    case IFAGather:
	    case IFAGatherAT:
	    case IFAGatherATDVI:
	    case IFAGatherATDV:
		/* VALUE OR FILTER                                        */
		if ( e->iport == 2 || e->iport == 3 )
		    return( TRUE );

		break;

	    case IFReduce:
	    case IFRedLeft:
	    case IFRedRight:
	    case IFRedTree:
	    case IFReduceAT:
	    case IFReduceATDV:
	    case IFReduceATDVI:
	    case IFRedLeftAT:
	    case IFRedRightAT:
	    case IFRedTreeAT:
		/* VALUE OR FILTER                                        */
		if ( e->iport == 3 || e->iport == 4 )
		    return( TRUE );

		break;

	    default:
		break;
	    }
	}

    return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************    ReduceArgAccessCost    ************************/
/**************************************************************************/
/* PURPOSE: INSERT ASSIGNMENT NODES TO REDUCE ARGUMENT ACCESS COSTS FOR   */
/*          THOSE FOUND TO BE REFERENCED MORE THAN 2 TIMES, REFERENCED IN */
/*          A LOOP BODY, OR EACH EXECUTION OF A LOOP'S RETURN SUBGRAPH.   */
/**************************************************************************/

static void ReduceArgAccessCost( f )
PNODE f;
{
    register PEDGE e;
    register PEDGE ee;
    register int   u;
    register PEDGE se;
    register PNODE n;
    register int   eport;

    for ( ;; ) {
	for ( e = f->exp; e != NULL; e = e->esucc ) {
            for ( u = 0, ee = e; ee != NULL; ee = ee->esucc ) {
	        if ( ee->eport != e->eport )
	            continue;

	        switch ( ee->dst->type ) {
	            case IFForall:
			if ( !IsExport( ee->dst->F_BODY, ee->iport ) && 
			     !IsExport( ee->dst->F_RET,  ee->iport )  ) {
			    u--;
                            break;
			    }

			if ( IsDerefCandidate( ee->iport, ee->dst->F_RET,
					       ee->dst->F_BODY          )  )
		            goto FoundIt;

		        break;

	            case IFLoopA:
	            case IFLoopB:
			if ( IsDerefCandidate( ee->iport, ee->dst->L_RET,
					       ee->dst->L_BODY          )  )
		            goto FoundIt;

			if ( IsExport( ee->dst->L_TEST, ee->iport ) )
			    goto FoundIt;

		        break;

		    case IFSelect:
		    case IFTagCase:
			goto FoundIt;

	            default:
		        break;
	            }

	        if ( (++u) >= 3 )
	            goto FoundIt;
                }
            }

        return;

FoundIt:
	n = NodeAlloc( ++maxint, IFAssign );
	LinkNode( f, n );

	ee = EdgeAlloc( f, e->eport, n, 1 );
	ee->info = e->info;
	ee->pm   = e->pm;

	/* FOR SDBX */
	ee->name = e->name;
	e->name  = NULL;

	LinkImport( n, ee );

	eport = e->eport;

	for ( e = f->exp; e != NULL; e = se ) {
	    se = e->esucc;

	    if ( e->eport != eport )
		continue;

	    UnlinkExport( e );
	    e->eport = 1;
	    e->pm    = 0;
	    LinkExport( n, e );
	    }

	LinkExport( f, ee );
	}
}


/**************************************************************************/
/* GLOBAL **************         MakeName          ************************/
/**************************************************************************/
/* PURPOSE: RETURN A UNIQUE NAME OF THE FORM "%s%d%s", GIVEN STRINGS s1   */
/*          AND s2 AND IDENTIFIER id.                                     */
/**************************************************************************/

char *MakeName( s1, s2, id )
char *s1;
char *s2;
int   id;
{
    char buf[100];

    SPRINTF( buf, "%s%d%s", s1, id, s2 );
    return( CopyString( buf ) );
}


/**************************************************************************/
/* LOCAL  **************   IsOnlyFinalValueNode    ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF FinalValue NODE n IS THE ONLY FinalValue NODE  */
/*          IMPORTING n->imp.                                             */
/**************************************************************************/

static int IsOnlyFinalValueNode( n )
PNODE n;
{
    register PEDGE e;
    register int   c = 0;

    if ( IsConst( n->imp ) ) /* CANN 9/8/89 */
	return( TRUE );      /* CANN 9/8/89 */

    for ( e = n->imp->src->exp; e != NULL; e = e->esucc )
	if ( e->eport == n->imp->eport )
	    if ( IsFinalValue( e->dst ) )
		c++;

    return( (c > 1)? FALSE : TRUE );
}


/**************************************************************************/
/* LOCAL  **************     DecrementRefCount     ************************/
/**************************************************************************/
/* PURPOSE: DECREMENT THE REFERENCE COUNT OF EXPORTS OF NODE n WITH       */
/*          EXPORT PORT NUMBER eport.                                     */
/**************************************************************************/

static void DecrementRefCount( n, eport )
PNODE n;
int   eport;
{
    register PEDGE e;

    for ( e = n->exp; e != NULL; e = e->esucc )
	if ( e->eport == eport )
            e->rc--;
}


/**************************************************************************/
/* LOCAL  **************        IsTypeMatch        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF INFO i1 AND i2 MATCH. NOTE, INTEGER, CHARACTER,*/
/*          AND BOOLEAN ALL MATCH.                                        */
/**************************************************************************/

static int IsTypeMatch( i1, i2 )
PEDGE i1;
PEDGE i2;
{
    if ( i1->info == i2->info )
	return( TRUE );

    switch ( i1->info->type ) {
	case IF_BOOL:
	case IF_CHAR:
	case IF_INTEGER:
	    switch ( i2->info->type ) {
		case IF_BOOL:
		case IF_CHAR:
		case IF_INTEGER:
		    return( TRUE );
		
		default:
		    break;
                }

	    break;

	default:
	    break;
        }

    return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************       IsTNameMatch        ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF TNAMES t1 AND t2 MATCH. NOTE, int MATCHES char.*/
/**************************************************************************/

static int IsTNameMatch( t1, t2 )
char *t1;
char *t2;
{
    if ( strcmp( t1, t2 ) == 0 )
	return( TRUE );

    if ( strcmp( t1, "char" ) == 0 )
	if ( strcmp( t2, "int" ) == 0 )
	    return( TRUE );

    if ( strcmp( t1, "int" ) == 0 )
	if ( strcmp( t2, "char" ) == 0 )
	    return( TRUE );

    return( FALSE );
}


/**************************************************************************/
/* GLOBAL **************          GetTemp          ************************/
/**************************************************************************/
/* PURPOSE: RETURN A FREE TEMPORARY FROM THE TEMPORARY LIST THAT HAS      */
/*          A COST NEAREST TO ccost. IF A TEMPORARY IS NOT FOUND, A NEW   */
/*          TEMPORARY IS ALLOCATED.  THE RETURNED TEMPORARY IS MARKED     */
/*          "ALLOCATED",                                                  */
/**************************************************************************/

PTEMP GetTemp( name, TempInfo, mod )
char  *name;
PINFO  TempInfo;
int    mod;
{
    register PTEMP  t;
    register PTEMP  c;
    register double c1;
    register double c2;

    /* TRY AND FIND A FREE TEMPORARY IN THE TEMP LIST WITH A COST      */
    /* NEAREST TO ccost. IN A TIE, THE MOST POPULAR TEMP IS RETURNED.  */

    for ( c = NULL, t = thead; t != NULL; t = t->next ) {
        if ( t->status != FREE )
            continue;

        if ( IsTNameMatch( TempInfo->tname, t->info->tname ) ) {
	    if ( c != NULL ) {
		c1 = fabs( c->ccost - ccost );
		c2 = fabs( t->ccost - ccost );

		if ( c1 == c2 ) {
		    if ( c->pop < t->pop )
			c = t;
		    }
		else if ( c1 > c2 )
		    c = t;
		}
	    else
	        c = t;
	    }
        }

    if ( c != NULL ) {
	c->status = ALLOCATED;

	if ( c->ccost < ccost )
	    c->ccost = ccost;

	return( c );
	}

    /* ALLOCATE A NEW TEMPORARY                                           */

    t = TempAlloc( name, TempInfo, mod, -1 );

    t->ccost = ccost;
    t->next  = thead;
    thead    = t;

    t->status = ALLOCATED;

    return( t );
}


/**************************************************************************/
/* GLOBAL **************   InitializeSymbolTable   ************************/
/**************************************************************************/
/* PURPOSE: EMPTY THE SYMBOL TABLE.                                       */
/**************************************************************************/

void InitializeSymbolTable()
{
    register PTEMP t;
    register PTEMP ti;

    for ( t = thead; t != NULL; t = ti ) {
	ti = t->next;
	/* free( t ); */
	}

    thead  = NULL;
    tmpid  = 0;

    vimax = vfmax = vdmax = 0;

    rags = -1;
    psb  = -1;
}


/**************************************************************************/
/* GLOBAL **************     ChangeToAllocated     ************************/
/**************************************************************************/
/* PURPOSE: CHANGE STATUSES ON NODE n's INPUT TEMPORARIES TO "ALLOCATED". */
/*          A STATUS IS ONLY CHANGED IF IT IS FOUND "FREE". AN IMPORT     */
/*          EQUAL TO EDGE e IS NOT ALTERED.                               */
/**************************************************************************/

void ChangeToAllocated( e, n )
PEDGE e;
PNODE n;
{
    register PEDGE i;

    for ( i = n->imp; i != NULL; i = i->isucc ) {
	if ( i->temp == NULL )                                /* CONSTANT */
	    continue;

	if ( i == e )
	    continue;

	if ( i->temp->status == FREE )
	    i->temp->status = ALLOCATED;
        }
}


/**************************************************************************/
/* GLOBAL **************         FreeTemp          ************************/
/**************************************************************************/
/* PURPOSE: FREE THE TEMPORARY ASSOCIATED WITH EDGE e IF ITS REFERENCE    */
/*          COUNT IS ONE, ELSE JUST DECREMENT THE REFERENCE COUNT.        */
/**************************************************************************/

void FreeTemp( e )
PEDGE e;
{
  if ( e->temp == NULL ) {                /* CONSTANT OR INDEX OPERATION */
    if ( IsConst( e ) )
      return;

    switch ( e->src->type ) {
      case IFAIndexPlus:
      case IFAIndexMinus:
	FreeTemp( e->src->imp );
	FreeTemp( e->src->imp->isucc );
	break;

      default:
	break;
      }

    return;
    }

  DecrementRefCount( e->src, e->eport );

  if ( e->rc == 0 )
    e->temp->status = FREE;
}


static int DistanceFromCriticalLoop( i )
register PEDGE i;
{
    register PEDGE ii;
    register PNODE owner;
    register int   d = 0;

    for ( ;; ) {
	if ( IsConst( i ) )
	    return( d );

        if ( !IsSGraph( i->src ) )
	    return( d );

        owner = i->src->G_DAD;

	/* TAGCASE VALUE REFERENCE?  */
	if ( IsTagCase( owner ) && (i->eport == 1) )
	    return( d );

	/* A K-PORT VALUE REFERENCE? */
        if ( (ii = FindImport( owner, i->eport )) == NULL )
	    return( d );

        i = ii;
	d++;
	}
}


/**************************************************************************/
/* GLOBAL **************       PropagateTemp       ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN TEMPORARY t TO EACH EXPORT OF NODE n HAVING EXPORT     */
/*          PORT NUMBER eport AND ASSIGN INITIAL REFERENCE COUNTS. Temp'S */
/*          POPULARITY IS INCREASED ACCORDINGLY. IF t IS READ-ONLY, AN    */
/*          EXTRA REFERENCE COUNT IS ASSIGNED.                            */
/**************************************************************************/

void PropagateTemp( n, eport, ronly, t )
PNODE n;
int   eport;
int   ronly;
PTEMP t;
{
    register PEDGE  e;
    register PEDGE  i;
    register int    u = UsageCount( n, eport );

    /* CONSTANT */
    if ( t == NULL )
	return;

    i = NULL;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
	if ( e->eport == eport ) {
	    e->temp = t;
	    e->rc   = u + ((ronly)? 1 : 0);
	    i = e;
	    }
        }

    if ( i == NULL )
	return;

    t->pop += u;

    if ( n != critb )
	return;

    if ( t->ccost < ccost ) {
	t->ccost = ccost;

	t->dist = DistanceFromCriticalLoop( i );
	}
}


/**************************************************************************/
/* GLOBAL **************      IsTempExported       ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF TEMPORARY t IS EXPORTED FROM NODE n.           */
/**************************************************************************/

int IsTempExported( n, t )
PNODE n;
PTEMP t;
{
    register PEDGE e;

    for ( e = n->exp; e != NULL; e = e->esucc )
	if ( e->temp == t )
	    return( TRUE );

    return( FALSE );
}


/**************************************************************************/
/* GLOBAL **************      IsTempImported       ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF TEMPORARY t IS IMPORTED TO NODE n.             */
/**************************************************************************/

int IsTempImported( n, t )
PNODE n;
PTEMP t;
{
    register PEDGE i;

    for ( i = n->imp; i != NULL; i = i->isucc )
	if ( i->temp == t )
	    return( TRUE );

    return( FALSE );
}


/**************************************************************************/
/* LOCAL  **************       SortByCost          ************************/
/**************************************************************************/
/* PURPOSE: SORT THE TEMPORARY LIST HEADED BY h BY COST, GREATEST FIRST.  */
/**************************************************************************/

static PTEMP SortByCost( h )
PTEMP h;
{
    register PTEMP p;
    register PTEMP pr;
    register PTEMP pn;
    register int   chng = TRUE;

    if ( h == NULL )
	return( NULL );

    while ( chng ) {
	chng = FALSE;
	pr   = NULL;

	for ( p = h; p->next != NULL; p = p->next ) {
	    pn = p->next;

	    if ( p->ccost < pn->ccost ) {
		chng = TRUE;

		if ( pr == NULL )
		    h = pn;
                else
		    pr->next = pn;

                p->next  = pn->next;
		pn->next = p;

		p = pn;
		}

            pr = p;
	    }
	}

    return( h );
}


/**************************************************************************/
/* LOCAL  **************     SortByPopularity      ************************/
/**************************************************************************/
/* PURPOSE: SORT TEMPORARIES WITH SIMILAR DISTANCES AND COSTS BY          */
/*          POPULARITY AND RETURN A POINTER TO THE NEW LIST.              */
/**************************************************************************/

static PTEMP SortByPopularity( h )
PTEMP h;
{
    register PTEMP p;
    register PTEMP pr;
    register PTEMP pn;
    register int   chng = TRUE;
    register int   p1,p2;

    if ( h == NULL )
	return( NULL );

    while ( chng ) {
	chng = FALSE;
	pr   = NULL;

	for ( p = h; p->next != NULL; p = p->next ) {
	    pn = p->next;

	    if ( (p->ccost == pn->ccost) && (p->dist == pn->dist) ) {
		p1 = p->pop;
		p2 = pn->pop;

	        if ( p1 < p2 ) {
		    chng = TRUE;

		    if ( pr == NULL )
		        h = pn;
                    else
		        pr->next = pn;

                    p->next  = pn->next;
		    pn->next = p;

		    p = pn;
		    }
                }

            pr = p;
	    }
	}

    return( h );
}


/**************************************************************************/
/* LOCAL  **************      SortByDistance       ************************/
/**************************************************************************/
/* PURPOSE: SORT TEMPORARIES WITH SIMILAR COSTS BY DISTASNCE  AND RETURN  */
/*          A POINTER TO THE NEW LIST.                                    */
/**************************************************************************/

static PTEMP SortByDistance( h )
PTEMP h;
{
    register PTEMP p;
    register PTEMP pr;
    register PTEMP pn;
    register int   chng = TRUE;
    register int   d1,d2;

    if ( h == NULL )
	return( NULL );

    while ( chng ) {
	chng = FALSE;
	pr   = NULL;

	for ( p = h; p->next != NULL; p = p->next ) {
	    pn = p->next;

	    if ( p->ccost == pn->ccost ) {
		d1 = p->dist;
		d2 = pn->dist;

	        if ( d1 > d2 ) {
		    chng = TRUE;

		    if ( pr == NULL )
		        h = pn;
                    else
		        pr->next = pn;

                    p->next  = pn->next;
		    pn->next = p;

		    p = pn;
		    }
                }

            pr = p;
	    }
	}

    return( h );
}




/**************************************************************************/
/* GLOBAL **************        PrintLocals        ************************/
/**************************************************************************/
/* PURPOSE: PRINT LOCAL DECLARATION FOR ALL TEMPORARIES DEFINED IN THE    */
/*          SYMBOL TABLE TO output: GIVING PRIORITY TO THOSE TEMPORARIES  */
/*          WITH THE HIGHEST COSTS. TIES GO TO THOSE WITH THE GREATEST    */
/*          POPULARITY.                                                   */
/**************************************************************************/

void PrintLocals( )
{
    register PTEMP p;
    register PINFO i;
    register int   bcnt = 0;
    register int   callcnt = 0;
    register int   v;

    thead = SortByPopularity( SortByDistance( SortByCost( thead ) ) );

    for ( p = thead; p != NULL; p = p->next ) {
	i = p->info;

	switch ( i->type ) {
	    case IF_BUFFER:
		FPRINTF( output, "  %s buffer%d;\n", i->sname, ++bcnt );
		FPRINTF( output, "  register %s %s = &buffer%d;", 
			 i->tname, p->name, bcnt                 );
		break;

            case IF_FUNCTION:
		if ( p->mod == NONE ) {
		  FPRINTF( output, "  %s CallFrame%d;\n", 
			   i->sname, ++callcnt            );
		  FPRINTF( output, "  register %s %s = ", 
			   i->tname, p->name           );
		  FPRINTF( output, "(%s) &CallFrame%d;",
			   i->tname, callcnt              );
		  }
                else if ( p->mod == FPTR ) {
		  if ( p->DeAl ) {
	            FPRINTF( output, "  register %s %s = ", i->tname, p->name );
		    FPRINTF( output, "(%s) Alloc(sizeof(%s));",
			     i->tname, i->sname              );
		    }
                  else
	            FPRINTF( output, "  register %s %s = (%s) &FrAmE%d;",
			     i->tname, p->name, i->tname, p->fid       );
		  }
                else
		  Error2( "PrintLocals", "ILLEGAL CALL MODE TYPE" ); 

		break;

            case IF_BOOL:
	    case IF_CHAR:
	    case IF_NULL:
		FPRINTF( output, "  register int %s;", p->name );
		break;

	    case IF_BRECORD:
		FPRINTF( output, "  %s %s;", i->tname, p->name );
		break;

            default:
		FPRINTF( output, "  register %s %s;", i->tname, p->name );
		break;
	    }

	FPRINTF( output, "\n" );
	}

    for ( v = 0; v <= psb; v++ )
      FPRINTF( output, "  PSBUFFER psb%d;\n", v );

    for ( v = 0; v <= rags; v++ )
      FPRINTF( output, "  register int rag%d = 0;\n", v );

    for ( v = 1; v <= vdmax; v++ )
      FPRINTF( output, "  register double *VD%d;\n", v );

    for ( v = 1; v <= vfmax; v++ )
      FPRINTF( output, "  register float *VF%d;\n", v );

    for ( v = 1; v <= vimax; v++ )
      FPRINTF( output, "  register int *VI%d;\n", v );

    FPRINTF( output, "\n" );
}


/**************************************************************************/
/* LOCAL  **************       AddAssignNode       ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE AND INSERT AN ASSIGN NODE SO TO FOLLOW NODE pr AND   */
/*          TAKE EDGE i AS ITS INPUT.  AN EDGE IS ADDED TO THREAD THE     */
/*          NEW NODE TO NODE dst; THIS EDGE IS RETURNED TO THE CALLER.    */
/**************************************************************************/

static PEDGE AddAssignNode( pr, dst, i, t )
PNODE pr;
PNODE dst;
PEDGE i;
PTEMP t;
{
    register PNODE a;
    register PEDGE e;

    a = NodeAlloc( ++maxint, IFAssign );
    LinkNode( pr, a );

    a->wmark = dst->wmark;
    dst->wmark = FALSE;

    e = EdgeAlloc( a, 1, dst, i->iport );
    e->info = i->info;
    e->temp = t;
    e->rc   = 1;
    e->cm   = i->cm;
    i->cm   = 0;

    UnlinkImport( i );
    i->iport = 1;
    LinkImport( a, i );
    LinkImport( dst, e );
    LinkExport( a, e );

    return( e );
}


/**************************************************************************/
/* LOCAL  **************      FixAlternatives      ************************/
/**************************************************************************/
/* PURPOSE: FORCE, VIA THE INSERTION OF Assign NODES, ALL BRANCHES OF     */
/*          Select AND TagCase NODES TO PRODUCE CORRESPONDING RESULTS IN  */
/*          THE SAME TEMPORARY STORAGE (i->temp).                         */
/**************************************************************************/

static void FixAlternatives( i, st )
PEDGE i;
PNODE st;
{
    register PNODE sg;
    register PEDGE ii;

    for ( sg = st->gsucc; sg != NULL; sg = sg->gsucc ) {
	ii = FindImport( sg, i->iport );

	if ( ii->temp != i->temp ) {
	    AddAssignNode( FindLastNode( sg ), sg, ii, i->temp );

	    if ( IsTempImported( sg->G_DAD, ii->temp ) )
	        DecrementRefCount( ii->src, ii->eport );
            else
	        FreeTemp( ii );
            }
	}
}


/**************************************************************************/
/* LOCAL  **************     SafeInAlternatives    ************************/
/**************************************************************************/
/* PURPOSE: IF THE TEMPORTARY ASSIGNED TO IMPORT i OF SUBGRAPH st IS ONLY */
/*          USED TO DEFINE THE SAME R PORT VALUE IN OTHER SUBGRAPHS OF    */
/*          THE OWNING Select OR TagCase NODE, RETURN TRUE.               */
/**************************************************************************/

static int SafeInAlternatives( i, st )
PEDGE i;
PNODE st;
{
    register PNODE sg;
    register PEDGE ii;

    for ( sg = st->gsucc; sg != NULL; sg = sg->gsucc )
	for ( ii = sg->imp; ii != NULL; ii = ii->isucc )
	    if ( ii->temp == i->temp )
		if ( ii->iport != i->iport )
		    return( FALSE );

    return( TRUE );
}


/**************************************************************************/
/* LOCAL  **************     AssignUReduceTemps     ***********************/
/**************************************************************************/
/* PURPOSE: ASSIGN TEMPORARIES TO UReduce NODE r.  AN ATTEMPT IS MADE TO  */
/*          REUSE STORAGE.                                                */
/**************************************************************************/

static void AssignUReduceTemps( r )
PNODE r;
{
    register PEDGE i;
    register PEDGE ii;
    register PEDGE si;
    register PNODE n;

    /* PROPAGATE K PORT TEMPORARIES TO ALL SUBGRAPHS                      */

    for ( i = r->imp; i != NULL; i = i->isucc ) {
	if ( IsLastUse( i ) && (i->cm != -1) ) {
	    if ( !(IsExport( r->R_BODY, i->iport ) ||
		   IsExport( r->R_RET,  i->iport )) )
		PropagateTemp( r->R_INIT, i->iport, FALSE, i->temp );
            else
		PropagateTemp( r->R_INIT, i->iport, TRUE, i->temp );
	    }
        else
            PropagateTemp( r->R_INIT, i->iport, TRUE, i->temp );

	PropagateTemp( r->R_BODY, i->iport, TRUE, i->temp );
	PropagateTemp( r->R_RET,  i->iport, TRUE, i->temp );
	}

    /* ALLOCATE TEMPORARIES FOR ALL L PORT VALUES                         */

    for ( i = r->R_INIT->imp; i != NULL; i = si ) {
	si = i->isucc;

	if ( IsConst( i ) ) {
	    AddAssignNode( FindLastNode( r->R_INIT ), r->R_INIT, i,
			   GetTemp( "tmp", i->info, NONE )       );
	    continue;
	    }

	if ( !IsLastUse( i ) ) {
	    DecrementRefCount( i->src, i->eport );
	    AddAssignNode( FindLastNode( r->R_INIT ), r->R_INIT, i,
			   GetTemp( "tmp", i->info, NONE )       );
	    }
	}

    /* PROPAGATE L PORT TEMPORARIES TO ALL SUBGRAPHS                      */

    for ( i = r->R_INIT->imp; i != NULL; i = i->isucc ) {
	PropagateTemp( r->R_RET,  i->iport, TRUE, i->temp );

	ii = FindImport( r->R_BODY, i->iport );

	if ( IsConst(ii))
	  if ( !IsExport( r->R_BODY, i->iport ) )
	    continue;

	/* IF NOT REFERENCED IN THE BODY AND NOT NEEDED AFTER ITS NEW     */
	/* VALUE IS DEFINED THEN DIRECTLY REDEFINE IT.                    */

	if ( !IsExport( r->R_BODY, i->iport ) )
	    if ( (ii->cm != -1) && (ii->temp == NULL) )
                switch ( ii->src->type ) {
		    case IFSelect: 
		    case IFForall: 
		    case IFTagCase:
		    case IFLoopA:  
		    case IFLoopB:  
		    case IFFirstSum:
		    case IFFirstMin:
		    case IFFirstAbsMin:
		    case IFFirstAbsMax:
		    case IFTri:
		    case IFFirstMax:
		    case IFCall:
		    case IFLoopPoolEnq: 
		    case IFOptLoopPoolEnq:
		    case IFSGraph:
		        break;

                    default:
			if ( !IsConst( ii ) )
		            PropagateTemp( ii->src, ii->eport, TRUE, i->temp );

			continue;
		    }

	if ( ii->cm != -1 )
	    PropagateTemp( r->R_BODY, i->iport, FALSE, i->temp); /* WRITE */
	else
	    PropagateTemp( r->R_BODY, i->iport, TRUE, i->temp );  /* READ */
	}

    /* MARK ALL FINALVALUE NODES CANDIDATE FOR EXECUTION AFTER THE LAST   */
    /* LOOP ITERATION COMPLETES AND NOT DURING EXECUTION.                 */

    for ( n = r->R_RET->G_NODES; n != NULL; n = n->nsucc )
	if ( IsFinalValue( n ) && IsBasic( n->exp->info ) )
	    if ( n->imp->isucc == NULL )
		if ( IsOnlyFinalValueNode( n ) )
		    n->lmark = TRUE;

    /* ASSIGN TEMPORARIES IN THE RETURN AND BODY SUBGRAPHS                */ 

    AssignTemps( r->R_RET );
    AssignTemps( r->R_BODY );

				    /* BODY MAY HAVE PREMATURELY FREED    */
				    /* SOME L PORT TEMPORARIES            */
    ChangeToAllocated( NULL_EDGE, r->R_INIT ); 

    /* WHERE NECESSARY, FORCE ASSIGNMENTS TO L PORT TEMPORARIES IN THE    */
    /* BODY SUBGRAPH.                                                     */

    for ( i = r->R_BODY->imp; i != NULL; i = si ) {
	si = i->isucc;

	if ( i->iport == 0 )
	    continue;

        if ( (ii = FindImport( r->R_INIT, i->iport )) == NULL ) {   /* T */
	    if ( IsConst( i ) || IsTempImported( r->R_INIT, i->temp ) ) {
		ii = AddAssignNode( FindLastNode( r->R_BODY ), r->R_BODY, i,
				    GetTemp( "tmp", i->info, NONE )       );

		}

	    continue;
	    }

        if ( ii->temp == i->temp )
	    continue;

        if ( IsConst( i ) )
	    continue;

	if ( !IsTempExported( r->R_BODY, i->temp ) )
	    continue;

	if ( IsSGraph( i->src ) )
	    continue;

	AddAssignNode( FindLastNode( r->R_BODY ), r->R_BODY, i,
		       GetTemp( "tmp", i->info, NONE )       );

	if ( !IsTempImported( r->R_INIT, i->temp ) )
	    FreeTemp( i );
	}

    /* BIND TEMPORARIES FOR THOSE FINALVALUE NODES SCHEDULED TO EXECUTE   */
    /* AFTER THE LOOP IS DONE */


    for ( n = r->R_RET->G_NODES; n != NULL; n = n->nsucc )
	if ( n->lmark ) {
	    if ( n->imp->temp != NULL ) /* !CONSTANT */ /* CANN 9/8/89 */
	    n->exp->temp = n->imp->temp;

	    PropagateTemp( r, n->exp->iport, FALSE, n->exp->temp );
	    }

    /* ASSIGN FINAL TEMPORARIES FROM REDUCTION */

    for ( i = r->exp; i!=NULL; i = i->esucc ) {
        if ( !i->temp )
            i->temp = GetTemp( "tmp", i->info, NONE );
        PropagateTemp( i->dst->G_DAD, i->iport, FALSE, i->temp);
    }


    /* FREE TEMPORARIES ALLOCATED IN r OR IMPORTED TO r THAT ARE DEAD     */
    /* WHEN IT COMPLETES.                                                 */

    for ( i = r->imp; i != NULL; i = i->isucc )
	if ( !IsTempExported( r, i->temp ) )
	    FreeTemp( i );

    for ( i = r->R_INIT->imp; i != NULL; i = i->isucc )
	if ( !( IsTempExported( r, i->temp ) ||
		IsTempImported( r, r->temp ) ) )
            FreeTemp( i );

    for ( i = r->R_BODY->imp; i != NULL; i = i->isucc )
	if ( !IsTempImported( r->R_INIT, i->temp ) )
	    FreeTemp( i );
}


/**************************************************************************/
/* LOCAL  **************     AssignReturnsTemps     ***********************/
/**************************************************************************/
/* PURPOSE: ASSIGN TEMPORARIES TO RETURN SUBGRAPH g.                      */
/**************************************************************************/

static void AssignReturnsTemps( g )
PNODE g;
{
    register PNODE r;

    for ( r = g->nsucc; r != NULL; r = r->nsucc ) {
        if ( r->type == IFUReduce )
            AssignUReduceTemps( r );
    }
}


/**************************************************************************/
/* LOCAL  **************     AssignSelectTemps     ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN TEMPORARIES TO Select OR TagCase NODE s. AN ATTEMPT IS */
/*          MADE TO RESUE STORAGE IMPORTED TO s AND SHARE STORAGE ACROSS  */
/*          SUBGRAPHS.                                                    */
/**************************************************************************/

static void AssignSelectTemps( s )
PNODE s;
{
    register PEDGE e  = NULL;
    register PEDGE i;
    register PNODE st;
    register PNODE sg;
    register PEDGE ii;
    register PEDGE iii;
    register PEDGE si;
    register PTEMP t;

    /* PROPAGATE SELECTION TEMPORARY INTO TEST SUBGRAPH OF Select NODE    */
    /* AND FREE IT IF IT ISN'T REFERENCED IN THE OTHER SUBGRAPHS.         */

    if ( IsSelect( s ) ) {
	e = s->S_TEST->imp;
	if ( (i = FindImport( s, e->eport )) != NULL ) {
	    e->temp = i->temp;  e = i;

	    if ( !( IsExport( s->S_ALT, i->iport  ) || 
		    IsExport( s->S_CONS, i->iport ) ) ) {
                if ( IsLastUse( i ) ) {
		    i->temp->status = FREE;  /* KEEP REFERENCE COUNT AT 1 */
		    e = i;               /* DON'T FORCE ALLOCATED BETWEEN */
				         /* SUBGRAPH PROCESSING. IF IT    */
					 /* HAPPENS TO BE FREE, REUSE IT. */
		    }
                else
		    e = NULL;
                }
	    }
        else
	    e = NULL;

	st = s->S_ALT;
	}
    else
	st = s->C_SUBS;

    /* PROPAGATE K PORT TEMPORARIES INTO THE SUBGRAPHS.                   */

    for ( i = s->imp; i != NULL; i = i->isucc )
	for ( sg = st; sg != NULL; sg = sg->gsucc )
	    if ( IsLastUse( i ) && (i->cm != -1) )
		PropagateTemp( sg, i->iport, FALSE, i->temp );   /* WRITE */
            else
		PropagateTemp( sg, i->iport, TRUE, i->temp );    /* READ  */
    
    /* ATTEMPT TO FORCE REUSE OF R PORT TEMPORARIES ALLOCATED IN THE 1ST  */
    /* SUBGRAPH IN THE OTHER SUBGRAPHS.                                   */

    AssignTemps( st );
    ChangeToAllocated( e, s );

    for ( i = st->imp; i != NULL; i = i->isucc ) {
	if ( i->iport == 0 )
	    continue;

        if ( IsConst( i ) )
	    continue;

	if ( i->src->cmark ) /* CANN BUG FIX 4/14/92 */
	   continue;

        if ( IsTempImported( s, i->temp ) )
	    continue;

        if ( IsTempImported( s, i->temp ) )
	    if ( IsTagCase( s ) )
		continue;

        for ( sg = st->gsucc; sg != NULL; sg = sg->gsucc ) {
	    ii = FindImport( sg, i->iport );

            if ( IsTempImported( s, i->temp ) ) {
		if ( IsTempExported( sg, i->temp ) )
		    continue;

                if ( !IsLastUse( i ) )
		    continue;
		}
		
	    if ( IsConst( ii ) )
		continue;

	    if ( ii->temp != NULL )
		continue;

            switch ( ii->src->type ) {
		case IFSelect: 
		case IFForall: 
		case IFTagCase:
		case IFLoopA:  
		case IFLoopB:  
		case IFFirstSum:
		case IFFirstMin:
		case IFFirstAbsMin:
		case IFFirstAbsMax:
		case IFTri:
		case IFFirstMax:
		case IFCall:
		case IFLoopPoolEnq: 
		case IFOptLoopPoolEnq:
		    break;

                default:
		    if ( ii->src->cmark ) /* CANN BUG FIX 4/14/92 */
		      break;

		    PropagateTemp( ii->src, ii->eport, FALSE, i->temp );
		    break;
		}
            }
	}

    /* ALLOCATE TEMPORARIES IN THE OTHER SUBGRAPHS                        */
    
    for ( sg = st->gsucc; sg != NULL; sg = sg->gsucc ) {
	AssignTemps( sg );
	ChangeToAllocated( e, s );
	}

    /* FORCE CORRESPONDING IMPORTS OF ALL SUBGRAPHS TO PLACE RESULTS IN   */
    /* THE SAME TEMPORARY STORAGE (THAT DEFINED IN THE 1ST SUBGRAPH) AND  */
    /* PROPAGATE THE STORAGE TO THE COMPOUND NODES EXPORTS                */

    for ( i = st->imp; i != NULL; i = si ) {
	si = i->isucc;

	if ( i->iport == 0 )
	    continue;

	/* CONSTANT REFERENCE COUNT IS -1!                                */
        if ( IsLastUse( i ) )
	    if ( SafeInAlternatives( i, st ) ) {
		PropagateTemp( s, i->iport, FALSE, i->temp );
		FixAlternatives( i, st );
		continue;
		}

	/* IF SELECT NODE AND i IS A CONSTANT, SEARCH FOR TEMP IN         */
	/* CONSEQUENT SUBGRAPH.                                           */

	if ( IsConst( i ) && IsSelect( s ) ) {
	    ii = FindImport( st->gsucc, i->iport );

	    if ( !IsLastUse( ii ) )
		goto MoveOn;

	    if ( UsageCount( ii->src, ii->eport ) > 1 )
		goto MoveOn;

	    for ( iii = st->imp; iii != NULL; iii = iii->isucc ) 
		if ( iii->temp == ii->temp )
		    goto MoveOn;

	    AddAssignNode( FindLastNode( st ), st, i, ii->temp );
	    PropagateTemp( s, ii->iport, FALSE, ii->temp );
	    continue;
	    }

MoveOn:
	ii = AddAssignNode( FindLastNode( st ), st, i, 
			    t = GetTemp( "tmp", i->info, NONE ) );

        FixAlternatives( ii, st );
	PropagateTemp( s, ii->iport, FALSE, t );

	if ( IsTempImported( s, i->temp ) )
	    DecrementRefCount( i->src, i->eport );
        else
	    FreeTemp( i );
	}

    /* FREE TEMPORARIES ALLOCATED IN s OR IMPORTED TO s THAT ARE DEAD     */
    /* WHEN THE NODE COMPLETES.                                           */

    for ( i = s->imp; i != NULL; i = i->isucc )
	if ( !IsTempExported( s, i->temp ) )
	    FreeTemp( i );

    for ( sg = st; sg != NULL; sg = sg->gsucc )
	for ( i = sg->imp; i != NULL; i = i->isucc )
	    if ( i->iport == 0 )
		if ( !IsTempImported( s, i->temp ) )
		    FreeTemp( i );
}


static int CriticalPath( i )
PEDGE i;
{
  if ( IsConst( i ) ) 
    return( 1 );

  if ( IsSGraph( i->src ) ) 
    return( 1 );

  if ( i->src->type == IFOptAElement )
    return( 1 );

  if ( i->temp != NULL )
    return( 1 );

  return( 1 + CriticalPath( i->src->imp ) );
}


/**************************************************************************/
/* LOCAL  **************     AssignForallTemps     ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN TEMPORARIES TO Forall NODE f. AN ATTEMPT IS MADE TO    */
/*          TO REUSE STORAGE.                                             */
/**************************************************************************/

static void AssignForallTemps( f )
PNODE f;
{
    register PEDGE i;
    register PEDGE si;
    register PEDGE e;
    register PNODE n;
    register PTEMP t;
    register int   mtmp;
    register int   idx;
    register int   iidx;
    register int   u;
    struct { PNODE node; int ucnt; } fan[MAX_VEC_REGS+10];

    /* ASSIGN TEMPORARIES IN THE RETURN SUBGRAPH                          */
    AssignTemps( f->F_RET );

    /* PROPAGATE K PORT TEMPORARIES INTO ALL SUBGRAPHS                    */

    for ( i = f->imp; i != NULL; i = i->isucc ) {
	if ( IsLastUse( i ) && (i->cm != -1) ) {
	    if ( !( IsExport( f->F_BODY, i->iport ) ||
		    IsExport( f->F_RET,  i->iport ) ) )
	        PropagateTemp( f->F_GEN, i->iport, FALSE, i->temp ); 
	    else
	        PropagateTemp( f->F_GEN, i->iport, TRUE, i->temp );
	    }
        else
	    PropagateTemp( f->F_GEN, i->iport, TRUE, i->temp );

	PropagateTemp( f->F_BODY, i->iport, TRUE, i->temp );

	/* if ( IsExport( f->F_BODY, i->iport ) )
	    if ( i->temp != NULL )
		if ( i->temp->ccost < ccost )
		    i->temp->ccost = ccost; */

	PropagateTemp( f->F_RET,  i->iport, TRUE, i->temp );
	}

    /* ASSIGN TEMPORARIES IN THE GENERATE SUBGRAPH AND PROPAGATE THE M    */
    /* PORT TEMPORARIES TO THE BODY AND RETURN SUBGRAPHS.                 */

    AssignTemps( f->F_GEN );

    for ( i = f->F_GEN->imp; i != NULL; i = i->isucc ) {
	PropagateTemp( f->F_BODY, i->iport, TRUE, i->temp );
	PropagateTemp( f->F_RET,  i->iport, TRUE, i->temp );
	}

    /* ASSIGN TEMPORARIES IN THE BODY SUBGRAPH AND PROPAGATE THE T PORT   */
    /* TEMPORARIES TO THE RETURN SUBGRAPH.                                */

    if ( f->vmark && alliantfx ) {
      vdcnt = vfcnt = vicnt = 0;

      for ( i = f->imp; i != NULL; i = i->isucc ) {
	if ( IsBuffer( i->info ) )
	  switch ( i->info->A_ELEM->A_ELEM->type ) {
	    case IF_DOUBLE:
	      i->vtemp = (++vdcnt);

	      if ( vdcnt > vdmax )
		vdmax = vdcnt;

	      break;

	    case IF_INTEGER:
	      i->vtemp = (++vicnt);

	      if ( vicnt > vimax )
		vimax = vicnt;

	      break;

	    case IF_REAL:
	      i->vtemp = (++vfcnt);

	      if ( vfcnt > vfmax )
		vfmax = vfcnt;

	      break;

	    default:
	      Error2( "AssignForallTemps", "ILLEGAL VECTOR BUFFER LOOP INPUT" );
	      break;
            }
	}

      mtmp = MAX_VEC_REGS;

      for ( n = f->F_BODY->G_NODES; n != NULL; n = n->nsucc ) {
	switch ( n->type ) {
	  case IFPlus:
	  case IFMinus:
	    if ( n->exp->dst->type == IFOptAElement )
              break;

	  case IFTimes:
	  case IFDiv:
	  case IFNeg:
	  case IFAbs:
	  case IFDouble:
	  case IFTrunc:
	  case IFSingle:
	    if ( n->type == IFPlus || n->type == IFTimes )
	      /* IT APPEARS THAT IN SOME CASES > IS BETTER THAN < ON THE */
	      /* ALLIANT */
	      if ( CriticalPath( n->imp ) < CriticalPath( n->imp->isucc ) )
		ImportSwap( n );

	    if ( n->exp->esucc == NULL )
	      break;

	    mtmp--;
	    t = GetTemp( "tmp", n->exp->info, NONE );

	    for ( e = n->exp; e != NULL; e = e->esucc )
	      e->temp = t;

	    break;

	  case IFOptAElement:
	    break;

	  default:
	    FPRINTF( stderr, " *** node type %d, line %d, funct %s, file %s\n",
			     n->type, n->line, n->funct, n->file );

	    Error2( "AssignForallTemps", "ILLEGAL VECTOR BODY NODE" );
	    break;
	  }
	}

      if ( mtmp < 0 ) 
	mtmp = 0;

      idx = 0;

      for ( n = f->F_BODY->G_NODES; n != NULL; n = n->nsucc ) {
	if ( n->type != IFOptAElement )
	  continue;

	if ( (u = UsageCount( n, 1 )) < 2 )
	  continue;

	if ( idx < mtmp )  {
	  fan[idx].node = n;
	  fan[idx].ucnt = u;
	  idx++;
        } else if ( idx == mtmp )  {
	  for ( iidx = 0; iidx < mtmp; iidx++ )
	    if ( fan[iidx].ucnt < u ) {
	      fan[iidx].node = n;
	      fan[iidx].ucnt = u;
	      break;
              }
	  }
        }

      for ( iidx = 0; iidx < idx; iidx++ ) {
	 n = fan[iidx].node;
	 t = GetTemp( "tmp", n->exp->info, NONE );

	 for ( e = n->exp; e != NULL; e = e->esucc )
	   e->temp = t;
         }

      for ( n = f->F_RET->G_NODES; n != NULL; n = n->nsucc ) {
	switch ( n->type ) {
	  case IFReduce:
	  case IFRedTree:
	  case IFRedRight:
	  case IFRedLeft:
	    if ( n->imp->CoNsT[0] == REDUCE_LEAST ||
	         n->imp->CoNsT[0] == REDUCE_GREATEST )
	      if ( n->imp->isucc->isucc->temp == NULL )
	        if ( !IsConst( n->imp->isucc->isucc ) )
		  if ( IsImport( f->F_BODY, n->imp->isucc->isucc->eport ) )
	            n->imp->isucc->isucc->temp = 
			  GetTemp( "tmp", n->exp->info, NONE );

	    /* if ( n->imp->isucc->isucc->temp == NULL )
	      if ( !IsConst( n->imp->isucc->isucc ) ) {
		ee = FindImport( f->F_BODY, n->imp->isucc->isucc->eport );

		if ( ee->temp == NULL ) {
	          t = GetTemp( "tmp", n->exp->info, NONE );

	          for ( e = ee->src->exp; e != NULL; e = e->esucc )
		    if ( e->iport == ee->iport )
	              e->temp = t;
		  }

		n->imp->isucc->isucc->temp = ee->temp;
		} */

	    break;

	  default:
	    break;
	  }
        }
    } else {
      AssignTemps( f->F_BODY );

      for ( i = f->F_BODY->imp; i != NULL; i = si ) {
	  si = i->isucc;

	  if ( IsConst( i ) )
	      AddAssignNode( FindLastNode( f->F_BODY ), f->F_BODY, i,
			     GetTemp( "tmp", i->info, NONE )       );
	  }

      for ( i = f->F_BODY->imp; i != NULL; i = i->isucc )
	  PropagateTemp( f->F_RET, i->iport, TRUE, i->temp );
      }

    /* ASSIGN REDUCTION TEMPORARIES AFTER PUSHING THROUGH THE BODY.       */

    AssignReturnsTemps( f->F_RET );

    /* FREE TEMPORARIES ALLOCATED IN f OR IMPORTED TO f THAT ARE DEAD     */
    /* WHEN THE NODE COMPLETES.                                           */

    for ( i = f->imp; i != NULL; i = i->isucc )
	FreeTemp( i );

    for ( i = f->F_GEN->imp; i != NULL; i = i->isucc ) {
	if ( !IsTempImported( f, i->temp ) )
	    FreeTemp( i );

	if ( IsRangeGenerate( i->src ) ) 
	    if ( (si = i->src->imp->isucc) != NULL )
	        if ( !IsConst( si ) )
		    if ( IsAssign( si->src ) )
			FreeTemp( si );
        }

    if ( f->vmark && alliantfx )
      for ( n = f->F_BODY->G_NODES; n != NULL; n = n->nsucc )
	if ( n->exp->temp != NULL )
	  n->exp->temp->status = FREE;
    else
      for ( i = f->F_BODY->imp; i != NULL; i = i->isucc )
	  FreeTemp( i );
}


/**************************************************************************/
/* LOCAL  **************      AssignLoopTemps      ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN TEMPORARIES TO LOOP NODE l.  AN ATTEMPT IS MADE TO     */
/*          REUSE STORAGE IMPORTED TO l AND SHARE STORAGE ACROSS          */
/*          SUBGRAPHS.                                                    */
/**************************************************************************/

static void AssignLoopTemps( l )
PNODE l;
{
    register PEDGE i;
    register PEDGE ii;
    register PEDGE si;
    register PNODE n;

    /* PROPAGATE K PORT TEMPORARIES TO ALL SUBGRAPHS                      */

    for ( i = l->imp; i != NULL; i = i->isucc ) {
	if ( IsLastUse( i ) && (i->cm != -1) ) {
	    if ( !(IsExport( l->L_TEST, i->iport ) ||
		   IsExport( l->L_BODY, i->iport ) ||
		   IsExport( l->L_RET,  i->iport )) )
		PropagateTemp( l->L_INIT, i->iport, FALSE, i->temp );
            else
		PropagateTemp( l->L_INIT, i->iport, TRUE, i->temp );
	    }
        else
            PropagateTemp( l->L_INIT, i->iport, TRUE, i->temp );

	PropagateTemp( l->L_TEST, i->iport, TRUE, i->temp );
	PropagateTemp( l->L_BODY, i->iport, TRUE, i->temp );

	/* if ( IsExport( l->L_BODY, i->iport ) )
	    if ( i->temp != NULL )
		if ( i->temp->ccost < ccost )
		    i->temp->ccost = ccost; */

	PropagateTemp( l->L_RET,  i->iport, TRUE, i->temp );
	}

    /* ALLOCATE TEMPORARIES FOR ALL L PORT VALUES                         */

    for ( i = l->L_INIT->imp; i != NULL; i = si ) {
	si = i->isucc;

	if ( IsConst( i ) ) {
	    AddAssignNode( FindLastNode( l->L_INIT ), l->L_INIT, i,
			   GetTemp( "tmp", i->info, NONE )       );
	    continue;
	    }

	if ( !IsLastUse( i ) ) {
	    DecrementRefCount( i->src, i->eport );
	    AddAssignNode( FindLastNode( l->L_INIT ), l->L_INIT, i,
			   GetTemp( "tmp", i->info, NONE )       );
	    }
	}

    /* PROPAGATE L PORT TEMPORARIES TO ALL SUBGRAPHS                      */

    for ( i = l->L_INIT->imp; i != NULL; i = i->isucc ) {
	PropagateTemp( l->L_TEST, i->iport, TRUE, i->temp );
	PropagateTemp( l->L_RET,  i->iport, TRUE, i->temp );

	ii = FindImport( l->L_BODY, i->iport );

	/* BUG FIX ? ---7/13/89 CANN */
	/* if ( IsConst( ii ) ) continue; */
	/* BUG MODIFICATION 7/90 CANN */
	if ( IsConst(ii))
	  if ( !IsExport( l->L_BODY, i->iport ) )
	    continue;
	/* END BUG FIX */

	/* IF NOT REFERENCED IN THE BODY AND NOT NEEDED AFTER ITS NEW     */
	/* VALUE IS DEFINED THEN DIRECTLY REDEFINE IT.                    */

	if ( !IsExport( l->L_BODY, i->iport ) )
	    if ( (ii->cm != -1) && (ii->temp == NULL) )
                switch ( ii->src->type ) {
		    case IFSelect: 
		    case IFForall: 
		    case IFTagCase:
		    case IFLoopA:  
		    case IFLoopB:  
		    case IFFirstSum:
		    case IFFirstMin:
		    case IFFirstAbsMin:
		    case IFFirstAbsMax:
		    case IFTri:
		    case IFFirstMax:
		    case IFCall:
		    case IFLoopPoolEnq: 
		    case IFOptLoopPoolEnq:
		    case IFSGraph:
		        break;

                    default:
			if ( !IsConst( ii ) )
		            PropagateTemp( ii->src, ii->eport, TRUE, i->temp );

			continue;
		    }

	if ( ii->cm != -1 )
	    PropagateTemp( l->L_BODY, i->iport, FALSE, i->temp); /* WRITE */
	else
	    PropagateTemp( l->L_BODY, i->iport, TRUE, i->temp );  /* READ */
	}

    /* MARK ALL FINALVALUE NODES CANDIDATE FOR EXECUTION AFTER THE LAST   */
    /* LOOP ITERATION COMPLETES AND NOT DURING EXECUTION.                 */

    for ( n = l->L_RET->G_NODES; n != NULL; n = n->nsucc )
	if ( IsFinalValue( n ) && IsBasic( n->exp->info ) )
	    if ( n->imp->isucc == NULL )
		if ( IsOnlyFinalValueNode( n ) )
		    n->lmark = TRUE;

    /* ASSIGN TEMPORARIES IN THE RETURN AND BODY SUBGRAPHS                */ 

    AssignTemps( l->L_RET );
    AssignTemps( l->L_BODY );

				    /* BODY MAY HAVE PREMATURELY FREED    */
				    /* SOME L PORT TEMPORARIES            */
    ChangeToAllocated( NULL_EDGE, l->L_INIT ); 

    /* WHERE NECESSARY, FORCE ASSIGNMENTS TO L PORT TEMPORARIES IN THE    */
    /* BODY SUBGRAPH. ALSO PROPAGATE T PORT TEMPORARIES TO THE TEST       */
    /* SUBGRAPH.                                                          */

    for ( i = l->L_BODY->imp; i != NULL; i = si ) {
	si = i->isucc;

	if ( i->iport == 0 )
	    continue;

        if ( (ii = FindImport( l->L_INIT, i->iport )) == NULL ) {   /* T */
	    if ( IsConst( i ) || IsTempImported( l->L_INIT, i->temp ) ) {
		ii = AddAssignNode( FindLastNode( l->L_BODY ), l->L_BODY, i,
				    GetTemp( "tmp", i->info, NONE )       );

                PropagateTemp( l->L_TEST, ii->iport, TRUE, ii->temp );
		}
	    else
		PropagateTemp( l->L_TEST, i->iport, TRUE, i->temp );

	    continue;
	    }

        if ( ii->temp == i->temp )
	    continue;

        if ( IsConst( i ) )
	    continue;

	if ( !IsTempExported( l->L_BODY, i->temp ) )
	    continue;

	if ( IsSGraph( i->src ) )
	    continue;

	AddAssignNode( FindLastNode( l->L_BODY ), l->L_BODY, i,
		       GetTemp( "tmp", i->info, NONE )       );

	if ( !IsTempImported( l->L_INIT, i->temp ) )
	    FreeTemp( i );
	}

    /* ASSIGN TEMPORARIES IN THE TEST SUBGRAPH AND BIND TEMPORARIES FOR   */
    /* THOSE FINALVALUE NODES SCHEDULED TO EXECUTE AFTER THE LOOP IS DONE */

    if ( !AreAllUnitFanout( l->L_TEST ) ) {
        AssignTemps( l->L_TEST );
        FreeTemp( l->L_TEST->imp );
	}

    for ( n = l->L_RET->G_NODES; n != NULL; n = n->nsucc )
	if ( n->lmark ) {
	    if ( n->imp->temp != NULL ) /* !CONSTANT */ /* CANN 9/8/89 */
	    n->exp->temp = n->imp->temp;

	    PropagateTemp( l, n->exp->iport, FALSE, n->exp->temp );
	    }

    /* ASSIGN REDUCTION TEMPORARIES AFTER PUSHING THROUGH THE BODY.       */

    AssignReturnsTemps( l->L_RET );

    /* FREE TEMPORARIES ALLOCATED IN l OR IMPORTED TO l THAT ARE DEAD     */
    /* WHEN IT COMPLETES.                                                 */

    for ( i = l->imp; i != NULL; i = i->isucc )
	if ( !IsTempExported( l, i->temp ) )
	    FreeTemp( i );

    for ( i = l->L_INIT->imp; i != NULL; i = i->isucc )
	if ( !( IsTempExported( l, i->temp ) ||
		IsTempImported( l, l->temp ) ) )
            FreeTemp( i );

    for ( i = l->L_BODY->imp; i != NULL; i = i->isucc )
	if ( !IsTempImported( l->L_INIT, i->temp ) )
	    FreeTemp( i );
}

/**************************************************************************/
/* LOCAL  **************      BindExportTemps      ************************/
/**************************************************************************/
/* PURPOSE: ALLOCATE TEMPORARIES DESCRIPTORS FOR n'S OUTPUT ARGUMENTS.    */
/**************************************************************************/

static void BindExportTemps( n, TempInfo, name )
PNODE  n;
PINFO  TempInfo;
char  *name;
{
    register PEDGE e;

    for ( e = n->exp; e != NULL; e = e->esucc ) {
        e->temp = TempAlloc( "", TempInfo, NONE, e->eport );
        e->rc   = UsageCount( n, e->eport ) + 1;

        e->temp->name = name;
	}
}


PNODE  FindCriticalPath( g, cbody )
PNODE  g;
PNODE  cbody;
{
    register PNODE n;
    register PNODE cb;

    cb = NULL; 

    for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
	switch ( n->type ) {
	    case IFLoopA:
	    case IFLoopB:
		if ( cb != NULL )
		    return( cbody );

		cb = n->L_BODY;
		break;

	    case IFForall:
		if ( cb != NULL )
		    return( cbody );

		cb = n->F_BODY;
		break;

	    case IFSelect:
	    case IFTagCase:
		return( cbody );

	    default:
		break;
            }
	}

    if ( cb != NULL )
        return( FindCriticalPath( cb, cb ) );
    else
	return( cbody );
}


/**************************************************************************/
/* GLOBAL **************        AssignTemps        ************************/
/**************************************************************************/
/* PURPOSE: ASSIGN TEMPORARIES TO THE NODES IN GRAPH g.                   */
/**************************************************************************/

void AssignTemps( g )
PNODE g;
{
    register PNODE  n;
    register PEDGE  e;
    register PNODE  f;
    register PTEMP  t;
    register PEDGE  i;
    register int    cnt;
    register PEDGE  ii;
	     char   buf[100];

    switch ( g->type ) {
	case IFLGraph:
	case IFXGraph:
	case IFLPGraph:
	    ReduceArgAccessCost( g );
	    /* 1 LINE: CANN 10/5 */
	    critb = FindCriticalPath( g, NULL_NODE );
	    ccost = 0.0;
	    break;

	default:
	    break;
	}

    /* FOR ALL OF THE NODES IN THIS GRAPH, ASSIGN TEMPORARIES TO        */
    /* THE EXPORT EDGES (PropagateTemp, INCREMENT REFERENCE COUNT) AND  */
    /* FREE TEMPORARIES ON IMPORTS (FreeTemp, DECREMENT REFERENCE COUNT */
    /* AND CHECK FOR 0).  RECURSIVELY DESCEND INTO COMPOUND GRAPHS.     */

    for ( n = g; n != NULL; n = n->nsucc ) {
	switch ( n->type ) {
            case IFPSAllocCond:
            case IFPSAllocOne:
            case IFPSAllocTwo:
            case IFPSAllocSpare1:
            case IFPSAllocSpare2:
	      for ( i = n->imp; i != NULL; i = i->isucc )
		FreeTemp( i );

	      PropagateTemp( n, 1, FALSE, 
			     t = GetTemp( "tmp", n->exp->info, NONE ) );

	      if ( n->type == IFPSAllocCond ) {
		SPRINTF( buf, "cpsb%d", ++cpsb );
                FPRINTF( output, "\nstatic PSBUFFER %s = { ", buf ); 
		FPRINTF( output, " NULL,NULL,NULL, %d, 0,0,0,0,0,0 };\n", 
			 n->imp->iport                                );
		}
	      else
		SPRINTF( buf, "psb%d", ++psb );

	      n->gname = CopyString( buf );
	      break;

            case IFPSFreeOne:
            case IFPSFreeTwo:
            case IFPSFreeSpare1:
            case IFPSFreeSpare2:
	      FreeTemp( n->imp );

	      if ( n->imp->isucc != NULL )
		FreeTemp( n->imp->isucc );

	      break;

            case IFPSManager:
            case IFPSManagerSwap:
	      PropagateTemp( n, 1, FALSE, 
			     t = GetTemp( "tmp", n->exp->info, NONE ) );

	      FreeTemp( n->imp );
	      break;

            case IFPSScatter:
	      FreeTemp( n->imp );
	      FreeTemp( n->imp->isucc );
	      FreeTemp( n->imp->isucc->isucc );

	      PropagateTemp( n, 1, FALSE, 
			     t = GetTemp( "tmp", n->exp->info, NONE ) );
	      break;

	    case IFRagged:
		n->imp->iport = ++rags;

		FreeTemp( n->imp );

		PropagateTemp( n, 1, FALSE, 
			       t = GetTemp( "tmp", n->exp->info, NONE ) );
		break;

	    case IFOptNoOp:
	    case IFAssign:
		PropagateTemp( n, 1, FALSE, 
			       t = GetTemp( "tmp", n->exp->info, NONE ) );
			       
		FreeTemp( n->imp );
		break;

            case IFDefArrayBuf:
              Error2( "AssignTemps", "IFDefArrayBuf NODE ENCOUNTERED" );
	      break;

	    case IFPSMemAllocDVI:
	      FreeTemp( n->imp );
	      FreeTemp( n->imp->isucc );
	      FreeTemp( n->imp->isucc->isucc );

	      PropagateTemp( n, 1, FALSE, 
			     GetTemp( "tmp", n->exp->info, NONE ) );
	      break;

	    case IFMemAllocDVI:
	    case IFMemAllocDV:
	    case IFMemAlloc:
		if ( n->cmark )
		    break;

		if ( n->imp->isucc != NULL ) {    /* LOWER BOUND AND ABS SIZE */
		  if ( n->imp->isucc->isucc != NULL )
		    FreeTemp( n->imp->isucc->isucc );

		  FreeTemp( n->imp->isucc);
		  }

		PropagateTemp( n, 1, FALSE, 
			       GetTemp( "tmp", n->exp->info, NONE ) );
			       

		FreeTemp( n->imp );         /* DefArrayBuf SIZE */
		break;

	    case IFShiftBuffer:
		if ( n->exp->temp == NULL ) {
		    if ( IsLastUse( n->imp ) ) {
		        PropagateTemp( n, 1, FALSE, n->imp->temp );
			FreeTemp( n->imp->isucc );
			break;
			}

                    PropagateTemp( n, 1, FALSE, 
				   GetTemp( "tmp", n->exp->info, NONE ) );
		    }

		FreeTemp( n->imp );
		FreeTemp( n->imp->isucc );
		break;

	    case IFError:
		if ( n->exp->temp == NULL )
		  PropagateTemp( n, n->exp->eport, FALSE, 
			         GetTemp( "tmp", n->exp->info, NONE ) );
		break;

	    case IFAAddLATDV:
	    case IFAAddLATDVI:
	    case IFAAddHATDV:
	    case IFAAddHATDVI:
	    case IFAAddHAT:
	    case IFAAddLAT:
		/* FREE SIZE AND THEN LOWER BOUND */
		FreeTemp( n->imp->isucc->isucc->isucc );
		FreeTemp( n->imp->isucc->isucc->isucc->isucc );

		if( n->exp->temp != NULL ) {
		    FreeTemp( n->imp );
		    FreeTemp( n->imp->isucc );
		    break;
		    }

		if ( n->imp->pmark ) {
		    if ( !IsLastUse( n->imp ) )
			Error1( "AssignTemps: AddHLAT: IMP P BUT !LAST USE" );

		    PropagateTemp( n, n->exp->eport, FALSE, n->imp->temp );
		    FreeTemp( n->imp->isucc );
		    break;
		    }

		PropagateTemp( n, n->exp->eport, FALSE, 
			       GetTemp( "tmp", n->exp->info, NONE ) );

		FreeTemp( n->imp );
		FreeTemp( n->imp->isucc );
		break;

	    case IFACatenateAT:
	    case IFACatenateATDV:
	    case IFACatenateATDVI:
		/* FREE SIZE AND THEN LOWER BOUND */
		FreeTemp( n->imp->isucc->isucc->isucc );
		FreeTemp( n->imp->isucc->isucc->isucc->isucc );

		if( n->exp->temp != NULL ) {
		    FreeTemp( n->imp );
		    FreeTemp( n->imp->isucc );
		    break;
		    }

		if ( n->imp->pmark ) {
		    if ( !IsLastUse( n->imp ) )
			Error1( "AssignTemps: ACatAT 1ST IMP P BUT !LAST USE" );

		    PropagateTemp( n, n->exp->eport, FALSE, n->imp->temp );
		    FreeTemp( n->imp->isucc );
		    break;
		    }

		if ( n->imp->isucc->pmark ) {
		    if ( !IsLastUse( n->imp->isucc ) )
			Error1( "AssignTemps: ACatAT 2ND IMP P BUT !LAST USE" );

		    PropagateTemp( n, n->exp->eport, FALSE, 
				   n->imp->isucc->temp   );
		    FreeTemp( n->imp );
		    break;
		    }

		PropagateTemp( n, n->exp->eport, FALSE, 
			       GetTemp( "tmp", n->exp->info, NONE ) );

		FreeTemp( n->imp );
		FreeTemp( n->imp->isucc );
		break;

	    case IFRElements:
		for ( e = n->exp; e != NULL; e = e->esucc )
		    if ( e->temp == NULL )
			PropagateTemp( n, e->eport, FALSE, 
				       GetTemp( "tmp", e->info, NONE ) );

		FreeTemp( n->imp );
		break;

	    case IFABuild:
	    case IFABuildAT:
	    case IFABuildATDV:
	    case IFABuildATDVI:
		if ( n->cmark ) {
		    t = TempAlloc( "", n->exp->info, GLOB, -1 );

		    t->name = n->G_NAME;

		    for ( e = n->exp; e != NULL; e = e->esucc ) {
		        e->temp = t;
		        e->rc   = UsageCount( n, e->eport ) + 1;
		        }

		    break;
		    }

		if ( n->exp->temp == NULL )
		    PropagateTemp( n, 1, FALSE, 
				   GetTemp( "tmp", n->exp->info, NONE ) );

		for ( i = n->imp; i != NULL; i = i->isucc )
		    FreeTemp( i );

		break;

            case IFPeek:
		if ( n->exp != NULL )
		  if ( n->exp->temp == NULL )
		    PropagateTemp( n, 1, FALSE, 
				   GetTemp( "tmp", n->exp->info, NONE ) );

		for ( i = n->imp; i != NULL; i = i->isucc )
		    FreeTemp( i );

		break;

	    case IFBRBuild:
	    case IFUBuild:
	    case IFRBuild:
		if ( n->cmark ) {
		    t = TempAlloc( "", n->exp->info, GLOB, -1 );

		    t->name = n->G_NAME;

		    for ( e = n->exp; e != NULL; e = e->esucc ) {
		        e->temp = t;
		        e->rc   = UsageCount( n, e->eport ) + 1;
		        }

		    break;
		    }

		if ( n->exp->temp == NULL )
		    PropagateTemp( n, 1, FALSE, 
				   GetTemp( "tmp", n->exp->info, NONE ) );
			           
		 
		for ( i = n->imp; i != NULL; i = i->isucc )
		    FreeTemp( i );

		break;

	    case IFAAddH:
	    case IFACatenate:
		if ( n->exp->temp == NULL )
		    PropagateTemp( n, 1, FALSE, 
			           GetTemp( "tmp", n->exp->info, NONE ) );

		FreeTemp( n->imp );
		FreeTemp( n->imp->isucc );
		break;

	    case IFAIndexPlus:
	    case IFAIndexMinus:
		if ( !IsConst( n->imp ) )
		  for ( e = n->imp->src->exp; e != NULL; e = e->esucc )
		    if ( e->eport == n->imp->eport )
		      e->rc += UsageCount( n, 1 ) - 1;

		if ( !IsConst( n->imp->isucc ) )
		  for ( e = n->imp->isucc->src->exp; e != NULL; e = e->esucc )
		    if ( e->eport == n->imp->isucc->eport )
		      e->rc += UsageCount( n, 1 ) - 1;

		break;

	    case IFBRElements:
		for ( e = n->exp; e != NULL; e = e->esucc )
		    if ( e->temp == NULL )
			PropagateTemp( n, e->eport, FALSE, 
				       GetTemp( "tmp", e->info, NONE ) );

		if ( n->imp->src->type == IFOptAElement )
		  if ( n->imp->src->exp->esucc == NULL ) {
		    FreeTemp( n->imp->src->imp->isucc );
		    FreeTemp( n->imp->src->imp );
		    break;
		    }

		FreeTemp( n->imp );
		break;

	    case IFOptAElement:
		if ( n->exp->esucc == NULL )
		  if ( n->exp->dst->type == IFBRElements )
                    break;
	    case IFAElement:
		FreeTemp( n->imp->isucc );

		if ( n->imp->cm != -1 )
		    FreeTemp( n->imp );

		if ( n->exp->temp == NULL )
		    PropagateTemp( n, 1, FALSE, 
			           GetTemp( "tmp", n->exp->info, NONE ) );

		if ( n->imp->cm == -1 )
		    FreeTemp( n->imp );

		break;

	    case IFOptAReplace:
	    case IFAReplace:
	    case IFRReplace:
		if ( n->exp->temp == NULL ) {
		    if ( IsImport( n, MAX_PORT_NUMBER ) )
			n->imp->temp->pop--;

		    PropagateTemp( n, 1, FALSE, n->imp->temp );
		    }
                else
		    FreeTemp( n->imp );

		for ( i = n->imp->isucc; i != NULL; i = i->isucc )
		    FreeTemp( i );

		break;

	    case IFAAdjust:                         /* HAS A NoOp SOURCE */
		if ( n->exp->temp == NULL )
		    PropagateTemp( n, 1, FALSE, n->imp->temp );
                else 
		    FreeTemp( n->imp );

		FreeTemp( n->imp->isucc );
		FreeTemp( n->imp->isucc->isucc );
		break;

	    case IFASetL:                            /* HAS A NoOp SOURCE */
		if ( n->exp->temp == NULL )
		    PropagateTemp( n, 1, FALSE, n->imp->temp );
                else
		    FreeTemp( n->imp );

		FreeTemp( n->imp->isucc );
		break;

	    case IFARemL:
	    case IFARemH:                      /* THESE HAVE NoOp SOURCES */
		if ( n->exp->temp == NULL )
		    PropagateTemp( n, 1, FALSE, n->imp->temp );
		else
		    FreeTemp( n->imp );

		break;

	    case IFPrefixSize:
	    case IFASize:
	    case IFALimL:
	    case IFALimH:
	    case IFAIsEmpty:
	    case IFUTagTest:
	    case IFUGetTag:
		if ( n->exp->temp == NULL )
		    PropagateTemp( n, 1, FALSE, 
				   GetTemp( "tmp", n->exp->info, NONE ) );

		FreeTemp( n->imp );
		break;

            case IFSum:
            case IFProduct:
            case IFLeast:
            case IFGreatest:
            case IFAStore:
                FreeTemp( n->imp );
                break;

	    case IFFinalValueAT:
	    case IFFinalValue:
		if ( n->lmark && (!IsConst(n->imp)) )
		    break;
	    case IFAGather:
		n->exp->temp = GetTemp( "tmp", n->exp->info, NONE );
		PropagateTemp( n->exp->dst->G_DAD, n->exp->iport, FALSE, 
			       n->exp->temp                           );
		break;

	    case IFReduce:
                if ( n->imp->CoNsT[0] == REDUCE_USER ) {
                    f = FindFunction( UpperCase( n->imp->CoNsT, TRUE, FALSE ) );
                    /* THESE ARE NEVER FREED! */
                    n->temp = GetTemp( "cframe", f->info, NONE );
                    BindExportTemps( n, f->info, n->temp->name );
                    } /* DROP THROUGH */
	    case IFRedLeft:
	    case IFRedRight:
	    case IFRedTree:
		n->exp->temp = GetTemp( "tmp", n->exp->info, NONE );
		PropagateTemp( n->exp->dst->G_DAD, n->exp->iport, FALSE, 
			       n->exp->temp                           );
		break;

	    case IFAGatherAT:
	    case IFAGatherATDVI:
	    case IFAGatherATDV:
	    case IFReduceAT:
	    case IFReduceATDVI:
	    case IFReduceATDV:
	    case IFRedLeftAT:
	    case IFRedRightAT:
	    case IFRedTreeAT:
		n->exp->temp  = GetTemp( "tmp", n->exp->info, NONE );
		PropagateTemp( n->exp->dst->G_DAD, n->exp->iport, FALSE, 
			       n->exp->temp                           );
		break;

            case IFUReduce:             /* HANDLE IN AssignReturnsTemps() */
                break;

	    case IFScatterBufPartitions:
		if ( !(n->exp->dst->G_DAD->vmark) ) {
		  DecrementRefCount( n->imp->src, n->imp->eport );

		  if ( n->imp->rc > 0 )
		      PropagateTemp( n, 1, FALSE, 
				     GetTemp( "tmp", n->imp->info, NONE ) );
                  else
		      PropagateTemp( n, 1, FALSE, n->imp->temp );

		  if ( IsConst( n->imp->isucc ) )
	              break;

		  if ( !IsFunction( n->imp->isucc->temp->info ) )
		      break;

		  AddAssignNode( n->npred, n, n->imp->isucc, 
			         GetTemp( "tmp", n->imp->isucc->info, NONE ) );
		  break;
		  }

		/* IN VECTOR MODE! */
		PropagateTemp( n, 1, FALSE, 
			       GetTemp( "tmp", n->imp->info->A_ELEM, NONE ) );

		if ( !IsConst( n->imp->isucc ) )
		  if ( IsFunction( n->imp->isucc->temp->info ) )
		    AddAssignNode( n->npred, n, n->imp->isucc, 
			           GetTemp( "tmp", n->imp->isucc->info, NONE ) );
		FreeTemp( n->imp );
		break;

	    case IFRangeGenerate:
		if ( !IsConst( n->imp ) )
		    DecrementRefCount( n->imp->src, n->imp->eport );

		if ( (n->imp->rc > 0) || IsConst( n->imp ) ||
		      n->exp->dst->G_DAD->smark             ) {
		    AddAssignNode( n->npred, n, n->imp,  
				   t = GetTemp( "tmp", n->imp->info, NONE ) );
				   
		    PropagateTemp( n, 1, FALSE, t );
		    }
                else
		    PropagateTemp( n, 1, FALSE, n->imp->temp );

		if ( n->imp->isucc == NULL )               /* CONTROLLER? */
		    break;

		if ( IsConst( n->imp->isucc ) ) {
		    if ( !(n->exp->dst->G_DAD->smark) )
			break;

		    AddAssignNode( n->npred, n, n->imp->isucc, 
				   GetTemp( "tmp", n->imp->isucc->info, NONE ));
				   
		    break;
		    }

		if ( !IsFunction( n->imp->isucc->temp->info ) &&
		     !(n->exp->dst->G_DAD->smark)              )
		    break;

		AddAssignNode( n->npred, n, n->imp->isucc, 
			       GetTemp( "tmp", n->imp->isucc->info, NONE ) );
			       
		break;

	    case IFNoOp:
		if ( IsLastUse( n->imp ) && (n->imp->cm != -1) ) {
		    if ( n->imp->rmark1 == RMARK ) {
			if ( !IsArray( n->imp->info ) ) {
		            PropagateTemp( n, 1, FALSE, n->imp->temp );
		            n->imp->temp->pop--;
			    break;
			    }

                        if ( n->imp->omark1 || n->imp->pmark ) {
		            PropagateTemp( n, 1, FALSE, n->imp->temp );
		            n->imp->temp->pop--;
			    break;
			    }
			}
		    }

		 PropagateTemp( n, 1, FALSE, 
				GetTemp( "tmp", n->exp->info, NONE ) );

                 FreeTemp( n->imp );
                 break;

	    case IFSelect:
	    case IFTagCase:
		AssignSelectTemps( n );
		break;

	    case IFForall:
		ccost += n->ccost;
		AssignForallTemps( n );
		ccost -= n->ccost;
		break;

	    case IFFirstMax:
	    case IFFirstMin:
	    case IFFirstAbsMin:
	    case IFFirstAbsMax:
	    case IFTri:
	    case IFFirstSum:
		ccost += n->ccost;
		AssignVectorTemps( n );
		ccost -= n->ccost;
		break;

            case IFLoopA:
	    case IFLoopB:
if ( IsConst( n->L_TEST->imp ) )
  if ( n->L_TEST->imp->CoNsT[0] == 't' || n->L_TEST->imp->CoNsT[0] == 'T' )
    if ( Warnings ) {
      FPRINTF( stderr, 
	      "%s: W - FOR INITIAL ON LINE %d OF %s IN %s ITERATES FOREVER\n",
	      program, n->line, (n->funct == NULL)? "FUNCT?()" : n->funct,
	      (n->file == NULL)? "FILE?.sis" : n->file );
    }

		ccost += n->ccost;
		AssignLoopTemps( n );
		ccost -= n->ccost;
		break;

	    case IFLGraph:
	    case IFXGraph:
	    case IFLPGraph:
		BindExportTemps( n, n->info, "args" );
		break;

	    case IFLoopPoolEnq:
	    case IFOptLoopPoolEnq:
		/* THESE ARE NEVER FREED! */
		n->temp = GetTemp( "tframe", n->usucc->info, FPTR );

		/* NOT RECURSIVE AND SEQUENTIAL AND GLOBALS ARE SHARED */
		if ( (!recursive) && sequential && gshared ) {
                  FPRINTF( output, "\nstatic shared %s FrAmE%d;\n", 
			   n->temp->info->sname, frametop            );
		  n->temp->fid = frametop++;
		  }
                else
		  n->temp->DeAl = TRUE;

		BindExportTemps( n, n->usucc->info, n->temp->name );

		for ( i = n->imp; i != NULL; i = i->isucc )
		    FreeTemp( i );

                break;

	    case IFSaveSliceParam:
	    case IFBuildSlices:
		for ( i = n->imp; i != NULL; i = i->isucc )
		    FreeTemp( i );

		break;

	    case IFSaveCallParam:
		FreeTemp( n->imp );
		break;

	    case IFCall:
		f = FindFunction( n->imp->CoNsT );

 {
  register int eport;
  register PINFO iii;

  eport = 1;

  for ( iii = f->info->F_OUT; iii != NULL; iii = iii->L_NEXT, eport++ ) {
    if ( IsExport( n, eport ) )
      continue;

    if ( Warnings ) {
      FPRINTF( stderr,
	      "%s: W - RESULT %d OF %s ON LINE %d IN %s IS NEVER USED\n",
	      program, eport,
	      (f->funct == NULL)? "FUNCT?()" : f->funct, n->line, 
	      (n->file == NULL)? "FILE?.sis" : n->file
	      );
    }
  }
 }

		if ( IsIGraph( f ) && f->mark != 's' ) { /* NEW CANN 2/92 */
/* NEW CANN 3/92 */
		    if ( f->mark == 'c' || f->mark == 'f' ) {
		      for ( e = n->exp; e != NULL; e = e->esucc )
		        if ( e->temp == NULL )
			  PropagateTemp( n, e->eport, FALSE, 
				         GetTemp( "tmp", e->info, NONE ) );

		      goto CallMoveOn;
		      }
/* END NEW CANN 3/92 */

		    if ( n->exp != NULL )
		      if ( n->exp->temp == NULL ) {
			  /* ARE THERE ANY INPUTS? */
			  if ( n->imp->isucc != NULL )
			    /* BUG FIX 7/26/91 */
			    if ( n->exp->info == n->imp->isucc->info )
		              if ( IsLastUse( n->imp->isucc ) ) {
		                  PropagateTemp(n,1,FALSE,n->imp->isucc->temp);
			          break;
			          }

                          PropagateTemp( n, 1, FALSE,  
				         GetTemp( "tmp", n->exp->info, NONE ) );
			  }
		     /* END CHANGE */
                } else {
		    /* THESE ARE NEVER FREED! */
		    n->temp = GetTemp( "cframe", f->info, NONE );
		    BindExportTemps( n, f->info, n->temp->name );
		    }

CallMoveOn:
		for ( i = n->imp->isucc; i != NULL; i = i->isucc )
		    FreeTemp( i );

                break;

	    case IFLess:
	    case IFGreat:
	    case IFGreatEqual:
	    case IFLessEqual:
	    case IFNotEqual:
	    case IFEqual:
	    case IFPlus:
	    case IFTimes:
		if ( n->exp == NULL ) {
		    FreeTemp( n->imp );
		    FreeTemp( n->imp->isucc );
		    break;
		    }

	    case IFDiv:
	    case IFExp:
	    case IFMax:
	    case IFMin:
	    case IFMinus:
	    case IFMod:
		if ( n->exp->temp != NULL ) {
		    FreeTemp( n->imp );
		    FreeTemp( n->imp->isucc );
		    break;
		    }

		if ( IsLastUse( n->imp ) && 
		     IsTypeMatch( n->imp, n->exp ) ) {
		    PropagateTemp( n, 1, FALSE, n->imp->temp );
		    FreeTemp( n->imp->isucc );
		    break;
		    }

		if ( IsLastUse( n->imp->isucc ) && 
		     IsTypeMatch( n->imp->isucc, n->exp ) ) {
		    PropagateTemp( n, 1, FALSE, n->imp->isucc->temp );
		    FreeTemp( n->imp );
		    break;
		    }

                FreeTemp( n->imp );
                FreeTemp( n->imp->isucc );

                PropagateTemp( n, 1, FALSE, 
			       GetTemp( "tmp", n->exp->info, NONE ) );
		break;


	    case IFNot:
		if ( n->exp == NULL ) {
		    FreeTemp( n->imp );
		    break;
		    }

	    case IFGetArrayBase:
	    case IFAbs:
	    case IFNeg:
		if ( n->exp->temp == NULL ) {
		    if ( IsLastUse( n->imp ) && 
			 IsTypeMatch( n->imp, n->exp ) ) {
		        PropagateTemp( n, 1, FALSE, n->imp->temp );
			break;
			}

                    PropagateTemp( n, 1, FALSE,  
				   GetTemp( "tmp", n->exp->info, NONE ) );
		    }

		FreeTemp( n->imp );
                break;

	    case IFBool:
	    case IFChar:
	    case IFDouble:
	    case IFFloor:
	    case IFInt:
	    case IFSingle:
	    case IFTrunc:
            case IFUElement:
		FreeTemp( n->imp );

		if ( n->exp->temp == NULL )
                    PropagateTemp( n, 1, FALSE,  
				   GetTemp( "tmp", n->exp->info, NONE ) );

		break;

	    case IFBRReplace:
		if ( n->exp->temp == NULL ) {
		    if ( IsLastUse( n->imp ) ) {
		        PropagateTemp( n, 1, FALSE, n->imp->temp );
			goto FreeRest;
			}

                    PropagateTemp( n, 1, FALSE,  
				   GetTemp( "tmp", n->exp->info, NONE ) );
		    }

		FreeTemp( n->imp );
FreeRest:
		for ( i = n->imp->isucc; i != NULL; i = i->isucc )
		  FreeTemp( i );

		break;

	    case IFSGraph:
	    case IFIGraph:
		break;

	    default:
	        SPRINTF( buf, "Missing node type %d", n->type );
                UNEXPECTED( buf );
	    }

	/* ADJUST FOR OVER-ESTIMATES OF POPULARITY WITHIN COMPOUNDS      */

	if ( IsCompound( n ) )
	    for ( g = n->C_SUBS; g != NULL; g = g->gsucc ) {
		DecExportPopularity( g );
		DecImportPopularity( g );
		}

	/* ADJUST FOR MULTIPLE REFERENCES WITHIN SOME MACROS             */

	switch ( n->type ) {
	    case IFMax:
	    case IFMin:
		if ( !IsConst( n->imp ) )
		    IncPopularity( n->imp, 1 );

		if ( !IsConst( n->imp->isucc ) )
		    IncPopularity( n->imp->isucc, 1 );

		break;

	    case IFALimH:
		IncPopularity( n->imp, 1 );
		break;

	    case IFASetL:
		IncPopularity( n->imp, 3 );
		break;

	    case IFAAdjust:
	    case IFARemL:
		IncPopularity( n->imp, 1 );
		break;

	    case IFAAddHATDVI:
	    case IFAAddLATDVI:
	    case IFAAddHAT:
	    case IFAAddLAT:
	    case IFABuildAT:
		IncPopularity( n->exp, 4 );
		break;

	    case IFAGatherAT:
	    case IFAGatherATDVI:
	    case IFShiftBuffer:
		IncPopularity( n->exp, 1 );
		break;

	    case IFOptAReplace:
		cnt = -1;

		ii = FindImport( n, MAX_PORT_NUMBER );

		for ( i = n->imp->isucc->isucc; i != ii; i = i->isucc )
		    cnt++;

		IncPopularity( ii, cnt );
		break;

	    default:
		break;
	    }
        }
}


/**************************************************************************/
/* GLOBAL **************   PrintFrameDeallocs      ************************/
/**************************************************************************/
/* PURPOSE: PRINT THE LOOP POOL FRAME STORAGE DEALLOCATIONS TO output.    */
/**************************************************************************/

void PrintFrameDeallocs()
{
  register PTEMP p;

  for ( p = thead; p != NULL; p = p->next )
    if ( p->DeAl )
      FPRINTF( output, "  DeAlloc( %s );\n", p->name );
}
