#ifndef SEMANTICBASE_HH
#define SEMANTICBASE_HH
#include <string>

namespace sisalc {
   class semanticBase {
   public:
      semanticBase() 
         : mStartLine(0), mStartColumn(0), mEndLine(0), mEndColumn(0)
      {}
      semanticBase(string& fileName, int startLine, int startColumn, int endLine, int endColumn)
         : mFileName(fileName),
           mStartLine(startLine), mStartColumn(startColumn),
           mEndLine(endLine), mEndColumn(endColumn)
      {}

      virtual void error(const char*) const;
      virtual string self() const { return string(""); }
      virtual void append(semanticBase*) { throw "No appends"; }
      string fetchLine() const;
      string file() const { return mFileName; }
      int lineno() const { return mStartLine; }

      void setLocation(semanticBase* low,
                       semanticBase* high) {
         mFileName = low->mFileName;
      }

      void setLocation() {
         mFileName = "?";
         mStartLine = 1;
         mEndLine = 1;
         mStartColumn = 1;
         mEndColumn = 1;
      }
         
   protected:
      string mFileName;
      unsigned mStartLine;
      unsigned mEndLine;
      unsigned mStartColumn;
      unsigned mEndColumn;
   
   };

   static ostream& operator<<(ostream& os, const semanticBase& object) { return os << object.self(); }
}


#endif
