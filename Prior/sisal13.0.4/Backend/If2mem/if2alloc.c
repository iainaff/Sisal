/* if2alloc.c,v
 * Revision 12.7  1992/11/04  22:05:05  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:09:21  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"
extern int agg;


/**************************************************************************/
/* STATIC **************    AttachSizeExpression   ************************/
/**************************************************************************/
/* PURPOSE: ATTACH p'S SIZE EXPRESION exp TO p AT IMPORT PORT iport.      */
/**************************************************************************/

static void AttachSizeExpression( p, expr, iport )
PNODE p;
PEDGE expr;
int   iport;
{
  register PEDGE e;

  if ( IsConstSize( expr ) ) {
    e = EdgeAlloc( NULL_NODE, CONST_PORT, p, iport );
    e->info  = integer;
    e->CoNsT = IntToAscii( expr->csize );

    LinkImport( p, e );
    }
  else
    ThreadToUse( expr->esize, 1, p, iport, integer );
}


/**************************************************************************/
/* LOCAL  **************  MakeMultipleOrBufferInfo ************************/
/**************************************************************************/
/* PURPOSE: APPEND A MULTIPLE OR BUFFER TYPE TO THE SYMBOL TABLE AND      */
/*          RETURN IT.                                                    */
/**************************************************************************/

static PINFO MakeMultipleOrBufferInfo( type, aelem )
int   type;
PINFO aelem;
{
    register PINFO i;

    i = FindInfo( ++maxint, type );

    i->funct = FALSE;                  /* REFER TO if1build.c: MakeInfo(...) */
    i->file = FALSE;

    i->A_ELEM = aelem;

    return( i );
}


/**************************************************************************/
/* LOCAL  **************   ABuildReconstruction    ************************/
/**************************************************************************/
/* PURPOSE: REPRODUCE ABuild NODE e->src LINKING IT DIRECTLY TO THE LEFT  */
/*          OF e->dst IN ITS NODE LIST.  A REFERENCE TO THE COPY IS       */
/*          RETURNED.                                                     */
/**************************************************************************/

static PEDGE ABuildReconstruction( e )
PEDGE e;
{
    register PNODE pr;
    register PEDGE ee;
    register PEDGE i;
    register PNODE bld;

    pr = e->dst->npred;
    ee = FindSource( e );

    bld = NodeAlloc( ++maxint, IFABuildAT );
    bld->lstack = pr->lstack;
    bld->level  = pr->level;

    CopyPragmas( ee->src, bld );

    LinkNode( pr, bld );

    for ( i = ee->src->imp; i != NULL; i = i->isucc ) {
	if ( IsConst( i ) ) {
	    CopyEdgeAndThreadToUse( i, bld, i->iport );
	    continue;
	    }

	ThreadToUse( i->src, i->eport, bld, i->iport, i->info );

	if ( !IsArray( i->info ) )
	    continue;

	/* MAKE SURE THE NEW REFERENCE HAS THE PROPER SIZE INFORMATION    */

        for ( ee = i->src->exp; ee != NULL; ee = ee->esucc )
	    if ( ee->eport == i->eport ) {
		ee->lsize = i->lsize;
		ee->esize = i->esize;
		ee->csize = i->csize;
		}
        }

    /* LINK AND RETURN A REFERENCE TO THE COPY                            */

    ee = EdgeAlloc( bld, 1, NULL_NODE, -1 );

    ee->info  = e->info;
    ee->lsize = e->lsize;
    ee->csize = e->csize;
    ee->esize = e->esize;

    LinkExport( bld, ee );

    return( ee );
}


/**************************************************************************/
/* LOCAL  **************       AllocNewBuffer      ************************/
/**************************************************************************/
/* PURPOSE: CONSTRUCT AND RETURN A REFERENCE TO A BUFFER DEFINITION AND   */
/*          STORAGE ALLOCATION SUBGRAPH FOR THE SIZE EXPRESSION DEFINED   */
/*          BY EDGE e USING NODE rpoint AS THE REFERENCE POINT.           */
/**************************************************************************/

