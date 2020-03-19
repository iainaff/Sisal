/**************************************************************************/
/* FILE   **************          vector.c         ************************/
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
 * Revision 1.1.1.1  2000/12/31 17:57:53  patmiller
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
 * Revision 1.3  1994/06/16  21:31:17  mivory
 * info format and option changes M. Y. I.
 *
 * Revision 1.2  1994/04/15  15:51:19  denton
 * Added config.h to centralize machine specific header files.
 * Fixed gcc warings.
 *
 * Revision 1.1  1993/01/14  22:29:11  miller
 * Carry along work to propagate the new pragmas.  Also fixed up to report
 * reasons why loops don't vectorize / parallelize.  Split off some of the
 * work from if2part.c into slice.c stream.c vector.c
 */
/**************************************************************************/


#include "world.h"

static int vok      = 0;
static int vrg      = 0;
static int vrtype   = 0;
static int vrfan    = 0;
static int vrn      = 0;
static int vnum     = 0;
static int vbn      = 0;
static int vbtype   = 0;
static int vbadst   = 0;
static int vbaeldst = 0;
static int vbasrc   = 0;
static int vbinvar  = 0;
static int vbpmfan  = 0;
static int vbaelx   = 0;
static int vprag    = 0;
static int vrrf     = 0;
static int vbc      = 0;


/**************************************************************************/
/* LOCAL  **************     PartIsVecCandidate    ************************/
/**************************************************************************/
/* PURPOSE: True iff the input values to this node are constant in this   */
/*          context (e.g. loop body).                                     */
/**************************************************************************/

static int PartIsInvariant( n )
PNODE n;
{
    register PEDGE i;

    for ( i = n->imp; i != NULL; i = i->isucc ) {
        if ( IsConst( i ) )
            continue;

        if ( !IsSGraph( i->src ) )
            return( FALSE );

        if ( !IsImport( i->src->G_DAD, i->eport ) )
            return( FALSE );
        }

    return( TRUE );
}


/**************************************************************************/
/* GLOBAL **************     PartIsVecCandidate    ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF FORALL f IS A CANDIDATE FOR VECTORIZATION.     */
/**************************************************************************/

