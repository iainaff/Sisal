/* if1reduce.c,v 
 * Revision 12.7  1992/11/04  22:04:57  denton
 * Initial revision
 *
 * */
 
#include "world.h"

static int rafs  = 0;                /* COUNT OF REDUCED ARRAYS FUNCTIONS */
static int Trafs  = 0;                /* COUNT OF ARRAYS FUNCTIONS */
static int racs  = 0;            /* COUNT OF REDUCED ARRAY FUNCTION CALLS */
static int Tracs  = 0;            /* COUNT OF ARRAY FUNCTION CALLS */
static int maelm = 0;                 /* COUNT OF MODIFIED AElement NODES */
static int Tmaelm = 0;                 /* COUNT OF AElement NODES */


#define CALL_INIT	0
#define CALL_BODY	1
#define CALL_MERGE	2
#define MAX_CALLS	3

#define WAY_IN		0
#define WAY_OUT		1
#define MAX_WAYS	2

#define R_TYPE_IN	0
#define R_TYPE_IN_OUT	1
#define R_TYPE_OUT	2
#define MAX_TYPES	3

#define LABEL_ARGS	0
#define LABEL_RETS	1
#define LABEL_FUNC	2
#define MAX_LABELS	3

#define BASE_ONE	1
#define CALL_OFFSET	1

#define MAX_PORTS	10
#define MAX_NFORALL	10	/* MAXIMUM NUMBER OF NESTED FORALL */

/**************************************************************************/
/* LOCAL  **************    IsReductionCandidate   ************************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF NODE n IS A CANDIDATE FOR REDUCTION.          */
/**************************************************************************/

static int IsReductionCandidate( r_graph, r_loop_handle, r_port_count, r_ports )
  PNODE r_graph;			/* reduction function graph */
  PNODE *r_loop_handle;			/* reduction LOOP subgraphs */
  int r_port_count[MAX_CALLS][MAX_TYPES];
  int r_ports[MAX_CALLS][MAX_TYPES][MAX_PORTS];
{
  PNODE n;
  PEDGE edge;

  /*** Module must be an LGraph. ***/

  if ( !IsLGraph( r_graph ) )
    return FALSE;

  /*** LoopB must follow. ***/
  /*** Note:  If1Normalize will move initialization before loop. ***/

  n = r_graph->nsucc;
  if ( !IsLoopB( n ) )
      return FALSE;
  *r_loop_handle = n;

  /*** Index of test must loop over the entire updated array. ***/

  /* test TBD */

  /*** Body must reference current loop index only. ***/

  /* test TBD */

  /*** Returns node must be an AGather or FinalValue. ***/

  for ( n = n->L_RET; n != NULL; n = n->nsucc ) { /* TBD */
    if ( IsAGather( n ) || IsFinalValue( n ) )
      break;
  }
  if ( n == NULL )
    return FALSE;

  /*** Find the initialization and body call imports and exports. ***/

  edge = FindExport(r_graph, 1);              /* init. argument */
  if (edge==NULL || edge->dst!=r_graph->nsucc)
      return FALSE;
  r_port_count[CALL_INIT][R_TYPE_IN] = 1;     /* r_loop->L_INIT exp input */
  r_ports[CALL_INIT][R_TYPE_IN][0] = 1;	      /* %na=gr (eport==1) */

  r_port_count[CALL_INIT][R_TYPE_IN_OUT] = 0; /* r_loop->L_INIT input/output */
  r_port_count[CALL_MERGE][R_TYPE_IN_OUT] = 0;

  r_port_count[CALL_MERGE][R_TYPE_IN] = 0;
  r_port_count[CALL_MERGE][R_TYPE_OUT] = 0;

  r_port_count[CALL_INIT][R_TYPE_OUT] = 0;    /* r_loop->L_INIT imp output */
  r_port_count[CALL_BODY][R_TYPE_IN_OUT] = 0; /* r_loop->L_BODY input/output */
  for (edge=(*r_loop_handle)->L_INIT->imp; edge!=NULL; edge=edge->isucc) {
      if (FindExport((*r_loop_handle)->L_BODY, edge->iport)!=NULL &&
          FindImport((*r_loop_handle)->L_BODY, edge->iport)!=NULL &&
          FindExport((*r_loop_handle)->L_RET, edge->iport)!=NULL) {
          r_ports[CALL_INIT][R_TYPE_OUT][r_port_count[CALL_INIT][R_TYPE_OUT]++]
               = edge->iport;                 /* %na=total (iport==4) */
          r_ports[CALL_BODY][R_TYPE_IN_OUT][r_port_count[CALL_BODY]
              [R_TYPE_IN_OUT]++] = edge->iport;/* %na=old-total and total */
                                              /* (iport==4) (eport==4) */
          r_ports[CALL_MERGE][R_TYPE_IN_OUT][r_port_count[CALL_MERGE]
              [R_TYPE_IN_OUT]++] = edge->iport;/* %na=old-total and total */
      }
  }
  if (r_port_count[CALL_INIT][R_TYPE_OUT]!=1)
      return FALSE;

  edge = FindExport(r_graph, 2);              /* update argument */
  if (edge==NULL || FindExport((*r_loop_handle)->L_BODY, edge->iport)==NULL)
      return FALSE;
  r_port_count[CALL_BODY][R_TYPE_IN] = 1;     /* r_loop->L_BODY exp input */
  r_ports[CALL_BODY][R_TYPE_IN][0] = edge->iport;/* %na=b (eport==2) */

  r_port_count[CALL_BODY][R_TYPE_OUT] = 0;    /* r_loop->L_BODY imp output */

  return TRUE;
}

/**************************************************************************/
/* LOCAL  *************    IsReductionDriverCandidate   *******************/
/**************************************************************************/
/* PURPOSE: RETURNS TRUE IF NODE n IS A CANDIDATE FOR REDUCTION DRIVER.   */
/**************************************************************************/

