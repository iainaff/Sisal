/* if1inline.c,v
 * Revision 12.7  1992/11/04  22:04:57  miller
 * Initial revision
 *
 * Revision 12.7  1992/10/21  18:08:34  miller
 * Initial RCS Version by Cann
 * */

#include "world.h"


#define FIRST_COLOR       0
#define SIZE_THRESHOLD    6000
#define MIN_FUNCTION_SIZE 100

static PCALL callhead = NULL;           /* HEAD AND TAIL OF CALL GRAPH TRUNK */
static PCALL calltail = NULL;

static int   color = FIRST_COLOR;    /* CURRENT COLOR IN CYCLE DETECTION  */
static int   sskip = 0;                    /* COUNT OF SKIPPED EXPANSIONS */

int NoInlining = FALSE;		/* Allow Inlined functions */
int noincnt = -1;                             /* FUNCTION CALL LIST INDEX */
char *noin[500];                                    /* FUNCTION CALL LIST */


static int GetGraphSize( g )
PNODE g;
{
  register PNODE sg;
  register int   size;
  register PNODE n;

  size = 1;

  for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
    if ( IsCompound( n ) )
      for ( sg = n->C_SUBS; sg != NULL; sg = sg->gsucc )
	size += GetGraphSize( sg );

    size++;
    }

  return( size );
}


/**************************************************************************/
/* LOCAL  **************       OptFindCallee       ************************/
/**************************************************************************/
/* PURPOSE: FIND AND RETURN THE CALL GRAPH TRUNK NODE ASSOCIATED WITH     */
/*          FUNCTION nm. IF NOT FOUND, NULL IS RETURNED.                  */
/**************************************************************************/

static PCALL OptFindCallee( nm )
char *nm;
{
    register PCALL c;

    for ( c = callhead; c != NULL; c = c->callee )
	if ( strcmp( nm, c->graph->G_NAME ) == 0 )
	    return( c );

    return( NULL );
}


/**************************************************************************/
/* LOCAL  **************   OptAddCalleeReferences  ************************/
/**************************************************************************/
/* PURPOSE: RECURSIVELY TRAVERSE THE IF1 GRAPH g, ADDING A CALL GRAPH     */
/*          NODE TO THE CALLEE REFERENCE LIST HEADED BY TRUNK NODE c FOR  */
/*          EACH ENCOUNTERED IF1 CALL NODE. THE REFERENCE COUNT OF THE    */
/*          CALLEE'S CALL GRAPH TRUNK NODE IS INCREMENTED.                */
/**************************************************************************/

static void OptAddCalleeReferences( c, g )
PCALL c;
PNODE g;
{
    register PNODE n;
    register PCALL r;
    register PCALL cee; 

    if ( IsIGraph( g ) )
	return;

    /* BUG Oct. 21, 1991 */
    for ( n = g->G_NODES; n != NULL; n = n->nsucc ) {
	if ( IsCall( n ) ) {
	    if ( (cee = OptFindCallee( n->imp->CoNsT )) == NULL )
		continue;

	    r = CallAlloc( n );

	    r->caller = c->caller;
	    c->caller = r;

	    r->callee = cee;
	    r->callee->rcnt++;

	    continue;
	    }

        if ( IsCompound( n ) )
	    for ( g = n->C_SUBS; g != NULL; g = g->gsucc )
		OptAddCalleeReferences( c, g );
        }
}


/**************************************************************************/
/* LOCAL  **************     OptMakeCallGraph      ************************/
/**************************************************************************/
/* PURPOSE: CONSTRUCT A CALL GRAPH.  FIRST THE TRUNK OF THE CALL GRAPH IS */
/*          BUILT.  THE TRUNK CONTAINS AN ENTRY FOR EACH FUNCTION IN THE  */ 
/*          IF1 FILE.  IF A CALLEE IS EXPORTED, THEN ITS REFERENCE COUNT  */
/*          FIELD (rcnt) IS INCREMENTED TO MAKE SURE THAT IT WILL NOT BE  */
/*          DESTROYED DURING EXPANSION.  AN IMPORTED CALLEE CAN  NEVER BE */
/*          INLINED. AFTER THE TRUNK IS BUILT, THE CALLEE REFERENCE LISTS */
/*          ARE CONSTRUCTED.                                              */
/**************************************************************************/

