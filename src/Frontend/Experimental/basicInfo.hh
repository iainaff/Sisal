#ifndef BASICINFO_HH
#define BASICINFO_HH

#include "info.hh"

namespace sisalc {
   using sisalc::info;

   class basicInfo: public info {
   public:
      typedef enum { BOOL=0, CHAR=1, DOUBLE=2, INTEGER=3,
                     NIL=4, REAL=5, WILDTYPE=6, BRECORD=7,
                     PTR_DOUBLE=8, PTR_INTEGER=9, PTR_REAL=10,
                     PTR = 11 } BaseCode;
      // -----------------------------------------------
      // Constructors with SP<> shadows
      // -----------------------------------------------
      basicInfo(BaseCode basic=WILDTYPE) : mBasic(basic), info(info::BASIC) {}
      static SP<info> ctor(BaseCode basic=WILDTYPE) { return SP<info>(new basicInfo(basic)); }

      // -----------------------------------------------
      // IF Labeling
      // -----------------------------------------------
      virtual int i3() const { return (int)mBasic; }

   protected:
      BaseCode mBasic;
   };
}

#endif
