/**************************************************************************/
/* FILE   **************        FieldType.hh       ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 30 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef FIELDTYPE_HH
#define FIELDTYPE_HH

class ChainType;

class FieldType : public ChainType {
public:
   FieldType(Module* M=0) : ChainType(IFFIELD,M) {}
   virtual ~FieldType() {}
};

#endif
