/**************************************************************************/
/* FILE   **************        ChainType.hh       ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 30 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include "IFObject.hh"
#include <string>

#ifndef CHAINTYPE_HH
#define CHAINTYPE_HH

class Info;
class Module;

class ChainType : public Info {
public:
   typedef enum { IFFIELD=2, IFTAG=7, IFTUPLE=8, IFWILD=0 } KindOfChain;
   static ChainType* NIL;

   ChainType(KindOfChain,Module* M=0);
   virtual ~ChainType();

   virtual void append(Info*);

   virtual const char* letter() const;
   virtual int i2() const;
   virtual void endDump(ostream& os,unsigned level=0) const;

   virtual string canonicalName() const;

   size_t size() const;

protected:
   KindOfChain mKind;
   vector< IF<Info> > mInfo;
};

#endif
