/**************************************************************************/
/* FILE   **************        Function.hh        ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 17 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef FUNCTION_HH
#define FUNCTION_HH

// Do not include on its own, only as part of IFCore
#ifndef IFCORE_HH
error "Include only as part of IFCore.hh";
#endif

class Function : public graph {
public:
   // -----------------------------------------------
   // Constructors
   // -----------------------------------------------
   Function(const char*, unsigned int);
   Function(const string&, unsigned int);
   Function(const string&, const char*);

   // -----------------------------------------------
   // Output
   // -----------------------------------------------
   virtual bool valid() const;

   // -----------------------------------------------
   // Info to care about
   // -----------------------------------------------
   string name() const { return mName; }
   void name(const string& name) { mName = name; }
   void setType(const info*);
   const info* type() const { return mType; }
   void typeBinding();

   void setModule(module* M) { mParent = M; }
   module* parent() { return mParent; }
   const module* parent() const { return mParent; }

protected:
   virtual char letter() const;
   virtual int i1() const;
   virtual string sValue() const { string s("\""); s += mName; s+="\""; return s; }
   const info* mType;
   string mName;

   module* mParent;
};

#endif

