/* Output from p2c, the Pascal-to-C translator */
/* From input file "mono.bin.noblank.p" */

extern char *DaveMalloc(); /* PROFILE */


/* copyright (C) 1986 by the Regents of the University of California */

#include "p2c.h"
#define unchar UNCHAR
typedef unsigned int unchar;

/* cann 5/29/90 */
#define fread(w,x,y,z) fscanf( z, "%d", w )

/*========================================================
     declarations for parser.p
=========================================================*/
/* STRUTL has no data dependancies ...  first in .makeo file */
/* Pad String pads a string constant to a M4 known size  - pjm 1/88*/
/* SETUTL has on data dependancies ...
   second in .makeo file */
/* IF1INIT no dependancies..
   third in .makeo file */
/* FILUTL depends on STRUTL,
   Include fourth in .makeo files */
/* PARUTL depends on STRUTL, FILUTL,
   Include fifth in .makeo files   */
/* Graph.m4 depends on STRUTL, IF1INIT, SETUTL.
   Include sixth in the .makeo file      */
/* IF1INPUT depends on STRUTL, IF1INIT, GRAPH.
   Include seventh in .makeo file                */
/* IF1DUMP depends on STRUTL, GRAPH, and IF1INPUT
   eighth in .makeo file                          */
/* llprocs depends on STRUTL */
/* UNIX (tm Bell Labs.) specific declarations */
/*  global constants here */
/*#TITLE  STRUTL  CONSTANTS       Jan82   String Handling Routines.*/

#define maxstringchars  127

#define blankstring     "                                                                                                                                    "

#define maxbigint       SHORT_MAX
/* Constants from setutl.m4 */
/* The maximum size of a set of integers */

#define maxsetsize      6000   /*mlw*/
/*#TITLE  IF1INIT CONSTANTS       Oct83   Standard IF1 Constants*/
/*Constants derived from v9.me 84/7/26 by sks*/
#define ifmaxnamelen    15   /* dlz -8/87 for the name tables */

/* CANN PEEK */ /* #define ifmaxnode       195 */
#define ifmaxnode       196  /* CANN PEEK */

/*Node */
#define ifnforall       0
#define ifnselect       1
#define ifntagcase      2
#define ifnloopa        3
#define ifnloopb        4
#define ifnifthenelse   5   /*mlw - 2/3/86*/
#define ifniter         6
#define ifnmodule       20   /* jwrg */
#define ifngraph        21   /* jwrg */
#define ifnaaddh        100
#define ifnaaddl        101
#define ifnaextract     102
#define ifnabuild       103
#define ifnacatenate    104
#define ifnaelement     105
#define ifnafill        106
#define ifnagather      107
#define ifnaisempty     108
#define ifnalimh        109
#define ifnaliml        110
#define ifnaremh        111
#define ifnareml        112
#define ifnareplace     113
#define ifnascatter     114
#define ifnasetl        115
#define ifnasize        116
#define ifnabs          117
#define ifnbindarguments  118
#define ifnbool         119
#define ifncall         120
#define ifnchar         121
#define ifndiv          122
#define ifndouble       123
#define ifnequal        124
#define ifnexp          125
#define ifnfirstvalue   126
#define ifnfinalvalue   127
#define ifnfloor        128
#define ifnint          129
#define ifniserror      130
#define ifnless         131
#define ifnlessequal    132
#define ifnmax          133
#define ifnmin          134
#define ifnminus        135
#define ifnmod          136
#define ifnneg          137
#define ifnnoop         138
#define ifnnot          139
#define ifnnotequal     140
#define ifnplus         141
#define ifnrangegenerate  142
#define ifnrbuild       143
#define ifnrelements    144
#define ifnrreplace     145
#define ifnredleft      146
#define ifnredright     147
#define ifnredtree      148
#define ifnreduce       149
#define ifnallbutlastvalue  150
#define ifnsingle       151
#define ifntimes        152
#define ifntrunc        153
#define ifnaprefixsize  154
#define ifnareplacen    160
#define ifnspawn        161
#define ifnfilter       168
#define ifnashift       169
/*IF2 Nodes*/
#define ifnaaddlat      170
#define ifnaaddhat      171
#define ifnabufpartition  172
#define ifnabuildat     173
#define ifnabufscatter  174
#define ifnacatenateat  175
#define ifnaelementsat  176
#define ifnaextractat   177
#define ifnafillat      178
#define ifnagatherat    179
#define ifnaremhat      180
#define ifnaremlat      181
#define ifnareplaceat   182
#define ifnarraytobuf   183
#define ifnasetlat      184
#define ifndefarraybuf  185
#define ifndefrecordbuf  186
#define ifnfinalvalueat  187
#define ifnmemalloc     188
#define ifnrbufelements  189
#define ifnrbuildat     190
#define ifnrecordtobuf  191
#define ifnrelementsat  192
#define ifnreduceat     193
#define ifnshiftbuffer  194
#define ifncondsetrefcnt  195

#define ifnpeek           196 /* CANN PEEK */

#define ifmaxerror      12
/*ErrorType */
/* genuine errors must be all < IFENoError */
#define ifebroken       0
#define ifeerror        1
#define ifemiselt       2
#define ifenegover      3
#define ifenegunder     4
#define ifeposover      5
#define ifeposunder     6
#define ifeundef        7
#define ifeunknown      8
#define ifezerodivide   9
#define ifenoerror      10   /*rky, means value present, not an error */
#define ifenovalue      11
    /*rky, means no value present, not even an error */
#define ifmrnovalue     12   /*dlz no min reeval previous value */
/*TypeTableEntry */
#define ifmaxtype       11
#define iftarray        0
#define iftbasic        1
#define iftfield        2
#define iftfunctiontype  3
#define iftmultiple     4
#define iftrecord       5
#define iftstream       6
#define ifttag          7
#define ifttuple        8
#define iftunion        9
#define iftwild         10
#define iftbuffer       11
/*BasicType */
#define ifmaxbasic      6
#define ifbboolean      0
#define ifbcharacter    1
#define ifbdouble       2
#define ifbinteger      3
#define ifbnull         4
#define ifbreal         5
#define ifbwild         6
/* types of reduction operations */
#define ifmaxreduction  5
#define ifrsum          0
#define ifrproduct      1
#define ifrleast        2
#define ifrgreatest     3
#define ifrcatenate     4
#define ifrappend       5
    /* rky 10dec84. AGather is just a kind of multiple reduction */
/* PARUTL TYPESApril 84Parameter Handling Constants*/
#define maxinfile       9

#define parflagchar     "-"
#define parsepchar      " "
#define parsplitchar    " "
#define parargchar      " "
/*dlz - 8/87 added new functions: NewTypeAlloc, NewEdgeAlloc, and
  NewNodeAlloc to consolidate and replace various allocation and
  initialization sequences throughout.  Also, made compilation
  of vivek's extra edge and node fields conditional.  Some new
  macros, etc. for simplification, fixed errors in some existing
  macro definitions, general "cleanup" and correction */
