/**************************************************************************/
/* FILE   **************         TagType.hh        ************************/
/************************************************************************ **/
/* Author: Patrick Miller June 30 2001                                    */
/* Copyright (C) 2001 Patrick J. Miller                                   */
/**************************************************************************/
/*  */
/**************************************************************************/
#ifndef TAGTYPE_HH
#define TAGTYPE_HH

class ChainType;

class TagType : public ChainType {
public:
   TagType(Module* M=0) : ChainType(IFTAG,M) {}
   virtual ~TagType() {}
};

#endif
