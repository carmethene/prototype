//------------------------------------------------------------------------------
// File: Menu.cpp
// Desc: Front end menu system
//
// Created: 24 September 2005 16:53:20
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Menu.h"
#include "TextManager.h"
#include "InputManager.h"
#include "ParticleManager.h"
#include "Core.h"
#include "SoundManager.h"
#include "constants.h"
#include <ctime>


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
Menu::Menu()
{
	LastUpdateTime = 0;

	LogoText		= ~0;
	GameText		= ~0;
	ScoreText		= ~0;
	QuitText		= ~0;
	CopyrightText	= ~0;

	MenuSelection = MENU_SELECTION_GAME;
}

//------------------------------------------------------------------------------------------------------------
Menu::~Menu()
{

}

//------------------------------------------------------------------------------------------------------------
bool				Menu::Initialise()
{
	//Initialise the particle system
	if( ! GParticleManager::Initialise() )
	{
		return false;
	}
	GParticleManager::SetStencilEnabled( false );

	//Create the background - we want two to make it appear more dense
	GParticleManager::CreateEffect( PARTICLE_EFFECT_STARFIELD, (PT_WORLD_WIDTH+PT_GUI_WIDTH) / 2.0f, (PT_WORLD_HEIGHT+PT_GUI_WIDTH) / 2.0f, 1.0f );
	GParticleManager::CreateEffect( PARTICLE_EFFECT_STARFIELD, (PT_WORLD_WIDTH+PT_GUI_WIDTH) / 2.0f, (PT_WORLD_HEIGHT+PT_GUI_WIDTH) / 2.0f, 1.0f );

	//Create the text objects
	GTextManager::CreateText( "PROTOTYPE",		LogoText );
	GTextManager::CreateText( "NEW GAME",		GameText );
	GTextManager::CreateText( "HIGH SCORES",	ScoreText );
	GTextManager::CreateText( "EXIT",			QuitText );
	GTextManager::CreateText( "(c)2005 Neil Wakefield", CopyrightText );

	//Initialise the menu
	MenuSelection = MENU_SELECTION_GAME;

	//Start the background sound
	GSoundManager::Play( 6, -1 );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				Menu::Shutdown()
{
	GParticleManager::Shutdown();

	GTextManager::DestroyText( LogoText );
	GTextManager::DestroyText( GameText );
	GTextManager::DestroyText( ScoreText );
	GTextManager::DestroyText( QuitText );
	GTextManager::DestroyText( CopyrightText );

	LogoText		= ~0;
	GameText		= ~0;
	ScoreText		= ~0;
	QuitText		= ~0;
	CopyrightText	= ~0;

	//Stop the background sound
	GSoundManager::Stop( 6 );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				Menu::Process()
{
	//Store the update time
	LastUpdateTime = clock();

	//Process user input
	if( ! ProcessInput() )
	{
		return false;
	}

	//Generate random explosions
	static unsigned int explosion_time = 10;
	static unsigned int explosion_timer = 0;

	if( explosion_timer == 0 )
	{
		const float pos_x = float( rand() % unsigned int( PT_WORLD_WIDTH + PT_GUI_WIDTH ) );
		const float pos_y = float( rand() % unsigned int( PT_WORLD_HEIGHT + PT_GUI_WIDTH ) );
		const float size = float( ( rand() % 2 ) + 1 );

		GParticleManager::CreateEffect( PARTICLE_EFFECT_EXPLOSION, pos_x, pos_y, size );
		explosion_timer = explosion_time;
	}
	else
	{
		--explosion_timer;
	}

	GParticleManager::Process();

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				Menu::Render()
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

	//Draw the particles
	GParticleManager::Render( frame_delta );

	//Draw the text
	const static float half_width = (PT_WORLD_WIDTH + PT_GUI_WIDTH) / 2.0f;
	const float spin = clock() * PT_FONT_SPIN_RATE;

	GTextManager::RenderText( LogoText, half_width - 209.0f, PT_WORLD_HEIGHT - 75.0f, 60.0f, 0.0f );
	GTextManager::RenderText( GameText, half_width - 125.0f, PT_WORLD_HEIGHT - 280.0f, 40.0f,
		MenuSelection == MENU_SELECTION_GAME ? spin : 0.0f );
	GTextManager::RenderText( ScoreText, half_width - 170.0f, PT_WORLD_HEIGHT - 350.0f, 40.0f, 
		MenuSelection == MENU_SELECTION_SCORE ? spin : 0.0f );
	GTextManager::RenderText( QuitText, half_width - 60.0f, PT_WORLD_HEIGHT - 420.0f, 40.0f,
		MenuSelection == MENU_SELECTION_QUIT ? spin : 0.0f );
	GTextManager::RenderText( CopyrightText, 8.0f, 25.0f - PT_GUI_WIDTH, 25.0f, 0.0f );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				Menu::ProcessInput()
{
	//Check if the user has switched elements downwards
	if( GInputManager::IsKeyDown( DIK_DOWN ) && ! GInputManager::WasKeyDown( DIK_DOWN ) )
	{
		GSoundManager::Play( 7 );

		if( MenuSelection == MENU_SELECTION_GAME )
		{
			MenuSelection = MENU_SELECTION_SCORE;
		}
		else if( MenuSelection == MENU_SELECTION_SCORE )
		{
			MenuSelection = MENU_SELECTION_QUIT;
		}
		else if( MenuSelection == MENU_SELECTION_QUIT )
		{
			MenuSelection = MENU_SELECTION_GAME;
		}
	}

	//Check if the user has switched elements upwards
	if( GInputManager::IsKeyDown( DIK_UP ) && ! GInputManager::WasKeyDown( DIK_UP )  )
	{
		GSoundManager::Play( 7 );

		if( MenuSelection == MENU_SELECTION_GAME )
		{
			MenuSelection = MENU_SELECTION_QUIT;
		}
		else if( MenuSelection == MENU_SELECTION_SCORE )
		{
			MenuSelection = MENU_SELECTION_GAME;
		}
		else if( MenuSelection == MENU_SELECTION_QUIT )
		{
			MenuSelection = MENU_SELECTION_SCORE;
		}
	}

	//Check if the user has selected an element
	if( ( GInputManager::IsKeyDown( DIK_SPACE ) && ! GInputManager::WasKeyDown( DIK_SPACE ) ) ||
		( GInputManager::IsKeyDown( DIK_RETURN ) && ! GInputManager::WasKeyDown( DIK_RETURN ) ) )
	{
		GSoundManager::Play( 7 );

		if( MenuSelection == MENU_SELECTION_GAME )
		{
			GCore::SwitchState( CORE_STATE_GAME );
		}
		else if( MenuSelection == MENU_SELECTION_SCORE )
		{
			GCore::SwitchState( CORE_STATE_SCORE );
		}
		else if( MenuSelection == MENU_SELECTION_QUIT )
		{
			GCore::SwitchState( CORE_STATE_QUIT );
		}
	}

	return true;
}
