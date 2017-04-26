//------------------------------------------------------------------------------
// File: ObjectPickup.h
// Desc: An object picked up by the player
//
// Created: 19 September 2005 11:53:03
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_OBJECTPICKUP_H
#define INCLUSIONGUARD_OBJECTPICKUP_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Object.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class ObjectPickup : public Object
{
protected:
	virtual bool		ProcessBounds();

};


#endif //INCLUSIONGUARD_OBJECTPICKUP_H