int PartIsVecCandidate( f,ReasonP )
     PNODE f;
     char  **ReasonP;
{
  register PEDGE e;
  register PEDGE i;
  register PNODE n;
  register PEDGE c;
  register PEDGE ee;
  register PNODE nn;
  register int   cu = 0;
  register PEDGE ii;

  vnum++;

  /* IF1OPT: if1split.c DISABLE CONCURRENTIZATION OF THIN COPY */
  if ( f->ThinCopy ) {
    *ReasonP = "THIN COPY has only zero or one iteration";
    return( FALSE );
  }

  /* GENERATE SUBGRAPH CAN ONLY COMPRISE 1 NODE: RangeGenerate, UNLESS    */
  /* COMPILING FOR THE CRAY AND THE OTHER NODES ARE BUFFER SCATTER NODES  */
  /* FOR ABuildAT nodes PART OF BUILD-IN-PLACE RETURN VALUE OF CATENATES. */ 
  if ( f->F_GEN->G_NODES->nsucc != NULL ) {
    if ( !cRay ) {
      vrg++;
      *ReasonP = "Multiple Generates on non-Cray architecture";
      return( FALSE );
    }

    /* START: CRAY ONLY ********************* */
    for ( nn = f->F_GEN->G_NODES->nsucc; nn != NULL; nn = nn->nsucc ) 
      switch ( nn->type ) {
       case IFScatterBufPartitions:
        if ( (e = FindExport( f->F_BODY, nn->exp->iport )) == NULL ) {
          vrg++;
          *ReasonP = "Scatter not used in body";
          return( FALSE );
        }

        if ( e->dst->type != IFABuildAT ) {
          vrg++;
          *ReasonP = "Scattered value not directed into an array build";
          return( FALSE );
        }

        ee = e->dst->exp;

        if ( ee == NULL ) { 
          vrg++;
          *ReasonP = "Value from scattered array build not used";
          return( FALSE );
        }

        if ( ee->sr != 1 ) {
          vrg++;
          *ReasonP = "Value from scattered array build is shared";
          return( FALSE );
        }

        if ( !IsSGraph( ee->dst ) ) {
          vrg++;
          *ReasonP = "Value from scattered array build is re-used";
          return( FALSE );
        }

        if ( (e = FindExport( f->F_RET, ee->iport )) == NULL ) {
          vrg++;
          *ReasonP = "Scattered array build not used in returns clause";
          return( FALSE );
        }

        if ( e->pm > 0 || e->pl > 0 ) {
          vrg++;
          *ReasonP = "Returned array is being shared";
          return( FALSE );
        }

        for ( ee = f->F_BODY->exp; ee != NULL; ee = ee->esucc ) {
          if ( ee->eport != e->eport )
            continue;

          switch ( ee->dst->type ) {
           case IFSGraph:
            if ( ee->iport != 0 ) {
              vrg++;
              *ReasonP = "Vector candidate value is grounded in body";
              return( FALSE );
            }

            break;

           case IFReduceAT:
           case IFRedLeftAT:
           case IFRedRightAT:
           case IFRedTreeAT:
            if ( !(ee->pmark) ) {
              vrg++;
              *ReasonP = "Non-parallel reduction of vector candidate";
              return( FALSE );
            }

            break;

           default:
            vrg++;
            *ReasonP = "Non-vector operation leading to vectorizable return";
            return( FALSE );
          }
        }

        break;

       default:
        vrg++;
        *ReasonP = "Non-vector generator";
        return( FALSE );
      }
  }
  /* STOP:  CRAY ONLY ********************* */

  if ( f->F_GEN->G_NODES->type != IFRangeGenerate ) {
    vrg++;
    *ReasonP = "First generator operator is not a range generator";
    return( FALSE );
  }

  c = f->F_GEN->G_NODES->exp;

  /* START: NOT NOT NOT CRAY */
  if ( !cRay ) {
    /* ALL NODES IN THE BODY SUBGRAPH MUST NOT BE INVARIANT AND MUST BE IN */
    /* THE NODE SET {+,-,*,/,neg,abs, AElement}.                           */
    /* THE FIRST AElement IMPORT MUST BE TO A K PORT VALUE.                */
    for ( n = f->F_BODY->G_NODES; n != NULL; n = n->nsucc ) {
      if ( PartIsInvariant( n ) ) {
        vbinvar++;
        *ReasonP = "Invariant operation in body";
        return( FALSE );
      }

      switch ( n->type ) {
       case IFPlus:
       case IFMinus:
       case IFTimes:
       case IFDiv:
       case IFAbs:
       case IFNeg:
       case IFDouble:
       case IFTrunc:
       case IFSingle:
        break;

       case IFAElement:
        if ( IsConst( n->imp ) ) {
          *ReasonP = "Invariant access to an array";
          return( FALSE );
        }

        if ( !IsSGraph( n->imp->src ) ) {
          *ReasonP = "Access to a non-imported array";
          return( FALSE );
        }

        if ( FindImport( f, n->imp->eport ) == NULL ) {
          *ReasonP = "An array is not imported";
          return( FALSE );
        }

        break;
  
       default:
        vbn++;
        *ReasonP = "Only +, -, *, /, neg, abs, and a[i] operations can be vectorized";
        return( FALSE );
      }
    }

    /* THE CONTROL CAN ONLY BE USED BY +, -, and AElement NODES. THE  */
    /* SUCCESSOR NODES OF THE + and - NODES MUST BE AElement NODES.   */
    /* NO REFERENCE COUNT PRAGMAS ARE ALLOWED IN THE BODY.            */
    /* THE CONTROL MUST BE USED AS MUST AN AELEMENT NODE.             */
    for ( e = f->F_BODY->exp; e != NULL; e = e->esucc ) {
      if ( e->pm > 0 || e->cm == -1 ) {
        vprag++;
        *ReasonP = "Control is shared";
        return( FALSE );
      }
  
      if ( e->eport == c->iport ) {
        cu++;
        switch ( e->dst->type ) {
         case IFPlus:
         case IFMinus:
          for ( ee = e->dst->exp; ee != NULL; ee = ee->esucc )
            if ( ee->dst->type != IFAElement ) {
              vbc++;
              *ReasonP = "Plus/minus operation leads to a non a[i] operation";
              return( FALSE );
            }

          break;

         case IFAElement:
          /* an = TRUE; */
          break;

         default:
          vbc++;
          *ReasonP = "Control can only be used by +, -, and a[i]";
          return( FALSE );
        }
      }
    }

    if ( cu <= 0 ) {
      *ReasonP = "Too many controls";
      return( FALSE );
    }
  }
  /* STOP:  NOT NOT NOT CRAY */

  /* START: CRAY ONLY ********************* */
  if ( cRay ) {
    if ( !IsInnerLoop( f->F_BODY ) ) {
      vbn++;
      *ReasonP = "Cray will only vectorize inner loops";
      return( FALSE );
    }
  }
  /* STOP:  CRAY ONLY ********************* */

  /* ALL EXPORTS OF RETURNS SUBGRAPH MUST CARRY int, real, or double */
  /* VALUES. RETURN SUBGRAPH EXPORT FANOUT IS NOT ALLOWED, UNLESS    */
  /* EACH EDGE DEFINES THE LOWER BOUND OR SIZE SPECIFICATION OF AN   */
  /* AGatherAT node.                                                 */

  /* BRecords are OK!!! */

  /* ON THE CRAY, ReduceAT's WITH pmarks ARE OK... AT LEAST FOR NOW */

  for ( e = f->F_RET->exp; e != NULL; e = e->esucc ) {
    switch ( e->info->type ) {
     case IF_ARRAY:
      if ( !cRay ) {
        vrtype++;
        *ReasonP = "Trying to reduce/gather arrays instead of elements";
        return( FALSE );
      }

      /* START: CRAY ONLY ********************* */
      if ( !IsBasic( e->info->A_ELEM ) ) {
        vrtype++;
        *ReasonP = "Array of non-scalars";
        return( FALSE );
      }

      switch ( e->dst->type ) {
       case IFSGraph:
        if ( e->iport != 0 ) {
          vrtype++;
          *ReasonP = "Grounded value in returns clause";
          return( FALSE );
        }

        break;

       case IFReduceAT:
       case IFRedLeftAT:
       case IFRedRightAT:
       case IFRedTreeAT:
        if ( e->pm > 0 || (!(e->pmark)) ) {
          vrtype++;
          *ReasonP = "Non-parallel or shared reduction";
          return( FALSE );
        }

        if ( (ee = FindImport( f->F_BODY, e->eport )) == NULL ) {
          vrtype++;
          *ReasonP = "Return export error";
          return( FALSE );
        }

        if ( ee->src->type != IFABuildAT ) {
          vrtype++;
          *ReasonP = "Return export cannot be built in place";
          return( FALSE );
        }

        break;

       default:
        vrtype++;
        *ReasonP = "Return export error (bad destination)";
        return( FALSE );
      }

      break;
      /* STOP:  CRAY ONLY ********************* */

     case IF_RECORD:
      if ( !cRay ) {
        vrtype++;
        *ReasonP = "Trying to vectorize a record";
        return( FALSE );
      }

      /* START: CRAY ONLY ********************* */
      if ( IsABRecord( e->info ) ) break;

      vrtype++;
      *ReasonP = "Trying to vectorize a non-block record";
      return( FALSE );
      /* STOP:  CRAY ONLY ********************* */

     case IF_INTEGER:
     case IF_REAL:
     case IF_DOUBLE:
      break;

     case IF_MULTIPLE:
      switch ( e->info->A_ELEM->type ) {
       case IF_ARRAY:
        if ( !cRay ) {
          vrtype++;
          *ReasonP = "Cannot vectorize array[non-scalar]";
          return( FALSE );
        }

        /* START: CRAY ONLY ********************* */
        if ( !IsBasic( e->info->A_ELEM->A_ELEM ) ) {
          vrtype++;
          *ReasonP = "Cannot vectorize array[array[non-scalar]]";
          return( FALSE );
        }
     
        switch ( e->dst->type ) {
         case IFSGraph:
          if ( e->iport != 0 ) {
            vrtype++;
            *ReasonP = "Non grounded multiple in loop";
            return( FALSE );
          }
     
          break;
     
         case IFReduceAT:
         case IFRedLeftAT:
         case IFRedRightAT:
         case IFRedTreeAT:
          if ( e->pm > 0 || (!(e->pmark)) ) {
            vrtype++;
            *ReasonP = "Shared or non-parallel reduction";
            return( FALSE );
          }
     
          if ( (ee = FindImport( f->F_BODY, e->eport )) == NULL ) {
            vrtype++;
            *ReasonP = "Return export type error (missing type)";
            return( FALSE );
          }
     
          if ( ee->src->type != IFABuildAT ) {
            vrtype++;
            *ReasonP = "Return export type error (source not built in place)";
            return( FALSE );
          }

          break;

         default:
          vrtype++;
          *ReasonP = "Some nebulous return export type error";
          return( FALSE );
        }

        break;
        /* STOP:  CRAY ONLY ********************* */

       case IF_RECORD:
        if ( !cRay ) {
          vrtype++;
          *ReasonP = "Trying to reduce records";
          return( FALSE );
        }

        /* START: CRAY ONLY ********************* */
        if ( IsABRecord( e->info->A_ELEM ) ) break;

        vrtype++;
        *ReasonP = "Trying to vectorize non-block records";
        return( FALSE );
        /* STOP:  CRAY ONLY ********************* */

       case IF_INTEGER:
       case IF_REAL:
       case IF_DOUBLE:
        break;
  
       default:
        vrtype++;
        *ReasonP = "Bad element type for return export";
        return( FALSE );
      }

      break;

     case IF_BUFFER:
      break;

     default:
      vrtype++;
      *ReasonP = "Bad type for return export";
      return( FALSE );
    }

    /* START: NOT NOT NOT CRAY */
    if ( !cRay ) {
      if ( UsageCount( f->F_RET, e->eport ) != 1 ) {
        for ( ee = f->F_RET->exp; ee != NULL; ee = ee->esucc )
          if ( ee->eport == e->eport ) {
            if ( ee->dst->type != IFAGatherAT ) {
              vrfan++;
              *ReasonP = "Return fanout not gathered in place";
              return( FALSE );
            }

            /* FIRST (LOWER BOUND) OR FIFTH (SIZE) */
            if ( ee->dst->imp != ee && 
                ee->dst->imp->isucc->isucc->isucc != ee ) {
              vrfan++;
              *ReasonP = "Return fanout can only control lower bound or size of return";
              return( FALSE );
            }
          }
      }
    }
    /* STOP:  NOT NOT NOT CRAY */
  }

  /* THE RETURN SUBGRAPH CAN ONLY COMPRISE GatherAT and Reduce (SUM, */
  /* PRODUCT, LEAST, AND GREATEST) nodes. THE REDUCTION NODES CANNOT */
  /* HAVE FILTERS.                                                   */

  /* SPECIAL ReduceAT NODES ARE OK ON THE CRAY                       */ 

  for ( n = f->F_RET->G_NODES; n != NULL; n = n->nsucc )
    switch ( n->type ) {
     case IFAGatherAT:
      break;

     case IFRedLeftAT:
     case IFRedRightAT:
     case IFRedTreeAT:
     case IFReduceAT:
      if ( !cRay ) {
        vrn++;
        *ReasonP = "Invalid reduction";
        return( FALSE );
      }

      break;

     case IFReduce:
     case IFRedLeft:
     case IFRedRight:
     case IFRedTree:
      if ( nopred ) {
        *ReasonP = "You used the secret -R option and have a reduction";
        return( FALSE );
      }

      switch ( n->imp->CoNsT[0] ) {
       case REDUCE_SUM:
       case REDUCE_PRODUCT:
       case REDUCE_GREATEST:
       case REDUCE_LEAST:
        if ( n->imp->isucc->isucc->isucc != NULL ) {
          vrrf++;
          *ReasonP = "Reduction is filtered";
          return( FALSE );
        }
        break;
                    
       default:
        vrn++;
        *ReasonP = "Reduce catenate cannot vectorize";
        return( FALSE );
      }

      break;

     default:
      vrn++;
      *ReasonP = "Invalid return operator";
      return( FALSE );
    }

  /* START: NOT NOT NOT CRAY */
  if ( !cRay ) {
    /* ALL BODY SUBGRAPH IMPORTS MUST CARRY int, real, or double VALUES */
    for ( i = f->F_BODY->imp; i != NULL; i = i->isucc )
      switch ( i->info->type ) {
       case IF_INTEGER:
       case IF_REAL:
       case IF_DOUBLE:
        break;

       default:
        vbtype++;
        *ReasonP = "Body defines a non-scalar value";
        return( FALSE );
      }

    /* ALL BODY SUBGRAPH EXPORTS MUST CARRY int, real, double, OR array */
    /* VALUES.  THE ARRAY VALUES MUST BE 1-D with int, real, or double  */
    /* CONSTITUENTS; THEY CAN ONLY BE USED BY AElement NODES.           */

    for ( e = f->F_BODY->exp; e != NULL; e = e->esucc )
      switch ( e->info->type ) {
       case IF_INTEGER:
       case IF_REAL:
       case IF_DOUBLE:
        break;

       case IF_ARRAY:
        switch ( e->info->A_ELEM->type ) {
         case IF_INTEGER:
         case IF_REAL:
         case IF_DOUBLE:
          if ( (ii = FindImport( f, e->eport )) == NULL ) {
            *ReasonP = "Vector value not imported";
            return( FALSE );
          }

          if ( ii->cm == -1 ) {
            *ReasonP = "Vector value being shared";
            return( FALSE );
          }

          break;

         default:
          vbtype++;
          *ReasonP = "Body uses an array[non-scalar] value";
          return( FALSE );
        }

        if ( e->dst->type != IFAElement ) {
          vbadst++;
          *ReasonP = "Body uses a vector in a non a[i] operation";
          return( FALSE );
        }

        break;

       default:
        vbtype++;
        *ReasonP = "Invalid type for vector";
        return( FALSE );
      }

    /* AN AElement NODE'S FIRST INPUT MUST BE A K PORT ARRAY. ITS      */
    /* SECOND INPUT CAN BE ANYTHING.                                   */
    for ( n = f->F_BODY->G_NODES; n != NULL; n = n->nsucc ) {
      switch( n->type ) {
       case IFTimes:
       case IFDiv:
       case IFAbs:
       case IFNeg:
       case IFDouble:
       case IFTrunc:
       case IFSingle:
        break;

       case IFPlus:
       case IFMinus:
        if ( n->imp == c || n->imp->isucc == c )
          for ( e = n->exp; e != NULL; e = e->esucc )
            if ( e->dst->type != IFAElement ) {
              vbpmfan++;
              *ReasonP = "Plus/Minus operation has fanout";
              return( FALSE );
            }
        break;

       case IFAElement:
        if ( n->imp->src != f->F_BODY ) {
          vbasrc++;
          *ReasonP = "Illegal source for a[i]";
          return( FALSE );
        }
  
        /* IF TRUE, THEN AElement NODE IS INVARIANT */
        if ( IsConst( n->imp->isucc ) ) {
          *ReasonP = "a[i] operation is invariant";
          return( FALSE );
        }
  
        for ( e = n->exp; e != NULL; e = e->esucc )
          switch ( e->dst->type  ) {
           case IFPlus:
           case IFMinus:
           case IFTimes:
           case IFDiv:
           case IFAbs:
           case IFNeg:
           case IFDouble:
           case IFTrunc:
           case IFSingle:
            break;

           case IFAElement:
            break;

           case IFSGraph:
            break;
  
           default:
            vbaeldst++;
            *ReasonP = "a[i] operation can only feed +,-,*,/,abs,neg,trunc,double,single";
            return( FALSE );
          }

        break;

       default:
        vbn++;
        *ReasonP = "Invalid body node";
        return( FALSE );
      }
    }
  }
  /* STOP:  NOT NOT NOT CRAY */
      
  vok++;
  return( TRUE );
}

