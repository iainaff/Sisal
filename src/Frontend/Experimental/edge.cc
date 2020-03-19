/**************************************************************************/
/* FILE   **************          edge.cc          ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#include "IFCore.hh"

namespace sisalc {

   edge::edge() 
   {
   }

   edge::edge(const info* type)
      : mSourcePort(0),
        mDestinationPort(0),
        mType(type)
   {
   }


   bool edge::valid() const {
      return mDestination && mSource;
   }

   int edge::i1() const {
      return (mSource)?(mSource->label()):0;
   }
   int edge::i2() const {
      return mSourcePort;
   }
   int edge::i3() const {
      return (mDestination)?(mDestination->label()):0;
   }
   int edge::i4() const {
      return mDestinationPort;
   }
   int edge::i5() const {
      if ( mType ) {
         mType->label();
      } else {
         return 0;
      }
   }

   void edge::setDestination(node* n, int p) {
      mDestination = n;
      mDestinationPort = p;
   }

   void edge::setSource(node* n, int p) {
      mSource = n;
      mSourcePort = p;
   }

   void edge::setType(const info* I) {
      mType = I;
   }
}