/* Constants from graph.m4 */
/* The number of the first Atomic node sort */

#define firstatom       100
/* The maximum number of Type Table Entries handled */
#define entrymax        1000
/* Constant for internal pragmas (for Vivek) */
#define extrainfomax    15
/* for Bit Marks set in node record */
#define maxbitrange     29
/*------------------- SEMANTIC ROUTINE CONSTANTS ---------------------*/
#define maxhashtable    90
#define hashtablesize   91
#define maxsemanticnumber  200
#define lbstartfunction  1
#define lbstartnamelist  2
#define lbaddtonamelist  3
#define lbstarttypelist  4
#define lbaddtotypelist  5
#define lbendglobalheader  6
#define lbendforwardglobheader  7
#define lbsmashintotable  8
#define lbinitsem       9
#define lbpushbasic     10
#define lbpushtypename  11
#define lbpusharray     12
#define lbfinishtypelinks  13
#define lbchecktypename  14
#define lblinkbase      15
#define lbpushstream    16
#define lbpushrecord    17
#define lbbuildfield    18
#define lbpushunion     19
#define lbbuildtag      20
#define lbpushnulltype  21
#define lbenddefinelist  22
#define lbaddtodefinelist  23
#define lbstartforwardfunct  24
#define lbstartglobalfunct  25
#define lbprocessparam  27
#define lbendfunctionheader  29
#define lbendfunctionlist  30
#define lbendfunction   31
#define lbstartsymlist  32
#define lbcheckatypes   33
#define lbcheckarity    34
#define lbassoctypes    35
#define lbincreasegenerator  36
#define lbcheckcross    37
#define lbexplicitcross  38
#define lbstartbody     39
#define lbendabody      40
#define lbmakeforall    41
#define lbfixforfunct   42
#define lbprocesstaghead  43
#define lbcreatetagsubgraph  44
#define lbchecktagnames  45
#define lbassocexprs    46
#define lbendtagcase    47
#define lbotherwisetag  48
#define lbstartforinit  49
#define lbendinitpart   50
#define lbpushwhile     51
#define lbpushuntil     52
#define lbstartatest    53
#define lbendtest       54
#define lbloopbassoclist  55
#define lbloopaassoclist  56
#define lbpushold       57
#define lbpushnoold     58
#define lbdoredright    59
#define lbdoredleft     60
#define lbdoredtree     61
#define lbdoreduce      62
#define lbdosum         63
#define lbdoproduct     64
#define lbdogreatest    65
#define lbdoleast       66
#define lbdocatenate    67
#define lbnoredop       68
#define lbdounlessexp   69
#define lbdowhenexp     70
#define lbnomaskingexp  71
#define lbdovalueof     72
#define lbdoarrayof     73
#define lbdostreamof    74
#define lbstartreturns  75
#define lbstartainitbody  76
#define lbendbody       77
#define lbendforinit    78
#define lbstartif       79
#define lbstartbtest    80
#define lbstartifsubgraph  81
#define lbendifsubgraph  82
#define lbendpredsubgraph  83
#define lbstartbinitbody  84
#define lbendif         85
#define lbbeforeexp     86
#define lbincboostfac   87
#define lbdecboostfac   88
#define lbreduceexp     89
#define lbpoppreclevel  90
#define lbafterexp      91
#define lbdofieldlist   92
#define lbpushnilttptr  93
#define lbstartrbuild   95
#define lbbuildfieldlist  97
#define lbendrbuild     98
#define lbstartabuild   99
#define lbstartsbuild   100
#define lbdolowerbound  101
#define lbendabuild     102
#define lbendsbuild     103
#define lbpushniltag    105
#define lbendubuild     106
#define lbpushemptyexplist  107
#define lbdofunctcall   108
#define lbdoarrayindex  109
#define lbdoarrayreplace  110
#define lbbuildreplace  111
#define lbdobinaryop    112
#define lbpopsymlist    113
#define lbbuildchar     114
#define lbfixfortag     115
#define lbbuilddouble   116
#define lbbuildint      118
#define lbbuildreal     120
#define lbcreateisunion  121
#define lbcheckold      122
#define lbpushuplus     123
#define lbpushuminus    124
#define lbpushunot      125
#define lbcreateiserror  127
#define lbpushnilcons   129
#define lbpushtruecons  130
#define lbpushfalsecons  131
#define lbpushintcons   132
#define lbpushrealcons  133
#define lbpushcharcons  134
#define lbpushstringcons  135
#define lbpusherrorcons  136
#define lbfindandpushname  137
#define lbopenllevel    138
#define lbendlevel      139
#define lbendforall     140
#define lbendprogram    141
#define lbpushdoubcons  142
#define lbendbbody      143
#define lbaftersimpexp  144
#define lbendtypedefs   145
#define lbpushdefarray  146
#define lbpushdefstream  147
#define lbpushdefrecord  148
#define lbpushdefunion  149
#define lbbuilddeffield  150
#define lbbuilddeftag   151
#define lbstarttypedefs  152
#define lbpreparefordot  153
#define lbstartnestedif  154
/* Type Name Key Word Terminal Values */
#define boolkw          21
#define charkw          23
#define doubkw          27
#define intgkw          41
#define nullkw          47
#define realkw          52
/*Type table labels for the basic types*/
#define boollabel       1
#define charlabel       2
#define doublabel       3
#define intlabel        4
#define reallabel       6
/*Type table codes for structured types*/
#define arraycode       0
#define fieldcode       2
#define recordcode      5
#define streamcode      6
#define tagcode         7
#define unioncode       9
/*CrossDotFlag constants*/
#define crossflag       1
#define dotflag         2
/* Smash into type table flags */

#define addtottable     true
#define noaddtottable   false

#define version         "1.8       "  /* Used as a String10 - pjm 1/88 */
#define compiled        "(Mar 28, 1989)"
/*    Compiled = '(unreleased version)';*/

/* #define maxlinelength   132 */
#define maxlinelength   40000  /* TOO SUPPORT THE C PREPROCESSOR */

#define maxprod         300
#define maxsym          300
#define maxstack        300
#define maxprodspace    4000   /* size of the array for storing productions */
#define maxstring       5000
/* !! constants for error correction phase */
#define maxinsertspace  2000
#define maxcorrection   20
#define maxterm         85
#define maximuminsert   maxcorrection
#define maxhisto        12   /* size of histogram array */
#define alphalength     12

#define blank           " "
#define tab             "\t"
#define newlinechar     blank
#define opendelete      "{"
#define closedelete     "}"

#define echooldline     false
#define scandebug       false
#define tracing         false
#define gorydetail      false
#define iedp            true   /* do we want Immediate detection property */
/*--------- all purpose constants --------------------*/

