#ifndef STAMP_HH
#define STAMP_HH

#include <string>
namespace sisalc {
   class stamp {
   public:
      stamp()
         : mLetter(' '), mMessage("")
      {
      }
      stamp(char letter)
         : mLetter(letter), mMessage("")
      {
      }
      stamp(char letter, const string& message)
         : mLetter(letter), mMessage(message)
      {
      }
      stamp(char letter, const char* message)
         : mLetter(letter), mMessage(message)
      {
      }
      void writeSelf(ostream& os) const {
         os << "C$  " << mLetter << ' ' << mMessage << endl;
      }
   private:
      char mLetter;
      string mMessage;
   };

   ostream& operator<<(ostream& os, const stamp& object);
}
#endif
