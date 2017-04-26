//------------------------------------------------------------------------------
// File: ObjectGroupSine.cpp
// Desc: Moves objects down the screen in a sine wave pattern
//
// Created: 27 September 2005 20:11:32
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectGroupSine.h"
#include "Object.h"


//------------------------------------------------------------------------------
// Constants:
//------------------------------------------------------------------------------
const static float THETA_DELTA = 0.12f;
const static float THETA_SEP = 0.5f;
const static float OBJECT_SEP = 3.0f;
const static float THRUST_MAGNITUDE = 2.0f;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
ObjectGroupSine::ObjectGroupSine()
{

}

//------------------------------------------------------------------------------------------------------------
ObjectGroupSine::~ObjectGroupSine()
{

}

//------------------------------------------------------------------------------------------------------------
bool				ObjectGroupSine::InitialiseObjectMovement()
{
	if( Objects.size() == 0 )
	{
		OutputDebugStringA( "PT_ERROR: No objects in group\n" );
		return false;
	}

	//Line up the objects so they fly through the start position
	D3DXVECTOR2 pos = Position;
	D3DXVECTOR2 sep = Direction * (*(Objects.begin()))->GetCollisionRadius() * OBJECT_SEP;

	for( ObjectList::iterator iter = Objects.begin(); iter != Objects.end(); ++iter )
	{
		Object *object = *iter;
		object->SetPosition( pos );
		pos -= sep;

		object->SetThrust( Direction );
	}

	//Work out the normal to the direction
    ThrustNormal.x = -Direction.y;
	ThrustNormal.y = -Direction.x;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				ObjectGroupSine::ProcessObjectMovement()
{
	//Set the object thrust
	float temp_theta = Theta;

	for( ObjectList::iterator iter = Objects.begin(); iter != Objects.end(); ++iter )
	{
		Object *object = *iter;

		D3DXVECTOR2 thrust = Direction;
		thrust += ThrustNormal * THRUST_MAGNITUDE * sinf( temp_theta );
		const float thrust_norm = sqrtf( thrust.x * thrust.x + thrust.y * thrust.y );
		thrust /= thrust_norm;

		object->SetThrust( thrust );

		temp_theta -= THETA_SEP;
	}

	//Update theta
	Theta += THETA_DELTA;
	Theta = fmodf( Theta, D3DX_PI * 2.0f );

	return true;
}