#define maxalphalength  75   /*for certain kind of string*/
/* local implementation restricts to 75 for file name */
/* ---------- template constants----------*/
#define tarraymax       500
/* --------- constants for scanning ----------*/
/* NonTerminal numerical identifiers */

#define tokentrace      false

#define numreswords     103
#define maxbackup       255   /* how far we might overscan */
#define andtoken        1
#define lparentoken     2
#define rparentoken     3
#define timestoken      4
#define plustoken       5
#define commatoken      6
#define minustoken      7
#define periodtoken     8
#define dividetoken     9
#define colontoken      10
#define assigntoken     11
#define semicolontoken  12
#define chartoken       13
#define stringtoken     14
#define integertoken    15
#define idtoken         16
#define realtoken       17
#define eqtoken         18
#define firstkeyword    19
#define lastkeyword     71
#define lbrackettoken   72
#define rbrackettoken   73
#define getoken         74
#define gttoken         75
#define letoken         76
#define lttoken         77
#define nottoken        78
#define netoken         79
#define stroketoken     80
#define cattoken        81
#define commenttoken    (-1)
#define eoftoken        83
#define escapetoken     83
#define doubtoken       82
#define incltoken       (-2)
#define backspace       8   /* ASCII code for backspace */
#define newline         10
#define blankord        32
#define tabord          9
#define fford           12


/*  global types here */
/*#TITLE  STRUTL  TYPES           Jan82   String Handling Routines.*/
typedef Char stryngar[maxstringchars];

typedef struct stryng {
  char len;
  stryngar str;
} stryng;

typedef Char str10[10];
typedef Char str20[20];
/* Types from setutl.m4 */
typedef unchar bitbucket[(maxsetsize + 7) / 8];   /*mlw*/
/*mlw*/

typedef struct setofint {
  short count;   /* number of elements in the set */
  short maxsize;   /* max numb of elements set can hold */
  bitbucket inset;   /* bit vector, true means ele in set */
} setofint;

/*#TITLE  IF1INIT TYPES             IFNAug83   Standard IF1 Types*/
/* Printable Name */
typedef Char printable[16];
/* Nodes */
/* Error Values */
/* Type Values */
/* Basic Values */
/* Types for reduction operations */
typedef enum {
  ifgfunction, ifgselector, ifgalternative, ifgvariant, ifgloopainit,
  ifgloopabody, ifgloopatest, ifgloopareturns, ifgloopbinit, ifgloopbbody,
  ifgloopbtest, ifgloopbreturns, ifgforallgenerator, ifgforallbody,
  ifgforallreturns, ifgifpredicate, ifgiftrue, ifgiffalse, ifgiterbody
} ifgraphtype;
/*dlz - 8/87*/
typedef enum {
  unaryalgeb, binaryalgeb, multiplereduct, multiplefilter, multiplegenr,
  structureaccess, structurebuild, indexchange, bufferaccess, bufferbuild,
  bufferreduct, buffergenr, bufferdefine, controlabstr, functioninvoke,
  graphnode, unclassified, unknown
} ndclasstype;
/*#TITLE  PARUTL  TYPES           Jan82   Parameter Handling Routines.*/
typedef enum {
  intpar, boolpar, strpar, ifilpar, ofilpar, flagpar, restpar
} partyp;
typedef struct parrec *infilelist[maxinfile];

typedef struct parrec {
  str10 longname, shortname;
  partyp partype;
  int normpos, specpos;
  stryng parvalue;
  struct parrec *nextpar;
} parrec;

/* Types from graph.m4 */
/* Type Table Entries
      STId -a special value used for dumping out if1 programs and
              for checking multiple definitions.
      STLabel - The label of the IF1 type entry.  Also used for type
                smashing.
      STEquivChain - Used for type smashing.
      STLiteral - The name of the type (if there is one).
      STSize - Amount of memory an object of this type requires. Used
               in IF1offset.
      STRecurFlag - Whether or not this type is recursive.
*/

typedef struct stentry {
  int stid, stlabel;   /*sks*/
  struct stentry *stequivchain;   /*sks*/
  stryng stliteral;   /*mlw*/
  int stsize;
  boolean strecurflag;   /*jef 10/1/85*/
  char stsort;
  union {
    char stbasic;
    struct {
      struct stentry *starg, *stres;
    } U3;
    struct stentry *stbasetype;
    struct {
      struct stentry *stelemtype, *stnext;
    } U2;
  } UU;
} stentry;

typedef double extrainfo[extrainfomax];   /*for vivek*/
/* Ports
   Each node has a list of input and output ports.  A single structure is
   used for Literals and Edges.  Edges have some extra fields descibing
   the source Port.  The fields are used as follows:
     PTType - points to a symbol table entry giving the type of the
                port.
     PTToNode - points to the node which is destination of the edge or
                literal.
     PTToPort - gives the number of this port.
     PTToNext - points to the next input port for the destination node.
     PTName - gives the symbolic name associated with this edge (if avail.)
     PTIF1Line - the line number of the if1 file that defined this edge.
     PTSrcLine - the line number of the SISAL text file where this edge is
                 created.
     PTWiLine - the column of the SISAL text file where this edge is
                 created.
     PTId - Global ID for this edge.  Does not correspond to anything in
            the IF1 test file.
     PTMark - ByValue, ByReference or Destroy pragma.
               Destoy means this is the last reference and the value may be removed.
     PTDFAddr - AR offset pragma.
     PTLBound, PTUBound - lower and upper bound pragma.
     PTNextEdge - Used by DI for associating similar edges.
     PTClass - ???
   Extra fields for Edges:
     PTFrNode - points to the origin node.
     PTFrPort - indicates the number of the output port.
     PTFrNext - points to the next output port for the origin node.
   Extra fields for Literals:
     PTLitValue - value of the literal edge stored in Stryng format.
 */
typedef enum {
  byref, byval, destroy
} egmark;   /***NEW** no ByDefault*/
typedef enum {
  ptlit, ptedge, ptdep, ptundef
} portsort;
typedef enum {
  pcuncoded, pccoded
} portclass;
/*mlw - more descriptive name*/

typedef struct port {
  stentry *pttype;
  struct node *pttonode;
  int pttoport;
  egmark ptmark;   /*mlw  - pass by value or reference */
  int ptif1line;   /* sks for structure analysis reporting */
  int ptsrcline, ptwiline;
  struct port *pttonext;
  stryng ptname;   /*mlw*/
  int ptid;   /*mlw*/
  int ptlbound, ptubound;   /* mlw, lower and upper bounds pragmas */
  int ptsetrc, ptconmodrc, ptprodmodrc;
      /* rjs, set & modify ref count pragmas */
  int ptdfaddr, ptmraddr;   /* dlz, for MinReeval offsets */
  struct port *ptnextedge;
  portclass ptclass;
  portsort ptsort;
  union {
    struct {
      struct node *ptfrnode;
      int ptfrport;
      struct port *ptfrnext;
    } U1;
    stryng ptlitvalue;
  } UU;
} port;

