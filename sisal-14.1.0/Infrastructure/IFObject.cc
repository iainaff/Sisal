#include "assert.h"
#include "IFObject.hh"
#include "Pragma.hh"

IFObject* IFObject::NIL = 0;

IFObject::IFObject(IFObject* parent) 
   : mParent(parent)
{
}

IFObject::~IFObject() 
{
   for(int i=0;i<registered.size();++i) {
      IFObject** x = reinterpret_cast<IFObject**>(registered[i]);

      if ( x ) *x = 0;
   }
}

void IFObject::renumbered() {
}

void IFObject::write(ostream& os) const {
   os << "< " << object() << ' ' << this << " >";
}

const char* IFObject::object() const {
   return "????";
}

const char* IFObject::letter() const {
   return "?";
}

void IFObject::endDump(ostream&,unsigned int) const {
}

Pragma& IFObject::pragma(string s) {
   return *(new Pragma);
}
const Pragma& IFObject::pragma(string s) const {
   return *(new Pragma);
}

IFObject* IFObject::parent() {
   return mParent.pointer();
}

const IFObject* IFObject::parent() const {
   return mParent.pointer();
}

void IFObject::parent(IFObject* P) {
   // Reset parent (which releases old reference)
   mParent = P;

   // This may result in a renumbering
   renumbered();
}

unsigned int IFObject::offset() const {
   return IFObject::UNDEFINED;
}

void IFObject::indent(ostream& os,unsigned int level) {
   for(int i=0; i<level; ++i) os << "  ";
}

void IFObject::LabelEntry(ostream& os, int x, const char* space, bool asInteger) const {
   switch ( (LabelType)x ) {
   case BLANK:
      break;
   case UNDEFINED:
      os << space << '?';
      break;
   default:
      if ( asInteger ) {
         os << space << x;
      } else {
         assert( x >= 0 && x < 256 );
         os << space << static_cast<char>(x);
      }
   }
}

int IFObject::i1() const { return BLANK; }
int IFObject::i2() const { return BLANK; }
int IFObject::i3() const { return BLANK; }
int IFObject::i4() const { return BLANK; }
int IFObject::i5() const { return BLANK; }
int IFObject::i6() const { return BLANK; }
const char* IFObject::value() const { return 0; }

void IFObject::dump(ostream& os,unsigned int level) const {
   // Dump self
   indent(os,level);
   os << letter();
   LabelEntry(os,i1()," ",isLabelInteger());
   LabelEntry(os,i2()," ");
   LabelEntry(os,i3(),"\t");
   LabelEntry(os,i4()," ");
   LabelEntry(os,i5(),"\t");
   LabelEntry(os,i6(),"\t");
   const char* xv = value();
   if ( xv ) {
      os << ' ';
      if ( isValueQuoted() ) os << '"';
      os << xv;
      if ( isValueQuoted() ) os << '"';
   }
   os << endl;

   // Dump other stuff
   endDump(os,level+1);
}

ostream& operator<<(ostream& os, const IFObject& x) {
   x.write(os);
   return os;
}

void IFObject::interested(IFObject** x) {
   // Find an empty spot
   vector<void*>::iterator location = find(registered.begin(),
                                           registered.end(),
                                           (void*)0);
   if ( location != registered.end() ) {
      *location = x;
   } else {
      registered.push_back(x);
   }
}

void IFObject::disinterested(IFObject** x) {

   // Push a 0 into pointer
   vector<void*>::iterator location = find(registered.begin(),
                                           registered.end(),
                                           x);
   if ( location != registered.end() ) {
      *location = x;
   }
}

bool IFObject::isLabelInteger() const {
   return true;
}

bool IFObject::isValueQuoted() const {
   return false;
}