static PEDGE AllocNewBuffer( rpoint, e )
PNODE rpoint;
PEDGE e;
{
    register PEDGE ee;
    register PNODE pr;
    register PNODE n;

    /* WHERE SHOULD THE SUBGRAPH BE PLACED WITHIN rpoint's NODE LIST?     */

    if ( IsConstSize( e ) )
	pr = rpoint->lstack[rpoint->level];
    else if ( e->esize->level < rpoint->level )
	pr = rpoint->lstack[rpoint->level];
    else
	pr = e->esize;

    /* ALLOCATE IFDefArrayBuf( SIZE, BUFFER LITERAL )                     */

    n = NodeAlloc( ++maxint, IFDefArrayBuf );
    n->lstack = pr->lstack;
    n->level  = pr->level;

    LinkNode( pr, n );
    pr = n;

    /* ATTACH SIZE EXPRESSION                                             */
    AttachSizeExpression( n, e, 1 );

    /* ATTACH BUFFER LITERAL                                             */
    ee = EdgeAlloc( NULL_NODE, CONST_PORT, n, 2 );
    ee->info  = e->info->A_ELEM;
    ee->CoNsT = "Buffer";

    LinkImport( n, ee );

    /* ALLOCATE IFMemAlloc( IFDefArrayBuf(...,...) )                     */

    n = NodeAlloc( ++maxint, IFMemAlloc );
    n->lstack = pr->lstack;
    n->level  = pr->level;
    LinkNode( pr, n );

    /* ATTACH IFDefArrayBuf NODE                                         */

    ThreadToUse( pr, 1, n, 1, MakeMultipleOrBufferInfo( IF_BUFFER, e->info ) );

    ee = EdgeAlloc( n, 1, NULL_NODE, -1 );
    ee->info  = n->imp->info;

    LinkExport( n, ee );

    return( ee );
}


/**************************************************************************/
/* LOCAL  **************         GetBuffer         ************************/
/**************************************************************************/
/* PURPOSE: LOCATE AN AT-NODE CONSUMER OF NODE p'S EXPORT AND GENERATE A  */
/*          SUBGRAPH (IF NECESSARY) CALCULATING p'S BUFFER (A FUNCTION OF */
/*          THE CHILD'S BUFFER).  DATA FLOW ORDERING CHECKS ARE MADE TO   */
/*          GUARANTEE ITS PRESERVATION.  A REFERENCE TO THE CALCULATED    */
/*          BUFFER IS RETURNED.  IF minopt IS TRUE, A NEW BUFFER IS       */
/*          ALLOCATED FOR p AND A CHILD AT-NODE SEARCH IS NOT PERFORMED.  */
/**************************************************************************/

