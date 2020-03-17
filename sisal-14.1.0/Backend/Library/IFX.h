
#ifndef IFX_H
#define IFX_H

/**************************************************************************/
/* FILE   **************           IFX.h           ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/**************************************************************************/

/* ------------------------------------------------------------ */
/* IFX.h - Applicative Language Intermediate Forms              */
/* ------------------------------------------------------------ */

#define IFX_LOADED

/* ------------------------------------------------------------ */
/* Node Definitions                                             */
/* ------------------------------------------------------------ */

#define IF_NODE_FIRST           0

/* COMPOUND */
#define IF_COMPOUND_FIRST       0
#define IFForall                0
#define IFSelect                1
#define IFTagCase               2
#define IFLoopA                 3
#define IFLoopB                 4

#define IFIfThenElse            5   /* IF90 COMPOUNDS */
#define IFIterate               6
#define IFWhileLoop             7
#define IFRepeatLoop            8
#define IFSeqForall             9
#define IFUReduce               10
#define IF_COMPOUND_LAST        10

/* SIMPLE */
#define IF_SIMPLE_FIRST         100
#define IFAAddH                 100
#define IFAAddL                 101
#define IFAAdjust               102 /* = AAdj */
#define IFABuild                103 /* = ABld */
#define IFACatenate             104 /* = ACat */
#define IFAElement              105 /* = AElm */
#define IFAFill                 106
#define IFAGather               107
#define IFAIsEmpty              108 /* = AEpty */
#define IFALimH                 109
#define IFALimL                 110
#define IFARemH                 111
#define IFARemL                 112
#define IFAReplace              113 /* = @ARepl */
#define IFAScatter              114 /* = AScat */
#define IFASetL                 115
#define IFASize                 116
#define IFAbs                   117
#define IFBindArguments         118 /* = BindA */
#define IFBool                  119
#define IFCall                  120
#define IFChar                  121
#define IFDiv                   122
#define IFDouble                123 /* = Dbl */
#define IFEqual                 124
#define IFExp                   125
#define IFFirstValue            126 /* = FstV */
#define IFFinalValue            127 /* = FnlV */
#define IFFloor                 128
#define IFInt                   129
#define IFIsError               130 /* = IsErr */
#define IFLess                  131
#define IFLessEqual             132 /* = LessE */
#define IFMax                   133
#define IFMin                   134
#define IFMinus                 135
#define IFMod                   136
#define IFNeg                   137
#define IFNoOp                  138
#define IFNot                   139
#define IFNotEqual              140 /* = NotE */
#define IFPlus                  141
#define IFRangeGenerate         142 /* = RgGen */
#define IFRBuild                143 /* = RBld */
#define IFRElements             144 /* = RElm */
#define IFRReplace              145 /* = RRepl */
#define IFRedLeft               146 /* = RedL */
#define IFRedRight              147 /* = RedR */
#define IFRedTree               148 /* = RedT */
#define IFReduce                149 /* = Red */
#define IFRestValues            150 /* = RestV */
#define IFSingle                151 /* = Sngle */
#define IFTimes                 152
#define IFTrunc                 153
#define IFPrefixSize            154
#define IFError                 155 /* IF1 Version 1.MY_DEFINITION         */
#define IFReplaceMulti          156 /* = ReplM */
#define IFConvert               157
#define IFCallForeign           158
#define IFAElementN             159
#define IFAElementP             160
#define IFAElementM             161

#define IFAAddLAT               170 /* = AddLAT */
#define IFAAddHAT               171 /* = AddHAT */
#define IFABufPartition         172 /* = BPart */
#define IFABuildAT              173 /* = BldAT */
#define IFABufScatter           174 /* = BScat */
#define IFACatenateAT           175 /* = CatAT */
#define IFAElementAT            176 /* = ElmAT */
#define IFAExtractAT            177 /* = AdjAT */
#define IFAFillAT               178 /* = FillAT */
#define IFAGatherAT             179 /* = GathAT */
#define IFARemHAT               180 /* = RemHAT */
#define IFARemLAT               181 /* = RemLAT */
#define IFAReplaceAT            182 /* = ReplAT */
#define IFArrayToBuf            183 /* = AToB */
#define IFASetLAT               184 /* = SetAt */
#define IFDefArrayBuf           185 /* = DefAB */
#define IFDefRecordBuf          186 /* = DefRB */
#define IFFinalValueAT          187 /* = FnlVAT */
#define IFMemAlloc              188 /* = MAlloc */
#define IFBufElements           189 /* = BElm */
#define IFRBuildAT              190 /* = RBldAT */
#define IFRecordToBuf           191 /* = RToB */
#define IFRElementsAT           192 /* = RElmAT */
#define IFReduceAT              193 /* = RedAT */
#define IFShiftBuffer           194 /* = ShiftB */
/* NEW AT-NODES NOT IN IF2 MANUAL */
#define IFScatterBufPartitions  195 /* = SBpart */
#define IFRedLeftAT             196 /* = RedLAT */
#define IFRedRightAT            197 /* = RedRAT */
#define IFRedTreeAT             198 /* = RedTAT */

#define RESERVED_NUMBER         199     /* DO NOT USE THIS NUMBER */

#define IFLoopPoolEnq           200
#define IFReadyListEnq          201
#define IFAssign                202 /* = Assgn */
#define IFGetArrayBase          203 /* = GABase */
#define IFOptAElement           204 /* = OptAElm */
#define IFDiv2                  205 /* = Div2 */
#define IFUTagTest              206
#define IFUBuild                207
#define IFUGetTag               208
#define IFUElement              209

#define IFOptLoopPoolEnq        210
#define IFBuildSlices           211

#define IFAIndexMinus           220
#define IFAIndexPlus            221

#define IFFirstSum              222
#define IFFirstMin              223
#define IFFirstMax              224
#define IFTri                   225
#define IFFirstAbsMin           226
#define IFFirstAbsMax           227

#define IFAStore                250     /* YANKED RETURN NODES (NONSTANDARD) */
#define IFLeast                 251
#define IFGreatest              252
#define IFProduct               253
#define IFSum                   254
#define IFGreat                 260
#define IFGreatEqual            261

#define IFBRBuild               262
#define IFBRElements            263
#define IFBRReplace             264

#define IFSaveCallParam         266
#define IFSaveSliceParam        267
#define IFBuildLoopSlices       268

#define IFOptAReplace           269

#define IFMemAllocDVI           299
#define IFMemAllocDV            300
#define IFAAddHATDVI            301
#define IFAAddHATDV             302
#define IFAAddLATDVI            303
#define IFAAddLATDV             304
#define IFACatenateATDVI        305
#define IFACatenateATDV         306
#define IFABuildATDVI           307
#define IFABuildATDV            308
#define IFReduceATDVI           309
#define IFReduceATDV            310
#define IFAGatherATDVI          311
#define IFAGatherATDV           312
#define IFOptNoOp               313
#define IFPeek                  314
#define IFRagged                315
#define IFPSAllocCond           316
#define IFPSAllocOne            317
#define IFPSAllocTwo            318
#define IFPSAllocSpare1         319
#define IFPSAllocSpare2         320

#define IFPSFreeOne             321
#define IFPSFreeTwo             322
#define IFPSFreeSpare1          323
#define IFPSFreeSpare2          324

#define IFPSManager             325
#define IFPSManagerSwap         326

#define IFPSScatter             327
#define IFPSMemAllocDVI         328

#define IFSpawn                 330
#define IFRat                   331
#define IFWat                   332

#define IFSStep                 333     /* Specialty Stream nodes */
#define IFSAddH                 334
#define IFSBuild                335
#define IFSCatenate             336
#define IFSGather               337
#define IFSIsEmpty              338
#define IFSScatter              339
#define IFSSize                 340
#define IFSPrefixSize           341

#define IFAReplaceRange         400     /* IF90 nodes */
#define IFASelectRange          401
#define IFAReplaceMulti         402
#define IFReduction             403
#define IFDot                   404
#define IFCross                 405
#define IFAssert                406
#define IFRequire               407
#define IF_SIMPLE_LAST          407

#define IF_GRAPH_FIRST          1000
#define IFSGraph                1000    /* IF1 GRAPH NODE TYPES (NONSTAND) */
#define IFLGraph                1001
#define IFIGraph                1002
#define IFXGraph                1003
#define IFLPGraph               1004
#define IFRLGraph               1005
#define IF_GRAPH_LAST           1005

#define IF_OTHER_FIRST          2000
#define IFCBegin                2000    /* NONSTANDARD GENERAL NODE TYPES */
#define IFCEnd                  2001
#define IFEdge                  2002
#define IFLiteral               2003
#define IF_OTHER_LAST           2003

#define IFUndefined             9999

#define IF_NODE_LAST            9999


#define IF_NODE_COMPOUND        0
#define IF_NODE_SIMPLE          1
#define IF_NODE_GRAPH           2
#define IF_NODE_OTHER           3

#define IF1SimpleNodes          (175)   /* NODE COUNTS FOR makenames.c CHECK */
#define IF1CompoundNodes        (11)
#define IF1GraphNodes           (6)
#define IF2AtNodes              (34)

/* ------------------------------------------------------------ */
/* Node Category Type Macros                                    */
/* ------------------------------------------------------------ */

#define IsAtNode(x)    ( ((x)->type >= IFAAddLAT) &&                        \
                         ((x)->type <= IFDiv2) )

#define IsGetArrayBase(x) ( (x)->type == IFGetArrayBase )

#define IsAssign(x)    ( (x)->type == IFAssign )
#define IsAAddHAT(x)   ( (x)->type == IFAAddHAT )
#define IsUndefined(x) ( (x)->type == IFUndefined )
#define IsOther(x)     ( (x)->type >= IFPeek && (x)->type <= IFPeek )
#define IsSimple(x)    ( (((x)->type >= IFAAddH)&&((x)->type < IFAAddLAT)) || \
                          ((x)->type == IFPeek ) )

#define IsAReplace(x)  ( ((x)->type == IFAReplace) ||                       \
                         ((x)->type == IFOptAReplace) )

#define IsABuildAT(x)  ( (x)->type == IFABuildAT )
#define IsAFillAT(x)   ( (x)->type == IFAFillAT  )
#define IsAAddHAT(x)   ( (x)->type == IFAAddHAT )
#define IsGraph(x)     ( (x)->type >= IF_GRAPH_FIRST && \
                         (x)->type <= IF_GRAPH_LAST )
