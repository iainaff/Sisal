#ifndef TUPLEINFO_HH
#define TUPLEINFO_HH

#include "info.hh"

namespace sisalc {
   using sisalc::info;
   using sisalc::INFO;

   class tupleInfo: public info {
   public:
      // -----------------------------------------------
      // Constructors with SP<> shadows
      // -----------------------------------------------
      tupleInfo(INFO base, INFO chain=info::null) : info(info::TUPLE) { assert(base.get()); mInfo1 = base; mInfo2=chain; }
      static SP<info> ctor(INFO base, INFO chain=info::null) { return SP<info>(new tupleInfo(base,chain)); }

   protected:
      virtual int i3() const {
         return (mInfo1.get())?mInfo1->label():0; }
      virtual int i4() const {
         return (mInfo2.get())?mInfo2->label():0; }

   };
}

#endif
