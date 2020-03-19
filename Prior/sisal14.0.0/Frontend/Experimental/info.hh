/**************************************************************************/
/* FILE   **************          info.hh          ************************/
/************************************************************************ **/
/* Author: Patrick Miller February 15 2001                                */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef INFO_HH
#define INFO_HH

// Do not include on its own, only as part of IFCore
#ifndef IFCORE_HH
error "Include only as part of IFCore.hh";
#endif

   // -----------------------------------------------
   // A type has a "kind" and two possible sub-fields
   // It is owned by a module
   // -----------------------------------------------
class info : public IFObject {
public:
   typedef enum { ARRAY=0, BASIC=1, FIELD=2, FUNCTION=3,
                  MULTIPLE=4, RECORD=5, STREAM=6, TAG=7, TUPLE=8, 
                  UNION=9, UNKNOWN=10, BUFFER=11, SET=12,
                  REDUCTION=13, FOREIGN=14 } TypeCode;

   // -----------------------------------------------
   // Constructors
   // -----------------------------------------------
   info(TypeCode kind=UNKNOWN);

   // -----------------------------------------------
   // IF Labeling
   // -----------------------------------------------
   int label() const;
   virtual bool valid() const;

   // -----------------------------------------------
   // Accessors
   // -----------------------------------------------
   void setParent(module*);
   const info* car() const { return mInfo1; }
   void car(info* I) { mInfo1 = I; }
   const info* cdr() const { return mInfo2; }
   void cdr(info* I) { mInfo2 = I; }

   vector<const info*> list() const;
   vector<string> names() const;

   virtual bool operator ==(const info&) = 0;


protected:
   virtual char letter() const { return 'T'; }

protected:
   int i1() const;
   int i2() const;
   int i3() const;
   int i4() const;

private:
   TypeCode mKind;
   info* mInfo1;
   info* mInfo2;
   module* mParent;
};

#endif