#define IsIGraph(x)    ( (x)->type == IFIGraph )
#define IsLGraph(x)    ( (x)->type == IFLGraph )
#define IsSGraph(x)    ( (x)->type == IFSGraph )
#define IsXGraph(x)    ( (x)->type == IFXGraph )
#define IsLPGraph(x)   ( (x)->type == IFLPGraph )
#define IsSelect(x)    ( (x)->type == IFSelect )
#define IsError(x)     ( (x)->type == IFError )
#define IsBranch(x)    ( (x)->type == IFSelect || (x)->type == IFIfThenElse )
#define IsCompound(x)  ( (x)->type >= IF_COMPOUND_FIRST && \
                         (x)->type <= IF_COMPOUND_LAST  )
#define IsNoOp(x)      ( (x)->type == IFNoOp )
#define IsMemAlloc(x)  ( (x)->type == IFMemAlloc )

#define IsAlloc(x)     ( (x)->type == IFMemAlloc   || \
                         (x)->type == IFMemAllocDV || \
                         (x)->type == IFMemAllocDVI )

#define IsForall(x)    ( (x)->type == IFForall )
#define IsAFill(x)     ( (x)->type == IFAFill  )
#define IsInt(x)       ( (x)->type == IFInt    )
#define IsExp(x)       ( (x)->type == IFExp   )
#define IsPlus(x)      ( (x)->type == IFPlus   )
#define IsMax(x)       ( (x)->type == IFMax    )
#define IsLoopA(x)     ( (x)->type == IFLoopA  )
#define IsLoopB(x)     ( (x)->type == IFLoopB  )
#define IsRepeatLoop(x)( (x)->type == IFRepeatLoop )
#define IsALimL(x)     ( (x)->type == IFALimL  )
#define IsALimH(x)     ( (x)->type == IFALimH  )
#define IsABuild(x)    ( (x)->type == IFABuild )
#define IsNot(x)       ( (x)->type == IFNot    )
#define IsMinus(x)     ( (x)->type == IFMinus  )
#define IsNeg(x)       ( (x)->type == IFNeg    )
#define IsLess(x)      ( (x)->type == IFLess   )
#define IsLessEqual(x) ( (x)->type == IFLessEqual )
#define IsCall(x)      ( (x)->type == IFCall )
#define IsPeek(x)      ( (x)->type == IFPeek   )
#define IsASize(x)     ( (x)->type == IFASize )
#define IsASetL(x)     ( (x)->type == IFASetL  )
#define IsGather(x)    ( (x)->type == IFAGather )
#define IsAGather(x)   ( (x)->type == IFAGather )
#define IsAScatter(x)  ( (x)->type == IFAScatter )
#define IsAElement(x)  ( (x)->type == IFAElement )
#define IsRElements(x) ( (x)->type == IFRElements )
#define IsTagCase(x)   ( (x)->type == IFTagCase )
#define IsRBuild(x)    ( (x)->type == IFRBuild )
#define IsLoop(x)      ( ((x)->type == IFLoopA) || ((x)->type == IFLoopB) )
#define IsAnyLoop(x)    (((x)->type==IFForall) ||\
                         ((x)->type==IFLoopA) || ((x)->type==IFLoopB) ||\
                         ((x)->type==IFIterate) || ((x)->type==IFWhileLoop) ||\
                         ((x)->type==IFRepeatLoop) || ((x)->type==IFSeqForall) )

#define IsLiteral(x)   ( (x)->type == IFLiteral )
#define IsEdge(x)      ( (x)->type == IFEdge )
#define IsGenerate(x)  ( ((x)->type == IFAScatter)     ||              \
                         ((x)->type == IFRangeGenerate) )
#define IsReduceAT(x)  ( ((x)->type == IFReduceAT)     ||              \
                         ((x)->type == IFRedLeftAT)    ||              \
                         ((x)->type == IFRedRightAT)   ||              \
                         ((x)->type == IFRedTreeAT)    )

#define IsReduction(x) ( ((x)->type == IFReduce)     ||                \
                         ((x)->type == IFRedLeft)    ||                \
                         ((x)->type == IFRedRight)   ||                \
                         ((x)->type == IFRedTree)    )
#define IsReturn(x)    ( ((x)->type == IFAGather)    ||                \
                         ((x)->type == IFReduce)     ||                \
                         ((x)->type == IFRedLeft)    ||                \
                         ((x)->type == IFRedRight)   ||                \
                         ((x)->type == IFRedTree)    ||                \
                         ((x)->type == IFFirstValue) ||                \
                         ((x)->type == IFFinalValue) ||                \
                         ((x)->type == IFRestValues) ||                \
                         ((x)->type == IFUReduce) )

#define IsRangeGenerate(x)        ( (x)->type == IFRangeGenerate )
#define IsFinalValue(x)           ( (x)->type == IFFinalValue  )
#define IsFinalValueAT(x)         ( (x)->type == IFFinalValueAT)
#define IsDefArrayBuf(x)          ( (x)->type == IFDefArrayBuf )
#define IsScatterBufPartitions(x) ( (x)->type == IFScatterBufPartitions )
#define IsOptAElement(x)          ( (x)->type == IFOptAElement )

/* ------------------------------------------------------------ */
/* Miscellaneous Definitions                                    */
/* ------------------------------------------------------------ */

#define REDUCE_SUM        'S'      /* FIRST CHARACTER OF REDUCTION FUNCTIONS  */
#define REDUCE_PRODUCT    'P'
#define REDUCE_LEAST      'L'
#define REDUCE_GREATEST   'G'
#define REDUCE_CATENATE   'C'
#define REDUCE_USER       '_'      /* USER-DEFINED */

#define RETURNS_CODE_FIRST      0
#define RETURNS_USER            0       /* USER-DEFINED */ 
#define RETURNS_SUM             1
#define RETURNS_PRODUCT         2
#define RETURNS_LEAST           3
#define RETURNS_GREATEST        4
#define RETURNS_CATENATE        5
#define RETURNS_ARRAY           6
#define RETURNS_STREAM          7
#define RETURNS_HISTOGRAM       8
#define RETURNS_VALUE           9
#define RETURNS_CODE_LAST       9

#define TYPE_CODE_FIRST   0
#define IF_ARRAY          0            /* IF SYMBOL TABLE TYPES */
#define IF_BASIC          1
#define IF_FIELD          2
#define IF_FUNCTION       3
#define IF_MULTIPLE       4
#define IF_RECORD         5
#define IF_STREAM         6
#define IF_TAG            7
#define IF_TUPLE          8
#define IF_UNION          9
#define IF_UNKNOWN        10
#define IF_BUFFER         11
#define IF_SET            12
#define IF_REDUCTION      13
#define IF_FOREIGN        14
#define TYPE_CODE_LAST    IF_FOREIGN

#define BASE_CODE_FIRST   (200 + TYPE_CODE_LAST)
#define IF_BOOL           ( 0 + BASE_CODE_FIRST)        /* boolean */
#define IF_CHAR           ( 1 + BASE_CODE_FIRST)        /* character */
#define IF_DOUBLE         ( 2 + BASE_CODE_FIRST)        /* double */
#define IF_INTEGER        ( 3 + BASE_CODE_FIRST)        /* integer */
#define IF_NULL           ( 4 + BASE_CODE_FIRST)        /* null */
#define IF_REAL           ( 5 + BASE_CODE_FIRST)        /* real */
#define IF_NONTYPE        ( 6 + BASE_CODE_FIRST)        /* no type */

#define IF_BRECORD        ( 7 + BASE_CODE_FIRST)        /* basic record */

#define IF_PTR_DOUBLE     ( 8 + BASE_CODE_FIRST)        /* double* */
#define IF_PTR_INTEGER    ( 9 + BASE_CODE_FIRST)        /* integer* */
#define IF_PTR_REAL       (10 + BASE_CODE_FIRST)        /* real* */
#define IF_PTR            (11 + BASE_CODE_FIRST)        /* void* */
#define BASE_CODE_LAST    IF_PTR

#define IsBasic(x)          ( (x)->type >= BASE_CODE_FIRST )

#define IsPointer(x)        ( (x)->type >= IF_PTR_DOUBLE &&  \
                              (x)->type <= IF_PTR )

#define IsBRecord(x)        ( (x)->type == IF_BRECORD )

#define IsBoolean(x)        ( (x)->type == IF_BOOL  )
#define IsChar(x)           ( (x)->type == IF_CHAR )
#define IsDouble(x)         ( (x)->type == IF_DOUBLE  )
#define IsInteger(x)        ( (x)->type == IF_INTEGER )
#define IsReal(x)           ( (x)->type == IF_REAL    )

#define IsFunction(x)       ( (x)->type == IF_FUNCTION )
#define IsMultiple(x)       ( (x)->type == IF_MULTIPLE )
#define IsRecord(x)         ( (x)->type == IF_RECORD )
#define IsUnion(x)          ( (x)->type == IF_UNION )
#define IsStream(x)         ( (x)->type == IF_STREAM )

#define IsArray(x)          ( (x)->type == IF_ARRAY )
#define IsArrayBase(x)      ( ((x)->type >= IF_PTR_DOUBLE) && \
                              ((x)->type <= IF_PTR)         )

#define IsBuffer(x)         ( (x)->type == IF_BUFFER )

#define IsArithmetic(x)     ( ((x)->type == IF_INTEGER) ||    \
                              ((x)->type == IF_DOUBLE)  ||    \
                              ((x)->type == IF_REAL)     )

#define IsArithmeticPtr(x)  ( (x)->type == IF_PTR_INTEGER || \
                              (x)->type == IF_PTR_DOUBLE  || \
                              (x)->type == IF_PTR_REAL    )

#define ERROR_CONSTANT    "error"
#define GRAPH_LABEL       0
#define CONST_PORT        -1

#define FOR_C             0
#define FOR_FORTRAN       1
#define FOR_SISAL         2

#define DFORDERED         'D'   /* LLNL STAMPS */
#define IF1CHECK          'C'
#define FRONTEND          'F'
#define OFFSETS           'O'
#define LOOPINVARS        'L'
#define COMMONSUBS        'C'
#define CONSTANT          'G'
#define PDBASE            'V'
#define MDBASE            'W'
#define PNODBASE          'X'
#define MNODBASE          'Y'

#define TIMECHECK         'T'   /* New Stamps */

