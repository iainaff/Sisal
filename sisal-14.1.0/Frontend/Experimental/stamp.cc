#include "stamp.hh"

namespace sisalc {
   ostream& operator<<(ostream& os, const stamp& object) {
      object.writeSelf(os);
      return os;
   }
}
