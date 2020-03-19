#include "world.h"

static void
TestCascade(G)
     PNODE	G;
{
  PNODE		SubG,N,Test,TrueSide,FalseSide;
  PEDGE		TestEdge,E,ENext;
  int		TestPort;

  for(N=G->G_NODES; N; N = N->nsucc) {
    if ( IsCompound(N) ) {
      /* ------------------------------------------------------------ */
      /* Recursively apply					      */
      /* ------------------------------------------------------------ */
      for(SubG = N->C_SUBS; SubG; SubG = SubG->gsucc) TestCascade(SubG);

      if ( !IsBranch(N) )
        continue;

      switch (N->type) {
      case IFSelect:

	Test 		= N->S_TEST;
	FalseSide	= N->S_ALT;
	TrueSide	= N->S_CONS;

	/* ------------------------------------------------------------ */
	/* Find the test edge -- It must be an imported value		*/
	/* ------------------------------------------------------------ */
	TestEdge = FindImport(Test,1);
	if ( TestEdge->src != Test ) break;
	TestPort = TestEdge->eport;

	/* ------------------------------------------------------------ */
	/* If the test edge is used internally, set to true or false    */
	/* ------------------------------------------------------------ */
	for(E=FalseSide->exp; E; E = ENext ) {
	  ENext = E->esucc;
	  if ( E->eport == TestPort ) {
	    UnlinkExport(E);
	    E->src = (PNODE)NULL;
	    E->eport = CONST_PORT;
	    E->CoNsT = "FALSE";
	  }
	}
 
	for(E=TrueSide->exp; E; E = ENext ) {
	  ENext = E->esucc;
	  if ( E->eport == TestPort ) {
	    UnlinkExport(E);
	    E->src = (PNODE)NULL;
	    E->eport = CONST_PORT;
	    E->CoNsT = "TRUE";
	  }
	}
        break;
 


      default:
	UNEXPECTED("Unknown branch");
      }
    }
  }
}

/**************************************************************************/
/* GLOBAL **************      If1ITestCascade      ************************/
/**************************************************************************/
/* PURPOSE: APPLY TEST CASCADE TO ALL THE NODES IN THE PROGRAM.     */
/**************************************************************************/

void
If1TestCascade()
{
  register PNODE f;

  for ( f = glstop->gsucc; f != NULL; f = f->gsucc ) {
    TestCascade( f );
    FastCleanGraph( f );
  }
}
