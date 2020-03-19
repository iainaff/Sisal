#include <algorithm>
#include <iostream>

#include "IFObject.hh"
#include "Info.hh"
#include "Stamp.hh"
#include "Node.hh"
#include "Graph.hh"
#include "Function.hh"
#include "Use.hh"
#include "Module.hh"


Module* Module::NIL = 0;

CHILD_IMPLEMENTATION(Module,module,Info,info,IFauto)
CHILD_IMPLEMENTATION(Module,module,Stamp,stamp,IFauto)
CHILD_IMPLEMENTATION(Module,module,Function,function,IFauto)
CHILD_IMPLEMENTATION(Module,module,Use,use,IFauto)

// Module::Module();
Module::Module() {}

// Module::Module(string);
Module::Module(string name)
   : mName(name)
{}

// Module::~Module();
Module::~Module() {
   cerr << "destroying module " << this << endl;
   mUse.clear();
   mInfo.clear();
   mStamp.clear();
   mFunction.clear();
   cerr << "end destroying module " << this << endl;
}

// const char* Module::object() const;
const char* Module::object() const {
   return "Module";
}

// const char* Module::letter() const;
const char* Module::letter() const {
   return "M";
}

// const char* Module::value() const;
const char* Module::value() const {
   return mName.c_str();
}

void Module::endDump(ostream& os,unsigned level=0) const {
   dumpObjects(mUse.begin(),mUse.end(), os, level);
   dumpObjects(mInfo.begin(),mInfo.end(), os, level);
   dumpObjects(mStamp.begin(),mStamp.end(), os, level);
   dumpObjects(mFunction.begin(),mFunction.end(), os, level);
   IFObject::endDump(os,level);
}
