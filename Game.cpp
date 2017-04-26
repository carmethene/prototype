//------------------------------------------------------------------------------
// File: Game.cpp
// Desc: Game core
//
// Created: 14 September 2005 13:13:11
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Game.h"
#include "ObjectManager.h"
#include "GroupManager.h"
#include "PlayerManager.h"
#include "TextManager.h"
#include "Background.h"
#include "ParticleManager.h"
#include "SoundManager.h"
#include "Player.h"
#include "HUD.h"
#include "InputManager.h"
#include "Core.h"
#include "Scoreboard.h"
#include "Constants.h"
#include <ctime>


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
Game::Game()
{
	LastUpdateTime = 0;

	HumanPlayer		= ~0;
	ComputerPlayer	= ~0;

	GameHud = NULL;

	GameOverText = ~0;
	GameOver = false;
}

//------------------------------------------------------------------------------------------------------------
Game::~Game()
{

}

//------------------------------------------------------------------------------------------------------------
bool					Game::Initialise()
{
	GameOver = false;

	//Initialise subsystems
	if( ! GObjectManager::Initialise() )
	{
		return false;
	}
	if( ! GGroupManager::Initialise() )
	{
		return false;
	}
	if( ! GPlayerManager::Initialise() )
	{
		return false;
	}
	if( ! GBackground::Initialise() )
	{
		return false;
	}
	if( ! GParticleManager::Initialise() )
	{
		return false;
	}
	GParticleManager::SetStencilEnabled( true );

	//Initialise HUD
	try
	{
		GameHud = new HUD;
	}
	catch( std::bad_alloc& )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory creating HUD\n" );
		return false;
	}
	if( ! GameHud->Initialise() )
	{
		return false;
	}

	//DEBUG
	GPlayerManager::CreatePlayer( PLAYER_TYPE_HUMAN, NULL, HumanPlayer );
	GPlayerManager::CreatePlayer( PLAYER_TYPE_COMPUTER, NULL, ComputerPlayer );
	//END DEBUG

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					Game::Shutdown()
{
	GameHud->Shutdown();
	delete GameHud;
	GameHud = NULL;

	GObjectManager::Shutdown();
	GGroupManager::Shutdown();
	GPlayerManager::Shutdown();
	GBackground::Shutdown();
	GParticleManager::Shutdown();

	if( GameOver )
	{
		GTextManager::DestroyText( GameOverText );
		GameOverText = ~0;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					Game::Process()
{
	//Store the update time
	LastUpdateTime = clock();

	//Update the background
	GBackground::Process();

	//Update the players - should be done first so that input is responsive
	if( ! GPlayerManager::Process() )
	{
		SetGameOver();
	}

	//Point the players at each other for targetting
	Player *human_player = GPlayerManager::GetPlayer( HumanPlayer );
	Player *computer_player = GPlayerManager::GetPlayer( ComputerPlayer );

	if( human_player && computer_player )
	{
		computer_player->SetTarget( human_player->GetMainObject() );
	}

	//Update the objects
	GGroupManager::Process();
	GObjectManager::ProcessObjects();

	//Update the HUD
	if( ! GameOver )
	{
		if( ! human_player )
		{
			OutputDebugStringA( "PT_ERROR: Human player does not exist\n" );
			return false;
		}

		GameHud->SetLives( human_player->GetLives() - 1 );
		GameHud->SetScore( human_player->GetScore() );
		GameHud->SetHealth( human_player->GetHealth() );
	}

	//Update the particle systems
	GParticleManager::Process();

	//Check user input for gameover screen
	static key_down = true;
	if( GInputManager::IsKeyDown( DIK_SPACE ) || GInputManager::IsKeyDown( DIK_RETURN ) )
	{
		if( ! key_down && GameOver )
		{
			GCore::SwitchState( CORE_STATE_SCORE );
		}

		key_down = true;
	}
	else
	{
		key_down = false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					Game::Render()
{
	//work out the frame delta
	float frame_delta = 1.0f;
	if( LastUpdateTime > 0 )
	{
		const unsigned int current_time = clock();
		const unsigned int time_since_last_update = current_time - LastUpdateTime;
		frame_delta = float(time_since_last_update) / float(PT_GAME_TICK_INTERVAL_MS);

		//cap
		frame_delta = min( frame_delta, 1.0f );
		frame_delta = max( frame_delta, 0.0f );
	}

	//draw the background
	GBackground::Render( frame_delta );

	//draw all game objects
	GObjectManager::RenderObjects( frame_delta );

	//draw the HUD
	if( ! GameHud->Render() )
	{
		return false;
	}

	//draw particles
	GParticleManager::Render( frame_delta );

	//draw any text
	if( GameOver )
	{
		float spin = clock() * PT_FONT_SPIN_RATE;
		GTextManager::RenderText( GameOverText, (PT_WORLD_WIDTH / 2.0f) - 160.0f, PT_WORLD_HEIGHT / 2.0f, 45.0f, spin );
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					Game::SetGameOver()
{
	Player *human_player = GPlayerManager::GetPlayer( HumanPlayer );
	if( human_player )
	{
		GameHud->SetLives( human_player->GetLives() - 1 );
		GameHud->SetScore( human_player->GetScore() );
		GameHud->SetHealth( human_player->GetHealth() );
		GScoreboard::SubmitScore( human_player->GetScore() );
	}

	GPlayerManager::DestroyPlayer( HumanPlayer );
	HumanPlayer = ~0;
	GPlayerManager::DestroyPlayer( ComputerPlayer );
	ComputerPlayer = ~0;

	GObjectManager::DeleteAllObjects();

	if( ! GTextManager::CreateText( "GAME OVER", GameOverText ) )
	{
		return false;
	}

	GSoundManager::Play( 5 );

	GameOver = true;

	return true;
}
