//------------------------------------------------------------------------------
// File: ObjectAmmo.cpp
// Desc: A missile object
//
// Created: 19 September 2005 12:07:28
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectAmmo.h"
#include "ParticleManager.h"
#include "SoundManager.h"
#include "constants.h"


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
void				ObjectAmmo::ReduceHealth( float amount )
{
	//Ammo needs a small explosion when it blows up
	if( Health > 0.0f && Health <= amount )
	{
		//Do an explosion
		GParticleManager::CreateEffect( PARTICLE_EFFECT_EXPLOSION, Position.x, Position.y, 1.0f );
		GSoundManager::Play( 4 );
	}

	Object::ReduceHealth( amount );
}

//------------------------------------------------------------------------------------------------------------
bool				ObjectAmmo::ProcessBounds()
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
