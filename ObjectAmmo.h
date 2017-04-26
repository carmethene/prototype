//------------------------------------------------------------------------------
// File: ObjectAmmo.h
// Desc: A missile object
//
// Created: 19 September 2005 12:07:53
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_OBJECTAMMO_H
#define INCLUSIONGUARD_OBJECTAMMO_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Object.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class ObjectAmmo : public Object
{
public:
	virtual void		ReduceHealth( float amount );

protected:
	virtual bool		ProcessBounds();

};


#endif //INCLUSIONGUARD_OBJECTAMMO_H
