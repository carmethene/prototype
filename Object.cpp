//------------------------------------------------------------------------------
// File: Object.cpp
// Desc: An object in the game world
//
// Created: 14 September 2005 17:40:37
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "Object.h"
#include "ObjectManager.h"
#include "AssetManager.h"
#include "Mesh.h"
#include "Constants.h"
#include <ctime>


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
Object::Object()
{
	PlayerNumber = 0;

	Position		= D3DXVECTOR2( 0.0f, 0.0f );
	OldPosition		= D3DXVECTOR2( 0.0f, 0.0f );
	Orientation		= 0.0f;
	OldOrientation	= 0.0f;
	Roll			= 0.0f;
	OldRoll			= 0.0f;

	Velocity		= D3DXVECTOR2( 0.0f, 0.0f );
	Thrust			= D3DXVECTOR2( 0.0f, 0.0f );
	Mass			= 1.0f;
	ThrustMagnitude	= 1.0f;
	MaxVelocity		= 1.0f;

	MeshNumber	= 0;
	MeshScale	= 1.0f;

	MaxHealth		= 1.0f;
	Health			= 1.0f;
	CollisionDamage	= 2.0f;
	CollisionRadius	= 1.0f;

	KillScore = 0;

	Indestructable	= false;
	Deleted			= false;
}

//------------------------------------------------------------------------------------------------------------
Object::~Object()
{
}

//------------------------------------------------------------------------------------------------------------
bool				Object::Process()
{
	//Store the old position/orientation for interpolated rendering
	OldPosition = Position;
	OldOrientation = Orientation;
	OldRoll = Roll;

	if( ! ProcessPhysics() )
	{
		return false;
	}

	if( ! ProcessBounds() )
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				Object::ProcessPhysics()
{
	//Calculate linear motion
	const static float time_interval = float(PT_GAME_TICK_INTERVAL_MS) / 1000.0f;

	D3DXVECTOR2 force = Thrust * ThrustMagnitude;
	D3DXVECTOR2 acceleration = (force / Mass) ;
	Velocity += acceleration * time_interval;

	float current_velocity = sqrtf(Velocity.x * Velocity.x + Velocity.y * Velocity.y);
	float shrinkage = MaxVelocity / (MaxVelocity + current_velocity);
	Velocity *= shrinkage;

	Position += Velocity * time_interval;

	//If the ship is moving very slowly, just stop it altogether since the damping
	// mechanism will not stop it
	if( Thrust.x == 0.0f && Thrust.y == 0.0f && current_velocity < PT_PHYSICS_VELOCITY_CUTOFF )
	{
		Velocity = D3DXVECTOR2( 0.0f, 0.0f );
	}

	//Calculate roll
	const float roll_max = Thrust.x * PT_PHYSICS_ROLL_MAX;

	if( Thrust.x > 0.0f )
	{
		if( Roll > roll_max )
		{
			Roll -= PT_PHYSICS_ROLL_RATE;
		}
		Roll = max( Roll, roll_max );
	}
	else if( Thrust.x < 0.0f )
	{
		if( Roll < roll_max )
		{
			Roll += PT_PHYSICS_ROLL_RATE;
		}
		Roll = min( Roll, roll_max );
	}
	else
	{
		if( Roll > 0.0f )
		{
			Roll -= PT_PHYSICS_ROLL_RATE;
			Roll = max( Roll, 0.0f );
		}
		else if( Roll < 0.0f )
		{
			Roll += PT_PHYSICS_ROLL_RATE;
			Roll = min( Roll, 0.0f );
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				Object::ProcessBounds()
{
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				Object::Render( float frame_delta )
{
	assert( DXUTGetD3DDevice() );

	//Indestructable objects should flash
	if( Indestructable )
	{
		unsigned int flash_time = clock() % PT_OBJECT_FLASH_RATE;
		if( flash_time < (PT_OBJECT_FLASH_RATE / 2) == 0 )
		{
			return true;
		}
	}

	//retrieve object mesh
	Mesh *mesh = GAssetManager::GetMesh( MeshNumber );
	if( ! mesh )
	{
		OutputDebugStringA( "PT_ERROR: Object failed to render\n" );
		return false;
	}

	//work out interpolated position and orientation
	const float interp_x = ((Position.x - OldPosition.x) * frame_delta) + OldPosition.x;
	const float interp_y = ((Position.y - OldPosition.y) * frame_delta) + OldPosition.y;
	const float interp_o = ((Orientation - OldOrientation) * frame_delta) + OldOrientation;
	const float interp_r = ((Roll - OldRoll) * frame_delta) + OldRoll;

	//transform the object to its position in the world
	D3DXMATRIX mat_scale;
	D3DXMatrixScaling( &mat_scale, MeshScale, MeshScale, MeshScale );

	D3DXMATRIX mat_rotation_x;
	D3DXMatrixRotationX( &mat_rotation_x, D3DX_PI / 2 );

	D3DXMATRIX mat_rotation_z;
	D3DXMatrixRotationZ( &mat_rotation_z, interp_o );

	D3DXMATRIX mat_rotation_y;
	D3DXMatrixRotationY( &mat_rotation_y, interp_r );

	D3DXMATRIX mat_translation;
	D3DXMatrixTranslation( &mat_translation, interp_x, interp_y, 0.0f );

	D3DXMATRIX world_transform;
	D3DXMatrixMultiply( &world_transform, &mat_rotation_y, &mat_translation );
	D3DXMatrixMultiply( &world_transform, &mat_rotation_z, &world_transform );
	D3DXMatrixMultiply( &world_transform, &mat_rotation_x, &world_transform );
	D3DXMatrixMultiply( &world_transform, &mat_scale, &world_transform );
	DXUTGetD3DDevice()->SetTransform( D3DTS_WORLD, &world_transform );

	//Draw the object
	return mesh->Draw();
}

//------------------------------------------------------------------------------------------------------------
void				Object::ReduceHealth( float amount )
{
	if( Indestructable && amount > 0.0f )
	{
		return;
	}

	Health -= amount;

	//Cap
	Health = max( Health, 0.0f );
	Health = min( Health, MaxHealth );
}
