#include "info.hh"

namespace sisalc {
   INFO info::null(0,true); // Info locked to null

   info::info(TypeCode kind) : mKind(kind) {
   }

   bool info::valid() const {
      return 1;
   }
   void info::setParent(MODULE m) {
      mParent = m;
      if ( mInfo1.get() ) mInfo1->setParent(m);
      if ( mInfo2.get() ) mInfo2->setParent(m);
   }

   void info::self(INFO p) {
      assert(p.get()==this);
      mSelf = p;
   }
   INFO info::self() const {
      assert(mSelf.get() == this);
      return mSelf;
   }

   int info::label() const {
      assert(mParent.get());
      return mParent->offset(self());
   }

   int info::i1() const { return label(); }
   int info::i2() const { return mKind; }
   int info::i3() const { return (mInfo1.get())?mInfo1->label():-1; }
   int info::i4() const { return (mInfo2.get())?mInfo2->label():-1; }
}

