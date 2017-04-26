//------------------------------------------------------------------------------
// File: ObjectPickup.cpp
// Desc: An object picked up by the player
//
// Created: 19 September 2005 12:07:22
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectPickup.h"
#include "constants.h"


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool				ObjectPickup::ProcessBounds()
{
	//If object is offscreen then it should be deleted
	const float margin_x = CollisionRadius + fabsf( Velocity.x );
	const float margin_y = CollisionRadius + fabsf( Velocity.y );
	if( Position.x + margin_x < 0.0f )
	{
		return false;
	}
	else if( Position.x - margin_x > PT_WORLD_WIDTH )
	{
		return false;
	}
	else if( Position.y + margin_y < 0.0f )
	{
		return false;
	}
	else if( Position.y - margin_y > PT_WORLD_HEIGHT )
	{
		return false;
	}
	
	return true;
}