static int IsReductionDriverCandidate( d_call, d_for_stack, d_depth_ptr, 
  d_ports )
  PNODE d_call;				/* reduction function graph */
  PNODE d_for_stack[MAX_NFORALL];	/* reduction driver outer FOR */
  int *d_depth_ptr;
  int d_ports[MAX_PORTS];
{
  PEDGE edge;
  PEDGE redge;
  PNODE d_for;

  /*** Search backwards from the call to the IFForall driver. ***/

  edge = FindImport( d_call, 3 );	/* update argument */
  if ( edge==NULL || edge->src==NULL )
      return FALSE;
  if (edge->src->type==IFASetL) {
      edge = FindImport( edge->src, 1 );/* array is first value to ASetL */
      if ( edge==NULL || edge->src==NULL )
          return FALSE;
  }
  if ( !IsForall( edge->src ) )
      return FALSE;
  *d_depth_ptr = 0;
  d_for_stack[(*d_depth_ptr)++] = edge->src;

  /*** Search backwards from edge out of IFForall to input to F_RET. ***/

  edge = FindImport( d_for_stack[(*d_depth_ptr)-1]->F_RET, edge->eport );
  if ( edge==NULL || edge->src==NULL)
      return FALSE;
  if ( IsReduction(edge->src) && edge->src->imp->CoNsT[0]=='C' ) {
      edge = FindImport( edge->src, 3 );			/* A */
      if ( edge==NULL )
          return FALSE;
      for ( ; ; ) {
          redge = edge;                  
          redge = FindImport( (d_for_stack[(*d_depth_ptr)-1])->F_BODY, 
              redge->eport );					/* B */
          if ( redge==NULL || !IsForall( redge->src ) )
              break;
          d_for = redge->src;
          redge = FindImport( d_for->F_RET, redge->eport );	/* C */
          if ( redge==NULL || redge->src==NULL)
              break;
          if ( IsReduction(redge->src) && redge->src->imp->CoNsT[0]=='C' ) {
              d_for_stack[(*d_depth_ptr)++] = d_for;
              edge = FindImport( redge->src, 3 );		/* D */
          } else {
              break;
          }
      } 
  } else if ( IsAGather(edge->src) ) {
      edge = FindImport( edge->src, 2 ); 
      if ( edge==NULL )
          return FALSE;
  } else {
      return FALSE;
  }

  d_ports[0] = edge->eport;                   /* d_for->F_RET exp input */  
                   /* d_port_count==r_port_count[CALL_BODY][R_TYPE_IN]  */
                                              /* %na=x (hooks to b) (2) */
  d_for_stack[*d_depth_ptr] = NULL;
  (*d_depth_ptr)--;					  /* zero-based */
  return TRUE;
}

/**************************************************************************/
/* LOCAL **************      CollapsePortArray     ************************/
/**************************************************************************/
/* PURPOSE: COPY SPECIFIED PORTIONS OF THE IMPORT LIST OF NODE            */
/* template_node AND ATTACH FROM source_node TO NODE dest_node.           */ 
/**************************************************************************/

static void CollapsePortArray( port_count, dest_ports, dest_offset )
int port_count;
int *dest_ports;
int dest_offset;
{
    int i;
    for ( i = 0; i < port_count; i++ ) {
        dest_ports[i] = i + 1 + dest_offset;
    }
}

/**************************************************************************/
/* LOCAL **************    CopySelectedImports2    ************************/
/**************************************************************************/
/* PURPOSE: COPY SPECIFIED PORTIONS OF THE IMPORT LIST OF NODE            */
/* template_node AND ATTACH FROM source_node TO NODE dest_node.           */ 
/**************************************************************************/

static int CopySelectedImports2( template_node, port_count, ports, 
    source_node, source_ports, dest_node, dest_ports )
PNODE template_node;
int port_count;
int *ports;
PNODE source_node;	/* use template sources if NULL */
int *source_ports;
PNODE dest_node;
int *dest_ports;
{
    int i;

    for ( i = 0; i < port_count; i++ ) {
        PEDGE	template_edge;
        PEDGE	other_edge;
        PNODE	src;
        if ( IsGraph( template_node ) ) {
            template_edge = FindExport( template_node, ports[i] );
        } else {
            template_edge = FindImport( template_node, ports[i] );
        }
        if ( source_node==NULL ) {
            src = template_edge->src;
        } else {
            src = source_node;
        }
        other_edge = CopyEdge( template_edge, src, dest_node );
        other_edge->iport = dest_ports[i];
        other_edge->eport = source_ports[i];
        LinkImport( dest_node, other_edge );
        if ( !IsConst( other_edge ) )
            LinkExport( src, other_edge );
    }
    return port_count;
}

/**************************************************************************/
/* LOCAL **************     CopySelectedImports    ************************/
/**************************************************************************/
/* PURPOSE: COPY SPECIFIED PORTIONS OF THE IMPORT LIST OF NODE            */
/* template_node AND ATTACH FROM source_node TO NODE dest_node.           */ 
/**************************************************************************/

static int CopySelectedImports( template_node, port_count, ports, 
    source_node, source_offset, dest_node, dest_offset )
PNODE template_node;
int port_count;
int *ports;
PNODE source_node;	/* use template sources if NULL */
int source_offset;
PNODE dest_node;
int dest_offset;
{
    int i;

    for ( i = 0; i < port_count; i++ ) {
        PEDGE	template_edge;
        PEDGE	other_edge;
        PNODE	src;
        if ( IsGraph( template_node ) ) {
            template_edge = FindExport( template_node, ports[i] );
        } else {
            template_edge = FindImport( template_node, ports[i] );
        }
        if ( source_node==NULL ) {
            src = template_edge->src;
        } else {
            src = source_node;
        }
        other_edge = CopyEdge( template_edge, src, dest_node );
        other_edge->iport = i + 1 + dest_offset;
        other_edge->eport = other_edge->eport + source_offset;
        LinkImport( dest_node, other_edge );
        if ( !IsConst( other_edge ) )
            LinkExport( src, other_edge );
    }
    return port_count + dest_offset;
}

/**************************************************************************/
/* LOCAL **************     CopySelectedExports    ************************/
/**************************************************************************/
/* PURPOSE: COPY SPECIFIED PORTIONS OF THE EXPORT LIST OF NODE            */
/* template_node AND ATTACH FROM source_node TO NODE dest_node.           */ 
/**************************************************************************/

static int CopySelectedExports( template_node, port_count, ports, 
    source_node, source_offset, dest_node, dest_offset)