static void OptMakeCallGraph()
{
    register PNODE f;
    register PCALL c;

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
	c = CallAlloc( cfunct = f );

	if ( IsIGraph( f ) )
	    c->InLineFunction = FALSE;
        else if ( IsXGraph( f ) )
	    c->rcnt++;

	if ( callhead != NULL )
	    calltail->callee = c;
        else
	    callhead = c;

        calltail = c;
	}

    for ( c = callhead; c != NULL; c = c->callee )
	OptAddCalleeReferences( c, c->graph );
}


/**************************************************************************/
/* LOCAL  **************    ReadInlineRequests     ************************/
/**************************************************************************/
/* PURPOSE: READ FROM stdin EXPANSION REQUESTS.  A PROMPT IS WRITTEN  TO  */
/*          stdout FOR ONLY FUNCTION'S THAT ARE ALLOWED TO BE INLINED;    */
/*          THE USER HAS NO CONTROL OVER OTHER FUNCTIONS: THOSE IMPORTED  */
/*          OR PART OF RECURSIVE CYLCES THAT WERE BROKEN.                 */
/**************************************************************************/

static void ReadInlineRequests()
{
    register int   x;
    register PCALL c;
	     char  ans;

    /* FIRST, OBEY COMMAND LINE REQUESTS FOR FUNCTION PRESERVATION */
    for ( x = 0; x <= noincnt; x++ ) {
      if ( (c = OptFindCallee( noin[x] )) == NULL )
	Error2( "CANNOT FIND FUNCTION", noin[x] );

      if ( c->InLineFunction ) {
	c->InLineFunction   = FALSE;
	c->disabled = TRUE;
	}
      }

    if ( !inter )
      return;

    for ( c = callhead; c != NULL; c = c->callee )
	if ( c->InLineFunction ) {
	    PRINTF(" Expand %s Calls Inline (y/n): ",
			 c->graph->G_NAME );
	    ans = '\0';
	    (void)scanf( " %c", &ans );

	    if ( ans == 'n' ) {
		c->InLineFunction   = FALSE;
		c->disabled = TRUE;
		}
	    }
}


/**************************************************************************/
/* LOCAL  **************        OptBreakCycles        ************************/
/**************************************************************************/
/* PURPOSE: BREAK RECURSIVE CYCLES.  STARTING AT TRUNK NODE c WITH COLOR  */
/*          color, FOLLOW ALL CALL PATHS THAT HAVE YET TO BE TRAVERSED.   */
/*          IF c HAS THE SAME COLOR AS color THEN IT CAN NOT BE INLINED.  */
/**************************************************************************/

static void OptBreakCycles( c )
PCALL c;
{
    register PCALL r;

    if ( c->checked )
	return;

    if ( c->color == color ) {
	c->InLineFunction = FALSE;
	c->cycle  = TRUE;

	return;
	}

    c->color = color;

    for ( r = c->caller; r != NULL; r = r->caller )
	OptBreakCycles( r->callee );

    c->checked = TRUE;
}


/**************************************************************************/
/* GLOBAL **************       SpliceInGraph       ************************/
/**************************************************************************/
/* PURPOSE: REPLACE CALL NODE c WITH THE NODES DEFINING GRAPH g AND MAKE  */
/*          THE APPROPRIATE EDGE LINKAGES. DEAD EDGES AND NODES ARE       */
/*          DESTROYED (INCLUDING c).                                      */
/**************************************************************************/

