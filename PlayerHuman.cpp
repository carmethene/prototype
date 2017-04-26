//------------------------------------------------------------------------------
// File: PlayerHuman.cpp
// Desc: Representation of a human player
//
// Created: 15 September 2005 17:00:21
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "PlayerHuman.h"
#include "Object.h"
#include "ObjectManager.h"
#include "GroupManager.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "Constants.h"

//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
PlayerHuman::PlayerHuman()
{
	Ship = NULL;
}

//------------------------------------------------------------------------------------------------------------
PlayerHuman::~PlayerHuman()
{

}

//------------------------------------------------------------------------------------------------------------
bool					PlayerHuman::Initialise( const char *filename )
{
	Lives = PT_PLAYER_LIVES;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					PlayerHuman::Shutdown()
{
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					PlayerHuman::Process()
{
	//Update the computer control time
	if( ComputerControlsShip )
	{
		if( ComputerControlTime > 0 )
		{
			ComputerControlTime--;
		}
		if( ComputerControlTime == 0 )
		{
			ComputerControlsShip = false;
			Ship->SetIndestructable( false );
		}
	}

	//Update ship
	if( ! ProcessShipStatus() )
	{
		return false;
	}

	//update player controls
	if( ! ComputerControlsShip )
	{
		if( ! ProcessShipMovement() )
		{
			return false;
		}

		if( ! ProcessShipWeapon() )
		{
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					PlayerHuman::ProcessShipStatus()
{
	//Check the status of our ship
	if( Ship && Ship->HasBeenDeleted() )
	{
		//Lost a ship - update lives and lose the reference to this one
		--Lives;

		if( Lives == 0 )
		{
			//Game over!
			return false;
		}

		Ship = NULL;

        //Clear the playing field
		GObjectManager::DeleteAllObjects();
		GGroupManager::DeleteAllGroups();
	}

	if( ! Ship )
	{
		Ship = GObjectManager::CreateObject( OBJECT_TYPE_SHIP_HUMAN, PT_PLAYER_SHIP, 0, D3DXVECTOR2( PT_WORLD_WIDTH / 2.0f, -30.0f ), 0.0f );
		if( ! Ship )
		{
			return false;
		}
		ComputerControlsShip = true;
		ComputerControlTime = 20;
		Ship->SetThrust( D3DXVECTOR2( 0.0f, 0.02f ) );
		Ship->SetIndestructable( true );
	}

	//Update health from ship
	Health = Ship->GetHealth() / Ship->GetMaxHealth();

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					PlayerHuman::ProcessShipMovement()
{
	//Set the ship's force from the player input
	D3DXVECTOR2 thrust = D3DXVECTOR2( 0.0f, 0.0f );

	if( GInputManager::IsKeyDown( DIK_LEFT ) )
	{
		thrust.x -= 1.0f;
	}
	if( GInputManager::IsKeyDown( DIK_RIGHT ) )
	{
		thrust.x += 1.0f;
	}
	if( GInputManager::IsKeyDown( DIK_UP ) )
	{
		thrust.y += 1.0f;
	}
	if( GInputManager::IsKeyDown( DIK_DOWN ) )
	{
		thrust.y -= 1.0f;
	}
	D3DXVec2Normalize( &thrust, &thrust );
	Ship->SetThrust( thrust );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					PlayerHuman::ProcessShipWeapon()
{
	if( GInputManager::IsKeyDown( DIK_SPACE ) )
	{
		D3DXVECTOR2 launch_pos = Ship->GetPosition();
		launch_pos.y += Ship->GetCollisionRadius();

		Object *ammo = GObjectManager::CreateObject( OBJECT_TYPE_AMMO, PT_PLAYER_AMMO_1, PlayerNumber, launch_pos, 0.0f );
		ammo->SetThrust( D3DXVECTOR2( 0.0f, 1.0f ) );

		GSoundManager::Play( 0 );
	}

	return true;
}