PNODE template_node;
int port_count;
int *ports;
PNODE source_node;
int source_offset;
PNODE dest_node;	/* use template destinations if NULL */
int dest_offset;
{
    int i;

    for ( i = 0; i < port_count; i++ ) {
        PEDGE	template_edge;
        PEDGE	other_edge;
        PNODE	dst;
        if ( IsGraph( template_node ) ) {
            template_edge = FindImport( template_node, ports[i] );
        } else {
            template_edge = FindExport( template_node, ports[i] );
        }
        if (dest_node==NULL) {
            dst = template_edge->dst;
        } else {
            dst = dest_node;
        }
        other_edge = CopyEdge( template_edge, source_node, dst );
        other_edge->iport = i + 1 + dest_offset;
        other_edge->eport = other_edge->eport + source_offset;
        LinkImport( dst, other_edge );
        if ( !IsConst( other_edge ) )
            LinkExport( source_node, other_edge );
    }
    return port_count + dest_offset;
}

/**************************************************************************/
/* LOCAL **************    ShiftSelectedImports    ************************/
/**************************************************************************/
/* PURPOSE: SHIFT SPECIFIED PORTIONS OF THE IMPORT LIST OF NODE           */
/* dest_node.                                                             */
/**************************************************************************/

static int ShiftSelectedImports( port_count, ports, dest_offset, dest_node )
int port_count;
int *ports;
int dest_offset;
PNODE dest_node;
{
    PEDGE edge;

    for ( edge = dest_node->imp; edge!=NULL; edge = edge->isucc ) {
        int	i;
        for ( i = 0; i < port_count; i++ ) {
            if ( edge->iport == ports[i] ) {
                edge->iport = i + 1 + dest_offset;
                break;
            }
        }
        if ( i == port_count ) {
            UnlinkImport( edge );
            UnlinkExport( edge );
        }
        if ( edge->isucc==NULL )
            break;
    }
    return port_count + dest_offset;
}

/**************************************************************************/
/* LOCAL **************    ShiftSelectedExports    ************************/
/**************************************************************************/
/* PURPOSE: SHIFT SPECIFIED PORTIONS OF THE EXPORT LIST OF NODE           */
/* source_node.                                                           */
/**************************************************************************/

static int ShiftSelectedExports( port_count, ports, source_offset, source_node )
int port_count;
int *ports;
int source_offset;
PNODE source_node;
{
    PEDGE edge;

    for ( edge = source_node->exp; edge!=NULL; edge = edge->esucc ) {
        int	i;
        for ( i = 0; i < port_count; i++ ) {
            if ( edge->eport == ports[i] ) {
                edge->eport = i + 1 + source_offset;
                break;
            }
        }
        if ( i == port_count ) {
            UnlinkImport( edge );
            UnlinkExport( edge );
        }
        if ( edge->esucc==NULL )
            break;
    }
    return port_count + source_offset;
}

/**************************************************************************/
/* LOCAL ***************     GroupInOutPorts       ************************/
/**************************************************************************/
/* PURPOSE: GROUP IN/IN-OUT AND IN-OUT/OUT PORTS TOGETHER.                */
/**************************************************************************/

static void GroupInOutPorts( r_port_count, r_ports, port_count, ports )
  int r_port_count[MAX_CALLS][MAX_TYPES];			/* in */
  int r_ports[MAX_CALLS][MAX_TYPES][MAX_PORTS];			/* in */
  int port_count[MAX_CALLS][MAX_WAYS];				/* out */
  int ports[MAX_CALLS][MAX_WAYS][MAX_PORTS];			/* out */
{
  int i, j, k;

  for ( i = 0; i < MAX_CALLS; i++ ) {
    port_count[i][WAY_IN] =  r_port_count[i][R_TYPE_IN] +
                             r_port_count[i][R_TYPE_IN_OUT];
    port_count[i][WAY_OUT] = r_port_count[i][R_TYPE_IN_OUT] +
                             r_port_count[i][R_TYPE_OUT];
    for ( j = 0; j < r_port_count[i][R_TYPE_IN_OUT]; j++ ) {
        ports[i][WAY_IN][j] = r_ports[i][R_TYPE_IN_OUT][j];
    }
    for ( k = 0; k < r_port_count[i][R_TYPE_IN]; k++ ) {
        ports[i][WAY_IN][j+k] = r_ports[i][R_TYPE_IN][k];
    }
    for ( j = 0; j < r_port_count[i][R_TYPE_IN_OUT]; j++ ) {
        ports[i][WAY_OUT][j] = r_ports[i][R_TYPE_IN_OUT][j];
    }
    for ( k = 0; k < r_port_count[i][R_TYPE_OUT]; k++ ) {
        ports[i][WAY_OUT][j+k] = r_ports[i][R_TYPE_OUT][k];
    }
  }

  return;
}

/**************************************************************************/
/* LOCAL ***************     AddInitCallTypes     *************************/
/**************************************************************************/
/* PURPOSE: MAKE TYPE LABELS FOR REDUCTION INIT COMPONENT CALLS.          */
/**************************************************************************/

static void AddInitCallTypes( r_loop, r_port_count, r_ports, 
  port_count, ports, labels )
  PNODE r_loop;							/* in */
  int r_port_count[MAX_CALLS][MAX_TYPES];			/* in */
  int r_ports[MAX_CALLS][MAX_TYPES][MAX_PORTS];			/* in */
  int port_count[MAX_CALLS][MAX_WAYS];				/* in */
  int ports[MAX_CALLS][MAX_WAYS][MAX_PORTS];			/* in */
  int labels[MAX_CALLS][MAX_LABELS];				/* out */
{
  PEDGE edge;
  int last_label;
  int i;

  last_label = 0;
  for ( i = 0; i<port_count[CALL_INIT][WAY_IN]; i++ ) {
    edge = FindExport( r_loop->L_INIT, ports[CALL_INIT][WAY_IN][i] );
    labels[CALL_INIT][LABEL_ARGS] = FindLargestLabel() + 1;
    MakeInfo( labels[CALL_INIT][LABEL_ARGS], IF_TUPLE, 
        edge->info->label, last_label );
    last_label = labels[CALL_INIT][LABEL_ARGS];
  }
  last_label = 0;
  for (i = 0; i<port_count[CALL_INIT][WAY_OUT]; i++ ) {
    edge = FindImport( r_loop->L_INIT, ports[CALL_INIT][WAY_OUT][i] );
    labels[CALL_INIT][LABEL_RETS] = FindLargestLabel() + 1;
    MakeInfo( labels[CALL_INIT][LABEL_RETS], IF_TUPLE, 
        edge->info->label, last_label );
    last_label = labels[CALL_INIT][LABEL_RETS];
  }
  labels[CALL_INIT][LABEL_FUNC] = FindLargestLabel() + 1;
  MakeInfo( labels[CALL_INIT][LABEL_FUNC], IF_FUNCTION, 
      labels[CALL_INIT][LABEL_ARGS], labels[CALL_INIT][LABEL_RETS] );
  return;
}

