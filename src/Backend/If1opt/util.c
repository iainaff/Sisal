/**************************************************************************/
/* FILE   **************           util.c          ************************/
/**************************************************************************/
/* Author: Dave Cann                                                      */
/* Update: Patrick Miller -- Ansi support (Dec 2000)                      */
/* Copyright (C) University of California Regents                         */
/**************************************************************************/
/*
 * $Log$
 * Revision 1.2  2001/01/01 05:46:22  patmiller
 * Adding prototypes and header info -- all will be broken
 *
 * Revision 1.1.1.1  2000/12/31 17:56:43  patmiller
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
 * Revision 1.1  1993/01/07  00:37:58  miller
 * Make changes for LINT and combined files.
 */
/**************************************************************************/


#include "world.h"


/**************************************************************************/
/* GLOBAL  **************      OptIsInvariant      ************************/
/**************************************************************************/
/* PURPOSE: RETURN TRUE IF NODE n IS LOOP INVARIANT. RETURN AND GENERATE  */
/*          NODES ARE NEVER CONSIDERED INVARIANT.  NODE n IS INVARIANT IF */
/*          ITS IMPORTS ARE CONSTANTS OR IMPORTS TO ITS OWNING COMPOUND   */
/*          NODE.                                                         */
/**************************************************************************/

int OptIsInvariant( n )
PNODE n;
{
    register PEDGE i;

    if ( IsReturn( n ) || IsGenerate( n ) )
        return( FALSE );

    if ( glue && IsCall( n ) )
      return( FALSE );

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
/* GLOBAL  **************      DecodeIndexing       ************************/
/**************************************************************************/
/* PURPOSE: DECONDE INDEXING OPERATIONS AElementN, AElementM, and         */
/*          AElementP IN GRAPH b.                                         */
/**************************************************************************/

void DecodeIndexing( b )
PNODE b;
{
  register PNODE n;
  register PNODE nn;
  register PEDGE ee;
  register PEDGE ii;
  register PEDGE iii;

  for ( n = b->G_NODES; n != NULL; n = n->nsucc ) {
    /* DO SOME CLEANUP!!! */
    if ( n->label < 0 )
      n->label = -(n->label);

    switch ( n->type ) {
      case IFAElementN:
        n->type = IFAElement;
        break;

      case IFAElementM:
      case IFAElementP:
        if ( n->type == IFAElementM )
          nn = NodeAlloc( ++maxint, IFMinus );
        else
          nn = NodeAlloc( ++maxint, IFPlus );

        CopyPragmas( n, nn );
        LinkNode( n->npred, nn );

        ii = n->imp->isucc;
        iii = ii->isucc;

        UnlinkImport( ii );
        UnlinkImport( iii );

        ii->iport  = 1;
        iii->iport = 2;

        LinkImport( nn, ii );
        LinkImport( nn, iii );

        ee = EdgeAlloc( nn, 1, n, 2 );
        ee->info = iii->info;

        LinkExport( nn, ee );
        LinkImport( n, ee );

        n->type = IFAElement;
        break;

      default:
        break;
      }
    }
}


/**************************************************************************/
/* GLOBAL **************      EncodeIndexing       ************************/
/**************************************************************************/
/* PURPOSE: ENCODE INDEXING OPERATIONS OF THE FORM a[i], a[i+I], AND      */
/*          a[i-I], WHERE a IS A K PORT ARRAY AND i and I ARE ONE OF THE  */
/*          FOLLOWING: CONSTANT, K PORT VALUE, OR LOOP CONTROL REFERENCE  */
/*          (HAVING PORT NUMBER cport).                                   */
/**************************************************************************/

void EncodeIndexing( b, cport, maelmp )
PNODE b;
int   cport;
int   *maelmp;
{
  register PNODE n;
  register PNODE op;
  register PEDGE i;
  register int   eport;

  for ( n = b->G_NODES; n != NULL; n = n->nsucc ) {
    /* DO SOME CLEANUP AND MAKE SURE ALL LABELS ARE GREATER THAN ZERO */
    if ( n->label < 0 )
      n->label = -(n->label);

    if ( !IsAElement( n ) ) continue;

    /* MAKE SURE THE SOURCE IS A K VALUE ARRAY OR SPECIAL AElement EXPORT */

    if ( !IsArray( n->imp->info ) )
      continue;

    if ( !IsConst( n->imp ) ) {
      if ( !IsSGraph( n->imp->src ) ) {
        if ( maelmp ) continue;

        switch ( n->imp->src->type ) {
          case IFAElementN:
          case IFAElementP:
          case IFAElementM:
            break;

          default:
            continue;
          }
        }
      else if ( !IsImport( b->G_DAD, n->imp->eport ) )
        continue;
      }
      
    /* CHECK THE INDEXING */

    if ( IsConst( n->imp->isucc ) ) {
      n->type = IFAElementN;
      if ( maelmp ) (*maelmp)++;
      continue;
      }

    op = n->imp->isucc->src;

    if ( IsSGraph( op ) ) {
      eport = n->imp->isucc->eport;

      if ( IsImport( b->G_DAD, eport ) || eport == cport ) {
        n->type = IFAElementN;
        if ( maelmp ) (*maelmp)++;
        }

      continue;
      }

    switch ( op->type ) {
      case IFPlus:
      case IFMinus:
        OptNormalizeNode( op );

        if ( !IsConst( op->imp ) ) {
          if ( !IsSGraph( op->imp->src ) )
            break;

          eport = op->imp->eport;

          if ( !( IsImport( b->G_DAD, eport ) || eport == cport ) )
            break;
          }

        if ( !IsConst( op->imp->isucc ) ) {
          if ( !IsSGraph( op->imp->isucc->src ) )
            break;

          eport = op->imp->isucc->eport;

          if ( !( IsImport( b->G_DAD, eport ) || eport == cport ) )
            break;
          }
            
        n->type = (IsPlus(op))? IFAElementP : IFAElementM;

        i = n->imp->isucc;
        UnlinkImport( i );

        CopyEdgeAndLink( op->imp, n, 2 );
        CopyEdgeAndLink( op->imp->isucc, n, 3 );

        UnlinkExport( i );
        OptRemoveDeadNode( op ); /* WILL REMOVE op IF op->exp IS NULL */
        if ( maelmp ) (*maelmp)++;
        break;

      default:
        break;
      }
    }
}


