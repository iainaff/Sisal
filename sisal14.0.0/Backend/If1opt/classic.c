#include "world.h"

FILE* input;
FILE* output;
char* program = "if1classic";
int sgnok = 1;

/**************************************************************************/
/* LOCAL  **************     ConvertIfThenElse     ************************/
/**************************************************************************/
/* Convert IfThenElse form (boolean branch) to Select (integer branch)    */
/**************************************************************************/
static void ConvertIfThenElse(PNODE node) {
  PNODE C;
  PNODE sg;
  PNODE G;
  PNODE lastn;
  PNODE n;
  PNODE n1;
  PNODE test_graph;
  PNODE then_graph;
  PNODE else_graphs;
  PNODE else_part;
  PNODE subg;
  PEDGE e;
  PEDGE condition;
  PEDGE e2;
  PEDGE literal;
  PALIST L;
  int label;
  int port;
  int i;

  if ( IsSimple(node) ) {
    /* Get rid of NOOP */
    if ( node->type == IFNoOp ) {
      literal = node->imp;
      /* Unlink output edges and turn into literal */
      for(e = node->exp; e; e = e2) {
	e2 = e->esucc;
	UnlinkExport(e);
	e->src = 0;
	e->eport = -1;
	e->CoNsT = literal->CoNsT;
      }
    }
    /* Nothing */
  } else if IsGraph(node) {
    G = node;
    for(n=G->G_NODES; n; n = n->nsucc) ConvertIfThenElse(n);
  } else if ( IsCompound(node) ) {
    C = node;
    for( sg=C->C_SUBS; sg; sg = sg->gsucc ) ConvertIfThenElse(sg);
    if ( C->type == IFIfThenElse ) {
      /* ----------------------------------------------- */
      /* if test then G1 else G2 end if -->              */
      /* select (int(not(test))) G1/G2                   */
      /* ----------------------------------------------- */
      test_graph = C->C_SUBS;
      then_graph = test_graph->gsucc;
      else_graphs = then_graph->gsucc;

      for(n1=test_graph->nsucc; n1; n1=test_graph->nsucc) {
	RemoveNode(n1,test_graph);
	InsertNode(C,n1);
      }

      /* Not the test  */
      n1 = NodeAlloc(1,IFNot);
      LinkNode(test_graph,n1);
      condition = test_graph->imp;
      UnlinkImport(condition);
      LinkImport(n1,condition);

      AttachEdge(n1,1,test_graph,1,ihead,0);

      RemoveNode(n1,test_graph);
      InsertNode(C,n1);

      /* Convert boolean to an integer */
      n1 = NodeAlloc(1,IFInt);
      LinkNode(test_graph,n1);
      condition = test_graph->imp;
      UnlinkImport(condition);
      LinkImport(n1,condition);

      AttachEdge(n1,1,test_graph,1,ihead->next->next->next,0);

      RemoveNode(n1,test_graph);
      InsertNode(C,n1);

      C->type = IFSelect;

      /* ----------------------------------------------- */
      /* We either have a single else or a bunch of test */
      /* truepart, falsepart graphs.  We need to convert */
      /* to SELECT if we have the chain */
      /* ----------------------------------------------- */
      if ( else_graphs->gsucc ) {
	else_part = NodeAlloc(0,IFSGraph);
	LinkGraph(then_graph,else_part);
	n1 = NodeAlloc(1,IFIfThenElse);
	LinkNode(else_part,n1);

	L = AssocListAlloc( 0 );
	L = LinkAssocLists( L, AssocListAlloc( 1 ));
	L = LinkAssocLists( L, AssocListAlloc( 2 ));
	
	C->C_SCNT = 3;
	C->C_ALST = L;


	for(e=C->imp; e; e=e->isucc) {
	  AttachEdge(else_part,e->iport, n1,e->iport, e->info, 0);
	}

	for(e=C->exp; e; e=e->esucc) {
	  if ( !FindExport(n1,e->eport) ) {
	    AttachEdge(n1,e->eport, else_part,e->eport,e->info, 0);
	  }
	}

	G = n1;
	i = 0;
	L = 0;
	while(else_part->gsucc) {
	  L = L?LinkAssocLists( L, AssocListAlloc(i++)):AssocListAlloc(i++);
	  subg = else_part->gsucc;
	  UnlinkGraph(subg);
	  G = LinkGraph(G,subg);
	}

	n1->C_SCNT = i;
	n1->C_ALST = L;

	/* Fix the rest of the chain of elseif parts */
	ConvertIfThenElse(C);

      }
    }
  } else {
    fprintf(stderr,"%s:%d: Mistake\n",__FILE__,__LINE__);
    exit(1);
  }
}

int main(int argc, char** argv) {
  PNODE f;

  output = stdout;

  if ( argc <= 1 ) {
    input = stdin;
  } else {
    input = fopen(argv[1],"r");
    if (!input) {
      perror(argv[1]);
      exit(1);
    }
  }

  If1Read();

  /* ----------------------------------------------- */
  /* This assumes Select not IfThenElse...           */
  /* ----------------------------------------------- */
  for(f=glstop->gsucc; f; f = f->gsucc) {
    ConvertIfThenElse(f);
  }

  if ( argc <= 1 ) {
    output = stdout;
  } else {
    output = fopen(argv[1],"w");
    if (!output) {
      perror(argv[1]);
      exit(1);
    }
  }

  If1Write();
}