/**************************************************************************/
/* LOCAL ***************     AddBodyCallTypes     *************************/
/**************************************************************************/
/* PURPOSE: MAKE TYPE LABELS FOR REDUCTION BODY COMPONENT CALLS.          */
/**************************************************************************/

static void AddBodyCallTypes( r_loop, r_port_count, r_ports, 
  port_count, ports, labels )
  PNODE r_loop;							/* in */
  int r_port_count[MAX_CALLS][MAX_TYPES];			/* in */
  int r_ports[MAX_CALLS][MAX_TYPES][MAX_PORTS];			/* in */
  int port_count[MAX_CALLS][MAX_WAYS];				/* in */
  int ports[MAX_CALLS][MAX_WAYS][MAX_PORTS];			/* in */
  int labels[MAX_CALLS][MAX_LABELS];				/* out */
{
  PEDGE edge;
  int last_label;
  int i;

  /*** Make body function call types (alongside the existing body). ***/

  last_label = 0;
  for ( i = 0; i<r_port_count[CALL_BODY][R_TYPE_IN]; i++ ) {
    edge = FindExport( r_loop->L_BODY, r_ports[CALL_BODY][R_TYPE_IN][i] );
    labels[CALL_BODY][LABEL_ARGS] = FindLargestLabel() + 1;
    MakeInfo( labels[CALL_BODY][LABEL_ARGS], IF_TUPLE, 
        edge->info->info1->label, last_label );		/* Take off AElement */
    last_label = labels[CALL_BODY][LABEL_ARGS];
  }
  for ( i = 0; i<r_port_count[CALL_BODY][R_TYPE_IN_OUT]; i++ ) {
    edge = FindExport( r_loop->L_BODY, r_ports[CALL_BODY][R_TYPE_IN_OUT][i] );
    labels[CALL_BODY][LABEL_ARGS] = FindLargestLabel() + 1;
    MakeInfo( labels[CALL_BODY][LABEL_ARGS], IF_TUPLE, 
        edge->info->label, last_label );
    last_label = labels[CALL_BODY][LABEL_ARGS];
  }

  last_label = 0;
  for (i = 0; i<port_count[CALL_BODY][WAY_OUT]; i++ ) {
    edge = FindImport( r_loop->L_BODY, ports[CALL_BODY][WAY_OUT][i] );
    labels[CALL_BODY][LABEL_RETS] = FindLargestLabel() + 1;
    MakeInfo( labels[CALL_BODY][LABEL_RETS], IF_TUPLE, 
        edge->info->label, last_label );
    last_label = labels[CALL_BODY][LABEL_RETS];
  }
  labels[CALL_BODY][LABEL_FUNC] = FindLargestLabel() + 1;
  MakeInfo( labels[CALL_BODY][LABEL_FUNC], IF_FUNCTION, 
      labels[CALL_BODY][LABEL_ARGS], labels[CALL_BODY][LABEL_RETS] );
  return;
}

/**************************************************************************/
/* LOCAL ***************     AddMergeCallTypes     ************************/
/**************************************************************************/
/* PURPOSE: MAKE TYPE LABELS FOR REDUCTION MERGE COMPONENT CALLS.         */
/**************************************************************************/

static void AddMergeCallTypes( r_loop, r_port_count, r_ports, 
  port_count, ports, labels )
  PNODE r_loop;							/* in */
  int r_port_count[MAX_CALLS][MAX_TYPES];			/* in */
  int r_ports[MAX_CALLS][MAX_TYPES][MAX_PORTS];			/* in */
  int port_count[MAX_CALLS][MAX_WAYS];				/* in */
  int ports[MAX_CALLS][MAX_WAYS][MAX_PORTS];			/* in */
  int labels[MAX_CALLS][MAX_LABELS];				/* out */
{
  PEDGE edge;
  int last_label;
  int i;

  /*** Make merge function call types (for intermediate levels). ***/

  last_label = 0;
  for ( i = 0; i<r_port_count[CALL_MERGE][R_TYPE_IN_OUT]; i++ ) {
    edge = FindExport( r_loop->L_BODY, r_ports[CALL_MERGE][R_TYPE_IN_OUT][i] );
    labels[CALL_MERGE][LABEL_ARGS] = FindLargestLabel() + 1;
    MakeInfo( labels[CALL_MERGE][LABEL_ARGS], IF_TUPLE,
      edge->info->label, last_label );
    last_label = labels[CALL_MERGE][LABEL_ARGS];
   }

  if ( i==0 )
      return;
  last_label = 0;
  for ( i = 0; i<r_port_count[CALL_MERGE][R_TYPE_IN_OUT]; i++ ) {
    edge = FindImport( r_loop->L_BODY, r_ports[CALL_MERGE][R_TYPE_IN_OUT][i] );
    labels[CALL_MERGE][LABEL_RETS] = FindLargestLabel() + 1;
    MakeInfo( labels[CALL_MERGE][LABEL_RETS], IF_TUPLE, 
        edge->info->label, last_label );
    last_label = labels[CALL_MERGE][LABEL_RETS];
  }
  if ( i==0 )
      return;
  labels[CALL_MERGE][LABEL_FUNC] = FindLargestLabel() + 1;
  MakeInfo( labels[CALL_MERGE][LABEL_FUNC], IF_FUNCTION, 
      labels[CALL_MERGE][LABEL_ARGS], labels[CALL_MERGE][LABEL_RETS] );
  return;
}