/* Graphs
   Each compound node has a list of subsidiary graphs. Each graph is
   accessed via the special Graph node.
 */

typedef struct graph {
  struct node *grnode;
  struct graph *grnext;
} graph;

/* Nodes
    Nodes are either Simple nodes, Compound nodes, or Graph nodes.
    Further, a Graph node may be a Function graph (in which case it
    has an associated Linkage record) or a subgraph of a compound node.
    NDId - A global identifier with no counterpart in IF1 text files.
    NDCode - The opcode of this node, e.g. IFNPlus, IFNSelect, IFNGraph.
    NDMisc - an integer (or set of bits) for miscellaneous use.
    NDLabel - Label of the node within a graph.  Graph nodes always have
              a label of zero.
    NDLine - Line number in the IF1 text file where this node was defined.
    NDSrcLine - Line number within the SISAL text file that defined this
                node.
    NDWiLine - column number in SISAL text file where this node was defined.
    NDXCoord, NDYCoord - (X,Y) coordinate pargmas
    NDParent - If the node is Simple or Compound then Parent is the
               graph node corresponding to the graph immediately containing
               this node.  If the node is a Graph node representing a
               subgraph of a compound node the Parent is the compound
               node.If the node is a Function graph node then the
               parent is the dummy compound node called "Module".
    NDNext - The nodes of a graph are connected in a linked list.  This
             field points to the next node in the list.  The NDNext
             fields of Graph nodes point to the first (simple or compound)
             node of the graph it defines.
    NDNextInLine - A pointer to the next node of the IF1 graph that
                   came from the same source line.  Used by DI.
    NDIList - A pointer to the list of input edges of the node.  CAUTION:
              No fan-in is allowed.  Therefore, the input edges of a
              GRAPH node are the edges where the values LEAVE the graph.
    NDOList - A pointer to the list of output edges of the node.  On a
              GRAPH node, these are the edges where values ENTER the graph.
    NDDepIList - A pointer to the list of dependence edges that are input
              to the node.  NOTE: there can be fan-in of dependence edges.
              The input port of all dep edges is port 0.
    NDDepOList - A pointer to the list of dependence edges that are output
              by the node.  The output port of all dep edges is port 0.
 Fields for Graph nodes only:
    NDLink - A pointer to the Linkage record if this node represents a
             function graph node.
    NDType - A pointer into the type table used only for Function graph
             nodes.  Subgraphs of Compound nodes have a nil type.
 Fields for Compound nodes only:
    NDSubsid - a pointer to a list of subgraphs.
    NDAssoc - a pointer to an association list used to associate tag
              numbers with subgraphs of a TAGCASE nodes.
*/

typedef struct assoclist {
  int graphnum;
  struct assoclist *next;
} assoclist;

typedef enum {
  ndatomic, ndgraph, ndcompound, ndundef
} nodesort;

typedef union intbitset {
  int numb;
  long bits;
} intbitset;

typedef struct node {
  int ndid, ndcode;
  intbitset ndmisc;   /* dlz (new) -- for miscellaneous uses */
  int ndlabel;   /* sks for structure analysis reporting */
  int ndline;   /* sks for structure analysis reporting */
  int ndsrcline, ndwiline;   /* mlw, Source Line and Within Line markers */
  int ndxcoord, ndycoord;   /* mlw, X and Y coordinates - graphics */
  struct node *ndparent, *ndnext;
  port *ndilist, *ndolist, *nddepilist, *nddepolist;
  struct node *ndnextinline;
  double ndfrequency;   /*rjs, pragma for vivek */
  int ndexpanded;   /* rjs, pragma for vivek */
  nodesort ndsort;
  union {
    struct {
      struct linkrec *ndlink;
      stentry *ndtype;
      int ndfirstmro, ndlastmro;   /* dlz, added for MR */
    } U1;
    struct {
      graph *ndsubsid;
      assoclist *ndassoc;
    } U2;
  } UU;
} node;

/* Linkage Records
   Each Function in a compilation unit has a record associating the
   appropriate Graph node with a string used to identify the function
   in literals.
   LKSort - Is it a Local (G), Imported (I), or Exported (X) function.
   LKGraph - a pointer to the Function graph node.
   LKName - The name of the function.
   LKModuleName - Name of the Module this function resides in
   LKARSize - Number of DATUMS in an AR for this function (pragma).
   LKMRSize - Number of Datums in an MRV for this function (someday a pragma?)
   LKExpand - Marked to be expanded inline or not (pragma).
   LKARIndex - a field for DI internal use.
   LKNext - pointer to the next linkage record in this module.
 */
typedef enum {
  lslocal, lsimported, lsexported
} linksort;

typedef struct linkrec {
  linksort lksort;
  node *lkgraph;
  stryng lkname, lkmodulename;
  int lkarsize;   /*mlw - Size of activation record for this fun*/
  int lkmrsize;   /*dlz - Size of MR value record for this fun */
  boolean lkexpand;   /*mlw - Is this function inline expandable? */
  int lkarindex;
  struct linkrec *lknext;
} linkrec;

/* a more descriptive name for pointers to type table
                     entries.  JEF */
/*------------------ POINTERS ------------------------------------------*/
/* -------------- TYPES OF LEXICAL LEVELS THAT EXIST ------------------- */
typedef enum {
  forloop, specfa, let, funct, tagcase, initloop, ifcomp, root, nestedifcomp
} lexleveltype;
/* -------------- TYPES OF SYMBOL TABLE OBJECTS THAT EXIST ------------- */
typedef enum {
  ttype, tglobal, tfunction, tforward, tvariable, tparam, tpredefined, none
} identtype;
/* Variants rec types that occur on the undef pointer list of symbol
   tabel entried for type names.*/
typedef enum {
  typetag, symtag
} torntype;
/* Flag to denote what type of reduction node is to be places in the
   returns graph of a for loop */
typedef enum {
  rfnone, rfsum, rfproduct, rfgreatest, rfleast, rfcatenate
} reduceflag;
/* List of predefined functions that exist so a case statement can be
   used to distiguish them. */
typedef enum {
  nopredef, tgabs, tgarrayaddh, tgarrayaddl, tgarrayadjust, tgarrayfill,
  tgarraylimh, tgarrayliml, tgarrayprefixsize, tgarrayremh, tgarrayreml,
  tgarraysetl, tgarraysize, tgexp, tgfloor, tgmax, tgmin, tgmod,
/* CANN PEEK */ tgpeek,
  tgstreamappend, tgstreamempty, tgstreamfirst, tgstreamprefixsize,
  tgstreamrest, tgstreamsize, tgtrunc
} predeftype;
/* Flag to denote whether the test boolean in a forinit loop should be
   negated or not */
