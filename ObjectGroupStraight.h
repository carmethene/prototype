//------------------------------------------------------------------------------
// File: ObjectGroupStraight.h
// Desc: Makes objects move in a straight line down the screen
//
// Created: 27 September 2005 18:22:52
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_OBJECTGROUPSTRAIGHT_H
#define INCLUSIONGUARD_OBJECTGROUPSTRAIGHT_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectGroup.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class ObjectGroupStraight : public ObjectGroup
{
public:
	ObjectGroupStraight();
	~ObjectGroupStraight();

protected:
	bool				InitialiseObjectMovement();
	bool				ProcessObjectMovement();
	
};


#endif //INCLUSIONGUARD_OBJECTGROUPSTRAIGHT_H
