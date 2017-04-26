//------------------------------------------------------------------------------
// File: ObjectGroupSine.h
// Desc: Moves objects down the screen in a sine wave pattern
//
// Created: 27 September 2005 20:11:20
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_OBJECTGROUPSINE_H
#define INCLUSIONGUARD_OBJECTGROUPSINE_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectGroup.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class ObjectGroupSine : public ObjectGroup
{
public:
	ObjectGroupSine();
	~ObjectGroupSine();

protected:
	bool				InitialiseObjectMovement();
	bool				ProcessObjectMovement();

	D3DXVECTOR2			ThrustNormal;
	float				Theta;

};


#endif //INCLUSIONGUARD_OBJECTGROUPSINE_H