typedef enum {
  neitherloop, whileloop, untilloop
} testflagtype;
/* Flag to denote whether a pragma is to be processed by the first pass
   of the compiler or not */
typedef enum {
  pother, pinclude
} pragmaname;
/* Flags to denote the status of a loop variable at a given time */
/* not a loop var */
/* is a loop variable */
/* is a loop constant */
/* appears in the init part, could end a LVar or a
           LConst */
typedef enum {
  lnone, lvar, lconst, linit, lbinit
} loopvarflags;
/* appears in the init part of a LoopB, set to
           a LBInit before the test and to an LInit after
           the test.  This is so it cannot be used with
           the old modifier in the test clause */
/* Flag to tell AssignPorts with it is to assign the to or from port of
   the given edge. */
typedef enum {
  noport, toport, fromport
} toorfromtype;
typedef enum {
  truetag, falsetag, errortag
} tagflag;

typedef struct defrec {
  /* Definition lines for names in the source code */
  int fline, fcolumn, sline, scolumn;
} defrec;

typedef struct reflist {
  /* List items for refernces of names in the source code */
  int line, column;
  struct reflist *nextref;
} reflist;

typedef struct refrec {
  /* List pointers for the references list */
  reflist *front, *back;
} refrec;

typedef struct edgelist {
  /* Items for the list of edges to be assigned the */
  port *edge;   /* same port number.  ToOrFrom denotes whether the */
  toorfromtype toorfrom;   /* to port ot from port is to be filled in. */
  struct edgelist *next;
} edgelist;

typedef struct edgelisthead {
  /* list structure for the lists of edges */
  stryng edgename;   /* Edges name, used to find it on the list */
  edgelist *edges;   /* List of edges */
  struct edgelisthead *next;
} edgelisthead;

/* holds infomation as to where name exists in the graph at any given time.
   Used in the symbol table, by AddEdge to connect edges from one place to
   another and as a way to say this information on the semantic stack. */

typedef struct inforecord {
  node *node_;   /* node value comes from */
  int port_;   /* port value comes from */
  stentry *typeptr;   /* type of value */
  stryng name;   /* name of value */
  stryng litvalue;   /* value if it is a literal, then no node or port*/
  edgelisthead *onlist;   /* If port isn't assigned yet, this points to */
} inforecord;   /* the edge list the value is on */

/*  This record holds information used in creating a Tagcase node.
    It mimicks the type structure of a union so info on the tagnames
    found and the subgraphs they are associated with can be saved.*/

typedef struct tcstacklistrec {
  stryng tagname;
  int sgnum;
  stentry *typeptr;
  struct tcstacklistrec *next;
} tcstacklistrec;

/*  This record hold information for the cross reference that can be printed
    at the end of a function.  Since there may be internal scope within a
    function, as these scope are exited information about there identifiers
    are placed in these reords which are then stored in a large array by
    alphabetical order.*/

typedef struct xrefrec {
  stryng name;
  reflist *refs;
  defrec *defs;
  stentry *typeptr;
} xrefrec;

/* ------------------ SYMBOL TABLE ENTRIES ---------------------------- */

typedef struct symtblbucket {
  stryng name;
  defrec *defloc;
  refrec *references;
  stentry *typeptr;
  inforecord *graphinfo;
  /* info as to where this name currently
                                        is in the graph */
  struct symtblbucket *nextbucket;
  boolean importedflag;
  /* has name been pulled into this
                                        scope from an outer scope or was
                                        it defined in this scope? */
  identtype identtag;
  union {
    struct tornrec *undeftypeptrs;
    struct {
      /* List of places in type table and
         symbol table to fill in when this
         type name is defined.  Types can
         be refernced before they are
         defined.  */
      loopvarflags *loopflag;   /*see LoopVarFlags*/
      boolean tflag;   /*Denotes a loop Temporary*/
      inforecord *oldfrom;   /*Old value information*/
      boolean redefflag;   /*been redefined?*/
    } U4;
    predeftype predefined;   /*see PreDefType*/
  } UU;
} symtblbucket;

/* ------------------------ LEX LEVEL OBJECTS --------------------------- */

typedef struct treestackobj {
  symtblbucket *hashtable[maxhashtable + 1];
  node *prevnode;
  /* Last node in the node chain, connect the next
                               one to this one. */
  node *currentsubn0;
  /* Node pointer for the current subgraph being
                               constructed. */
  int nodecounter;   /* Node count for the current subgraph */
  struct treestackobj *lastlevel, *nextlevel;
  lexleveltype lleveltag;
  union {
    struct {
      edgelisthead *kflist, *mflist, *tflist;
      /* List of ports to be assigned after loop is complete.  */
      node *forallnode;   /* ptr to Compound Node*/
      inforecord *lowerbound;
      /* Infomation for the lower bound
                                      if an array is returned */
      int crossdotflag;
      /* flag to make sure dots and
                                      crosses aren't mixed */
      boolean crossdoterror;   /* dots and crosses were mixed */
      node *returnn0, *bodyn0;
      int bodynodecount, returnnodecount;
      node *bodyprevnode, *returnprevnode;
      int bodyoutputportnum;
      /* Above variables allow construction of both the
         body and returns graphs to occur at once.  A
         "context switch" is done between the two.  */
      int freturnportnum;   /* count of output ports */
    } U0;
    struct {
      edgelisthead *kilist, *lilist, *tilist;
      /* List of ports to be assigned after loop is complete.*/
      node *initnode;   /* ptr to Compound Node */
      int ireturnportnum;   /* count of output ports */
    } U5;
    struct {
      int ktport;   /* Count of KPorts */
      node *tagcasenode;   /* ptr to compound node */
      struct typelistrec *tarmreturnstype;
    } U4;
    struct {
      /* types or values returned, used to
         make sure all arms return the same
         types */
      int kiport;   /* Count of KPorts */
      node *ifnode;   /* ptr to compound node */
      struct typelistrec *iarmreturnstype;
    } U8;
    /* types or values returned, used to
       make sure all arms return the same
       types */
    symtblbucket *lfunctname;   /*Sym Tab entry for the funct */
  } UU;
} treestackobj;