#define IF1OPTIMIZED      'I'   /* CSU STAMPS */
#define MONOLITH          'M'
#define NORMALIZED        'N'
#define BUILDINPLACE      'B'
#define UPDATEINPLACE     'U'
#define UNIVSTROWNER      'S'
#define PARTITIONED       'P'
#define CYCLE             'C'
#define OPTPARTITIONED    'Z'
#define QMODE             'Q'


typedef struct node       NODE,  *PNODE, **PPNODE;
#define NULL_NODE ((PNODE)NULL)
typedef struct info       INFO,  *PINFO;
#define NULL_INFO ((PINFO)NULL)
typedef struct prags      PRAGS;
typedef struct edge       NAME,  *PNAME;
typedef struct edge       EDGE,  *PEDGE;
#define NULL_EDGE ((PEDGE)NULL)
typedef struct alist      ALIST, *PALIST;
typedef struct dope       DOPE,  *PDOPE;
typedef struct temp       TEMP,  *PTEMP;
typedef struct ade        ADE,   *PADE;
typedef struct call       CALL,  *PCALL;
#define NULL_CALL ((PCALL)NULL)
typedef struct set        SET,   *PSET;
#define NULL_SET ((PSET)NULL)
typedef struct mdata      MDATA, *PMDATA;

/* ------------------------------------------------------------ */
/* These types are used in building and accessing IF1 archive   */
/* files.                                                       */
/* ------------------------------------------------------------ */
typedef struct namelink {
  char                  *name;
  struct namelink       *next;
} namelink;

typedef struct filenode {
  char                  *file;
  namelink              *have;
  namelink              *need;
  struct filenode       *next;
} filenode;

#define MarkAsBuiltInplace(x) { (x)->mark = 'P'; }



#define SIZE  int     lsize;                           /* LEVEL OF SIZE */ \
              int     csize;                           /* CONSTANT SIZE */ \
              PNODE   esize                            /* SIZE SUBGRAPH */

#define UNKNOWN_CSIZE    -1

#define InitSizeInfo(x)  (x)->lsize = 0; (x)->csize = UNKNOWN_CSIZE;       \
                         (x)->esize = NULL

#define IsSizeUnknown(x) (((x)->csize==UNKNOWN_CSIZE) && ((x)->esize == NULL))
#define IsConstSize(x)   ( (x)->csize != UNKNOWN_CSIZE)

#define PRAGMAS unsigned pmark   : 1;  /* %mk=PRrODLMCNBWF */                 \
                unsigned rmark1  : 2;                                         \
                unsigned omark1  : 1;                                         \
                unsigned rmark2  : 2;                                         \
                unsigned omark2  : 1;                                         \
                unsigned dmark   : 1;                                         \
                unsigned lmark   : 1;                                         \
                unsigned emark   : 1;                                         \
                unsigned imark   : 1;                                         \
                unsigned cmark   : 1;                                         \
                unsigned fmark   : 1;                                         \
                unsigned nmark   : 1;                                         \
                unsigned bmark   : 1;                                         \
                unsigned time    : 1;                                         \
                unsigned trace   : 1;                                         \
                unsigned flp     : 1;                                         \
                unsigned wmark   : 1;                                         \
                unsigned umark   : 1;                                         \
                unsigned smark   : 1;                                         \
                unsigned vmark   : 1;                                         \
                unsigned Pmark   : 1;                                         \
                unsigned Cmark   : 1;                                         \
                unsigned xmark   : 1;                                         \
                unsigned Fmark   : 1;                                         \
                                                                              \
                unsigned rstable : 1;  /* ARE THE R AND O MARKS STABLE?   */  \
                unsigned ostable : 1;                                         \
                                                                              \
                unsigned print : 1;                                        \
                unsigned lazy : 1;                                         \
                                                                           \
                short    sr;           /* PRODUCER REFERENCE COUNT VALUES */  \
                short    pm;                                                  \
                short    pl;                                                  \
                                                                              \
                short    cm;           /* CONSUMER REFERENCE COUNT VALUE  */  \
                                                                              \
                double   ccost;        /* CALCULATED EXECUTION COST       */  \
                char     mark;         /* FUNCTION DOMAIN TYPE            */  \
                                                                              \
                int      ID;           /* Unique Identifier for this node */  \
                char     *reason1;     /* REASON FOR ! Concurrent */          \
                char     *reason2;     /* REASON FOR ! Vector */              \
                char     *MinSlice;    /* Min number of iters/slice */        \
                int      ThinCopy;     /* Thin copy from If1opt/if1split */   \
                char     Style;        /* Parallelism style */                \
                char     *LoopSlice;   /* Loop Slice formula */               \
                                                                              \
                char     *name;        /* %na=source_data_name            */  \
                char     *file;        /* %fn=source_file_name            */  \
                char     *funct;       /* %sf=source_function_name        */  \
                int      norm;         /* normalization                   */  \
                int       line         /* %sl=source_line_number          */

#define CopyVitals(x,y)         \
{                               \
  (y)->line     = (x)->line;    \
  (y)->funct    = (x)->funct;   \
  (y)->file     = (x)->file;    \
}

#define StandardPragmas(x) \
{ \
  (x)->pmark    = FALSE;        \
  (x)->rmark1   = NOMARK;       \
  (x)->Pmark    = FALSE;        \
  (x)->Cmark    = FALSE;        \
  (x)->xmark    = FALSE;        \
  (x)->Fmark    = FALSE;        \
  (x)->omark1   = FALSE;        \
  (x)->rmark2   = NOMARK;       \
  (x)->omark2   = FALSE;        \
  (x)->dmark    = FALSE;        \
  (x)->lmark    = FALSE;        \
  (x)->emark    = FALSE;        \
  (x)->imark    = FALSE;        \
  (x)->mark     = '\0';         \
  (x)->cmark    = FALSE;        \
  (x)->fmark    = FALSE;        \
  (x)->nmark    = FALSE;        \
  (x)->umark    = FALSE;        \
  (x)->bmark    = FALSE;        \
  (x)->time     = FALSE;        \
  (x)->trace    = FALSE;        \
  (x)->flp      = FALSE;        \
  (x)->wmark    = FALSE;        \
  (x)->rstable  = TRUE;         \
  (x)->ostable  = TRUE;         \
  (x)->vmark    = FALSE;        \
  (x)->smark    = FALSE;        \
  (x)->sr       = 0;            \
  (x)->pm       = 0;            \
  (x)->pl       = 0;            \
  (x)->cm       = 0;            \
  (x)->name     = NULL;         \
  (x)->file     = NULL;         \
  (x)->funct    = NULL;         \
  (x)->ccost    = 0.0;          \
  (x)->print    = FALSE;        \
  (x)->lazy     = FALSE;        \
  (x)->ID       = 0;            \
  (x)->reason1  = NULL;         \
  (x)->reason2  = NULL;         \
  (x)->MinSlice = NULL;         \
  (x)->ThinCopy = FALSE;        \
  (x)->line     = 0;            \
  (x)->Style    = '\0';         \
  (x)->LoopSlice= NULL;         \
  (x)->norm     = 0;            \
}

#define CopyPragmas(y,x) \
{ \
  (x)->pmark    = (y)->pmark;   \
  (x)->rmark1   = (y)->rmark1;  \
  (x)->Pmark    = (y)->Pmark;   \
  (x)->Cmark    = (y)->Cmark;   \
  (x)->xmark    = (y)->xmark;   \
  (x)->Fmark    = (y)->Fmark;   \
  (x)->omark1   = (y)->omark1;  \
  (x)->rmark2   = (y)->rmark2;  \
  (x)->omark2   = (y)->omark2;  \
  (x)->dmark    = (y)->dmark;   \
  (x)->lmark    = (y)->lmark;   \
  (x)->emark    = (y)->emark;   \
  (x)->imark    = (y)->imark;   \
  (x)->mark     = (y)->mark;    \
  (x)->cmark    = (y)->cmark;   \
  (x)->fmark    = (y)->fmark;   \
  (x)->nmark    = (y)->nmark;   \
  (x)->umark    = (y)->umark;   \
  (x)->bmark    = (y)->bmark;   \
  (x)->time     = (y)->time;    \
  (x)->trace    = (y)->trace;   \
  (x)->flp      = (y)->flp;     \
  (x)->wmark    = (y)->wmark;   \
  (x)->rstable  = (y)->rstable; \
  (x)->ostable  = (y)->ostable; \
  (x)->vmark    = (y)->vmark;   \
  (x)->smark    = (y)->smark;   \
  (x)->sr       = (y)->sr;      \
  (x)->pm       = (y)->pm;      \
  (x)->pl       = (y)->pl;      \
  (x)->cm       = (y)->cm;      \
  (x)->name     = (y)->name;    \
  (x)->file     = (y)->file;    \
  (x)->funct    = (y)->funct;   \
  (x)->ccost    = (y)->ccost;   \
  (x)->print    = (y)->print;   \
  (x)->lazy     = (y)->lazy;    \
  (x)->ID       = (y)->ID;      \
  (x)->reason1  = (y)->reason1; \
  (x)->reason2  = (y)->reason2; \
  (x)->MinSlice = (y)->MinSlice;\
  (x)->ThinCopy = (y)->ThinCopy;\
  (x)->line     = (y)->line;    \
  (x)->Style    = (y)->Style;   \
  (x)->LoopSlice= (y)->LoopSlice;\
  (x)->norm     = (y)->norm;    \
}

/**************************************************************************/
/* MACRO  **************        DYNMACROS          ************************/
/**************************************************************************/
/* PURPOSE: USE A BUFFER dyn_ptr OFF THE STACK dyn_buf WITH INITIAL SIZE  */
/*          count AND LENGTH dyn_len AND TRUE BUFFER LENGTH dyn_count     */
/*          HOLDING ELEMENTS OF TYPE dyn_type.  BUT PUT ON HEAP IF IT     */
/*          GROWS TO LARGE WITH DYNEXPAND CHECK.  FREE AFTER DONE WITH    */
/*          DYNFREE.                                                      */  
/**************************************************************************/

#define DYNDECLARE(dyn_ptr, dyn_buf, dyn_len, dyn_count, dyn_type, count) \
static dyn_type dyn_buf[count]; \
static dyn_type  *dyn_ptr = dyn_buf; \
static int dyn_len = 0; \
static int dyn_count = count

#define DYNFREE(dyn_ptr, dyn_buf, dyn_len, dyn_count, dyn_type, new_ptr) \
{ if (dyn_ptr!=dyn_buf) (void)free(dyn_ptr); dyn_ptr = new_ptr; }

