//------------------------------------------------------------------------------
// File: ObjectShipHuman.cpp
// Desc: A player-controlled ship
//
// Created: 19 September 2005 12:11:46
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectShipHuman.h"
#include "ParticleManager.h"
#include "SoundManager.h"
#include "constants.h"


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
ObjectShipHuman::ObjectShipHuman()
{

}

//------------------------------------------------------------------------------------------------------------
ObjectShipHuman::~ObjectShipHuman()
{

}

//------------------------------------------------------------------------------------------------------------
void				ObjectShipHuman::ReduceHealth( float amount )
{
	//Ships needs a large explosion when they blows up
	if( Health > 0.0f && Health <= amount )
	{
		//Do an explosion
		GParticleManager::CreateEffect( PARTICLE_EFFECT_EXPLOSION, Position.x, Position.y, 2.0f );
		GSoundManager::Play( 1 );
	}

	Object::ReduceHealth( amount );
}

//------------------------------------------------------------------------------------------------------------
bool				ObjectShipHuman::ProcessBounds()
{
	//Ensure object does not travel out of the world (travelling in from out of bounds is fine)
	if( Position.x - CollisionRadius < 0.0f && Velocity.x < 0.0f )
	{
		Position.x = CollisionRadius;
		Velocity.x = 0.0f;
	}
	else if( Position.x + CollisionRadius > PT_WORLD_WIDTH && Velocity.x > 0.0f )
	{
		Position.x = PT_WORLD_WIDTH - CollisionRadius;
		Velocity.x = 0.0f;
	}

	if( Position.y - CollisionRadius < 0.0f && Velocity.y < 0.0f )
	{
		Position.y = CollisionRadius;
		Velocity.y = 0.0f;
	}
	else if( Position.y + CollisionRadius > PT_WORLD_HEIGHT && Velocity.y > 0.0f )
	{
		Position.y = PT_WORLD_HEIGHT - CollisionRadius;
		Velocity.y = 0.0f;
	}

	return true;
}