static PEDGE GetBuffer( p )
PNODE p;
{
    register PEDGE e;
    register PNODE n;
    register PEDGE i1;
    register PNODE pr;
    register PEDGE ee;
    register PEDGE b;
    register PEDGE rat;
    register PNODE scat;
    register int   eport;

    /* ATTEMPT TO LOCATE AN AT-NODE CHILD OF p GIVING PRIORITY TO        */
    /* IFReduceAT NODES (A SPECIAL CASE!).                               */

    switch ( p->type ) {
	case IFAGather:
	    if ( p->exp->esucc != NULL )
		Error1( "GetMemoryBuffer: RETURN NODE WITH FANOUT > ONE" );

	    if ( p->imp->isucc->isucc != NULL )               /* FILTER? */
		return( AllocNewBuffer( p->exp->dst->G_DAD, p->exp ) );

            eport = p->exp->iport;
	    p     = p->exp->dst->G_DAD;
	    break;

	case IFFinalValue:
	case IFReduce:
	case IFRedLeft:
	case IFRedRight:
	case IFRedTree:
	    if ( p->exp->esucc != NULL )
		Error1( "GetMemoryBuffer: RETURN NODE WITH FANOUT > ONE" );

            eport = p->exp->iport;
	    p     = p->exp->dst->G_DAD;
	    break;

	default:
	    eport = 1;
	    break;
	}

    /* UNLESS minopt IS TRUE, CHECK IF p HAS A IFReduceAT CHILD           */

    if ( !minopt )
        for ( e = p->exp; e != NULL; e = e->esucc ) {
	    if ( e->eport != eport )
	        continue;

            if ( !IsSGraph( e->dst ) )  /* IS IMMEDIATE CHILD A SUBGRAPH? */
	        continue;

	    n = e->dst->G_DAD;

            if ( !IsForall( n ) )              /* IS IT PART OF A FORALL? */
                continue;

            if ( n->F_BODY != e->dst )          /* IS IT THE FORALL BODY? */
	        continue;

	    if ( (rat = FindExport( n->F_RET, e->iport )) == NULL )
	        continue;

            if ( !IsReduceAT( rat->dst ) )
	        continue;

	    /* BUILD IFScatterBufPartitions IN THE GENERATE SUBGRAPH AND  */
	    /* RETURN A REFERENCE TO THE SCATTERED PARTITIONS WITHIN THE  */
	    /* BODY SUBGRAPH.                                             */
    
	    pr = FindLastNode( n->F_GEN );   /* PLACE AT END OF NODE LIST */

	    scat = NodeAlloc( ++maxint, IFScatterBufPartitions );
	    scat->lstack = pr->lstack;
	    scat->level  = pr->level;
	    LinkNode( pr, scat );

	    b = FindImport( rat->dst, 5 );       /* FIND BUFFER REFERENCE */

	    /* ATTACH BUFFER TO IFScatterBufPartitions                    */

            ThreadToUse( n->F_GEN, b->eport, scat, 1, b->info );

	    /* ATTACH STEP SIZE TO IFScatterBufPartitions                 */
            AttachSizeExpression( scat, e, 2 );


	    /* ATTACH IFScatterBufPartitions EXPORT AND BODY REFERENCE    */

	    eport = ++maxint;

	    ThreadToUse( scat, 1, n->F_GEN, eport, 
		         MakeMultipleOrBufferInfo( IF_MULTIPLE, b->info ) );

	    ee = EdgeAlloc( n->F_BODY, eport, NULL_NODE, -1 ); 
	    ee->info = b->info;

	    LinkExport( n->F_BODY, ee );

	    /* ASSIGN %mk=P TO THE MULTIPLE IMPORT OF THE REDUCE AT-NODE  */

	    MarkAsBuiltInplace( rat->dst->imp->isucc->isucc );
	    MarkAsBuiltInplace( e );                       

	    return( ee );
	    }

    /* AN IFReduceAT CHILD WAS NOT FOUND; HENCE, ATTEMPT TO  LOCATE A       */
    /* NON-IFReduceAT AT-NODE CHILD (THE FIRST ENCOUNTER WINS!)             */

    for ( e = p->exp; e != NULL; e = e->esucc )
	if ( e->eport == eport ) {
	    ee = e;                     /* TO REMEMBER THE SIZE EXPRESSION  */
                                        /* IF AN AT-NODE CHILD NOT FOUND    */

	    if ( IsAtNode( e->dst ) ) {
		if ( IsABuildAT( e->dst ) || IsAFillAT( e->dst ) )
		    continue;

	        break;
		}
            }

    if ( minopt || (e == NULL) )                  /* CREATE A NEW BUFFER? */
	return( AllocNewBuffer( p, ee ) );


    /* THE CHILD AT-NODE MUST BE EITHER AN IFAAddHAT, IFAAddLAT, OR         */
    /* IFACatenateAT WITH THE THIRD IMPORT DEFINING ITS BUFFER              */

    b = e->dst->imp->isucc->isucc;

    switch ( e->dst->type ) {
	case IFAAddHAT:
	    if ( (MaxNodeInDFO( p, b->src, p ) != p) || (e->iport == 2) )
		return( AllocNewBuffer( p, e ) );

            MarkAsBuiltInplace( e );

	    /* RETURN A NEW REFERENCE TO THE BUFFER                         */

	    ee = EdgeAlloc( b->src, b->eport, NULL_NODE, -1 );
	    ee->info = b->info;

	    LinkExport( b->src, ee );

	    return( ee );

	case IFAAddLAT:
	    if ( (MaxNodeInDFO( p, b->src, p ) != p ) || (e->iport == 2) )
		return( AllocNewBuffer( p, e ) );

	    /* ALLOCATE IFShiftBuffer( b, 1 )                               */

	    n = NodeAlloc( ++maxint, IFShiftBuffer );
	    n->lstack = p->lstack;
	    n->level  = p->level;

	    LinkNode( b->src, n );

	    /* ATTACH 1 TO THE SECOND IMPORT OF THE SHIFT NODE              */

	    ee = EdgeAlloc( NULL_NODE, CONST_PORT, n, 2 );
	    ee->info  = integer;
	    ee->CoNsT = "1";

	    LinkImport( n, ee );

	    /* ATTACH A b REFERENCE TO THE FIRST IMPORT OF THE SHIFT NODE   */
	    CopyEdgeAndThreadToUse( b, n, 1 );

	    /* RETURN A REFERENCE TO THE SHIFT NODE                         */

            ee = EdgeAlloc( n, 1, NULL_NODE, -1 );
	    ee->info = b->info;

	    LinkExport( n, ee );

	    /* ASSIGN %mk=P TO e AS e->src WILL BUILD e INPLACE AND e->dst  */
	    /* NEED DO NOTHING WITH e.                                      */

	    MarkAsBuiltInplace( e );

	    return( ee );

	case IFACatenateAT:
	    if ( e->iport == 1 ) {
		if ( MaxNodeInDFO( p, b->src, p ) != p )
		    return( AllocNewBuffer( p, e ) );

	        /* ASSIGN %mk=P TO e AS e->src WILL BUILD e INPLACE AND    */
	        /* e->dst NEED DO NOTHING WITH e.                          */
                MarkAsBuiltInplace( e );

		ee = EdgeAlloc( b->src, b->eport, NULL_NODE, -1 );
		ee->info = b->info;

		LinkExport( b->src, ee );

		return( ee );
		}

	    i1 = e->dst->imp;

	    if ( MaxNodeInDFO( p, b->src, p ) != p )
		return( AllocNewBuffer( p, e ) );

            if ( !IsConstSize( i1 ) )
		if ( MaxNodeInDFO( p, i1->esize, p ) != p )
		    return( AllocNewBuffer( p, e ) );

	    /* ALLOCATE IFShiftBuffer( b, SIZE OF FIRST IMPORT )            */

            n = NodeAlloc( ++maxint, IFShiftBuffer );
	    n->lstack = p->lstack;
	    n->level  = p->level;

	    if ( IsConstSize( i1 ) )
		pr = b->src;
            else if ( i1->esize->level < p->level )
		pr = b->src;
            else
		pr = MaxNodeInDFO( p, b->src, i1->esize );

            LinkNode( pr, n );

	    /* LINK b REFERENCE TO THE SHIFT NODE                           */
	    CopyEdgeAndThreadToUse( b, n, 1 );

	    /* LINK THE SIZE OF THE FIRST IMPORT TO THE SHIFT NODE          */
            AttachSizeExpression( n, i1, 2 );

	    /* RETURN A REFERENCE TO THE SHIFT NODE                         */

            ee = EdgeAlloc( n, 1, NULL_NODE, -1 );
	    ee->info = b->info;

	    LinkExport( n, ee );

	    /* ASSIGN %mk=P TO e AS e->src WILL BUILD e INPLACE AND e->dst  */
	    /* NEED DO NOTHING WITH e.                                      */

	    MarkAsBuiltInplace( e );

	    return( ee );

        default:
	    Error1( "GetBuffer: ILLEGAL CHILD AT-NODE ENCOUNTERED" );
        }

    return NULL;
}