/**************************************************************************/
/* GLOBAL **************        VectorSummary      ************************/
/**************************************************************************/
/* PURPOSE: Display vectorizing statistics                                */
/**************************************************************************/
void VectorSummary()
{
    FPRINTF( infoptr, "\n **** VECTORIZATION SUMMARY\n" );
    FPRINTF( infoptr, "\n\n" );

    FPRINTF( infoptr, " NUMBER OF VECTORIZATION ATTEMPTS:  %d\n", vnum );
    FPRINTF( infoptr, " NUMBER OF VECTORIZED LOOPS:        %d\n", vok );
    FPRINTF( infoptr, " NOT A SINGLE RangeGenerate:        %d\n", vrg );
    FPRINTF( infoptr, " ILLEGAL USE OF CONTROL IN BODY:    %d\n", vbc );
    FPRINTF( infoptr, " RETURN EXPORT TYPE ERROR:          %d\n", vrtype );
    FPRINTF( infoptr, " RETURN FANOUT:                     %d\n", vrfan );
    FPRINTF( infoptr, " ILLEGAL RETURN NODE:               %d\n", vrn );
    FPRINTF( infoptr, " BODY EXPORT AND IMPORT TYPE ERROR: %d\n", vbtype );
    FPRINTF( infoptr, " ILLEGAL ARRAY USAGE IN BODY:       %d\n", vbadst );
    FPRINTF( infoptr, " INVARIANTS IN BODY:                %d\n", vbinvar );
    FPRINTF( infoptr, " ILLEGAL + and - NODE FANOUT:       %d\n", vbpmfan );
    FPRINTF( infoptr, " ILLEGAL AElement SOURCE:           %d\n", vbasrc );
    FPRINTF( infoptr, " ILLEGAL AElement INDEX:            %d\n", vbaelx );
    FPRINTF( infoptr, " ILLEGAL AElement DESTINATION:      %d\n", vbaeldst );
    FPRINTF( infoptr, " ILLEGAL BODY NODE:                 %d\n", vbn );
    FPRINTF( infoptr, " REDUCTION FILTER:                  %d\n", vrrf );
}
