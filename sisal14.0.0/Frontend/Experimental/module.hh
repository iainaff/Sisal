/**************************************************************************/
/* FILE   **************         module.hh         ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 15 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef MODULE_HH
#define MODULE_HH

// Do not include on its own, only as part of IFCore
#ifndef IFCORE_HH
error "Include only as part of IFCore.hh";
#endif

// -----------------------------------------------
// A module is a collection of types, stamps, and
// functions.  The module optionally belongs to a
// cluster
// -----------------------------------------------
class module : public IFObject {
public:
   module();
   module(const string&);
   module(const char*);

   // -----------------------------------------------
   // Output
   // -----------------------------------------------
   virtual void writeSelf(ostream&) const;
   virtual bool valid() const;
   int offset(const info*) const;

   // -----------------------------------------------
   // Accessors
   // -----------------------------------------------
   void addInfo(info*);
   void addFunction(Function*);
   void addStamp(const stamp&);
   void addStamp(char, const string&);
   void addStamp(char, const char*);
   
   void setCluster(cluster* C) { mCluster = C; }
   cluster* parent() { return mCluster; }
   const cluster* parent() const { return mCluster; }

   // -----------------------------------------------
   // Lookup
   // -----------------------------------------------
   string name() const { return mName; }
   virtual Function* findFunction(const char*);
   virtual Function* findFunction(const string&);
   virtual Function* findFunction(const string& name, vector<const info*>);

    info* integer();

protected:
   virtual char letter() const { return 'M'; }
   cluster* mCluster;
   string mName;
   vector< info* > mInfos;
   vector< stamp > mStamps;
   vector< Function* > mFunctions;
};

#endif
