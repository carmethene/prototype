//------------------------------------------------------------------------------
// File: ObjectGroupCircle.h
// Desc: Moves objects in a circular pattern
//
// Created: 27 September 2005 20:15:19
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_OBJECTGROUPCIRCLE_H
#define INCLUSIONGUARD_OBJECTGROUPCIRCLE_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectGroup.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class ObjectGroupCircle : public ObjectGroup
{
public:
	ObjectGroupCircle();
	~ObjectGroupCircle();

protected:
	bool				InitialiseObjectMovement();
	bool				ProcessObjectMovement();
	
};


#endif //INCLUSIONGUARD_OBJECTGROUPCIRCLE_H