/**************************************************************************/
/* LOCAL ***************         AddInitCallGraph     *********************/
/**************************************************************************/
/* PURPOSE: ADD REDUCTION INITIALIZATION CALL GRAPH TO MODULE LIST.       */
/*          MAKE FROM THE REDUCTION INIT SUBGRAPH.                        */
/**************************************************************************/

static void AddInitCallGraph( r_loop, labels, call_name, counter, call_graph )
  PNODE r_loop;							/* in */
  int labels[MAX_CALLS][MAX_LABELS];				/* in */
  char* call_name[MAX_CALLS];					/* in */
  int counter;							/* in */
  PNODE call_graph[MAX_CALLS];					/* out */
{
  call_graph[CALL_INIT] = CopyNode( r_loop->L_INIT ); /* imports are copied */
  call_graph[CALL_INIT]->type = IFLGraph;
  call_graph[CALL_INIT]->funct = r_loop->L_INIT->funct;
  call_graph[CALL_INIT]->gname = (char*)malloc(20);
  sprintf(call_graph[CALL_INIT]->gname, call_name[CALL_INIT], counter);
  call_graph[CALL_INIT]->info = FindInfo( labels[CALL_INIT][LABEL_FUNC], 
      IF_FUNCTION );
  AddToGraphList( call_graph[CALL_INIT] );
  return;
}

/**************************************************************************/
/* LOCAL ***************         AddBodyCallGraph     *********************/
/**************************************************************************/
/* PURPOSE: ADD REDUCTION BODY CALL GRAPH TO MODULE LIST.                 */
/*          MAKE FROM THE REDUCTION BODY SUBGRAPH.                        */
/**************************************************************************/

static void AddBodyCallGraph( r_loop, labels, call_name, counter, call_graph )
  PNODE r_loop;							/* in */
  int labels[MAX_CALLS][MAX_LABELS];				/* in */
  char* call_name[MAX_CALLS];					/* in */
  int counter;							/* in */
  PNODE call_graph[MAX_CALLS];					/* out */
{
  call_graph[CALL_BODY] = CopyNode( r_loop->L_BODY );	/* imports are copied */
  call_graph[CALL_BODY]->type = IFLGraph;
  call_graph[CALL_BODY]->funct = r_loop->L_BODY->funct;
  call_graph[CALL_BODY]->mark = 'd';			/* mark as reduction */
  call_graph[CALL_BODY]->gname = (char*)malloc(20);
  sprintf(call_graph[CALL_BODY]->gname, call_name[CALL_BODY], counter);
  call_graph[CALL_BODY]->info = FindInfo( labels[CALL_BODY][LABEL_FUNC], 
      IF_FUNCTION );
  AddToGraphList( call_graph[CALL_BODY] );
  return;
}

/**************************************************************************/
/* LOCAL ***************         AddMergeCallGraph     ********************/
/**************************************************************************/
/* PURPOSE: ADD REDUCTION MERGE CALL GRAPH TO MODULE LIST.                */
/**************************************************************************/

static void AddMergeCallGraph( r_loop, labels, call_name, counter, call_graph )
  PNODE r_loop;							/* in */
  int labels[MAX_CALLS][MAX_LABELS];				/* in */
  char* call_name[MAX_CALLS];					/* in */
  int counter;							/* in */
  PNODE call_graph[MAX_CALLS];					/* out */
{
  call_graph[CALL_MERGE] = NodeAlloc( 0, IFLGraph );
  call_graph[CALL_MERGE]->funct = r_loop->L_INIT->funct;
  call_graph[CALL_MERGE]->mark = 'd';			/* mark as reduction */
  call_graph[CALL_MERGE]->gname = (char*)malloc(20);
  sprintf(call_graph[CALL_MERGE]->gname, call_name[CALL_MERGE], counter);
  call_graph[CALL_MERGE]->info = FindInfo( labels[CALL_MERGE][LABEL_FUNC], 
      IF_FUNCTION );
  AddToGraphList( call_graph[CALL_MERGE] );
  return;
}

/**************************************************************************/
/* LOCAL ***************         CreateMerge       ************************/
/**************************************************************************/
/* PURPOSE: Add nodes to merge function.                                  */
/**************************************************************************/

static void CreateMerge( r_port_count, r_ports, d_for_stack, d_depth, 
  call_graph )
  int r_port_count[MAX_CALLS][MAX_TYPES];			/* in */
  int r_ports[MAX_CALLS][MAX_TYPES][MAX_PORTS];			/* in */
  PNODE d_for_stack[MAX_NFORALL];				/* in */
  int d_depth;							/* in */
  PNODE call_graph[MAX_CALLS];					/* in/out */
{ 
  PEDGE edge, other_edge;
  int i;
  PNODE node, other_node;

  /*** Add no-op to merge function. ***/        /* TBD */

  node = call_graph[CALL_MERGE];
  for ( i = 0; i < r_port_count[CALL_MERGE][R_TYPE_IN_OUT]; i++) {
    other_node = NodeAlloc( i+1, IFASetL );
    LinkNode( node, other_node );
    node = other_node;
    edge = FindExport( call_graph[CALL_BODY], 
        r_ports[CALL_MERGE][R_TYPE_IN_OUT][i] );

    other_edge = CopyEdge( edge, call_graph[CALL_MERGE], node );
    other_edge->eport = BASE_ONE + i;
    other_edge->iport = BASE_ONE;
    LinkExport( call_graph[CALL_MERGE], other_edge );
    LinkImport( node, other_edge );

    other_edge = CopyEdge( edge, node, call_graph[CALL_MERGE] );
    other_edge->eport = BASE_ONE;
    other_edge->iport = BASE_ONE + i;
    LinkExport( node, other_edge );
    LinkImport( call_graph[CALL_MERGE], other_edge );

    other_edge = EdgeAlloc( (PNODE)NULL, CONST_PORT, node, BASE_ONE+1 );
    other_edge->info = integer;
    other_edge->CoNsT = "1";
    LinkImport( node, other_edge );
  }

  return;
}

