#ifndef IFOBJECT_HH
#define IFOBJECT_HH

#include "SP.hh"
#include <string>

namespace sisalc {

   class IFObject {
   public:
      // -----------------------------------------------
      // Constructors
      // -----------------------------------------------
      IFObject();
      IFObject(string&,unsigned int);

      // -----------------------------------------------
      // Output
      // -----------------------------------------------
      virtual bool valid() const = 0;
      virtual void writeSelf(ostream&) const;

      // -----------------------------------------------
      // IF Labeling
      // -----------------------------------------------
   protected:
      virtual char letter() const { return '?'; }
      virtual int i1() const { return -1; }
      virtual int i2() const { return -1; }
      virtual int i3() const { return -1; }
      virtual int i4() const { return -1; }
      virtual int i5() const { return -1; }
      virtual string value() const { return ""; }

   protected:
      string mSourceFile;
      unsigned int mSourceLine;
   };

   ostream& operator<<(ostream& os, const IFObject& object);
}
#endif
