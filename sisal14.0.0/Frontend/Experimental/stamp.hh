#ifndef STAMP_HH
#define STAMP_HH

#ifndef IFCORE_HH
error "Include only as part of IFCore.hh";
#endif

// -----------------------------------------------
// Stamp is a special kind of formatted comment
// -----------------------------------------------
class stamp : public IFObject {
public:
   stamp() : mLetter(' '), mMessage("") {}
   stamp(char letter) : mLetter(letter), mMessage("") {}
   stamp(char letter, const string& message) : mLetter(letter), mMessage(message) {}
   stamp(char letter, const char* message) : mLetter(letter), mMessage(message) {}
   
   virtual bool valid() const { return true; }

   void writeSelf(ostream& os) const {
      os << "C$  " << mLetter << ' ' << mMessage << endl;
   }
protected:
   virtual char letter() const { return 'C'; }
private:
   char mLetter;
   string mMessage;
};

#endif