void SpliceInGraph( g, c )
PNODE g;
PNODE c;
{
    register PEDGE e;
    register PEDGE i;
    register PNODE n;
    register PNODE sn;
    register PEDGE se;
    register PEDGE si;
    register PEDGE ee;
    register PEDGE see;

    for ( i = c->imp; i != NULL; i = i->isucc )  /* SO ARGUMENTS USE PORTS    */
	i->iport--;                              /* 1 -> N AND NOT 2 -> N + 1 */

    for ( e = g->exp; e != NULL; e = se ) {                 /* LINK ARGUMENTS */
	se = e->esucc;

	/* IS e A SHORT CIRCUIT: funct f( x:int returns int ) x end function */
	if ( e->dst == g ) {
	  for ( ee = c->exp; ee != NULL; ee = see ) {
	    see = ee->esucc;

	    if ( ee->eport != e->iport )
	      continue;

	    UnlinkExport( ee );
	    ee->eport = e->eport;
	    FindAndLinkToSource( c, ee, NULL_NODE );
	    }

	  continue;
	  }

        FindAndLinkToSource( c, e, NULL_NODE );
	}

    for ( e = c->exp; e != NULL; e = se ) {                   /* LINK RESULTS */
	se = e->esucc;
        FindAndLinkToSource( g, e, NULL_NODE );
	}

    for ( n = g->G_NODES; n != NULL; n = sn ) {           /* MOVE GRAPH NODES */
	sn = n->nsucc;
	LinkNode( c->npred, n );
	}

    for ( i = g->imp; i != NULL; i = si ) {      /* REMOVE GRAPH RESULT EDGES */
	si = i->isucc;

	/* IF NOT PART OF A SHORT CIRCUIT */
	if ( i->src != g )
	  RemoveDeadEdge( i );
	}

    c->exp = NULL;

    if ( glue )
      c->type = IFNoOp; /* FORCE c'S REMOVAL IN glue MODE */

    /* OptRemoveDeadNode WILL NOT FREE CALL NODES!!!!!! AS THIS WILL CORRUPT THE */
    /* CALL GRAPH.                                                            */
    OptRemoveDeadNode( c );
}


/**************************************************************************/
/* LOCAL  **************        ExpandCalls        ************************/
/**************************************************************************/
/* PURPOSE: EXPAND THE CALL NODES FOUND IN THE FUNCTION DEFINED BY CALL   */
/*          GRAPH TRUNK NODE c.  IF THE FUNCTION HAS ALREADY BEEN         */
/*          EXPANDED, THEN NOTHING IS DONE.  BEFORE A CALL IS EXPANDED,   */
/*          THE CALLEE ITSELF HAS ITS CALLS EXPANDED. IF THE REFERENCE    */
/*          BEING EXPANDED IS THE CALLEE'S LAST REFERENCE, THEN A COPY OF */
/*          CALLEE's GRAPH NEED NOT BE MADE; IT CAN BE DIRECTLY SPLICED   */
/*          INTO THE CALLER'S GRAPH; THEN THE CALLEE'S GRAPH NODE IS      */
/*          REMOVED FROM THE IF1 FUNCTION GRAPH LIST.                     */
/**************************************************************************/

static void ExpandCalls( c )
PCALL c;
{
    register PCALL r;
    register int   size;

    if ( c->expanded )
	return;

    if ( c->caller == NULL )
      goto TheEnd;

    size = c->graph->size;


    for ( r = c->caller; r != NULL; r = r->caller ) {
      if ( r->callee->InLineFunction ) {

	ExpandCalls( r->callee );

	size += r->callee->graph->size;

	if ( !inlineall )  {
	  if ( r->callee->graph->size > MIN_FUNCTION_SIZE ) {
	    if ( size > SIZE_THRESHOLD && (!inter) ) {
	      sskip++;
	      r->skipped = TRUE;
	      size -= r->callee->graph->size;
	      continue;
	    }
	  }
	}

	/* ------------------------------------------------------------ */
	/* IS THIS CALL NODE NOW DEAD AS A RESULT OF INLINING?		*/
	/* ( Or anything else (like CSE)				*/
	/* ------------------------------------------------------------ */
	if ( r->graph->exp == NULL ) {
	  r->callee->rcnt--;	/* No results, so don't bother to inline */
	  continue;
	}

	if ( glue || (--(r->callee->rcnt)) != 0 ) {
	  SpliceInGraph( CopyNode( r->callee->graph ), r->graph );
	  continue;
	}

	SpliceInGraph( r->callee->graph, r->graph );
	UnlinkGraph( r->callee->graph );
      }
    }

    c->graph->size = size;

TheEnd:
    c->expanded = TRUE;
}