#define DYNEXPAND(dyn_ptr, dyn_buf, dyn_len, dyn_count, dyn_type, max_len) \
{ if (max_len>dyn_count) { \
      int _temp_count = max_len>=(2*dyn_count) ? max_len : (2*dyn_count); \
      dyn_type *_temp_ptr = (dyn_type*)malloc(_temp_count*sizeof(dyn_type)); \
      memcpy(_temp_ptr, dyn_ptr, dyn_count*sizeof(dyn_type)); \
      dyn_count = _temp_count; \
      DYNFREE(dyn_ptr, dyn_buf, dyn_len, dyn_count, dyn_type, _temp_ptr); \
  } \
}

/**************************************************************************/
/* MACRO  **************        WritePragmas       ************************/
/**************************************************************************/
/* PURPOSE: WRITE NAME, MARK, REFERENCE COUNT, FILE, FUNCTION, AND SOURCE */
/*          LINE PRAGMAS, FOLLOWED BY EOLN, TO output.                    */
/**************************************************************************/
  /* If you add pragmas, please add them to the pretty printer */
#define WritePragmas(x) \
{ \
  if ( (x)->name )      FPRINTF( output, " %%na=%s", (x)->name ); \
  if ( (x)->file )      FPRINTF( output, " %%sf=%s", (x)->file ); \
  if ( (x)->funct )     FPRINTF( output, " %%fn=%s", (x)->funct ); \
  if ( (x)->line > 0 )  FPRINTF( output, " %%sl=%d", (x)->line ); \
  if ( (x)->sr > 0 )    FPRINTF( output, " %%sr=%d", (x)->sr ); \
  if ( (x)->pm > 0 )    FPRINTF( output, " %%pm=%d", (x)->pm ); \
  if ( (x)->pl > 0 )    FPRINTF( output, " %%pl=%d", (x)->pl ); \
  if ( (x)->cm != 0 )   FPRINTF( output, " %%cm=%d", (x)->cm ); \
  if ( (x)->ccost > 0.0 ) FPRINTF( output, " %%cc=%g", (x)->ccost ); \
  if ( (x)->MinSlice )  FPRINTF( output, " %%MS=\"%s\"",(x)->MinSlice ); \
  if ( (x)->mark )      FPRINTF( output, " %%mk=%c", (x)->mark ); \
  if ( (x)->imark )     FPRINTF( output, " %%mk=I" ); \
  if ( (x)->pmark )     FPRINTF( output, " %%mk=P" ); \
  if ( (x)->rmark1 == RMARK ) FPRINTF( output, " %%mk=R" ); \
  if ( (x)->rmark1 == rMARK ) FPRINTF( output, " %%mk=r" ); \
  if ( (x)->omark1 )    FPRINTF( output, " %%mk=O" ); \
  if ( (x)->dmark )     FPRINTF( output, " %%mk=D" ); \
  if ( (x)->lmark )     FPRINTF( output, " %%mk=L" ); \
  if ( (x)->emark )     FPRINTF( output, " %%mk=E" ); \
  if ( (x)->fmark )     FPRINTF( output, " %%mk=F" ); \
  if ( (x)->cmark )     FPRINTF( output, " %%mk=C" ); \
  if ( (x)->nmark )     FPRINTF( output, " %%mk=N" ); \
  if ( (x)->bmark )     FPRINTF( output, " %%mk=B" ); \
  if ( (x)->umark )     FPRINTF( output, " %%mk=@" ); \
  if ( (x)->wmark )     FPRINTF( output, " %%mk=W" ); \
  if ( (x)->smark )     FPRINTF( output, " %%mk=S" ); \
  if ( (x)->vmark )     FPRINTF( output, " %%mk=V" ); \
  if ( (x)->Pmark )     FPRINTF( output, " %%mk=p" ); \
  if ( (x)->Fmark )     FPRINTF( output, " %%mk=Y" ); \
  if ( (x)->ID    )     FPRINTF( output, " %%ID=%d",(x)->ID); \
  if ( (x)->lazy )      FPRINTF( output, " %%LZ"); \
  if ( (x)->ThinCopy )  FPRINTF( output, " %%TC"); \
  if ( (x)->Style )     FPRINTF( output, " %%ST=%c",(x)->Style); \
  if ( (x)->LoopSlice ) FPRINTF( output, " %%LS=\"%s\"",(x)->LoopSlice); \
  if ( (x)->temp && (x)->temp->name ) { \
    FPRINTF( output, " %%tn=\"" ); \
    if ( !IsFunction( (x)->temp->info ) ) \
      FPRINTF( output, "%s\"", (x)->temp->name ); \
    else if ( strcmp( (x)->temp->name, "args" ) == 0 ) \
      FPRINTF( output, "((%s*)%s)->In%d\"", \
        (x)->temp->info->sname, (x)->temp->name, (x)->temp->fld ); \
    else \
      FPRINTF( output, "((%s*)%s)->Out%d\"", \
        (x)->temp->info->sname, (x)->temp->name, (x)->temp->fld ); } \
  (void)fputc('\n', output); \
}

struct node {
  int    if1line;               /* LINE NUMBER OF NODE IN IF1 FILE */

  int    type;                  /* IF NODE TYPE */
  int    label;                 /* UNIQUE (IN SCOPE) NODE IDENTIFIER */

  char  *CoNsT;                 /* LITERAL CONSTANT OR FUNCTION NAME */
  PEDGE  exp;                   /* EXPORTED (GENERATED) VALUES */
  PEDGE  imp;                   /* IMPORTED (ARGUMENT)  VALUES */

  int    snode;                 /* EDGE AND LITERAL SOURCE AND DEST LABELS */
  int    eport;
  int    dnode;
  int    iport;
  PNODE  copy;                  /* POINTS TO A COPY OF THIS NODE */

  PADE   aexp;                  /* EXPORTED ADES */
  PADE   aimp;                  /* IMPORTED ADES */

  PNODE  gpred;                 /* DOUBLE LINK LIST POINTERS (2 SETS) */
  PNODE  gsucc;                 /* n FOR NODE LIST, g FOR GRAPH LIST */
  PNODE  npred;
  PNODE  nsucc;

  PNODE  usucc;                 /* UTILITY NODE LIST NODE SUCCESSOR */

  unsigned checked  : 1;        /* HAS THIS NODE BEEN CHECKED DURING */

  short  cnum;                  /* NUMBER OF CALL REFERENCES AND SUM */
  int     level;                /* LEVEL OF THIS NODE */
  PPNODE  lstack;               /* LEVEL STACK FOR THIS NODE */

  short  pbusy;                 /* NUMBER OF BUSY PROCESSORS */

  unsigned visited  : 1;        /* HAS THIS FUNCTION BEEN VISITED DURING */
  unsigned sorted   : 1;        /* MARK PROPAGATION, AND IS IT IN THE */
  /* SORTED FUNCTION LIST?                 */
  unsigned executed : 1;        /* HAS THE NODE BEEN EXECUTED? */

  PRAGMAS;

  PINFO  info;                  /* EDGE, LITERAL OR GRAPH SYMBOL TABLE ENTRY */
  PNODE  next;                  /* NEXT NODE IN THE NODE LIST */
  int     color;                /* EXPRESSION NODE'S JERSEY COLOR */

  char   *gname;
  PALIST  alst;
  int    *flps;
  int     scnt;

  SIZE;                         /* NUMBER OF ITERATIONS INFORMATION */

  int     size;
  int     sdbx;

  PTEMP   temp;                 /* TEMPORARY STORAGE */
};

struct info {
  int    if1line;               /* LINE NUMBER OF INFO IN IF1 FILE */

  int   type;                   /* IF SYMBOL TABLE TYPE */
  int   label;                  /* UNIQUE SYMBOL IDENTIFIER */
  int   eid;                    /* EQUIVALENCE CLASS IDENTIFIER */

  unsigned  touch1 : 1;         /* UTILITY FLAGS */
  unsigned  touch2 : 1;
  unsigned  touch3 : 1;
  unsigned  touch4 : 1;         /*  *** INTERFACE READ-WRITE ROUTINES */
  unsigned  touch5 : 1;
  unsigned  touch6 : 1;
  unsigned  LibNames : 1;       /* Using library name fields */

  int   touchflag;              /* UTILITY FIELD */

  PRAGMAS;

  PINFO info1;                  /* UTILITY POINTERS */
  PINFO info2;

  PINFO  next;                  /* NEXT INFO NODE IN SYMBOL TABLE */

  PINFO  mnext;                 /* NEXT MEMBER OF EQUIVALENCE CLASS */
  PINFO  fmem;                  /* FIRST MEMBER OF EQUIVALENCE CLASS */

  char *tname;                  /* TYPE DECLARATION NAME               */
  char *sname;                  /* STRUCTURE NAME                      */
  char *rname;                  /* READ ROUTINE NAME                   */
  char *wname;                  /* WRITE ROUTINE NAME                  */
  char *fname1;                 /* FIRST FREE STORAGE ROUTINE          */
  char *fname2;                 /* SECOND FREE STORAGE ROUTINE         */
  char *cname;                  /* UNIQUE PART OF COPY ROUTINE NAME    */

  PTEMP temp;                   /* TEMPORARY STORAGE */
};

struct prags {
    PRAGMAS;
    };

struct edge {
  int    if1line;               /* LINE NUMBER OF EDGE IN IF1 FILE     */

  int   iport;                  /* IMPORT PORT IDENTIFIER              */
  int   eport;                  /* EXPORT PORT IDENTIFIER              */

  PNODE  node;                  /* GLOBAL GRAPH NODE                   */
  PNODE dst;                    /* DESTINATION NODE                    */
  PNODE src;                    /* SOURCE NODE                         */

  PINFO  info;                  /* SYMBOL TABLE ENTRY FOR THE FUNCTION */
  PINFO  Xinfo;                 /* Value assigned by prototype interpreter */
  PNAME  next;                  /* NEXT FUNCTION NAME IN NAME LIST     */
  PEDGE usucc;                  /* UTILITY EDGE LIST EDGE SUCCESSOR    */

  PRAGMAS;

  PEDGE ipred;                  /* IMPORT PREDECESSOR                  */
  PEDGE isucc;                  /* IMPORT SUCCESSOR                    */
  PEDGE epred;                  /* EXPORT PREDECESSOR                  */
  PEDGE esucc;                  /* EXPORT SUCCESSOR                    */

