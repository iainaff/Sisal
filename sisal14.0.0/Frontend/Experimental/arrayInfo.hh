#ifndef ARRAYINFO_HH
#define ARRAYINFO_HH

#include "info.hh"

namespace sisalc {
   using sisalc::info;
   using sisalc::INFO;

   class arrayInfo: public info {
   public:
      // -----------------------------------------------
      // Constructors with SP<> shadows
      // -----------------------------------------------
      arrayInfo(INFO base) : info(info::ARRAY) { assert(base); mInfo1 = base; }
      static SP<info> ctor(INFO base) { return SP<info>(new arrayInfo(base)); }
   };
}

#endif