/**************************************************************************/
/* LOCAL ***************        EliminateAEMultiples     ******************/
/**************************************************************************/
/* PURPOSE: ELIMINATE EXTRA AELEMENT ACCESS TO MULTIPLES.                 */
/**************************************************************************/

static void EliminateAEMultiples( r_port_count, r_ports, call_graph )
  int r_port_count[MAX_CALLS][MAX_TYPES];			/* in */
  int r_ports[MAX_CALLS][MAX_TYPES][MAX_PORTS];			/* in */
  PNODE call_graph[MAX_CALLS];					/* in/out */
{
  PEDGE edge, other_edge;
  int i;

  /*** Fix body AElement array accesses to direct element accesses. ***/

  for ( i = 0; i < r_port_count[CALL_BODY][R_TYPE_IN]; i++) {
      for ( edge = call_graph[CALL_BODY]->exp; edge!=NULL; edge = edge->esucc) {
          if ( IsAElement( edge->dst )) {
	      ++Tmaelm;
          if ( edge->eport==r_ports[CALL_BODY][R_TYPE_IN][i] && edge->iport==1 )
	  {
              UnlinkExport( edge );
              other_edge = FindExport( edge->dst, 1 );	/* the value */
              UnlinkExport( other_edge );
              other_edge->eport = edge->eport;
              other_edge->src = edge->src; /* route around AElement */
              LinkExport( other_edge->src, other_edge );
              maelm++;
          }
          }
      }
  }
  return;
}
    
/**************************************************************************/
/* LOCAL ***************        AdjustCallGraphs     **********************/
/**************************************************************************/
/* PURPOSE: ADJUST REDUCTION CALL GRAPHS TO USE ONLY NECESSARY PORTS.     */
/**************************************************************************/

static void AdjustCallGraphs( port_count, ports, call_graph )
  int port_count[MAX_CALLS][MAX_WAYS];				/* in */
  int ports[MAX_CALLS][MAX_WAYS][MAX_PORTS];			/* in */
  PNODE call_graph[MAX_CALLS];					/* in/out */
{
  /*** Shift the init function ports over. ***/

  (void)ShiftSelectedExports( 
    port_count[CALL_INIT][WAY_IN], ports[CALL_INIT][WAY_IN], 
    0, call_graph[CALL_INIT] );
  (void)ShiftSelectedImports( 
    port_count[CALL_INIT][WAY_OUT], ports[CALL_INIT][WAY_OUT], 
    0, call_graph[CALL_INIT] );

  /*** Shift the body function ports over. ***/

  (void)ShiftSelectedExports( 
    port_count[CALL_BODY][WAY_IN], ports[CALL_BODY][WAY_IN], 
    0, call_graph[CALL_BODY] );
  (void)ShiftSelectedImports( 
    port_count[CALL_BODY][WAY_OUT], ports[CALL_BODY][WAY_OUT], 
    0, call_graph[CALL_BODY] );

  return;
}

/**************************************************************************/
/* LOCAL ***************     RewireReduction       ************************/
/**************************************************************************/
/* PURPOSE: COPY REDUCTION FUNCTION UP INTO FORALL DRIVER.                */
/**************************************************************************/

