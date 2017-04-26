//------------------------------------------------------------------------------
// File: ObjectShipHuman.h
// Desc: A player-controlled ship
//
// Created: 19 September 2005 12:11:32
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_OBJECTSHIPHUMAN_H
#define INCLUSIONGUARD_OBJECTSHIPHUMAN_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Object.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class GObjectManager;

class ObjectShipHuman : public Object
{
	friend class GObjectManager;

public:
	virtual void		ReduceHealth( float amount );

protected:
	ObjectShipHuman();
	~ObjectShipHuman();

	virtual bool		ProcessBounds();

};


#endif //INCLUSIONGUARD_OBJECTSHIPHUMAN_H
