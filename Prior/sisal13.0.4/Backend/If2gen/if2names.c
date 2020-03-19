/* $Log$
 * Revision 1.3  1994/02/15  23:40:53  miller
 * Changes to allow new IF1/2 types (complex, typesets, etc...)
 *
 * Revision 1.2  1993/01/07  00:38:26  miller
 * Make changes for LINT and combined files.
 *
 * Revision 1.1  1992/12/22  23:32:04  miller
 * Merged removed files into ../Library
 * */

#include "world.h"


/**************************************************************************/
/* GLOBAL **************      GetCopyFunction      ************************/
/**************************************************************************/
/* PURPOSE: RETURN COMPONENT COPY FUNCTION NAME FOR ARRAY i.              */
/**************************************************************************/

char *GetCopyFunction( i )
PINFO i;
{
  static char buf[100];                      /* COPY FUNCTION NAME BUFFER */

  if ( i->A_ELEM->type<TYPE_CODE_FIRST || i->A_ELEM->type>BASE_CODE_LAST) {
    SPRINTF( buf, "Compiler_Error" );
    return (buf);
  }

  switch ( i->A_ELEM->type ) {
  case IF_ARRAY:
    SPRINTF( buf, "ArrArrCopy" );
    break;
  case IF_BASIC:
  case IF_FIELD:
  case IF_FUNCTION:
  case IF_MULTIPLE:
    SPRINTF( buf, "Compiler_Error" );
    break;
  case IF_RECORD:
    SPRINTF( buf, "Arr%sCopy", i->A_ELEM->cname );
    break;
  case IF_STREAM:
  case IF_TAG:
  case IF_TUPLE:
    SPRINTF( buf, "Compiler_Error" );
    break;
  case IF_UNION:
    SPRINTF( buf, "Arr%sCopy", i->A_ELEM->cname );
    break;
  case IF_UNKNOWN:
    SPRINTF( buf, "Compiler_Error" );
    break;
  case IF_BUFFER:
  case IF_SET:
    SPRINTF( buf, "Compiler_Error" );
    break;
  case IF_BOOL:
    SPRINTF( buf, "ArrBoolCopy" );
    break;
  case IF_CHAR:
    SPRINTF( buf, "ArrCharCopy" );
    break;
  case IF_DOUBLE:
    SPRINTF( buf, "ArrDblCopy" );
    break;
  case IF_INTEGER:
    SPRINTF( buf, "ArrIntCopy" );
    break;
  case IF_NULL:
    SPRINTF( buf, "ArrNullCopy" );
    break;
  case IF_REAL:
    SPRINTF( buf, "ArrFltCopy" );
  case IF_NONTYPE:
    SPRINTF( buf, "Compiler_Error" );
    break;
  case IF_BRECORD:
    SPRINTF( buf, "Arr%sCopy", i->A_ELEM->cname );
    break;
  case IF_PTR_DOUBLE:
  case IF_PTR_INTEGER:
  case IF_PTR_REAL:
  case IF_PTR:
    SPRINTF( buf, "Compiler_Error" );
    break;
  default:
    SPRINTF( buf, "Compiler_Error" );
  }
  return( buf );
}


/**************************************************************************/
/* GLOBAL **************      GetReadFunction      ************************/
/**************************************************************************/
/* PURPOSE: RETURN COMPONENT READ FUNCTION NAME FOR ARRAY INFO CODE.      */
/**************************************************************************/

