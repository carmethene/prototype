//------------------------------------------------------------------------------
// File: ObjectShipComputer.cpp
// Desc: A computer-controlled ship
//
// Created: 19 September 2005 12:11:14
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ObjectShipComputer.h"
#include "ObjectManager.h"
#include "ParticleManager.h"
#include "SoundManager.h"
#include "PlayerManager.h"
#include "Player.h"
#include "constants.h"


//------------------------------------------------------------------------------
// Constants:
//------------------------------------------------------------------------------
const static unsigned int FIRE_INTERVAL = 15;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
ObjectShipComputer::ObjectShipComputer()
{
	FireTimer = rand() % FIRE_INTERVAL;
}

//------------------------------------------------------------------------------------------------------------
ObjectShipComputer::~ObjectShipComputer()
{

}

//------------------------------------------------------------------------------------------------------------
void				ObjectShipComputer::ReduceHealth( float amount )
{
	//Ships needs a large explosion when they blows up
	if( Health > 0.0f && Health <= amount )
	{
		//Do an explosion
		GParticleManager::CreateEffect( PARTICLE_EFFECT_EXPLOSION, Position.x, Position.y, 2.0f );
		GSoundManager::Play( 3 );
	}

	Object::ReduceHealth( amount );
}

//------------------------------------------------------------------------------------------------------------
bool				ObjectShipComputer::Process()
{
	if( FireTimer == 0 )
	{
        FireTimer = FIRE_INTERVAL;

		D3DXVECTOR2 launch_pos = Position;
		launch_pos.y -= CollisionRadius;

		//Work out the direction to our target
		D3DXVECTOR2 thrust( 0.0f, -1.0f );
		Player *player = GPlayerManager::GetPlayer( PlayerNumber );
		if( player )
		{
			Object *target = player->GetTarget();
			if( target )
			{
				thrust = target->GetPosition() - launch_pos;
				const float norm = sqrtf( thrust.x * thrust.x + thrust.y * thrust.y );
				thrust /= norm;
			}
		}

		//Can only fire downwards
		if( thrust.y > 0.0f || -thrust.y < fabsf( thrust.x ) )
		{
			thrust = D3DXVECTOR2( 0.0f, -1.0f );
		}

		Object *ammo = GObjectManager::CreateObject( OBJECT_TYPE_AMMO, 1, PlayerNumber, launch_pos, 0.0f );
		ammo->SetThrust( thrust );

		GSoundManager::Play( 2 );
	}
	else
	{
		--FireTimer;
	}

	return Object::Process();
}

//------------------------------------------------------------------------------------------------------------
bool				ObjectShipComputer::ProcessBounds()
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
	
	if( Position.y + CollisionRadius > PT_WORLD_HEIGHT && Velocity.y > 0.0f )
	{
		Position.y = PT_WORLD_HEIGHT - CollisionRadius;
		Velocity.y = 0.0f;
	}

	//Enemy ships CAN fly off the bottom of the screen, but they get deleted
	const float margin_y = CollisionRadius + fabsf( Velocity.y );
	if( Position.y + margin_y < 0.0f )
	{
		return false;
	}

	return true;
}
