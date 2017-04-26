//------------------------------------------------------------------------------
// File: PlayerComputer.cpp
// Desc: Representation of a computerised player
//
// Created: 15 September 2005 17:00:24
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "PlayerComputer.h"
#include "GroupManager.h"
#include "SoundManager.h"
#include "Constants.h"


//------------------------------------------------------------------------------
// Constants:
//------------------------------------------------------------------------------
const unsigned int GROUP_INTERVAL_1 = 70;
const unsigned int GROUP_INTERVAL_2 = 100;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
PlayerComputer::PlayerComputer()
{
	Timer1 = 0;
	Timer2 = GROUP_INTERVAL_2;
}

//------------------------------------------------------------------------------------------------------------
PlayerComputer::~PlayerComputer()
{

}

//------------------------------------------------------------------------------------------------------------
bool					PlayerComputer::Initialise( const char *filename )
{
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					PlayerComputer::Shutdown()
{
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					PlayerComputer::Process()
{
	if( Target == NULL )
	{
		Timer1 = 0;
		Timer2 = GROUP_INTERVAL_2;

		return true;
	}
	
	if( Timer1 == 0 )
	{
		Timer1 = GROUP_INTERVAL_1;
		GGroupManager::CreateGroup( GROUP_TYPE_SINE, D3DXVECTOR2( 350.0f, 600.0f ), D3DXVECTOR2( 0.0f, -1.0f ), 10, OBJECT_TYPE_SHIP_COMPUTER, 3, PlayerNumber );
	}
	else
	{
		--Timer1;
	}

	if( Timer2 == 0 )
	{
		Timer2 = GROUP_INTERVAL_2;

		GGroupManager::CreateGroup( GROUP_TYPE_STRAIGHT, D3DXVECTOR2( 100.0f, 800.0f ), D3DXVECTOR2( 0.0f, -1.0f ), 4, OBJECT_TYPE_SHIP_COMPUTER, 2, PlayerNumber );
		GGroupManager::CreateGroup( GROUP_TYPE_STRAIGHT, D3DXVECTOR2( 300.0f, 800.0f ), D3DXVECTOR2( 0.0f, -1.0f ), 4, OBJECT_TYPE_SHIP_COMPUTER, 2, PlayerNumber );
	}
	else
	{
		--Timer2;
	}

	return true;
}
