#ifndef INFO_HH
#define INFO_HH

#include "SP.hh"
#include "IFObject.hh"
#include "module.hh"

namespace sisalc {
   class module;

   class info : public IFObject {
   public:
      typedef enum { ARRAY=0, BASIC=1, FIELD=2, FUNCTION=3,
                     MULTIPLE=4, RECORD=5, STREAM=6, TAG=7, TUPLE=8, 
                     UNION=9, UNKNOWN=10, BUFFER=11, SET=12,
                     REDUCTION=13, FOREIGN=14 } TypeCode;

      // -----------------------------------------------
      // Constructors with SP<> shadows
      // -----------------------------------------------
      info(TypeCode kind=UNKNOWN);
      static SP<info> ctor(TypeCode kind=UNKNOWN) { return SP<info>(new info(kind)); }

      // -----------------------------------------------
      // Reference counted registration
      // ----------------------------------------------
   protected:
      SP<info> mSelf;
   public:
      void self(SP<info>);
      SP<info> self() const;
      static SP<info> null;

      // -----------------------------------------------
      // Output
      // -----------------------------------------------
      //virtual void writeSelf(ostream&) const;
      virtual bool valid() const;

      // -----------------------------------------------
      // IF Labeling
      // -----------------------------------------------
      //int offset(?) const
      int label() const;
   protected:
      virtual char letter() const { return 'T'; }
      virtual int i1() const;
      virtual int i2() const;
      virtual int i3() const;
      virtual int i4() const;
      
      // -----------------------------------------------
      // Interconnect
      // -----------------------------------------------
   public:
      void setParent(SP<module> m);

   protected:
      unsigned int mKind;
      SP<info> mInfo1;
      SP<info> mInfo2;
      SP<module> mParent;
   };

   typedef SP<info> INFO;
}
#endif