  PEDGE uedge;                  /* UTILITY EDGE POINTER                */
  PSET  grset;                  /* GLOBAL READ SET                     */
  PSET  gwset;                  /* GLOBAL WRITE SET                    */

  char  *CoNsT;                 /* CONSTANT STRING (IF SRC IS NULL)    */

  SIZE;                         /* SIZE EXPRESSION FOR THIS VALUE      */

  PSET  lrset;                  /* LOCAL READ SET                      */
  PSET  lwset;                  /* LOCAL WRITE SET                     */

  /* dope AND lvl SHOULD NOT BE REFERENCED OUTSIDE THE CONFINES OF If1Dope  */
  PDOPE  dope;                  /* ARRAY BOUNDS INFORMATION            */
  int    lvl;                   /* SCOPE LEVEL OF EDGE                 */

  PTEMP  temp;                  /* TEMPORARY STORAGE                   */


  char  *dname;                 /* NAME USED IN PRETTY PRINTING        */
  int    rc;                    /* REFERENCE COUNT                     */

  int    vtemp;                 /* VECTOR TEMPORARY NAME               */

  int    sdbx;                      
};

struct alist {
  int    datum;                 /* INTEGER LIST DATUM                  */
  PALIST next;                  /* NEXT DATUM                          */
};

struct dope {
  PEDGE lo;                     /* EDGE DEFINING LOWER BOUND OF CORE */
  int   dec;                    /* NUMBER OF array_addl'S TO CORE    */
  PEDGE hi;                     /* EDGE DEFINING UPPER BOUND OF CORE */
  int   inc;                    /* NUMBER OF array_addh'S TO CORE    */
};

struct mdata {
  char *name;
  char *file;

  char  kind;

  int   cnum;                         /* NUMBER OF CALL REFERENCES AND SUM   */
  int   level;                        /* OF CALL SITE NESTING LEVEL          */
  int   pbusy;                        /* NUMBER OF BUSY PROCESSORS           */
  };

struct temp {
  char *name;                   /* NAME OF TEMPORARY          */
  char  status;                 /* ALLOCATION STATUS          */
  char  mod;                    /* TYPE MODIFIER              */
  char  DeAl;                   /* DEALLOC THIS ITEM?         */
  short fld;                    /* STRUCTURE FIELD IDENTIFIER */
  PTEMP next;                   /* NEXT TEMPORARY             */
  PINFO info;                   /* TYPE INFO FOR TEMPORARY    */
  int   pop;                    /* TOTAL USERS                */
  double ccost;                 /* EXECUTION COST OF SCOPE    */
  int   dist;
  int    fid;                   /* GLOBAL FRAME IDENTIFIER    */
};

#define WHITE             0

#define IsEmptySet(x)     ( (x)->last < 0 )
#define SET_INC           2

struct set {
  int   last;                   /* INDEX OF LAST SET ENTRY                */
  PEDGE *set;                   /* SET CONTENTS                           */
  int   ssize;                  /* CURRENT SET SIZE                       */
  PEDGE gen;                    /* EXPORT OF SET ORIGIN (IF A GENERATOR!) */
  PNODE graph;                  /* GRAPH NODE DEFINING CURRENT SCOPE EXIT */
  PSET  ssucc;                  /* NEXT SET IN GLOBAL READ/WRITE SET LIST */
};

struct call {
  PCALL    caller;              /* CALLEE REFERENCE LIST */
  PCALL    callee;              /* REFERENCED CALLEE OR NEXT GRAPH TRUNK NODE*/

  PNODE    graph;               /* IF1 GRAPH OR CALL NODE */

  unsigned InLineFunction : 1;  /* CAN IT BE INLINED?           */
  unsigned expanded : 1;        /* HAS IT BEEN EXPANDED?        */
  unsigned cycle    : 1;        /* NOT INLINED TO BREAK CYCLE?  */
  unsigned disabled : 1;        /* NOT INLINED BY USER REQUEST? */
  unsigned skipped  : 1;        /* SKIPPED BECAUSE OF SIZE OVERFLOW */

  int      checked;             /* ALREADY EXAMINED FOR CYCLES? */
  int      color;               /* CYCLE DETECTION COLOR  */
  int        rcnt;              /* NUMBER OF REFERENCES   */
};

struct ade {
  int   if1line;                /* LINE NUMBER OF INFO IN IF1 FILE     */

  PNODE dst;                    /* DESTINATION NODE                    */
  PNODE src;                    /* SOURCE NODE                         */

  PADE  ipred;                  /* ADE IMPORT PREDECESSOR              */
  PADE  isucc;                  /* ADE IMPORT SUCCESSOR                */
  PADE  epred;                  /* ADE EXPORT PREDECESSOR              */
  PADE  esucc;                  /* ADE EXPORT SUCCESSOR                */

  int   priority;               /* ADE REMOVAL PRIORITY                */
};

#ifndef MAX_PATH
#  define MAX_PATH      (1024)  /* Pathname length (sometimes in sys header) */
#endif
#define MaxClass      10000      /* MAX NUMBER OF EQUIVALENCE CLASSES */
#define MaxLevel  100           /* MAXIMUM NUMBER OF NESTING LEVELS */
struct level {                  /* LEVEL OCCURRENCE COUNTERS        */
    int lits;
    int edges;
    int simples;
    int ats;
    int comps;
    int graphs;
    };

union bblock {
  NODE n;
  EDGE e;
  NAME nm;
  INFO i;
  TEMP t;
  SET  s;
  ADE  a;
  CALL c;
};

typedef union bblock BBLOCK, *PBBLOCK;

#define A_ELEM      info1       /* ARRAY-STREAM-MULTIPLE SUBCOMPONENT */
#define R_FIRST     info1       /* FIRST RECORD-UNION MEMBER          */
#define F_IN        info1       /* FUNCTION IMPORT TYPES              */
#define F_OUT       info2       /* FUNCTION EXPORT TYPES              */
#define L_NEXT      info2       /* NEXT LIST ENTRY                    */
#define L_SUB       info1       /* LIST COMPONENT TYPE                */
#define R_SETUP     info1       /* SETUP TUPLE                        */
#define R_MAP       info2       /* MAPPING FUNCTION TYPE              */
#define R_ARGS      info2->F_IN /* MAPPING FUNCTION TYPE ARGS         */
#define R_RESULT    info2->F_OUT/* MAPPING FUNCTION TYPE RESULTS      */
#define R_TUPLE     info1       /* REDUCTION TUPLE TYPE LIST          */
#define R_FUNCT     info2       /* REDUCTION FUNCTION TYPE LIST       */

#define NOMARK             0    /* THE MARK IS DISABLED */
#define RMARK              1    /* THE R MARK IS ENABLED*/
#define rMARK              2    /* THE r MARK IS ENABLED*/

#define MAIN_NAME   "main"      /* NAME OF IF1 PROGRAM ENTRY POINT    */

#define UNKNOWN         0       /* TEMPORARY STORAGE STATUSES*/
#define ALLOCATED       1
#define FREE            2

#define NONE            0       /* TYPE MODIFIERS             */
#define GLOB            1
#define FPTR            2

#define AppendToUtilityList( h, t, n )  if ( (h) == NULL ) (h) = (n);      \
                                        else (t)->usucc = (n); (t) = (n)

#define G_DAD   npred           /* POINTER FROM SUBGRPAPH TO CMP NODE  */
#define G_NODES nsucc           /* NODES OF GRAPH                      */
#define G_INFO  info            /* FUNCTION GRAPH ARGUMENT TYPE INFO   */
#define G_NAME  gname           /* FUNCTION GRAPH NAME                 */
#define G_TAGS  alst            /* SUBGRAPH TAG LIST                   */

#define C_SUBS  gsucc           /* SUBGRAPHS OF COMPOUND NODE          */
#define C_SCNT  scnt            /* # OF SUBGRAPHS IN COMPOUND NODE     */
#define C_ALST  alst            /* ASSOCIATION LIST OF COMPOUND NODE   */

#define F_GEN   gsucc             /* FORALL SUBGRAPH POSITIONS IN GRAPH LIST */
#define F_BODY  gsucc->gsucc
#define F_RET   gsucc->gsucc->gsucc

#define I_TEST  gsucc         /* IFTHENELSE SUBGRAPH POSITIONS IN GRAPH LIST */
#define I_TRUE  gsucc->gsucc
#define I_FALSE gsucc->gsucc->gsucc

#define L_INIT  gsucc    /* LOOPA AND LOOPB SUBGRAPH POSITIONS IN GRAPH LIST */
#define L_TEST  gsucc->gsucc                
#define L_BODY  gsucc->gsucc->gsucc         
#define L_RET   gsucc->gsucc->gsucc->gsucc  

#define R_INIT  gsucc          /* REDUCTION SUBGRAPH POSITIONS IN GRAPH LIST */
#define R_BODY  gsucc->gsucc
#define R_RET   gsucc->gsucc->gsucc

#define RPT_BODY gsucc         /* FOR REPEAT SUBGRAPH POSITIONS IN G. LIST */
#define RPT_TEST gsucc->gsucc
#define RPT_RET gsucc->gsucc->gsucc

#define S_TEST  gsucc             /* SELECT SUBGRAPH POSITIONS IN GRAPH LIST */
#define S_ALT   gsucc->gsucc
#define S_CONS  gsucc->gsucc->gsucc

#define W_TEST  gsucc             /* FOR WHILE SUBGRAPH POSITIONS IN G. LIST */
#define W_BODY  gsucc->gsucc
#define W_RET   gsucc->gsucc->gsucc

#define IsNodeListEmpty(x)      ( (x)->G_NODES == NULL )
#define IsOneExport(x)          ( (x)->exp->esucc == NULL )

#define IsImport(x,y)      ( FindImport((x),(y)) != NULL )
#define IsExport(x,y)      ( FindExport((x),(y)) != NULL )

#define IsOctal(x)         ( ((x) >= '0') && ((x) <= '7') )
#define IsConst(x)         ( ((x)->src == NULL) && ((x)->eport == CONST_PORT) )
#define IsNonErrorConst(x) ( IsConst(x) && ((x)->CoNsT != NULL) )
#define IsErrorConst(x)    ( IsConst(x) && ((x)->CoNsT == NULL) )

#define Max(a,b)           (((a) > (b)) ? (a) : (b))

#define MAX_PORT_NUMBER    100000 /* FOR ARRAY IMPROVEMENTS */

