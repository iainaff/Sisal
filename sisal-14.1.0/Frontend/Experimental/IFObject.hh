/**************************************************************************/
/* FILE   **************        IFObject.hh        ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 15 2001                                */
/**************************************************************************/
/*  */
/**************************************************************************/

#ifndef IFOBJECT_HH
#define IFOBJECT_HH

// Do not include on its own, only as part of IFCore
#ifndef IFCORE_HH
error "Include only as part of IFCore.hh";
#endif

class IFObject {
public:
   // -----------------------------------------------
   // Constructors
   // -----------------------------------------------
   IFObject();
   IFObject(const string&,unsigned int);

   // -----------------------------------------------
   // Output
   // -----------------------------------------------
   virtual bool valid() const = 0;
   virtual void writeSelf(ostream&) const;

   // -----------------------------------------------
   // Pragmas
   // -----------------------------------------------
   void pragma(const char*,const string&);
   void pragma(const char*,const int);
   string spragma(const char*,const string& def="") const;
   int ipragma(const char*,int def=0) const;
   bool bpragma(const char*,bool def=false) const;
   void deletePragma(const char*);

   // -----------------------------------------------
   // IF Labeling
   // -----------------------------------------------
   virtual int label() const { return 0; }

   // -----------------------------------------------
   // Error processing
   // -----------------------------------------------
   void displayError(const char*,const char*) const;
   void information(const char*) const;
   void warning(const char*) const;
   void error(const char*) const;
   void fatal(const char*) const;

protected:
   virtual char letter() const = 0;
   virtual int i1() const { return -1; }
   virtual int i2() const { return -1; }
   virtual int i3() const { return -1; }
   virtual int i4() const { return -1; }
   virtual int i5() const { return -1; }
   virtual string sValue() const { return ""; }

protected:
   struct ltcharP {bool operator()(const char* s1, const char* s2) const {return strcmp(s1, s2) < 0;}};
   map<const char*,int,ltcharP> mIntegerPragmas;
   map<const char*,string,ltcharP> mStringPragmas;
};
#endif
