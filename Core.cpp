//------------------------------------------------------------------------------
// File: Core.cpp
// Desc: State manager for game, menu, etc
//
// Created: 24 September 2005 16:54:04
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "Core.h"
#include "Menu.h"
#include "Game.h"
#include "Scoreboard.h"
#include "InputManager.h"
#include "SoundManager.h"
#include "constants.h"
#include <ctime>
#include <new>


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
Menu*				GCore::MenuObject	= NULL;
Game*				GCore::GameObject	= NULL;
CORE_STATE			GCore::CurrentState	= CORE_STATE_UNINITIALISED;
CORE_STATE			GCore::NextState	= CORE_STATE_MENU;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool				GCore::Initialise()
{
	if( ! GInputManager::Initialise() )
	{
		return false;
	}

	if( ! GScoreboard::InitialiseOneTimeOnly() )
	{
		return false;
	}

	MenuObject = new(std::nothrow) Menu;
	if( ! MenuObject )
	{
		return false;
	}

	GameObject = new(std::nothrow) Game;
	if( ! GameObject )
	{
		delete MenuObject;
		MenuObject = NULL;

		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GCore::Shutdown()
{
	//Shut down the current state if it still exists
	ShutdownState( CurrentState );

	GInputManager::Shutdown();
	GScoreboard::ShutdownOneTimeOnly();

	delete MenuObject;
	MenuObject = NULL;
	delete GameObject;
	GameObject = NULL;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GCore::RestoreDevice()
{
	assert( DXUTGetD3DDevice() );

	//Set the world->camera transform
	D3DXVECTOR3 cam_pos( (PT_WORLD_WIDTH+PT_GUI_WIDTH) / 2.0f, (PT_WORLD_HEIGHT-PT_GUI_WIDTH) / 2.0f, -PT_CAMERA_DEPTH );
	D3DXVECTOR3 cam_tgt( (PT_WORLD_WIDTH+PT_GUI_WIDTH) / 2.0f, (PT_WORLD_HEIGHT-PT_GUI_WIDTH) / 2.0f, 0.0f );
	D3DXVECTOR3 cam_up( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX view_transform;
	D3DXMatrixLookAtLH( &view_transform, &cam_pos, &cam_tgt, &cam_up );
	DXUTGetD3DDevice()->SetTransform( D3DTS_VIEW, &view_transform );

	//Set the camera->screen transform
	D3DXMATRIX proj_transform;
	D3DXMatrixOrthoLH( &proj_transform, (PT_WORLD_WIDTH+PT_GUI_WIDTH), (PT_WORLD_HEIGHT+PT_GUI_WIDTH), 1.0f - PT_CAMERA_DEPTH, 100.0f );
	DXUTGetD3DDevice()->SetTransform( D3DTS_PROJECTION, &proj_transform );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GCore::Process()
{
	//Switch states if needed
	if( CurrentState != NextState )
	{
		ReallySwitchState();
	}

	if( CurrentState == CORE_STATE_QUIT )
	{
		return false;
	}

	//If we are running a game, we need to update it at regular intervals
	static unsigned int last_update = 0;
	const unsigned int current_time = clock();
	if( (current_time - last_update) > PT_GAME_TICK_INTERVAL_MS )
	{
		if( ! GInputManager::Process() )
		{
			return false;
		}

		if( ! ProcessState( CurrentState ) )
		{
			return false;
		}

		if( GInputManager::IsKeyDown( DIK_S ) && ! GInputManager::WasKeyDown( DIK_S ) )
		{
			GSoundManager::SetMuted( ! GSoundManager::IsMuted() );
		}

		last_update = current_time;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GCore::Render()
{
	RenderState( CurrentState );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GCore::InitialiseState( CORE_STATE state )
{
	switch( state )
	{
	case CORE_STATE_MENU:
		return MenuObject->Initialise();

	case CORE_STATE_SCORE:
		return GScoreboard::Initialise();

	case CORE_STATE_GAME:
		return GameObject->Initialise();

	default:
		//Other states don't require initialisation
		break;

	};

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GCore::ShutdownState( CORE_STATE state )
{
	//Shut down the last system
	switch( state )
	{
	case CORE_STATE_MENU:
		return MenuObject->Shutdown();

	case CORE_STATE_SCORE:
		return GScoreboard::Shutdown();

	case CORE_STATE_GAME:
		return GameObject->Shutdown();

	default:
		//Other states don't require shutdown
		break;

	};

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GCore::ProcessState( CORE_STATE state )
{
	switch( state )
	{
	case CORE_STATE_MENU:
		return MenuObject->Process();

	case CORE_STATE_SCORE:
		return GScoreboard::Process();

	case CORE_STATE_GAME:
		return GameObject->Process();

	default: break;
	};

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GCore::RenderState( CORE_STATE state )
{
	switch( state )
	{
	case CORE_STATE_MENU:
		return MenuObject->Render();

	case CORE_STATE_SCORE:
		return GScoreboard::Render();

	case CORE_STATE_GAME:
		return GameObject->Render();

	default: break;
	};

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GCore::ReallySwitchState()
{
	//Shut down the last system
	ShutdownState( CurrentState );

	//Initialise the new system
	if( ! InitialiseState( NextState ) )
	{
		CurrentState	= CORE_STATE_QUIT;
		NextState		= CORE_STATE_QUIT;
		return false;
	}

	CurrentState = NextState;

	return true;
}
