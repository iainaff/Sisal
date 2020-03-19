/**************************************************************************/
/* FILE   **************        BasicType.hh       ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 30 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef BASICTYPE_HH
#define BASICTYPE_HH

class Info;

class BasicType : public Info {
public:
   static BasicType* NIL;

   typedef enum { IFBOOL=0, IFCHAR=1, IFDOUBLE=2, IFINTEGER=3, IFNULL=4, IFREAL=5, IFWILD=6 } KindOfBasic;

   BasicType(KindOfBasic,Module* M=0);
   virtual ~BasicType();
   virtual int i2() const;
   virtual int i3() const;

   virtual string canonicalName() const;

protected:
   KindOfBasic mKind;
};

#endif