#define MAX_DOTS           2      /* ALSO DEFINED IN process.h OF Runtime */

#define IsEntry(x)         (x->mark == 'f' || x->mark == 'c' || x->mark == 'e')

#define ARITHMETICS        0    /* FLOP COUNT MACROS */
#define LOGICALS           1
#define INTRINSICS         2

#define SetSize(x)         ((x)->last + 1)
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

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
#define ANSWER_SIZE 100         /* RESULT BUFFER SIZE    */

#define BASE8           8       /* ASCII -> INTEGER CONVERSION BASES */
#define BASE10          10

#define BUFFER_SIZE     200

#define BOUND           2       /* ADE PRIORITIES */
#define HIGH_PRI        1
#define LOW_PRI         0

#define OK              0       /* PROGRAM EXIT STATUS CODES   */
#define ERROR           1

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
extern char     *program;
extern FILE     *input;
extern FILE     *output;
extern int info;        /* The info value */
extern char infofile[200];
#define RequestInfo(x,info) (info & (x))
#define _InfoMask(x)            (1<<((x)-1))
#define I_Info1         _InfoMask(1) /* IF1OPT */
#define I_Info2         _InfoMask(2) /* IF2MEM */
#define I_Info3         _InfoMask(3) /* IF2UP */
#define I_Info4         _InfoMask(4) /* IF2PART */
#define I_Info5         _InfoMask(5) /* IF2GEN */       

extern char     *reduct[];      /* REDUCTION INTERFACE FUNCTION TABLE */
extern int      rtop;
extern int      noassoc;        /* NO ASSOCIATIVE TRANSFORMATIONS */
extern PNODE    fhead;          /* Adjusted linked list of functions */
extern PNODE    nfirst;         /* FIRST NODE OF FILE          */
extern char     *entryt[];      /* ENTRY POINT NAME TABLE */
extern int      entrys[];
extern int      etop;
extern char     *fortt[];       /* FORTRAN INTERFACE FUNCTION TABLE */
extern int      ftop;
extern char     *ct[];          /* C INTERFACE FUNCTION TABLE */
extern int      ctop;
extern PNAME    xnames;         /* EXPORT AND LOCAL NAME LIST */
extern PNAME    inames;         /* IMPORT NAME LIST */
extern PRAGS    pragmas;        /* PRAGMAS OF CURRENT INPUT LINE */
extern int      nchange;        /* WAS A NODE UNLINKED OR LINKED? */
extern int      echange;        /* WAS AN EDGE UNLINKED OR LINKED?    */
extern PNODE    ftail;          /* End of linked list of functions */
extern int      ExpandedEqual;  /* Which def'n for AreEdgesEqual */
extern int      cycle;          /* COUNT OF CYCLE CAUSING ADEs */
extern double   mcosts[];       /* MISC COST TABLE */
extern int      InitialNodeLevel; /* For initialization in NodeAlloc */
extern int      streams;        /* TRUE iff a stream type is created */
extern int      recursive;      /* Has a recursive function %mk=B was found? */
extern int      tmpid;          /* Temporary name stamp */

extern int      glue;           /* ELIMINATE DEAD FUNCTION CALLS? */ 
extern int      sgnok;          /* ALLOW SIGNED ARITHMETIC CONSTANTS? */
extern int      dbl;            /* Convert float to double? */
extern int      flt;            /* Convert double to float? */
extern int      prof;           /* Do we want to profile? */
extern int      StreamsOK;      /* Are Stream types allowed? */
extern int      AllowVMarks;    /* True if you want %mk=V inputs */
extern int      FixPortsToo;    /* True to fix up loop port assignments */
extern int      FullyOrdered;   /* Assume Ordered Input?? */
extern int      DMarkProblem;   /* Do we have the DMARK problem - if2gen only*/
extern int      CheckForBadEdges;/* Check edges for legitimate src and dst. */

extern void     TypeAssignPragmas PROTO((PINFO));
extern void     NodeAssignPragmas PROTO((PNODE));
extern void     EdgeAssignPragmas PROTO((PEDGE));

extern void     PragInitPragmas PROTO((PRAGS*));
extern void     TypeInitPragmas PROTO((PINFO));
extern void     NodeInitPragmas PROTO((PNODE));
extern void     EdgeInitPragmas PROTO((PEDGE));

extern void     AddName PROTO((namelink**,char*));
extern void     BuildHaveList PROTO((namelink**));
extern void     DumpList PROTO((namelink*));
extern int      InNameList PROTO((char*,namelink*));

extern void     AddToNameList PROTO((PNODE));
extern void     AddToImportList PROTO((PNODE,char*,PINFO));
extern void     PlaceInEntryTable PROTO((char*));
extern void     PlaceInFortranTable PROTO((char*));
extern void     PlaceInCTable PROTO((char*));

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* VARIABLES */
/* Warning1.c */
extern int      Warnings;       /* TRUE iff we want to see warning msgs */
/* BuildItems.c */
extern int      tbase;          /* Label IF1 types from 0 - first file */
extern int      tmax;           /* Biggest type - not used */
extern char     *stamps[];      /* STAMPS FOR ANY CHARACTER */
extern char     *sfile;         /* SISAL FILE BEING READ */
extern PNODE    cfunct;         /* FUNCTION BEING BUILT OR EXAMINED */
extern PNODE    nhash[];
extern PNODE    nprd;
extern PNODE    nlstop;         /* NODE  LIST STACK TOP */
extern char     *sfunct;
extern PINFO    ihead;          /* SYMBOL TABLE HEAD POINTER */
extern PINFO    itail;          /* SYMBOL TABLE TAIL POINTER */
extern PNODE    nhead;          /* NODE LIST HEAD POINTER */
extern PNODE    ntail;          /* NODE LIST TAIL POINTER */
extern PNODE    glstop;
extern PINFO    ptr_real;       /* VARIOUS SYMBOL TABLE ENTRIES */
extern PINFO    ptr_double;
extern PINFO    ptr_integer;
extern PINFO    ptr;
extern PINFO    integer;

/* ReadItems.c */
extern int      token;
extern char     buffer[];
extern int      line;
extern int      maxint;
extern PRAGS    pragmas;

/* FoldItems.c */
extern int      fcnt;           /* COUNT OF FOLDED NODES */
extern int      pcnt;           /* COUNT OF PROPAGATED CONSTANTS */
extern int      sfcnt;          /* RUNNING COUNT OF FOLDED SELECT NODES */
extern int      lfcnt;          /* COUNT OF FOLDED LOGICALS */
extern int      esccnt;         /* COUNT OF EXPORTED SELECT CONSTANTS */
extern int      expcnt;         /* EXP STRENGTH REDUCTIONS */
extern long     iop1;           /* INTEGER, CHAR, OR BOOLEAN OPERANDS */
extern long     iop2;
extern double   dop1;           /* DOUBLE_REAL OR REAL OPERANDS */
extern double   dop2;
extern int      chain_cnt;      /* COUNT OF FOLDED +/- CHAINS */
extern int      zero_cnt;       /* COUNT OF A * 0 AND 0 * A FOLDS */
extern int      ident_cnt;      /* COUNT OF FOLDED IDENTITY NODES */
extern int      dncnt;          /* COUNT OF DIVIDE TO NEG CONVERSIONS */
extern int      pncnt;          /* COUNT OF PROPAGATED NEGATIONS */
extern int      idxm;           /* COUNT OF ARRAY INDEXING MODIFICATIONS */
extern int      negcnt;         /* COUNT OF NEG REDUCTIONS */
extern int      setlcnt;        /* COUNT OF SETL REDUCTIONS */
extern int      kcnt;           /* COUNT OF COMBINED K IMPORTS */
extern int      ckcnt;          /* COMBINED K IMPORTS DURING CSE */
extern int      gkcnt;          /* COMBINED K IMPORTS DURING GCSE */
extern int      ccnt;           /* COUNT OF COMBINED NODES */
extern int      vcnt;           /* COUNT OF INVARIANTS REMOVED */
extern int      gccnt;          /* COUNT OF GLOBALLY COMBINED NODES */
extern int      norm_cnt;       /* COUNT OF SIMPLE FOLDS */
extern int      neg_cnt;        /* COUNT OF FOLDED NEGATIVE NODES */
extern int      ama_cnt;        /* COUNT OF FOLDED ADD-MAX-ADD CHAINS */
extern int      asize_cnt;      /* COUNT OF FOLDED ASize-MAX CHAINS */
extern int      sccnt;          /* COUNT OF SELECT SUBGRAPH MOVEMENTS */
extern int      pycnt;          /* COUNT OF PUSHED YANKED RETURN NODES */
extern int      scpcnt;         /* COUNT OF SAVED CALL PARAMETERS */
extern int      scpinvcnt;      /* COUNT OF INVARIANT SAVED CALL PARAM */
extern int      sspcnt;         /* COUNT OF SAVED SLICE PARAMETERS */
extern int      sspinvcnt;      /* COUNT OF INVARIANT SAVED SLICE PARAM */
extern int      dicnt;          /* COUNT OF REMOVED COMPOUND IMPORTS */
extern int      tdicnt;         /* COUNT OF REMOVED COMPOUND IMPORTS */
extern int      leicnt;         /* COUNT OF REMOVED LOOP ENQUE IMPORTS */
extern int      tleicnt;                /* COUNT OF REMOVED LOOP ENQUE IMPORTS */
extern int      tagtcnt;        /* COUNT OF TAG TEST CONVERSIONS */

/* SmashItems.c */
extern int      lclass;         /* LAST EQUIVALENCE CLASS IN htable */
extern PINFO    htable[];       /* EQUIVALENCE CLASS HEAD POINTERS */
extern PINFO    ttable[];       /* EQUIVALENCE CLASS TAIL POINTERS */

/* CallItems.c */
extern PNODE    chead;          /* COMPOUND NODE TRUNK HEAD */
extern PNODE    ctail;          /* COMPOUND NODE TRUNK HEAD */
extern PCALL    cghead;         /* CALL GRAPH TRUNK HEAD */
extern PCALL    cgtail;         /* CALL GRAPH TRUNK HEAD */

/* TimeItems.c */
extern double   StartTime;
extern double   StopTime;

