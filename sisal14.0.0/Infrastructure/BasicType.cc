/**************************************************************************/
/* FILE   **************        BasicType.cc       ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 30 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "IFObject.hh"
#include "Info.hh"
#include "BasicType.hh"

BasicType* BasicType::NIL = 0;

BasicType::BasicType(KindOfBasic kind, Module* M )
   : Info(M), mKind(kind)
{
   assert(mKind >= IFBOOL && mKind <= IFWILD);
}

BasicType::~BasicType() {
   cerr << "Destroy basictype " << this << endl;
}

int BasicType::i2() const {
   return 1;
}

int BasicType::i3() const {
   return mKind;
}

string BasicType::canonicalName() const {
   switch( mKind ) {
   case IFBOOL: return "boolean";
   case IFCHAR: return "character";
   case IFDOUBLE: return "double";
   case IFINTEGER: return "integer";
   case IFNULL: return "null";
   case IFREAL: return "real";
   default: return Info::canonicalName();
   }
}