static void RewireReduction( r_graph, r_loop, r_port_count, r_ports, 
  d_call, d_for_stack, d_depth, d_ports )
  PNODE r_graph;			/* reduction function graph */
  PNODE r_loop;				/* reduction LOOP subgraphs */
  int r_port_count[MAX_CALLS][MAX_TYPES];
  int r_ports[MAX_CALLS][MAX_TYPES][MAX_PORTS];
  PNODE d_call;				/* reduction driver CALL */ 
  PNODE d_for_stack[MAX_NFORALL];	/* reduction driver embedded FOR's */
  int d_depth;				/* reduction driver FOR max depth */
  int d_ports[MAX_PORTS];
{
  int labels[MAX_CALLS][MAX_LABELS];
  int port_count[MAX_CALLS][MAX_WAYS];
  int ports[MAX_CALLS][MAX_WAYS][MAX_PORTS];
  PNODE call_graph[MAX_CALLS];
  PNODE caller_node[MAX_CALLS];

  int temp_ports[MAX_PORTS], source_ports[MAX_PORTS], dest_ports[MAX_PORTS];
  PEDGE edge, other_edge;
  PNODE node, other_node;
  int d_for_ret_port;
  int d_for_offset;
  char* name;
  int depth;		/* 0-based forall depth counter, outer to inner */
  int i;

  static char* call_name[MAX_CALLS] = {
      "_r_init_%d", "_r_body_%d", "_r_merge_%d"};
  static int counter = -1;

  PNODE d_for = d_for_stack[d_depth];	/* reduction driver inner FOR */
  int d_for_port = 0;

  counter++;				/* file name counter */

  /*** Group input and output into single lists. ***/

  if ( d_depth==0 ) {				/* no merge required */
      r_port_count[CALL_MERGE][R_TYPE_IN] = 0;
      r_port_count[CALL_MERGE][R_TYPE_IN_OUT] = 0;
      r_port_count[CALL_MERGE][R_TYPE_OUT] = 0;
  }
  GroupInOutPorts( r_port_count, r_ports, port_count, ports );

  /*** Add function call types and graphs from reduction subgraphs. ***/

  AddInitCallTypes( r_loop, r_port_count, r_ports, port_count, ports, labels );
  AddInitCallGraph( r_loop, labels, call_name, counter, call_graph );

  AddBodyCallTypes( r_loop, r_port_count, r_ports, port_count, ports, labels );
  AddBodyCallGraph( r_loop, labels, call_name, counter, call_graph );

  if ( d_depth!=0 ) {
    AddMergeCallTypes(r_loop, r_port_count, r_ports, port_count, ports, labels);
    AddMergeCallGraph( r_loop, labels, call_name, counter, call_graph );
    CreateMerge( r_port_count, r_ports, d_for_stack, d_depth, call_graph );
  }

  EliminateAEMultiples( r_port_count, r_ports, call_graph );
  AdjustCallGraphs( port_count, ports, call_graph );

  /*** Hook in the init function caller (before the forall driver). ***/

  caller_node[CALL_INIT] = CopyNode( d_call ); /* imports are not copied */
  node = d_for_stack[0]->npred;
  LinkNode( node, caller_node[CALL_INIT] );
  name = (char*)malloc( 20 );
  sprintf(name, call_name[CALL_INIT], counter);
  edge = FindImport( d_call, CALL_OFFSET ); /* function name on port 1 */
  AttachEdge( (PNODE)NULL, CONST_PORT, caller_node[CALL_INIT], CALL_OFFSET, 
    edge->info, name );
  for ( i = 0; i < port_count[CALL_INIT][WAY_IN]; i++ ) {
    edge = FindImport( r_loop, ports[CALL_INIT][WAY_IN][i] );
    temp_ports[i] = edge->eport + CALL_OFFSET; /* match graph to call */
  }
  d_for_offset = FindLargestPort( d_for_stack[0] );
  (void)CopySelectedImports( d_call,				/* Y */
    port_count[CALL_INIT][WAY_IN], temp_ports, 
    (PNODE)NULL, 0, caller_node[CALL_INIT], CALL_OFFSET );
  (void)CopySelectedExports( r_loop->L_INIT,			/* Z */
    port_count[CALL_INIT][WAY_OUT], ports[CALL_INIT][WAY_OUT], 
    caller_node[CALL_INIT], 0, d_for_stack[0], d_for_offset );

  /*** Add init call inputs. ***/

  for ( i = 0; i<port_count[CALL_INIT][WAY_OUT]; i++ ) {
    for ( depth = 0; depth<=d_depth; depth++ ) {
      if ( depth>0 ) {
        edge = FindImport( r_loop->L_INIT, ports[CALL_INIT][WAY_OUT][i] );
        other_edge = CopyEdge( edge, d_for_stack[depth-1]->F_BODY, 
            d_for_stack[depth] );
        other_edge->eport = d_for_offset + i + 1;
        d_for_offset = FindLargestPort( d_for_stack[depth] );
        other_edge->iport = d_for_offset + i + 1;
        LinkExport( d_for_stack[depth-1]->F_BODY, other_edge );	/* X */
        LinkImport( d_for_stack[depth], other_edge );
      }
    }
  }

  /*** Hook in the body function call (at end of the forall returns). ***/

  node = FindLastNode( d_for->F_RET );
  caller_node[CALL_BODY] = NodeAlloc( node->label+1, IFReduce );
  LinkNode( node, caller_node[CALL_BODY] );
  name = (char*)malloc( 20 );
  sprintf(name, call_name[CALL_BODY], counter);
  edge = FindImport( d_call, CALL_OFFSET ); /* function name on port 1 */
  AttachEdge( (PNODE)NULL, CONST_PORT, caller_node[CALL_BODY], CALL_OFFSET, 
    edge->info, name );

  /*** Add in body function call L ports inputs. ***/
  /*** (loop carried dependencies like total) ***/

  CollapsePortArray( r_port_count[CALL_BODY][R_TYPE_IN_OUT], source_ports, 
      d_for_offset );
  CollapsePortArray( r_port_count[CALL_BODY][R_TYPE_IN_OUT], dest_ports, 
      CALL_OFFSET );
  (void)CopySelectedImports2( r_loop->L_BODY, 	/* L ports from L_BODY */
    r_port_count[CALL_BODY][R_TYPE_IN_OUT], r_ports[CALL_BODY][R_TYPE_IN_OUT], 
    d_for->F_RET, source_ports, caller_node[CALL_BODY], dest_ports );

  /*** Add in body function call T ports inputs. ***/
  /*** (results of the body like x. ***/

  for ( i = 0; i < r_port_count[CALL_BODY][R_TYPE_IN]; i++ ) {
    dest_ports[i] = i + BASE_ONE + CALL_OFFSET + 
        r_port_count[CALL_BODY][R_TYPE_IN];
  } /* T ports from F_BODY */
  for ( i = 0; i < r_port_count[CALL_BODY][R_TYPE_IN]; i++ ) {
    edge = FindExport( d_for->F_RET, d_ports[i] );
    other_edge = CopyEdge( edge, d_for->F_RET, caller_node[CALL_BODY] );
    other_edge->iport = dest_ports[i];
    other_edge->eport = d_ports[i];
    LinkImport( caller_node[CALL_BODY], other_edge );
    LinkExport( d_for->F_RET, other_edge );
  }

  /*** Hook body function to downstream; work inner to outer level. ***/

  for ( i = 0; i<r_port_count[CALL_BODY][R_TYPE_IN_OUT]; i++ ) {
    for ( depth = d_depth; depth>=0; depth-- ) {
      d_for_ret_port = FindLargestImport( d_for_stack[depth]->F_RET ) + 1;
  
      /*** Add body function call outputs to returns graph output. ***/
  
      if ( depth==d_depth ) {		/* bottom level reduce */
        edge = FindExport( d_for_stack[d_depth]->F_RET, source_ports[i] );
        other_edge = CopyEdge( edge, caller_node[CALL_BODY], 
            d_for_stack[depth]->F_RET );
        other_edge->eport = i+BASE_ONE;
        other_edge->iport = d_for_ret_port;
        LinkExport( caller_node[CALL_BODY], other_edge );	/* E */
        LinkImport( d_for_stack[depth]->F_RET, other_edge );
      } else {					/* middle level pass through */
        edge = FindExport( d_for_stack[d_depth]->F_RET, source_ports[i] );
        node = FindLastNode( d_for_stack[depth]->F_RET );
        other_node = NodeAlloc( node->label+1, IFReduce );
        LinkNode( node, other_node );
        name = (char*)malloc( 20 );
        sprintf(name, call_name[CALL_MERGE], counter);
        edge = FindImport( d_call, CALL_OFFSET ); /* function name on port 1 */
        other_edge = CopyEdge( edge, (PNODE)NULL, (PNODE)NULL);
        AttachEdge( (PNODE)NULL, CONST_PORT, other_node, CALL_OFFSET,
            edge->info, name );

        other_edge = CopyEdge( edge, (PNODE)NULL, (PNODE)NULL);
        AttachEdge( (PNODE)NULL, CONST_PORT, other_node, CALL_OFFSET+2,
            edge->info, name );
        
        edge = FindImport( d_for_stack[depth]->F_BODY, d_for_port );
        other_edge = CopyEdge( edge, d_for_stack[depth]->F_RET,
            other_node); 
        other_edge->eport = d_for_port;
        other_edge->iport = CALL_OFFSET + 1;
        LinkExport( d_for_stack[depth]->F_RET, other_edge );	/* G */
        LinkImport( other_node, other_edge );	

        other_edge = CopyEdge( edge, other_node, 
            d_for_stack[depth]->F_RET );
        other_edge->eport = BASE_ONE + i;
        other_edge->iport = d_for_ret_port;
        LinkExport( other_node, other_edge );			/* H */
        LinkImport( d_for_stack[depth]->F_RET, other_edge );
      }
    
      /*** From forall new exports to downstream (cutting other outputs). ***/
    
      if ( depth==0 ) {		/* connect call to downstream */
        for ( edge = d_call->exp; edge != NULL; edge = other_edge ) {
          other_edge = edge->esucc;
          if ( edge->eport == i+BASE_ONE ) {
            UnlinkExport( edge ); 
            edge->eport = d_for_ret_port;
            LinkExport( d_for_stack[depth], edge );		/* I */
          }
        }
      } else {					/* connect middle level up */
        d_for_port = FindLargestImport( d_for_stack[depth-1] ) + 1;
        edge = FindExport( d_for_stack[d_depth]->F_RET, source_ports[i] );
        other_edge = CopyEdge( edge,  d_for_stack[d_depth],  
            d_for_stack[depth-1]->F_BODY );
        other_edge->eport = d_for_ret_port;
        other_edge->iport = d_for_port;
        LinkExport( d_for_stack[depth], other_edge );		/* F */
        LinkImport(  d_for_stack[depth-1]->F_BODY, other_edge );
      }
    }
  }

  return;
}
  