/* CountItems.c */
extern struct level     levels[];       /* OCCURRENCE COUNT LEVEL STACK */
extern int      maxl;           /* MAXIMUM ENCOUNTERED LEVEL */
extern int      topl;           /* TOP OF LEVEL STACK */
extern int      lits;           /* TOTAL OCCURRENCE COUNTERS */
extern int      edges;
extern int      simples;
extern int      ats;
extern int      graphs;
extern int      comps;
extern int      rsum;
extern int      rprod;
extern int      rcat;
extern int      rleast;
extern int      rgreat;
extern int      gnodes[];
extern int      snodes[];
extern int      atnodes[];
extern int      cnodes[];
extern int      convatns;
extern int      inatns;
extern int      patns;
extern int      syncatns;
extern int      fsyncatns;
extern int      incratns;
extern int      fincratns;

/* InfoAlloc.c */
extern int      LargestTypeLabelSoFar;

/* ------------------------------------------------------------ */
/* ------------------------------------------------------------ */
/* FUNCTIONS */
extern void     AddPatch PROTO((int));
extern void     AddStamp PROTO((int,char*));
extern void     AddTimeStamp PROTO((void));
extern void     AddToGraphList PROTO((PNODE));
extern void     AddToNodeList PROTO((PNODE));
extern PADE     AdeAlloc PROTO((PNODE,PNODE,int));
extern int      AreConstsEqual PROTO((PEDGE,PEDGE));
extern int      AreEdgesEqual PROTO((PEDGE,PEDGE));
extern int      AreNodesDependent PROTO((PNODE,PNODE));
extern int      AreNodesEqual PROTO((PNODE,PNODE));
extern int      FastAreNodesEqual PROTO((PNODE,PNODE));
extern int      AreValuesEqual PROTO((PEDGE,PEDGE));
extern int      AssignNewKports PROTO((PNODE));
extern void     AssignNewLabels PROTO((PNODE));
extern int      AssignNewLoopTports PROTO((int,PNODE));
extern int      AssignNewLports PROTO((int,PNODE));
extern int      AssignNewMports PROTO((int,PNODE));
extern void     AssignNewRports PROTO((PNODE));
extern int      voidAssignNewRports PROTO((void));
extern void     AssignSourceFileName PROTO((char*));
extern int      AssignNewTports PROTO((int,PNODE));
extern void     AssignTags PROTO((PALIST,PNODE));
extern PALIST   AssocListAlloc PROTO((int));
extern void     AttachEdge PROTO((PNODE,int,PNODE,int,PINFO,char*));
extern void     ChangeEdgeToConst PROTO((PEDGE,PEDGE));
extern void     ChangeExportPorts PROTO((PNODE,int,int));
extern void     ChangeExportsToConst PROTO((PNODE,int,PEDGE));
extern void     ChangeImportPorts PROTO((PNODE,int,int));
extern void     ChangeToConst PROTO((PEDGE,PEDGE));
extern void     ClearTouchFlags PROTO((void));
extern PEDGE    CopyEdge PROTO((PEDGE,PNODE,PNODE));
extern void     CopyEdgeAndLink PROTO((PEDGE,PNODE,int));
extern void     CopyEdgeAndThreadToUse PROTO((PEDGE,PNODE,int));
extern char     *CopyString PROTO((char*));
extern void     CreateAndInsertAde PROTO((PNODE,PNODE,int));
extern PEDGE    DirectEdge PROTO((PNODE,int,PNODE,int,PINFO));
extern int      DontCombineFill PROTO((PNODE));
extern PEDGE    EdgeAlloc PROTO((PNODE,int,PNODE,int));
extern void     EnterInSet PROTO((PSET,PEDGE));
extern void     EnterScope PROTO((void));
extern void     Error1 PROTO((char*));
extern void     Error2 PROTO((char*,char*));
extern void     ExitScope PROTO((void));
extern PNODE    FindEnclosing PROTO((PNODE));
extern PEDGE    FindExport PROTO((PNODE,int));
extern PNODE    FindFunction PROTO((char*));
extern PNODE    FindGraph PROTO((int));
extern PNODE    FindGraphNode PROTO((PNODE));
extern PEDGE    FindImport PROTO((PNODE,int));
extern PINFO    FindInfo PROTO((int,int));
extern int      FindLabel PROTO((int));
extern int      FindLargestExport PROTO((PNODE));
extern int      FindLargestImport PROTO((PNODE));
extern int      FindLargestPort PROTO((PNODE));
extern int      FindLargestLabel PROTO((void));
extern PNODE    FindLastGraph PROTO((PNODE));
extern PEDGE    FindLastImport PROTO((PNODE));
extern PNODE    FindLastNode PROTO((PNODE));
extern PNODE    FindLargestNode PROTO((PNODE));
extern PNODE    FindNode PROTO((int,int));
extern int      FindReturnsCode PROTO((char*));
extern char     *FindReturnsString PROTO((int));
extern PEDGE    FindSource PROTO((PEDGE));
extern int      FixConstantImports PROTO((PNODE,int));
extern int      FixSignedConstantImports PROTO((PNODE,int));
extern PCALL    GetNewCallNode PROTO((void));
extern char     *GetSourceLine PROTO((PNODE));
extern char     *GetStampString PROTO((int));
extern int      HasWriteExport PROTO((PNODE));
extern void     If1Read PROTO((void));
extern void     MonoIf1Read PROTO((void));
extern void     If1Write PROTO((void));
extern void     MonoIf1Write PROTO((void));
extern void     If2Read PROTO((void));
extern void     If2Write PROTO((void));
extern void     ImportSwap PROTO((PNODE));
extern PINFO    InfoAlloc PROTO((int,int));
extern char     *IntToAscii PROTO((int));
extern int      IntegerPower PROTO((int,int));
extern int      IsABRecord PROTO((PINFO));
extern int      IsAdePresent PROTO((PNODE,PNODE));
extern int      IsAggregate PROTO((PINFO));
extern int      IsCInterface PROTO((char*));
extern int      IsCalled PROTO((char*));
extern int      IsCaller PROTO((PNODE,char*));
extern int      IsDivByZero PROTO((PNODE));
extern int      IsEntryPoint PROTO((char*));
extern int      IsFortranInterface PROTO((char*));
extern int      IsInnerLoop PROTO((PNODE));
extern int      IsIntrinsic PROTO((char*,PINFO));
extern int      IsPMarked PROTO((PNODE,int));
extern int      IsPath PROTO((PNODE,PNODE));
extern void     IsPathInit PROTO((PNODE));
extern int      IsReadOnly PROTO((PNODE,int));
extern int      IsReductionInterface PROTO((char*));
extern int      IsStamp PROTO((int));
extern void     LinkAdeExport PROTO((PNODE,PADE));
extern void     LinkAdeImport PROTO((PNODE,PADE));
extern PALIST   LinkAssocLists PROTO((PALIST,PALIST));
extern void     LinkExport PROTO((PNODE,PEDGE));
extern void     LinkExportLists PROTO((PNODE,PNODE));
extern void     LinkExportToEnd PROTO((PNODE,PEDGE));
extern PNODE    LinkGraph PROTO((PNODE,PNODE));
extern void     LinkImport PROTO((PNODE,PEDGE));
extern void     LinkImportLists PROTO((PNODE,PNODE));
extern PNODE    LinkNode PROTO((PNODE,PNODE));
extern void     LinkNodeLists PROTO((PNODE,PNODE));
extern PINFO    LookupInfo PROTO((int,int));
extern char     *LowerCase PROTO((char*,int,int));
extern void     MakeAde PROTO((int,int));
extern void     MakeCompound PROTO((int,int,int,PALIST));
extern void     MakeConst PROTO((int,int,int,char*));
extern void     MakeEdge PROTO((int,int,int,int,int));
extern void     MakeGraph PROTO((int,int,char*));
extern void     MakeInfo PROTO((int,int,int,int));
extern void     MonoMakeCompound PROTO((int,char*));
extern void     MonoMakeConst PROTO((int,int,int,char*));
extern void     MonoMakeEdge PROTO((int,int,int,int,int));
extern void     MonoMakeGraph PROTO((int,int,char*));
extern void     MonoMakeNode PROTO((int,int));
extern void     MakeNode PROTO((int,int));
extern void     MakePtrTypes PROTO((void));
extern char     *MyAlloc PROTO((int));
extern PBBLOCK  MyBBlockAlloc PROTO((void));
extern PNAME    NameAlloc PROTO((PNODE,char*,PINFO));
extern PNODE    NodeAlloc PROTO((int,int));
extern void     NormalizeNode PROTO((PNODE));
extern void     PlaceGraph PROTO((PNODE));
extern void     PlaceNode PROTO((PNODE));
extern void     PopGraphList PROTO((void));
extern void     PopNodeList PROTO((void));
extern void     PrepareForNextFile PROTO((void));
extern void     PushGraphList PROTO((void));
extern void     PushNodeList PROTO((void));
extern PALIST   ReadAssocList PROTO((int));
extern void     ReadError PROTO((char*));
extern int      ReadInteger PROTO((void));
extern int      ReadLineId PROTO((void));
extern char     *ReadLiteral PROTO((int));
extern char     *MonoReadAssocList PROTO((void));
extern void     ReadPragmas PROTO((void));
extern void     ReadStamp PROTO((void));
extern char     *ReadString PROTO((int));
extern void     RemoveStamp PROTO((int));
extern PSET     SetAlloc PROTO((PSET,PNODE));
extern PTEMP    TempAlloc PROTO((char*,PINFO,int,int));
extern void     ThreadToUse PROTO((PNODE,int,PNODE,int,PINFO));
extern void     TouchTheUnions PROTO((PINFO));
extern void     UnlinkAdeExport PROTO((PADE));
extern void     UnlinkAdeImport PROTO((PADE));
extern void     UnlinkExport PROTO((PEDGE));
extern PNODE    UnlinkGraph PROTO((PNODE));
extern void     UnlinkImport PROTO((PEDGE));
extern PNODE    UnlinkNode PROTO((PNODE));
extern char     *UpperCase PROTO((char*,int,int));
extern int      UsageCount PROTO((PNODE,int));
extern void     Warning1 PROTO((char*));
extern void     WriteAggregateInfo PROTO((void));
extern void     WriteConst PROTO((PEDGE));
extern void     WriteGlobals PROTO((void));
extern void     WriteImports PROTO((PNODE));
extern void     WriteInfo PROTO((void));
extern void     MonoWriteConst PROTO((char*,PINFO));
extern void     MonoWriteInfo PROTO((void));
extern void     MonoWriteNodes PROTO((void));
extern void     WriteNodes PROTO((PNODE));
extern void     WriteStamps PROTO((void));
extern void     AddToEquivClass PROTO((PINFO,PINFO));
extern PCALL    CallAlloc PROTO((PNODE));
extern int      CanPropagateConst PROTO((PEDGE));
extern int      CaseCmp PROTO((char*,char*));
extern void     Count PROTO((PNODE));
extern void     CreateNewEquivClass PROTO((PINFO));
extern void     GatherOthers PROTO((PINFO,PINFO));
extern void     InitEquivClasses PROTO((void));
extern int      IsCandidate PROTO((PNODE));
extern void     PPrintConst PROTO((PEDGE));
extern void     PPrintIndentation PROTO((int,int));
extern void     PointToHead PROTO((void));
extern void     PrintLInfo PROTO((int,int,char*,PNODE));
extern void     PrintWarning PROTO((char*,int,char*,char*,int));
extern void     PropagateConst PROTO((PNODE));
extern void     RemoveFromEquivClass PROTO((PINFO,PINFO));
extern int      SameEquivClass PROTO((PINFO,PINFO));
extern void     Stop PROTO((int));
extern void     WriteLoopMap PROTO((char*));
extern void     WriteTheLMap PROTO((int,int,PNODE));
extern double   ElapsedTime PROTO((struct timeval*,struct timeval*));
extern double   TSECND PROTO((void));
extern void     StartProfiler PROTO((void));
extern void     StopProfiler PROTO((char*));
extern char     *GetNodeName PROTO((PNODE));
extern char     *OpCodeNameOf PROTO((int));
extern char     *GetMacro PROTO((PNODE));
extern char     *GraphName PROTO((int));
extern char     *SimpleName PROTO((int));
extern char     *CompoundName PROTO((int));
extern char     *AtName PROTO((int));
extern void     ReadItems PROTO((void));
extern void     BuildItems PROTO((void));
extern void     CopyEdgeAndReset PROTO((PEDGE,PNODE,PNODE));
extern void     CopyExports PROTO((PNODE,PNODE));
extern void     CopyImports PROTO((PNODE,PNODE,int));
extern PNODE    CopyNode PROTO((PNODE));
extern PINFO    NewType PROTO((int,int,int,char*));
extern PINFO    GetAggregate PROTO((int,PINFO,char*));
extern void     CheckEdge PROTO((PEDGE));
extern int      UsingPatch PROTO((int));