/**************************************************************************/
/* GLOBAL **************       AllocIf2Nodes       ************************/
/**************************************************************************/
/* PURPOSE: USE THE SIZE INFORMATION TO CONSTRUCT BUFFERS AND LINK THEM   */
/*          TO THEIR USES WITHIN FUNCTION GRAPH f.                        */
/**************************************************************************/

void AllocIf2Nodes( f )
PNODE f;
{
    register PNODE p, n, nn, m;
    register PEDGE b, init;
    register PNODE l, g, bld; 
    register PEDGE e, lval, lbody;
    register int   old;

    while ( (p = PopAtNode()) != NULL ) {
	if ( IsAtNode( p ) )
	    continue;

        if ( p->exp == NULL )                                    /* DEAD? */
	    continue;

        /* GET NODE p'S BUFFER, A FUNCTION OF ITS AT-NODE CHILD           */

	b = GetBuffer( p ); 

	++agg;

	/* LINK p'S BUFFER TO p.  IF p MAIPULATES A MULTIPLE VALUE THEN   */
	/* THE BUFFER IS APPROPRIATELY LINKED TO OTHER POTENTIAL AT-NODES */
	/* WITHIN THE OWNING LOOP.                                        */

	switch ( p->type ) {
	    case IFAFill:
		p->type  = IFAFillAT;
		b->iport = 4;

		LinkImport( p, b );

                /* ATTACH SIZE EXPRESSION: NOTE THIS VIOLATES IF2 */
                AttachSizeExpression( p, p->exp, 5 );
		break;

	    case IFABuild:
		p->type  = IFABuildAT;
		b->iport = p->exp->csize + 2;

		LinkImport( p, b );
		break;

	    case IFAAddH:
		p->type  = IFAAddHAT;
		b->iport = 3;
		LinkImport( p, b );

                /* ATTACH SIZE EXPRESSION: NOTE THIS VIOLATES IF2 */
                AttachSizeExpression( p, p->exp, 4 );
		break;

	    case IFAAddL:
		p->type  = IFAAddLAT;
		b->iport = 3;
		LinkImport( p, b );

                /* ATTACH SIZE EXPRESSION: NOTE THIS VIOLATES IF2 */
                AttachSizeExpression( p, p->exp, 4 );
		break;

	    case IFACatenate:
		p->type  = IFACatenateAT;
		b->iport = 3;
		LinkImport( p, b );

                /* ATTACH SIZE EXPRESSION: NOTE THIS VIOLATES IF2 */
                AttachSizeExpression( p, p->exp, 4 );
		break;

	    case IFAGather:
		p->type  = IFAGatherAT;
		b->iport = ++maxint;

		g = p->exp->dst;   /* SUBGRAPH WHOSE NODE LIST CONTAINS p */

		/* LINK BUFFER b TO THE LOOPS IMPORT LIST                 */
		LinkImport( g->G_DAD, b );

		/* LINK A REFERENCE TO THE IMPORTED BUFFER TO p           */
		ThreadToUse( g, maxint, p, 4, b->info );

		/* IF p'S BUFFER IS A FUNCTION OF ANOTHER AT-NODE THEN    */
		/* MARK ITS EXPORT.                                       */

                if ( (!IsMemAlloc( b->src )) || (b->src->exp->esucc != NULL) ) 
		    MarkAsBuiltInplace( p->exp );

                /* ATTACH SIZE EXPRESSION: NOTE THIS VIOLATES IF2 */
                AttachSizeExpression( p, p->exp, 5 );
		break;

	    case IFReduce:
		p->type = IFReduceAT;   goto ReduceStart;
	    case IFRedLeft:
		p->type = IFRedLeftAT;  goto ReduceStart;
	    case IFRedRight:
		p->type = IFRedRightAT; goto ReduceStart;
	    case IFRedTree:
		p->type = IFRedTreeAT;

ReduceStart:

		/* IF p'S BUFFER IS A FUNCTION OF ANOTHER AT-NODE THEN    */
		/* MARK ITS EXPORT.                                       */

                if ( (!IsMemAlloc( b->src )) || (b->src->exp->esucc != NULL) ) 
		    MarkAsBuiltInplace( p->exp );

		g = p->exp->dst;   /* SUBGRAPH WHOSE NODE LIST CONTAINS p */

		/* LINK BUFFER b TO THE FORALL NODE AND TO p ITSELF.      */

		b->iport = ++maxint;
		LinkImport( g->G_DAD, b );
		ThreadToUse( g, maxint, p, 5, b->info );

                /* ATTACH SIZE EXPRESSION: NOTE THIS VIOLATES IF2         */
                AttachSizeExpression( p, p->exp, 6 );
		break;

	    case IFFinalValue:
		p->type = IFFinalValueAT;

		/* IF p'S BUFFER IS A FUNCTION OF ANOTHER AT-NODE THEN    */
		/* MARK ITS EXPORT.                                       */

                if ( (!IsMemAlloc( b->src )) || (b->src->exp->esucc != NULL) ) 
		    MarkAsBuiltInplace( p->exp );

		g = p->exp->dst;   /* SUBGRAPH WHOSE NODE LIST CONTAINS p */
		l = g->G_DAD;      /* SEQUENTIAL LOOP OWNING g            */

		/* LINK THE BUFFER b TO l AND TO p ITSELF.  MARK p'S      */
		/* MULTIPLE IMPORT AS BUILT INPLACE (DONE IN THE BODY)    */

		b->iport = ++maxint;

		LinkImport( l, b );
		ThreadToUse( g, maxint, p, 3, b->info );
		MarkAsBuiltInplace( p->imp );

		/* FIND THE ABuild NODE SERVING AS p'S INITIAL VALUE     */
                lval = FindImport( l->L_INIT, p->imp->eport );
		init = FindImport( l, lval->eport );

		MarkAsBuiltInplace( lval );

		/* RECONSTRUCT THE INITIAL ABuild SO TO GUARANTEE         */
		/* PRESERVATION OF DATA FLOW ORDERING. Init IS MARKED AS  */
		/* BUILT INPLACE.                                         */

		init = ABuildReconstruction( init );
		bld  = init->src;

		init->iport = ++maxint;
		lval->eport = maxint;

		LinkImport( l, init );
		MarkAsBuiltInplace( init );

		/* LOCATE MONOTONIC CONSTRUCTION NODE IN THE LOOP BODY    */
		/* AND MARK IT AS BUILT INPLACE                           */

		lbody = FindImport( l->L_BODY, lval->iport );
		MarkAsBuiltInplace( lbody );

		/* COMPLETE MONOTONIC CONSTURCTION OF EACH INDIVIDUAL CASE */

		m = lbody->src;

		switch ( m->type ) {
		    case IFAAddH:
			m->type = IFAAddHAT;

			/* LINK l's IMPORT BUFFER b TO m IN THE BODY AND   */ 
			/* MARK THE FIRST IMPORT TO m AS BUILT INPLACE     */

			ThreadToUse( l->L_BODY, b->iport, m, 3, b->info );
			MarkAsBuiltInplace( m->imp );

			/* LINK A b REFERENCE TO THE RECONSTRUCTED ABuilt  */
			/* NODE.                                           */ 
			CopyEdgeAndThreadToUse( b, bld, init->csize + 2 );

                        /* ATTACH SIZE EXPRESSION: NOTE THIS VIOLATES IF2 */
                        /* AttachSizeExpression( m, p->exp, 4 ); */
                        AttachSizeExpression( m, m->exp, 4 );
			break;

		    case IFAAddL:
			m->type = IFAAddLAT;

			/* CONSTRUCT IFShiftBuffer( old b, -1 )            */

			n = NodeAlloc( ++maxint, IFShiftBuffer );
			n->lstack = m->lstack;
			n->level  = m->level;
			LinkNode( m->npred, n );

			/* ATTACH THE -1 IMPORT TO THE SHIFT NODE          */

			e = EdgeAlloc( NULL_NODE, CONST_PORT, n, 2 );
			e->info  = integer;
			e->CoNsT = "-1";
			LinkImport( n, e );

			/* ATTACH old b TO THE SHIFT NODE                  */
			ThreadToUse( l->L_BODY, ++maxint, n, 1, b->info );

			/* LINK A REFERENCE FROM n TO m AND n TO THE BODY  */
			/* AND MARK m's FIRST IMPORT AS BUILT INPLACE      */

			ThreadToUse( n, 1, m, 3, b->info );
			ThreadToUse( n, 1, l->L_BODY, maxint, b->info );
			MarkAsBuiltInplace( m->imp );

			/* INITIALIZE THE INITIAL SUBGRAPH SO THAT THE     */
			/* BUFFER CAN BE CARRIED ACROSS LOOP ITERATIONS.   */

                        old = maxint;
			ThreadToUse( l->L_INIT, ++maxint, 
				     l->L_INIT, old, b->info );
			old = maxint;

			/* ADJUST b LINKING IT TO THE RECONSTRUCTED ABuild */
			/* NODE AND l: IFShiftBuffer( b, SIZE OF RESULT -  */
			/* SIZE OF THE ABUILD )                            */

                        n = NodeAlloc( ++maxint, IFShiftBuffer );
			n->lstack = bld->lstack;
			n->level  = bld->level;
			LinkNode( bld->npred, n );

			/* ATTACH b REFERENCE TO THE SHIFT NODE            */ 
			CopyEdgeAndThreadToUse( b, n, 1 );

			/* ATTACH SHIFT AMOUNT TO THE SHIFT NODE           */

			nn = NodeAlloc( ++maxint, IFMinus );
			nn->lstack = n->lstack;
			nn->level  = n->level;
			LinkNode( n->npred, nn );

			/* LINK RESULT SIZE TO FIRST IMPORT OF MINUS NODE  */
			ThreadToUse( p->exp->esize, 1, nn, 1, integer );

			/* LINK ABuild SIZE TO SECOND IMPORT OF MINUS NODE */

			e = EdgeAlloc( NULL_NODE, CONST_PORT, nn, 2 );
			e->info = integer;
			e->CoNsT = IntToAscii( init->csize );
			LinkImport( nn, e );

			/* ATTACH THE MINUS NODE TO THE SHIFT NODE         */
			ThreadToUse( nn, 1, n, 2, integer );

			/* ATTACH REFERENCE FROM SHIFT NODE TO THE ABuild  */
			/* NODE AND TO l.                                  */

			ThreadToUse( n, 1, bld, init->csize + 2, b->info );
			ThreadToUse( n, 1, l, old, b->info );

                        /* ATTACH SIZE EXPRESSION: NOTE THIS VIOLATES IF2 */
                        /* AttachSizeExpression( m, p->exp, 4 ); */
                        AttachSizeExpression( m, m->exp, 4 );
			break;

		    case IFACatenate:
			m->type = IFACatenateAT;

                        /* ATTACH SIZE EXPRESSION: NOTE THIS VIOLATES IF2 */
                        /* AttachSizeExpression( m, p->exp, 4 ); */
                        AttachSizeExpression( m, m->exp, 4 );

			if ( m->imp->eport == lbody->iport ) { 

			    /* LINK l's IMPORT BUFFER b TO m IN THE BODY   */
			    /* AND MARK THE FIRST IMPORT TO m AS BUILT     */
			    /* INPLACE                                     */

			    ThreadToUse( l->L_BODY, b->iport, m, 3, b->info );
			    MarkAsBuiltInplace( m->imp );

			    /* LINK A b REFERENCE TO THE RECONSTRUCTED     */
			    /* ABuild NODE.                                */ 
			    CopyEdgeAndThreadToUse( b, bld, init->csize + 2 );
			    break;
			    }

			/* CONSTRUCT IFShiftBuffer( old b, SIZE OF FIRST  */
			/* IMPORT TO m)                                   */

			n = NodeAlloc( ++maxint, IFShiftBuffer );
			n->lstack = m->lstack;
			n->level  = m->level;

			/* LINK AT HEAD OF NODE LIST. THE EXPANSION SIZE   */
			/* EXPRESSION IS EITHER CONSTANT OR AT A LEVEL     */
			/* BELOW THE LOOP (IsMonotonicConstruction)        */
			LinkNode( m->lstack[m->level], n );

			/* ATTACH THE NEGATION OF OF m'S FIRST IMPORT SIZE */
			/* TO THE SHIFT NODE                               */

			if ( IsConstSize( m->imp ) ) {
			    e = EdgeAlloc( NULL_NODE, CONST_PORT, n, 2 );
			    e->info  = integer;
			    e->CoNsT = IntToAscii( -(m->imp->csize) );

			    LinkImport( n, e );
                        } else {
			    nn = NodeAlloc( ++maxint, IFNeg );
			    nn->lstack = n->lstack;
			    nn->level  = n->level;
			    LinkNode( n->npred, nn );

			    ThreadToUse( m->imp->esize, 1, nn, 1, integer );
			    ThreadToUse( nn, 1, n, 2, integer );
			    }


			/* ATTACH old b TO THE SHIFT NODE                  */
			ThreadToUse( l->L_BODY, ++maxint, n, 1, b->info );

			/* LINK A REFERENCE FROM n TO m AND n TO THE BODY  */
			/* AND MARK m's SECOND IMPORT AS BUILT INPLACE     */

			ThreadToUse( n, 1, m, 3, b->info );
			ThreadToUse( n, 1, l->L_BODY, maxint, b->info );
			MarkAsBuiltInplace( m->imp->isucc );

			/* INITIALIZE THE INITIAL SUBGRAPH SO THAT THE     */
			/* BUFFER CAN BE CARRIED ACROSS LOOP ITERATIONS.   */

                        old = maxint;
			ThreadToUse( l->L_INIT, ++maxint, 
				     l->L_INIT, old, b->info );
			old = maxint;

			/* ADJUST b LINKING IT TO THE RECONSTRUCTED ABuild */
			/* NODE AND l: IFShiftBuffer( b, SIZE OF RESULT -  */
			/* SIZE OF THE ABUILD )                            */

                        n = NodeAlloc( ++maxint, IFShiftBuffer );
			n->lstack = bld->lstack;
			n->level  = bld->level;
			LinkNode( bld->npred, n );

			/* ATTACH b REFERENCE TO THE SHIFT NODE            */ 
			CopyEdgeAndThreadToUse( b, n, 1 );

			/* ATTACH SHIFT AMOUNT TO THE SHIFT NODE           */

			nn = NodeAlloc( ++maxint, IFMinus );
			nn->lstack = n->lstack;
			nn->level  = n->level;
			LinkNode( n->npred, nn );

			/* LINK RESULT SIZE TO FIRST IMPORT OF MINUS NODE  */
			ThreadToUse( p->exp->esize, 1, nn, 1, integer );

			/* LINK ABuild SIZE TO SECOND IMPORT OF MINUS NODE */

			e = EdgeAlloc( NULL_NODE, CONST_PORT, nn, 2 );
			e->info = integer;
			e->CoNsT = IntToAscii( init->csize );
			LinkImport( nn, e );

			/* ATTACH THE MINUS NODE TO THE SHIFT NODE         */
			ThreadToUse( nn, 1, n, 2, integer );

			/* ATTACH REFERENCE FROM SHIFT NODE TO THE ABuild  */
			/* NODE AND TO l.                                  */

			ThreadToUse( n, 1, bld, init->csize + 2, b->info );
			ThreadToUse( n, 1, l, old, b->info );
			break;

		    default:
			Error1( "AllocIf2Nodes: ILLEGAL MONOTONIC BODY NODE" );
			break;
		    }

		break;

	    default:
		Error1( "AllocIf2Nodes: ILLEGAL POTENTIAL AT-NODE" );
		break;
	    }
	}
}