/**************************************************************************/
/* LOCAL  **************      WriteInlineInfo      ************************/
/**************************************************************************/
/* PURPOSE: PRINT INFORMATION GATHERED DURING GRAPH EXPANSION TO stderr.  */
/**************************************************************************/

static void WriteInlineInfo()
{
    register PCALL c;
    register PCALL r;
    char *prevc = "prevc";
    

    FPRINTF( infoptr, "\n **** ANNOTATED CALL GRAPH: (skip=%d,MaxSZ=%d) [%s]\n\n", 
		     sskip, SIZE_THRESHOLD,
		     (IsStamp(MONOLITH))? "MONOLITH" : "INCOMPLETE"       );

    for ( c = callhead; c != NULL; c = c->callee ) {
	if ( IsXGraph( c->graph ) )
	    FPRINTF( infoptr, " EXPORT" );

	FPRINTF( infoptr, " FUNCTION %s(...) sz=%d [mk=%c]:",
		      (c->graph->funct)? c->graph->funct : c->graph->G_NAME,
		      c->graph->size,
		      (c->graph->mark)? c->graph->mark : ' '            );

	if ( !c->InLineFunction ) {
	    if ( c->disabled )
		FPRINTF( infoptr, " Not Inlined By Request of User" );
            else if ( c->cycle )
		FPRINTF( infoptr, " Not Inlined Because Part of Cycle" );
	    else 
		FPRINTF( infoptr, " Not Inlined Because Imported" );
	    }

	FPRINTF( infoptr, "\n" );

	for ( r = c->caller; r != NULL; r = r->caller ) {
	    if (strcmp (prevc, r->callee->graph->G_NAME) != 0)
	    FPRINTF( infoptr, "     CALL %s %s\n", r->callee->graph->G_NAME,
		             (r->skipped == TRUE)? "(SKIPPED)" : "" );
            prevc = r->callee->graph->G_NAME;
        }
	}
}


/**************************************************************************/
/* GLOBAL **************         If1Inline         ************************/
/**************************************************************************/
/* PURPOSE: INLINE FUNCTION REFERENCES.  ALL FUNCTIONS ARE CONSIDERED     */
/*          CANDIDATES EXCEPT THOSE SPECIFIED BY THE USER OR SELECTED TO  */
/*          BREAK CYCLES RESULTING FROM RECURSION.  A CALL GRAPH IS BUILT */
/*          TO HELP DETECT THE CYCLES. IF inter IS TRUE, THE USER IS      */
/*          QUERIED FOR FUNCTIONS TO INLINE.                              */
/**************************************************************************/

void If1Inline()
{
    register PNODE f;
    register PCALL c;

    for ( f = glstop->gsucc; f != NULL; f = f->gsucc )
      f->size = GetGraphSize( f );

    OptMakeCallGraph();

    for ( c = callhead; c != NULL; c = c->callee ) {
	color++;
	OptBreakCycles( c );
	}

    /* ------------------------------------------------------------ */
    /* Only inline functions as needed.  Get requests if inter==1   */
    /* ------------------------------------------------------------ */
    if ( !NoInlining ) {
      ReadInlineRequests();

      for ( c = callhead; c != NULL; c = c->callee ) {
	ExpandCalls( c );
      }
    }

    if ( RequestInfo(I_Info1,info) )
	WriteInlineInfo();
}