/*
 * $Log$
 * Revision 1.3  2007/05/19 15:29:38  patmiller
 * new release
 *
 * Revision 1.2  2001/01/02 09:16:45  patmiller
 * Now ANSI compliant, but still a pthread problem
 *
 * Revision 1.1.1.1  2000/12/31 17:58:23  patmiller
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
 * Revision 1.47  1994/07/21  22:34:42  denton
 * Added DYNMACROS for buffers on stack and heap.
 *
 * Revision 1.46  1994/07/21  00:11:32  denton
 * Upped count of simple nodes to match in makenames.c
 *
 * Revision 1.45  1994/07/20  17:20:12  solomon
 * Added RETURNS_VALUE to reduction type codes.  Added IFASelectRange
 * and IFRequire codes to the IF90 nodes.  Changed the Repeat and While
 * loop subgraph position macros to reflect the removal of the
 * courtesy graph.
 *
 * Revision 1.44  1994/07/15  17:53:47  denton
 * Added " around tn
 *
 * Revision 1.43  1994/07/14  16:36:33  denton
 * Fix overlap of IF1 nodes for makenames.
 *
 * Revision 1.42  1994/07/13  18:08:35  denton
 * Removed BAD from IFX.h and fix MakeAde to use LOW_PRI (it incorrectly used LOW)
 *
 * Revision 1.41  1994/07/12  16:34:30  denton
 * Fix simple node count
 *
 * Revision 1.40  1994/07/12  16:32:25  denton
 * Added IFConvert, IFCallForeign, and IF_FOREIGN for foreign language interface
 *
 * Revision 1.39  1994/07/01  23:46:42  denton
 * color is now static
 *
 * Revision 1.38  1994/06/24  16:01:30  denton
 * Added -IF3.
 *
 * Revision 1.37  1994/06/16  21:31:57  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.36  1994/06/16  00:10:33  denton
 * Added IsBranch and IFUReduce to macros
 *
 * Revision 1.35  1994/06/06  23:31:16  denton
 * Added low and high markers for node categories: COMPOUND, SIMPLE, GRAPH, OTHER.
 *
 * Revision 1.34  1994/05/25  23:56:45  solomon
 * Added FindLargestNode() to Library.
 *
 * Revision 1.33  1994/05/25  19:44:33  denton
 * Added FindLargestLabel to return the value of LargestTypeLabelSoFar.
 * Shortened name of GetAggregateType.c to remove ar warnings.
 *
 * Revision 1.32  1994/05/04  18:12:02  denton
 * FindEnclosingCompound->FindEnclosing
 *
 * Revision 1.31  1994/05/04  00:50:23  denton
 * Added IsAnyLoop and repaird compound node count.
 *
 * Revision 1.30  1994/05/03  23:02:21  denton
 * Replaced tab with space so mypp parse would work correctly.
 *
 * Revision 1.29  1994/05/03  19:26:36  denton
 * Added IFCReduce and R_BODY for compound reduction node.
 *
 * Revision 1.28  1994/05/03  18:25:25  denton
 * Added FindReturns and RETURNS_* for Sisal90 returns clauses.
 *
 * Revision 1.27  1994/05/03  16:20:38  denton
 * Fixed funky CVS automatic merge.
 *
 * Revision 1.26  1994/04/29  17:05:28  denton
 * Added I_TEST etc. for IFIfThenElse
 *
 * Revision 1.25  1994/04/15  21:51:51  denton
 * Use config.h for system specific header files.
 * Removed most gcc -Wall warnings.
 *
 * Revision 1.24  1994/04/15  15:51:47  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.23  1994/04/14  20:39:17  solomon
 * Changed #define TYPE_CODE_LAST from IF_SET to IF_REDUCTION.
 * Added extern int LargestTypeLabelSoFar used when creating new types.
 * Added extern int CheckForBadEdges to be set in ifxstuff.c files.  When TRUE,
 * the function CheckEdge is invoked for every edge.
 * Added function declarations for the following functions: NewType(),
 * GetAggregate(), and CheckEdge().
 *
 * Revision 1.22  1994/04/01  00:02:46  denton
 * NULL -> '\0' where appropriate
 *
 * Revision 1.21  1994/03/31  01:46:31  denton
 * Replace anachronistic BSD functions, index->strchr, rindex->strrchr
 *
 * Revision 1.20  1994/03/24  19:39:46  denton
 * Distributed DSA, non-coherent cache, non-static shared memory.
 *
 * Revision 1.19  1994/03/22  00:43:48  miller
 * Fixed names, updated Makefile to reflect CopyEdgeAndRe.c and IntegerPower.c
 *
 * Revision 1.18  1994/03/09  23:14:44  miller
 * Changes for the new frontend -- Added a new typecode (IF_SET)
 *
 * Revision 1.17  1994/03/03  19:46:32  solomon
 * Added external declarations for the following functions:
 * CopyEdgeAndReset, CopyExports, CopyImports, and CopyNode.
 *
 * Revision 1.16  1994/03/03  17:19:30  solomon
 * Added the macro: #define IFRepeatLoop(x) ((x)->type == IFRepeatLoop).
 *
 * Revision 1.15  1994/02/18  23:39:19  denton
 * Removed redundant ifdef
 *
 * Revision 1.14  1994/02/17  16:50:57  denton
 * Removed redundant externs
 *
 * Revision 1.13  1994/02/15  23:20:29  miller
 * Allow new IF1 types (Typeset, complex, etc...)
 *
 * Revision 1.12  1994/02/01  22:27:10  miller
 * LINUX updates
 *
 * Revision 1.11  1994/01/28  17:26:28  denton
 * Changes for user-defined reductions
 *
 * Revision 1.10  1994/01/28  00:09:37  miller
 * new man pages and new helper functions for library
 *
 * Revision 1.9  1993/11/12  20:05:03  miller
 * Support for IF90 typeset type
 *
 * Revision 1.8  1993/06/15  20:47:18  miller
 * Support for libraries.
 *
 * Revision 1.7  1993/06/14  20:44:01  miller
 * BuildItems/ReadItems/world  (reset for new IFx read operation)
 * IFX.h/ReadPragmas (new lazy pragma)
 *
 * Revision 1.6  1993/06/08  16:48:59  miller
 * New Stream nodes (for Chad) in anticipation of 13.0
 *
 * Revision 1.5  1993/04/16  19:00:23  miller
 * Name shortening to keep the archiver from truncating names in Backend/Library
 * Since some names were changed, other files were affected.  All names in the
 * library now start with no more than 13 (unique) characters.
 *
 * Revision 1.4  1993/04/16  17:08:58  miller
 * HPUX support
 *
 * Revision 1.3  1993/04/01  23:30:43  miller
 * Changed to remove conflict with keyword: inline
 *
 * Revision 1.2  1993/03/23  22:51:42  miller
 * date problem
 *
 * Revision 1.1  1994/03/11  23:09:53  miller
 * Moved IFX.h into Backend/Library and added support for Minimal
 * installation (removing source as compiled).
 *
 * Revision 1.11  1994/03/11  18:24:05  miller
 * Removed dead function entry.
 *
 * Revision 1.10  1994/03/11  18:17:40  miller
 * Changed MinSlice to a char* to allow formulas.  Added support for information
 * masks.  Also added a timestamp to help control loop report abuse.
 *
 * Revision 1.9  1993/02/24  18:30:18  miller
 * Added a extern target for the AddPatch function.
 *
 * Revision 1.8  1993/01/15  22:31:56  miller
 * Added new fields to PRAGMAS (Style and LoopSlice).  Moved the info
 * field up from the world.h files.  Added GetSourceLine() to the list
 * of external functions.
 *
 * Revision 1.7  1993/01/14  22:23:13  miller
 * Fixed up some duplicated define's and added new fields to the PRAGMA
 * group (ID, reason1, reason2).
 *
 * Revision 1.6  1993/01/08  17:26:40  miller
 * Had a star-slash in the log comment.  This terminated the
 * comment that held the log causing really bad syntax errors!
 *
 * Revision 1.5  1993/01/07  00:36:47  miller
 * Changes for lint and to combine world.h
 * */

#endif
