#ifndef FUNCTIONINFO_HH
#define FUNCTIONINFO_HH

#include "info.hh"

namespace sisalc {
   using sisalc::info;
   using sisalc::INFO;

   class functionInfo: public info {
   public:
      // -----------------------------------------------
      // Constructors with SP<> shadows
      // -----------------------------------------------
      functionInfo(INFO args, INFO rets) : info(info::FUNCTION) { mInfo1 = args; mInfo2=rets; }
      static SP<info> ctor(INFO args, INFO rets) { return SP<info>(new functionInfo(args,rets)); }

   protected:
      virtual int i3() const { return (mInfo1.get())?mInfo1->label():0; }
      virtual int i4() const { return (mInfo2.get())?mInfo2->label():0; }

   };
}

#endif
