/**************************************************************************/
/* FILE   **************        TupleType.hh       ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 30 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef TUPLETYPE_HH
#define TUPLETYPE_HH

class ChainType;

class TupleType : public ChainType {
public:
   TupleType(Module* M=0) : ChainType(IFTUPLE,M) {}
   virtual ~TupleType() {}
};

#endif