typedef enum {
  nulltype, booltype, intgtype, realtype, doubtype, chartype
} basictypes;
typedef enum {
  noop, orop, andop, mult, divd, plus, minus, concat, lt, le, gt, ge, equal,
  notequal, uplus, uminus, unot
} opertype;
/*------------------  SEMANTIC ERROR TYPES -------------------------------*/
typedef enum {
  fndoubdef, tnameundef, fieldnameddef, tagnameddef, defineerr, doubledefarg,
  forwardtype, innerfndoubdef, binaryoparity, chararity, doublearity,
  intarity, realarity, binarytypesnoteq, charitype, doubleitype, intitype,
  realitype, binarytypeerr, typefunctres, numfunctres, asstypeclash,
  vardoubledef, longexplist, longnamelist, vardoubledecl, vnameundef,
  undeffunct, typefunctarg, numfunctarg, unaryoparity, unarytypeerr,
  vnamenotloopvar, arityonerror, arityonisu, isutype, expimporterr,
  arityonrbuild, typeonrbuild, forwardnotresolved, arityonubuild, tnonubuild,
  undeftagub, typeclashub, lbarity, lbtype, notypeabuild, atypenameerr,
  notarraytype, arraytypeclash, abuildelemtype, arityonrselect, etypeonrs,
  nameundrs, ntypeonrs, arityonrexp, arityonoexp, ntyperr, nameundrr,
  typeclashrr, ifreturnstypes, tagreturnstypes, numifreturns, numtagreturns,
  prednotbool, predarity, nothingsc, typenssc, expsnstypesc, typeclashsc,
  undefntag, arityontag, typenutc, nameundeftc, ddtname, typesnsametc,
  othernone, notalltc, arrayarity, needarraytype, indexexperr, arityareplace,
  typeareplace, replaceoverdim, replaceelemtype, valueofarity, maskingarity,
  masknotbool, arityonfa, typeonfa, atinrg, reducetype, arrayofarity,
  streamofarity, icnotas, cdmix, decbutnotdef, lvarnotdef, testarity,
  testnotbool, lvarusebredef, loopcvmix, loopconsassn, redefimlvar,
  lvarneverredef, deflistnotempty, typedoubdef
/* p2c: mono.bin.noblank.p, line 1257: Note:
 * Line breaker spent 4.5+1.02 seconds, 5000 tries on line 1012 [251] */
} errortypes;

typedef struct errorrecord {
  int linenumber, column;
  errortypes etype;
  union {
    stryng errorstr;
    opertype op;
    stentry *typeptr;
    struct {
      stentry *btypeptr;
      opertype bop;
    } U18;
    struct {
      stentry *utypeptr;
      opertype uop;
    } U31;
    struct {
      stryng functname;
      int resnum;
    } U19;
    struct {
      stentry *exptype, *nametype;
    } U21;
    struct {
      stryng varname;
      stentry *exp1type, *exp2type;
    } U25;
    struct {
      stryng functionname;
      int argnum;
    } U29;
    int elemnum;
    int expnum;
    struct {
      stentry *redtype;
      reduceflag redflag;
    } U91;
  } UU;
} errorrecord;

/*---------------  RECORDS FOR BUILDING LISTS  -----------------------*/
/*  This record is used to build lists of identifiers.  It holds the line
    number where the id is found and the column (which is not currently
    used).  */

typedef struct namelistrec {
  stryng name;
  struct namelistrec *next, *prev;
  int linenum, column;
} namelistrec;

/*  This record is used to build lists of Type Table pointers.  */

typedef struct typelistrec {
  stentry *typeptr;
  struct typelistrec *next, *prev;
  int linenum;
} typelistrec;

/*  This record is used to build a list of type table and symbol table
    points.  The list is kept in a symbol table entry for a type name.
    When the structure for the type name is  then list is searched
    and the new type structure is associated with the list entries.*/

typedef struct tornrec {
  struct tornrec *next, *prev;
  torntype torntag;
  union {
    stentry *ttptr;
    symtblbucket *symptr;
  } UU;
} tornrec;

/*  This record is used to build lists of "graph information".  For instance
    it is used to place expressions of any arity on the semantic stack.  The
    expression is represented as a list of graph locations.*/

typedef struct explistnode {
  inforecord *graphinfo;
  struct explistnode *next;
} explistnode;

/*  This record is used to build lists of Symbol Table entries.*/

typedef struct symlistnode {
  symtblbucket *sym;
  struct symlistnode *next;
} symlistnode;

/*------------- SEMANTIC STACK STRUCTURES ------------------------------*/
typedef enum {
  tgnamelist, tgtypelist, tgttptr, tgsymptr, tgexplist, tgpreclevel,
  tgsymlist, tgnodeptr, tgsubgraphnum, tgoldflag, tgreduceflag, tgrednodetype,
  tgtestflag, tgtcstacklist
} typesemanticdata;

typedef struct semanticrec {
  struct semanticrec *next;
  typesemanticdata tag;
  union {
    namelistrec *namelist;
    typelistrec *typelist;
    stentry *ttptr;
    symtblbucket *symptr;
    explistnode *explist;
    struct {
      int preclevel, column, line;
      opertype operation;
    } U5;
    symlistnode *symlist;
    node *nodeptr;
    int subgraphnum;
    boolean oldflag;
    reduceflag redflag;
    int rednodetype;
    testflagtype testflag;
    tcstacklistrec *tcstacklist;
  } UU;
} semanticrec;

/*--------------- DEBUGGER TYPES  -------------------------------------*/
typedef Char alpha[alphalength];

typedef struct stringrec {
  unsigned start : 13, length : 13, foo : 2;   /* kluge !!! */
} stringrec;

typedef Char linebuffer[maxlinelength + 1];

typedef struct inputbuffer {
  linebuffer buf;
  stryng inputfile;
  struct inputbuffer *next;
  Char *underline;
  unchar pointer, length;
  int lineno;
  unsigned modified : 1, lastline : 1;
} inputbuffer;

typedef struct rabrec {
  /* ReadAhead Buffer records */
  unsigned token : 7;
  struct rabrec *next;
} rabrec;

/* !! type for error correction phase */

typedef struct insertstringrec {
  int first : 11, last : 11, cost : 31;
} insertstringrec;

typedef struct prodrec {
  /* one production in the grammar */
  unsigned start : 12;   /* pointer into production space */
  int length : 12;   /* number of symbols in uction */
} prodrec;

typedef struct stagerec {
  /* insertion in progress */
  int cost : 31;
  char string[maxcorrection + 1];
  int length : 5;
} stagerec;

typedef struct ptabrec {
  Signed int term : 10;
  unsigned pact : 12;
  struct ptabrec *next;
} ptabrec;

typedef insertstringrec pretabarray[maxsym + 3];
/* ------------- types for handling stringspace ------------*/

typedef struct extstringspace {
  struct extstringspace *nextextent;
  Char string[maxstring + 1];
} extstringspace;

typedef union stringptrklugerec {
  /*allows us to put the size of
                                        stringspace into record for writing to file */
  int strlength;
  extstringspace *ptr;
} stringptrklugerec;

