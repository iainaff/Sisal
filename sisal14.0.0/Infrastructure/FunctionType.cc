/**************************************************************************/
/* FILE   **************      FunctionType.cc      ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 30 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "IFObject.hh"
#include "Info.hh"
#include "FunctionType.hh"
#include "ChainType.hh"
#include "TupleType.hh"

FunctionType::FunctionType(TupleType* arguments,TupleType* results,Module* M=0) 
   : Info(M), mArguments(arguments), mResults(results)
{
}

FunctionType::~FunctionType() {
   cerr << "Destroy FunctionType" << endl;
}

int FunctionType::i2() const {
   return 3;
}

int FunctionType::i3() const {
   return (mArguments && mArguments->size())?(mArguments->i1()):(0);
}

int FunctionType::i4() const {
   return (mResults)?(mResults->i1()):(0);
}

string FunctionType::canonicalName() const {
   string result = itoa(i2());
   result += "_";
   result += mArguments->canonicalName();
   result += "_returns_";
   result += mResults->canonicalName();
   return result;
}
   
