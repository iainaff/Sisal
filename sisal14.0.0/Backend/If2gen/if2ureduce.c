#include "world.h"


/**************************************************************************/
/* GLOBAL **************        PrintUReduceInit       ********************/
/**************************************************************************/
/* PURPOSE: PRINT USER-DEFINED REDUCTION NODE INITIALIZATION OF e->temp   */
/* WITH THE INITIAL VALUE ASSOCIATED WITH THE REDUCTION FUNCTION DEFINED  */
/* BY EDGE f TO output.                                                   */
/**************************************************************************/

void PrintUReduceInit( indent, n )
int   indent;
PNODE n;
{
    UNIMPLEMENTED( "IFUReduceInit" );
    PrintIndentation( indent );
    FPRINTF( output, "/* UReduceInit begin */\n" );

    PrintProducerModifiers( indent, n->R_INIT );
    /*PrintReturnInit( indent, n->R_RET );*/
    PrintGraph( indent, n->R_INIT );

    PrintIndentation( indent );
    FPRINTF( output, "/* UReduceInit end */\n" );

    return;
}

/**************************************************************************/
/* GLOBAL **************        PrintUReduceUpd        ********************/
/**************************************************************************/
/* PURPOSE: PRINT USER-DEFINED REDUCTION NODE UPDATE OF e->temp           */
/* WITH THE NEW VALUE ASSOCIATED WITH THE REDUCTION FUNCTION DEFINED      */
/* BY EDGE f TO output.                                                   */
/**************************************************************************/

void PrintUReduceUpd( indent, n )
int   indent;
PNODE n;
{
    UNIMPLEMENTED( "IFUReduceUpd" );
    PrintIndentation( indent );
    FPRINTF( output, "/* UReduceUpd begin */\n" );

    PrintProducerModifiers( indent, n->R_BODY );
    PrintGraph( indent, n->R_BODY );

    PrintIndentation( indent );
    FPRINTF( output, "/* UReduceUpd end */\n" );

    return;
}

/**************************************************************************/
/* GLOBAL **************        PrintUReduceRapUp        ******************/
/**************************************************************************/
/* PURPOSE: PRINT USER-DEFINED REDUCTION NODE WRAP OF e->temp TO output   */
/* THIS IS DONE AFTER THE LOOP HAS COMPLETED TO FINALIZE THE RESULTS.     */
/**************************************************************************/

void PrintUReduceRapUp( indent, n )
int   indent;
PNODE n;
{
    UNIMPLEMENTED( "IFUReduceRapUp" );
    PrintIndentation( indent );
    FPRINTF( output, "/* UReduceRapUp begin */\n" );

    PrintConsumerModifiers( indent, n );
    PrintReturnRapUp( indent, n->R_RET );

    PrintIndentation( indent );
    FPRINTF( output, "/* UReduceRapUp end */\n" );

    return;
}

/**************************************************************************/
/* GLOBAL **************        PrintUReduceSlice        ******************/
/**************************************************************************/
/* PURPOSE: PRINT USER-DEFINED REDUCTION NODE SLICE.  EACH Out FIELD      */
/* IS INITIALIZED ACCORDING TO THE MULTIPLE NODE BUILDING IT.             */
/**************************************************************************/

void PrintUReduceSlice( indent, f, e )
int   indent;
PEDGE f;
PEDGE e;
{
    UNIMPLEMENTED( "IFUReduceSlice" );
    PrintIndentation( indent );
    FPRINTF( output, "/* ReduceSlice begin */\n" );

    PrintIndentation( indent );
    FPRINTF( output, "/* ReduceSlice end */\n" );

    return;
}