char *GetReadFunction( code )
  int code;
{
  static char buf[100];                      /* COPY FUNCTION NAME BUFFER */

  if ( code<TYPE_CODE_FIRST || code>BASE_CODE_LAST) {
    SPRINTF( buf, "Compiler_Error" );
    return (buf);
  }

  switch ( code ) {
  case IF_ARRAY:
  case IF_BASIC:
  case IF_FIELD:
  case IF_FUNCTION:
  case IF_MULTIPLE:
  case IF_RECORD:
  case IF_STREAM:
  case IF_TAG:
  case IF_TUPLE:
  case IF_UNION:
    SPRINTF( buf, "Compiler_Error" );
    break;
  case IF_UNKNOWN:
    SPRINTF( buf, "Unknown_Error" );
    break;
  case IF_BUFFER:
  case IF_SET:
    SPRINTF( buf, "Compiler_Error" );
    break;
  case IF_BOOL:
    SPRINTF( buf, "ReadBool" );
    break;
  case IF_CHAR:
    SPRINTF( buf, "ReadChar" );
    break;
  case IF_DOUBLE:
    SPRINTF( buf, "ReadDbl" );
    break;
  case IF_INTEGER:
    SPRINTF( buf, "ReadInt" );
    break;
  case IF_NULL:
    SPRINTF( buf, "ReadNil" );
    break;
  case IF_REAL:
    SPRINTF( buf, "ReadFlt" );
    break;
  case IF_NONTYPE:
  case IF_BRECORD:
  case IF_PTR_DOUBLE:
  case IF_PTR_INTEGER:
  case IF_PTR_REAL:
  case IF_PTR:
  default:
    SPRINTF( buf, "Compiler_Error" );
  }
  return( buf );
}


/**************************************************************************/
/* GLOBAL **************      GetWriteFunction      ***********************/
/**************************************************************************/
/* PURPOSE: RETURN COMPONENT WRITE FUNCTION NAME FOR ARRAY INFO CODE.     */
/**************************************************************************/

char *GetWriteFunction( code )
  int code;
{
  static char buf[100];                      /* COPY FUNCTION NAME BUFFER */

  if ( code<TYPE_CODE_FIRST || code>BASE_CODE_LAST) {
    SPRINTF( buf, "Compiler_Error" );
    return (buf);
  }

  switch ( code ) {
  case IF_ARRAY:
  case IF_BASIC:
  case IF_FIELD:
  case IF_FUNCTION:
  case IF_MULTIPLE:
  case IF_RECORD:
  case IF_STREAM:
  case IF_TAG:
  case IF_TUPLE:
  case IF_UNION:
  case IF_UNKNOWN:
  case IF_BUFFER:
  case IF_SET:
    SPRINTF( buf, "Compiler_Error");
    break;
  case IF_BOOL:
    SPRINTF( buf, "WriteBool" );
    break;
  case IF_CHAR:
    SPRINTF( buf, "WriteChar" );
    break;
  case IF_DOUBLE:
    SPRINTF( buf, "WriteDbl" );
    break;
  case IF_INTEGER:
    SPRINTF( buf, "WriteInt" );
    break;
  case IF_NULL:
    SPRINTF( buf, "WriteNil" );
    break;
  case IF_REAL:
    SPRINTF( buf, "WriteFlt" );
    break;
  case IF_NONTYPE:
  case IF_BRECORD:
  case IF_PTR_DOUBLE:
  case IF_PTR_INTEGER:
  case IF_PTR_REAL:
  case IF_PTR:
  default:
    SPRINTF( buf, "Compiler_Error" );
  }
  return( buf );
}


/**************************************************************************/
/* GLOBAL **************    GetIncRefCountName     ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE INCREMENT REFERENCE COUNT MACRO NAME FOR i. THE    */
/*          TYPE OF i IS ASSUMED TO BE UNION, RECORD, OR ARRAY.           */
/**************************************************************************/

char *GetIncRefCountName( i )
PINFO i;
{
  switch ( i->type ) {
    case IF_RECORD:
    case IF_UNION:
    case IF_ARRAY:
      return( "IncRefCount" );
    
    default:
      Error2( "GetIncRefCountName", "ILLEGAL TYPE" );
    }

  return NULL;
}


/**************************************************************************/
/* GLOBAL **************    GetSetRefCountName     ************************/
/**************************************************************************/
/* PURPOSE: RETURN THE SET REFERENCE COUNT MACRO NAME FOR i. THE          */
/*          TYPE OF i IS ASSUMED TO BE UNION, RECORD, OR ARRAY.           */
/**************************************************************************/

char *GetSetRefCountName( i )
PINFO i;
{
  switch ( i->type ) {
    case IF_RECORD:
    case IF_UNION:
    case IF_ARRAY:
      return( "SetRefCount" );

    default:
      Error2( "GetSetRefCountName", "ILLEGAL INFO TYPE" );
    }

  return NULL;
}
