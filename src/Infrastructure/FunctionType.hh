/**************************************************************************/
/* FILE   **************      FunctionType.hh      ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 30 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include "IFObject.hh"

#ifndef FUNCTIONTYPE_HH
#define FUNCTIONTYPE_HH

class TupleType;
class Info;
class FunctionType : public Info {
public:
   FunctionType(TupleType*,TupleType*,Module* M=0);
   ~FunctionType();

   virtual int i2() const;
   virtual int i3() const;
   virtual int i4() const;

   virtual string canonicalName() const;

protected:
   IF<TupleType> mArguments;
   IF<TupleType> mResults;
};


#endif
