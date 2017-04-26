//------------------------------------------------------------------------------
// File: PlayerManager.cpp
// Desc: Maintains player objects
//
// Created: 16 September 2005 13:31:27
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <dxstdafx.h>
#include "PlayerManager.h"
#include "Player.h"
#include "PlayerHuman.h"
#include "PlayerComputer.h"
#include "Object.h"
#include <cassert>
#include <new>


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
GPlayerManager::PlayerMap	GPlayerManager::Players;
unsigned int				GPlayerManager::NextPlayerNumber = 0;
bool						GPlayerManager::Initialised = false;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool						GPlayerManager::Initialise()
{
	assert( ! Initialised );

	Initialised = true;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool						GPlayerManager::Shutdown()
{
	assert( Initialised );

	//Destroy all player objects
	for( PlayerMap::iterator iter = Players.begin(); iter != Players.end(); ++iter )
	{
		Player *player = iter->second;
		player->Shutdown();
		delete player;
	}
	Players.clear();

	NextPlayerNumber = 0;

	Initialised = false;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool						GPlayerManager::CreatePlayer( PLAYER_TYPE type, const char *script_file, unsigned int& player_number )
{
	assert( Initialised );

	Player *player = NULL;

	switch( type )
	{
	case PLAYER_TYPE_HUMAN:
		player = new(std::nothrow) PlayerHuman;
		break;

	case PLAYER_TYPE_COMPUTER:
		player = new(std::nothrow) PlayerComputer;
		break;

	default:
		OutputDebugStringA( "PT_ERROR: Invalid player type specified\n" );
		return false;
	}

	if( ! player )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory creating player\n" );
		return false;
	}

	assert( Players.find( NextPlayerNumber ) == Players.end() );

	player_number = NextPlayerNumber;
	player->SetPlayerNumber( NextPlayerNumber );
	player->Initialise( script_file );
	Players[ NextPlayerNumber ] = player;
	NextPlayerNumber++;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool						GPlayerManager::DestroyPlayer( unsigned int player_number )
{
	assert( Initialised );

	PlayerMap::iterator iter = Players.find( player_number );
	if( iter == Players.end() )
	{
		OutputDebugStringA( "PT_ERROR: Could not find player\n" );
		return false;
	}

	Player *player = iter->second;
	player->Shutdown();
	delete player;
	Players.erase( iter );

	return true;
}

//------------------------------------------------------------------------------------------------------------
Player*						GPlayerManager::GetPlayer( unsigned int player_number )
{
	assert( Initialised );

	PlayerMap::iterator iter = Players.find( player_number );
	if( iter == Players.end() )
	{
		OutputDebugStringA( "PT_ERROR: Could not find player\n" );
		return NULL;
	}

	return iter->second;
}

//------------------------------------------------------------------------------------------------------------
bool						GPlayerManager::Process()
{
	assert( Initialised );

	bool retval = true;

	//Process all player objects
	for( PlayerMap::iterator iter = Players.begin(); iter != Players.end(); ++iter )
	{
		Player *player = iter->second;

		//Validate references
		Object *target = player->GetTarget();
		if( target && target->HasBeenDeleted() )
		{
			player->SetTarget( NULL );
		}

		if( ! player->Process() )
		{
			retval = false;
		}
	}

	return retval;
}
