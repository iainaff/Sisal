/**************************************************************************/
/* FILE   **************        ChainType.cc       ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 30 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/

#include "IFObject.hh"
#include "Info.hh"
#include "ChainType.hh"

ChainType::ChainType(KindOfChain kind,Module* M=0)
   : Info(M), mKind(kind)
{
}

ChainType::~ChainType() {
   cerr << "Destroy ChainType" << endl;
}

void ChainType::append(Info* element) {
   mInfo.push_back(element);
}

const char* ChainType::letter() const {
   return "{ Type";
}

int ChainType::i2() const {
   return mKind;
}

void ChainType::endDump(ostream& os,unsigned level=0) const {
   dumpObjects(mInfo.begin(),mInfo.end(), os, level);
   indent(os,level-1);
   os << "}" << endl;
   Info::endDump(os,level);
}

string ChainType::canonicalName() const {
   // chainType _ chainLength _ n*{element}
   string result = itoa(i2());
   result += "_";
   result += itoa(mInfo.size());
   for(vector< IF<Info> >::const_iterator i = mInfo.begin();
       i != mInfo.end();
       ++i ) {
      result += "_";
      result += (*i)->canonicalName();
   }
   return result;
}


size_t ChainType::size() const {
   return mInfo.size();
}