/*------------ types for message handler --------------*/
typedef enum {
  cmdunavailable, numtoobig, badrealnum, unclosedstring, overlap, opencomment,
  ctrlcharinstring, badsymbol, nomessage, nondeletable, ppromptmess,
  tooshallow, toohigh, cantputtree, copymess, oldfileexists, cantelide,
  notelided, lastcmd, prexpansion, cantundo, reallyzap, noundo, noredo,
  ttytype, notermfile, filemessage, nofilename, nosuchfile, nametoolong,
  wrotefilemessage, cantwrite, chngdquit, chngdread, reachedeof, reachedbof,
  invalidcmd, nonterminalrequired, nonexpandable, cantcontinue,
  nomoreexpansions, nondelterminal, terminalrequired, syntaxerror,
  deletedtoken, nosemanticroutine, headermess, inputmode, namenotfound,
  notreethere, nolasttree, insrtmess, deletemess, assumeno, execmess,
  noexecmess, nodefnofid, predefid, retrnfromdefn, elidedid
/* p2c: mono.bin.noblank.p, line 1257: Note:
 * Line breaker spent 5.3+0.56 seconds, 5000 tries on line 1212 [251] */
} messagecode;
/* *** types for scanner *** */
typedef enum {
  compareless, compareequal, comparegreater
} comparetype;

typedef enum {
  charerr
} errtype;
/* *** types for attributes *** */
/*----------types for pretty-print templates----------*/
/*type for templates */
/*-------- Super Rec, able initialize large tables in a single read */

typedef struct tablerec {
  long digits[9];
  long idcharset[9];
  stryng resword[maxsym + 3];
} tablerec;

typedef struct includestackrec {
  stryng fname;
  rabrec *readaheadbuf;
  int linenumber;
  struct includestackrec *next;
} includestackrec;


/* Include global variables here */
/*#TITLE  IF1INIT VARIABLES         IFNAug83   Standard IF1 Variables*/
extern printable nodename[ifmaxnode + 1];
extern printable errvname[ifmaxerror + 1];
extern printable typename[ifmaxtype + 1];
/* extern printable basename[ifmaxbasic + 1]; */
extern printable graphname[19];
extern ndclasstype nodeclass[ifmaxnode + 1];   /* dlz - 8/87 */
extern FILE *source, *diag;
extern infilelist infile;
extern parrec *outfile;
/* these vars are needed by module using the graph */
extern stentry *typetable[entrymax];
extern int univmodulecnt, univnodecnt, univedgecnt;
extern linkrec *funclist;
extern node *firstfunction, *module;
extern stryng stamp['[' - 'A'];
extern long stampset[9];
extern semanticrec *topsemstk;   /* Top of Semantic Stack */
extern namelistrec *definelist;
/* hold the list of function names
                                     listed in the  stmt that have
                                     not been declared yet.  Should be
                                     empty at the end of the module.  */
extern int boostfac;
/* Used to help determine which binary
                                     operation has higher precedence when
                                     () are involved.*/
extern long binaryops;
/* Set used to distiguish binary
                                     operators from others*/
extern long arithtypes;   /* holds TT labels for int,real,doub */
extern long algebtypes;   /* holds TT labels for int,real,doub,bool */
extern long streamorarray;
/* holds TT code for stream and
                                                                       array types*/
extern namelistrec *includelist;
/* holds stack of names of files that have
                                   been included.  List is checked to avoid
                                   recursive includes*/
extern boolean typedefsfinished;
/* Flag to say type def sections is done,
                                   used to know whether to add each piece
                                   of a type into the table or just
                                   smash it in when it is complete*/
/*------------------- SYMBOL TABLE VARS ----------------*/
extern treestackobj *currentlevel;   /* pointer to current Lex Level */
/*------------------- DEBUGGER VARS -------------------*/
extern long breakpointset[maxsemanticnumber / 32 + 2];
    /* Set of semantic routines to break on */
extern int currentstep, stepcount;
extern boolean semtrace;
/* Print out names of semantic routines
                                     as they are called it set*/
/*-------------------- COMMAND LINE PARAMS ------------*/
extern parrec *xrefparam;
/*--------------------- XREF --------------------------*/
extern xrefrec idunsorted[1000];
extern int idsorted[1000];
extern int idxrefmax;
extern inputbuffer *linebuf, *oldline;
extern unchar tokenstart;
extern int linenumber, tokenlineno;
extern boolean endfile;
extern int parsestack[maxstack + 1];
extern Char stringspace[maxstring + 1];
extern boolean epsprod[maxprod + 1];
extern int stringptr;
extern short stackptr;
extern short oldtoken;
extern boolean echoed;
extern ptabrec *table[maxsym + 3];
extern stryng symtab[maxsym + 3];
extern stringrec symkluge[maxsym + 3];
extern int prodspace[maxprodspace + 1];
/* all the productions. (strings of symbols) */
extern prodrec productions[maxprod + 1];   /* pointers into prodspace*/
extern short prodspceptr;
extern int numprods, numsymbols;
extern stryng inputtoken;
extern int toconsume, column;
/* !! variables for error correction */
extern int myinfinity, numterms;
extern rabrec *readaheadbuf;   /*head of buffer of inserted tokens*/
extern FILE *ptableout, *etableout;
/* index : intfile; */
extern FILE *inputfile;   /* file to be parsed */
extern boolean peeking;
extern int errorcount;
extern int deltriedhisto[maxhisto + 1];
    /* histograms of deletions and insertions */
extern int delmadehisto[maxhisto + 1], inshisto[maxhisto + 1];
extern int tokinserted, tokdeleted;
extern char insertspace[maxinsertspace + 1];
extern int deletecosts[maxterm + 1];
extern insertstringrec costtable[maxsym + 3];
extern pretabarray prefixtable;
extern int prefixindex[maxterm + 1];   /*sks*/
extern char curerrsym;   /* current terminal symbol resident in prefixtable*/
extern boolean errortables;
    /* whether error correction tables were created */
extern int insertptr, savinsertptr, numdeltried;
    /* number of deletions considered */
extern FILE *reservedwords;
/*---------- Current State of the editor ----------*/
extern boolean withininputmode, quittingtime;   /* time to leave the editor */
extern int tokenposition;   /* character position within a token */
/*--------- variables for prettyprinting-------------*/
extern short tempptr;
extern boolean readingbinaryfile;   /* true only when reading a binary file */
extern unchar debugflags[32];
/*-------- variables for maintaining stringspace ---------*/
extern extstringspace *firststring;   /* pointer to the first extent */
extern extstringspace *curextent;   /* pointer to current Extent */
extern int curbase;
/* index of the first element
                                             of CurExtent */