/**************************************************************************/
/* LOCAL  **************       ReduceArrayFuncs    ************************/
/**************************************************************************/
/* PURPOSE: REDUCE ARRAY FUNCTION NODES IN GRAPH g. THIS ROUTINE RETURNS  */
/*          COUNT OF FUNCTIONS REDUCED.                                   */
/**************************************************************************/

static int ReduceArrayFuncs( r_graph, r_loop, r_port_count, r_ports, g )
  PNODE r_graph;			/* reduction function graph */
  PNODE r_loop;				/* reduction LOOP subgraphs */
  int r_port_count[MAX_CALLS][MAX_TYPES];
  int r_ports[MAX_CALLS][MAX_TYPES][MAX_PORTS];
  PNODE g;				/* graph which may call r_graph */
{
  int d_ports[MAX_PORTS];
  PNODE d_call;				/* reduction driver CALL */ 
  PNODE sn, sg;
  int d_depth;
  PNODE d_for_stack[MAX_NFORALL];	/* reduction driver embedded FOR's */
  int rafcalls = 0;

  /*** Search across all nodes for matching driver call to reduction. ***/

  for ( d_call = g->nsucc; d_call != NULL; d_call = sn ) {
    sn = d_call->nsucc;			/* d_call may be deleted */

    /*** Recursively search down also. ***/

    if ( IsCompound( d_call ) ) {
      for ( sg = d_call->gsucc; sg != NULL; sg = sg->gsucc )
        rafcalls +=  ReduceArrayFuncs( r_graph, r_loop, r_port_count, r_ports, 
            sg );
    }

    /*** Check on driver call. ***/

    if ( IsCall( d_call ) ) {
      if ( strcmp( d_call->imp->CoNsT, r_graph->gname ) == 0 ) {
          ++Tracs;
        if ( IsReductionDriverCandidate( d_call, d_for_stack, &d_depth,
          d_ports ) ) {
          rafcalls++;
          RewireReduction( r_graph, r_loop, r_port_count, r_ports,
              d_call, d_for_stack, d_depth, d_ports );
        }
      }
    }
  }

  return rafcalls;
}

/**************************************************************************/
/* GLOBAL **************     WriteReduceInfo       ************************/
/**************************************************************************/
/* PURPOSE: WRITE ARRAY REDUCTION INFORMATION TO stderr.                  */
/**************************************************************************/

void WriteReduceInfo()
{
  FPRINTF (infoptr, "\n **** ARRAY REDUCTION FUNCTIONS\n\n");
  FPRINTF (infoptr, " Reduced Array Functions:     %d of %d\n", rafs, Trafs);
  FPRINTF (infoptr, " Reduced Function Calls:      %d of %d\n", racs, Tracs);
  FPRINTF (infoptr, " Modified AElement Nodes:     %d of %d\n", maelm, Tmaelm);
}

/**************************************************************************/
/* GLOBAL **************       If1Reduce           ************************/
/**************************************************************************/
/* PURPOSE: REDUCE THE ARRAY FUNCTIONS MARKED WITH 'd'.                   */
/**************************************************************************/

void If1Reduce( )
{
  PNODE r_graph;			/* reduction function graph */
  PNODE g;				/* graph which may call r_graph */
  PNODE r_loop;				/* reduction LOOP subgraphs */
  PNODE last_graph;
  int r_port_count[MAX_CALLS][MAX_TYPES];
  int r_ports[MAX_CALLS][MAX_TYPES][MAX_PORTS];

  for ( last_graph = glstop->gsucc; last_graph->gsucc != NULL; 
      last_graph = last_graph->gsucc )
      ;
  for ( r_graph = glstop->gsucc; ; r_graph = r_graph->gsucc ) {
    ++Trafs;
    if ( r_graph->mark == 'd' ) {
      rafs++;
      if ( !IsReductionCandidate( r_graph, &r_loop, r_port_count, r_ports ) )
        continue;
      for ( g = glstop->gsucc; g != NULL; g = g->gsucc ) {
        if ( g!=r_graph )
          racs += ReduceArrayFuncs( r_graph, r_loop, r_port_count, r_ports, g );
      }
    }
    if ( r_graph == last_graph)
      break;
  }

#ifdef DEBUG
  {
    FILE	*save_output;
    save_output = output;
    output = fopen("reduce.opt", "w");
    If1Write();
    fclose(output);
    output = save_output;
  }
#endif /* DEBUG */

  return;
}