/*--------- variables for the scanner ---------*/
extern short token;
extern int inttokenval;
extern double realtokenval;
extern int tokenstrlength, ordzero;
extern boolean eolnflag;
extern Char globalchar;
extern short savetoken;
extern Char newcommand;
/* digits          : set of char; */
/* idcharset       : set of char; */
/* resword         : array [ ResWordIndex ] of StringRec; */
/* following added by sks 83/1/31 */
extern int singlechar['\177' - ' '];   /* token numbers */
extern int reuseindex;
extern Char reusestring[maxbackup + 1];   /* pos 0 not used */
extern int statehistory[maxbackup + 1];
extern long badcharset[9];
/*------- Super Record, where most of the files are ----------*/
extern tablerec p;
extern FILE *listfile;
extern boolean listing, xref;
extern int rea;
extern Char debugchar;
extern boolean debugcg, debug, cortrace, firsttime, done, commentflag,
	       includeflag;
extern includestackrec *topincludestk;
extern stryng curinputfile, newfname;
extern int newlinenum;
extern parrec *listingparam, *debugparam, *pass1param;
extern int semerrcnt;
extern parrec *listfparam;
extern stryng directoryname, defaultdirectoryname, filename;
extern parrec *directoryparam, *paramlist;
extern boolean passerr;
extern stryng listfilename, stampstr, versionstr, versionnum;


/* Include all desired functions and procedures here */
/*#TITLE  STRUTL  ROUTINES        Jan83   String Handling Routines.*/
/* macro function IsDigit( C: char ): boolean */
/* macro function IsHexOnly( C: char ): boolean */
/* macro function IsLowerCase( C: char ): boolean */
/* macro function IsUpperCase( C: char ): boolean */
/* macro procedure MakeLowerCase( var C: char ) */
/* macro procedure MakeUpperCase( var C: char ) */
/* macro function StringLength( S: Stryng ): integer */
/* macro function IsEmptyString( S: Stryng ): boolean */
extern Char stringchar();
extern boolean readstringok();
extern Char uppercase();
extern Char lowercase();
extern boolean matchstrings();
extern boolean equalstrings();
extern boolean equivstrings();
extern long stringnumber();
extern boolean openread();
extern boolean openintread();
extern boolean canread();
extern boolean openwrite();
extern boolean canwrite();
extern parrec *defineparameter();
extern boolean boolparvalue();
extern long intparvalue();
extern Char gettoken();
extern boolean parsecommandline();
extern boolean openinputfile();
extern boolean openoutputfile();
extern boolean openlistingfile();
extern boolean openuncompfile();
extern stentry *newtypealloc();
extern port *newedgealloc();
extern node *newnodealloc();
extern boolean stampisset();
extern boolean stampismissing();
extern boolean isboundaryedge();
extern node *producernodeofedge();
extern node *tonamedgraph();
extern node *tolastnodeingraph();
extern node *toprevnode();
extern node *tonextchildgraph();
extern node *tochildgraph();
extern node *toenclosingcompound();
extern node *toenclosingfunction();
extern node *toinitgraph();
extern node *tobodygraph();
extern node *totestgraph();
extern node *toreturnsgraph();
extern node *toiterbodygraph();
extern node *toselectorgraph();
extern node *toalternativegraph();
extern node *toforallgeneratorgraph();
extern node *toforallbodygraph();
extern node *toforallreturnsgraph();
extern node *totagcasesubgraph();
extern port *getinputedge();
extern port *getoutputedge();
extern port *nextoutputedgesameport();
extern port *nextinputedgenextport();
extern port *nextoutputedgenextport();
extern port *inputedgegeport();
extern port *outputedgegeport();
extern node *getnodewithlabel();
extern node *getnodewithid();
extern node *tonextfunction();
extern stentry *getnextfunarg();
extern stentry *getnextfunres();
extern stentry *typeofnthfunarg();
extern stentry *typeofnthfunres();
extern stentry *typeofnthfield();
extern stentry *typeofnthtag();
extern stentry *searchtypetable();
extern stentry *getbasictype();
extern stentry *getwildtype();
extern stentry *getconstructortype();
extern stentry *addtotypetable();
extern stentry *makefuntypefromgraph();
extern node *tocalledgraph();
extern ifgraphtype graphkind();
extern char reductionop();
extern node *createsimplenode();
extern node *createcompoundnode();
extern port *insertedge();
extern port *insertliteral();
extern node *copynode();
extern node *copycompound();
extern node *copygraph();
extern node *graphwalk();
extern node *newnodeptr();
extern port *newedgeptr();
extern linkrec *newlinkageptr();
extern graph *newgraphptr();
extern edgelisthead *newheadptr();
extern edgelist *newedgelistptr();
extern refrec *newreferencesptr();
extern inforecord *newinfoptr();
extern explistnode *newexplistptr();
extern tcstacklistrec *newtcstacklist();
extern errorrecord *newerrorptr();
extern symlistnode *newsymlistptr();
extern node *findparent();
extern semanticrec *popsemantic();
extern tornrec *linktorntype();
extern inforecord *getalit1();
extern node *buildcompound_();
extern node *buildsubgraph();
extern comparetype stringcompare();
extern port *orderolist();
extern port *orderilist();
extern boolean openlistfile();
extern symtblbucket *createbucket();
extern loopvarflags getloopflag();
extern symtblbucket *incurrentlevel();
extern symtblbucket *getsymtabentry();
extern edgelisthead *addnametolist();
extern edgelisthead *addtoklist();
extern edgelisthead *addtoforklist();
extern edgelisthead *addtoiklist();
extern symtblbucket *copysym();
extern port *emptyoldedge();
extern port *emptyedge();
extern symtblbucket *pullname();
extern inforecord *createerr();
extern boolean Reduce();
extern inforecord *createerrlit();
extern edgelisthead *findnameonlist();
extern assoclist *buildforinitasslist();
extern inforecord *removelast();
extern stentry *buildredtype();
extern stryng buildfunctname();
extern boolean checklastlevel();
extern stentry *errorstruct();
extern tcstacklistrec *findtag();
extern boolean debugger();
extern boolean announceaction();

extern void CANN_DumpSpecialPragmas();
extern int  CANN_GlueMode;

extern /* Static */ int tthwm;   /* symbol table high water mark */


/* CANN ********** */
#define MAX_BBLOCKS 10000
#define MAX_SBLOCKS 10000

union sblock {
  graph g;
  assoclist al;
  defrec    dr;
  reflist   rl;
  refrec    rr;
  edgelist  el;
  typelistrec tlr;
  explistnode eln;
  symlistnode sln;
  semanticrec sr;
  };

union bblock {
  node n;
  port p;
  stentry i;
  inforecord ir;
  symtblbucket sb;
  edgelisthead elh;
  namelistrec  nlr;
  parrec       pr;
  linkrec      lr;
  };

typedef union bblock BBLOCK, *PBBLOCK;
typedef union sblock SBLOCK, *PSBLOCK;

extern PBBLOCK MyBBlockAlloc();
extern PSBLOCK MySBlockAlloc();

extern FILE *CANN_source;
extern char CANN_source_file[];
