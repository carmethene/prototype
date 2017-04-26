//------------------------------------------------------------------------------
// File: Scoreboard.cpp
// Desc: Manages display, entry and storage of high scores
//
// Created: 26 September 2005 12:12:30
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "Scoreboard.h"
#include "Core.h"
#include "TextManager.h"
#include "InputManager.h"
#include "ParticleManager.h"
#include "SoundManager.h"
#include <ctime>
#include <fstream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


//------------------------------------------------------------------------------
// Constants:
//------------------------------------------------------------------------------
const char * const SCORES_FILE = "hiscores.dat";


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
GScoreboard::SCOREBOARD_STATE	GScoreboard::ScoreboardState = GScoreboard::SCOREBOARD_STATE_DISPLAY;
GScoreboard::SCORE_ENTRY		GScoreboard::Scores[ PT_GUI_SCORE_NUM_ENTRIES ];
unsigned int					GScoreboard::TextMeshes[ 38 ];
unsigned int					GScoreboard::TitleText = ~0;
unsigned int					GScoreboard::EntryText = ~0;
unsigned int					GScoreboard::ContinueText = ~0;
unsigned int					GScoreboard::LastUpdateTime = 0;
unsigned int					GScoreboard::SubmittedPosition = 0;
unsigned int					GScoreboard::TextEntryColumn = 0;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool					GScoreboard::InitialiseOneTimeOnly()
{
	memset( Scores, 0, PT_GUI_SCORE_NUM_ENTRIES * sizeof( SCORE_ENTRY ) );
	LoadScores();

	//Create text meshes
	char text_string[ 2 ] = { 0 };
	char next_char = 'A';
	for( unsigned int i = 0; i < 26; ++i )
	{
		text_string[ 0 ] = next_char;
		if( ! GTextManager::CreateText( text_string, TextMeshes[ i ] ) )
		{
            TextMeshes[ i ] = ~0;
		}
		++next_char;
	}

	next_char = '0';
	for( unsigned int i = 26; i < 36; ++i )
	{
		text_string[ 0 ] = next_char;
		if( ! GTextManager::CreateText( text_string, TextMeshes[ i ] ) )
		{
			TextMeshes[ i ] = ~0;
		}
		++next_char;
	}

	if( ! GTextManager::CreateText( ".", TextMeshes[ 36 ] ) )
	{
		TextMeshes[ 36 ] = ~0;
	}

	if( ! GTextManager::CreateText( "_", TextMeshes[ 37 ] ) )
	{
		TextMeshes[ 37 ] = ~0;
	}

	if( ! GTextManager::CreateText( "HIGH SCORES", TitleText ) )
	{
		TitleText = ~0;
	}

	if( ! GTextManager::CreateText( "ENTER YOUR NAME", EntryText ) )
	{
		EntryText = ~0;
	}

	if( ! GTextManager::CreateText( "PRESS FIRE TO CONTINUE", ContinueText ) )
	{
		ContinueText = ~0;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GScoreboard::ShutdownOneTimeOnly()
{
	for( unsigned int i = 0; i < 38; ++i )
	{
		GTextManager::DestroyText( TextMeshes[ i ] );
		TextMeshes[ i ] = ~0;
	}

	GTextManager::DestroyText( TitleText );
	TitleText = ~0;

	GTextManager::DestroyText( EntryText );
	EntryText = ~0;

	GTextManager::DestroyText( ContinueText );
	ContinueText = ~0;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GScoreboard::Initialise()
{
	LastUpdateTime = 0;

	if( ! GParticleManager::Initialise() )
	{
		return false;
	}
	GParticleManager::SetStencilEnabled( false );

	//Create the background - we want two to make it appear more dense
	GParticleManager::CreateEffect( PARTICLE_EFFECT_STARFIELD, (PT_WORLD_WIDTH+PT_GUI_WIDTH) / 2.0f, (PT_WORLD_HEIGHT+PT_GUI_WIDTH) / 2.0f, 1.0f );
	GParticleManager::CreateEffect( PARTICLE_EFFECT_STARFIELD, (PT_WORLD_WIDTH+PT_GUI_WIDTH) / 2.0f, (PT_WORLD_HEIGHT+PT_GUI_WIDTH) / 2.0f, 1.0f );

	//Start the background sound
	GSoundManager::Play( 6, -1 );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GScoreboard::Shutdown()
{
	GParticleManager::Shutdown();

	//Stop the background sound
	GSoundManager::Stop( 6 );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GScoreboard::Process()
{
	LastUpdateTime = clock();

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

	if( ! GParticleManager::Process() )
	{
		return false;
	}

	if( ! ProcessInput() )
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GScoreboard::Render()
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

	//Draw particles
	GParticleManager::Render( frame_delta );

	//Draw static text
	const static float half_width = (PT_WORLD_WIDTH + PT_GUI_WIDTH) / 2.0f;

	GTextManager::RenderText( TitleText, half_width - 170.0f, PT_WORLD_HEIGHT - 75.0f, 40.0f, 0.0f );

	if( ScoreboardState == SCOREBOARD_STATE_ENTER )
	{
		GTextManager::RenderText( EntryText, half_width - 122.0f, 0.0f, 20.0f, 0.0f );
	}
	else
	{
		GTextManager::RenderText( ContinueText, half_width - 175.0f, 0.0f, 20.0f, 0.0f );
	}

	//Draw score text
	if( ! RenderScores() )
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GScoreboard::SubmitScore( unsigned int score )
{
	//Check if the score belongs on the table
	unsigned int index = PT_GUI_SCORE_NUM_ENTRIES - 1;
	if( score <= Scores[ index ].Score )
	{
		return false;
	}

	//Copy down the elements with a lower score to make room for the new entry
	while( index > 0 )
	{
		if( score > Scores[ index - 1 ].Score )
		{
			Scores[ index ] = Scores[ index - 1 ];
			--index;
		}
		else
		{
			break;
		}
	}

	//Insert the new score
	memset( &(Scores[ index ]), 0, sizeof( SCORE_ENTRY ) );
	Scores[ index ].Score = score;

	//Notify the score system that a name needs to be entered
	ScoreboardState = SCOREBOARD_STATE_ENTER;
	TextEntryColumn = 0;
	SubmittedPosition = index;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GScoreboard::ProcessInput()
{
	if( ( GInputManager::IsKeyDown( DIK_SPACE ) && ! GInputManager::WasKeyDown( DIK_SPACE ) ) ||
		( GInputManager::IsKeyDown( DIK_RETURN ) && ! GInputManager::WasKeyDown( DIK_RETURN ) ) )
	{
		if( ScoreboardState == SCOREBOARD_STATE_DISPLAY )
		{
			GSoundManager::Play( 7 );
			GCore::SwitchState( CORE_STATE_MENU );
		}
		else
		{
			GSoundManager::Play( 7 );
			++TextEntryColumn;
		}
	}
	else if( ScoreboardState == SCOREBOARD_STATE_ENTER )	//Check character keys for name input
	{
		//Can't use a loop since the keyboard values aren't sequential
		// Using lazy evaluation to ensure that only one key gets used per loop
		if( ProcessOneInputChar( DIK_A, 'A' ) ||
			ProcessOneInputChar( DIK_B, 'B' ) ||
			ProcessOneInputChar( DIK_C, 'C' ) ||
			ProcessOneInputChar( DIK_D, 'D' ) ||
			ProcessOneInputChar( DIK_E, 'E' ) ||
			ProcessOneInputChar( DIK_F, 'F' ) ||
			ProcessOneInputChar( DIK_G, 'G' ) ||
			ProcessOneInputChar( DIK_H, 'H' ) ||
			ProcessOneInputChar( DIK_I, 'I' ) ||
			ProcessOneInputChar( DIK_J, 'J' ) ||
			ProcessOneInputChar( DIK_K, 'K' ) ||
			ProcessOneInputChar( DIK_L, 'L' ) ||
			ProcessOneInputChar( DIK_M, 'M' ) ||
			ProcessOneInputChar( DIK_N, 'N' ) ||
			ProcessOneInputChar( DIK_O, 'O' ) ||
			ProcessOneInputChar( DIK_P, 'P' ) ||
			ProcessOneInputChar( DIK_Q, 'Q' ) ||
			ProcessOneInputChar( DIK_R, 'R' ) ||
			ProcessOneInputChar( DIK_S, 'S' ) ||
			ProcessOneInputChar( DIK_T, 'T' ) ||
			ProcessOneInputChar( DIK_U, 'U' ) ||
			ProcessOneInputChar( DIK_V, 'V' ) ||
			ProcessOneInputChar( DIK_W, 'W' ) ||
			ProcessOneInputChar( DIK_X, 'X' ) ||
			ProcessOneInputChar( DIK_Y, 'Y' ) ||
			ProcessOneInputChar( DIK_Z, 'Z' ) )
		{
			GSoundManager::Play( 7 );
		}
		else
		{
			if( GInputManager::IsKeyDown( DIK_BACKSPACE ) && GInputManager::WasKeyDown( DIK_BACKSPACE ) )
			{
				if( TextEntryColumn > 0 )
				{
					--TextEntryColumn;
					Scores[ SubmittedPosition ].Name[ TextEntryColumn ] = 0;
					GSoundManager::Play( 7 );
				}
			}
		}
	}

	//Check if we've entered enough characters
	if( ScoreboardState == SCOREBOARD_STATE_ENTER && TextEntryColumn >= PT_GUI_SCORE_MAX_NAME )
	{
		SaveScores();
        ScoreboardState = SCOREBOARD_STATE_DISPLAY;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GScoreboard::ProcessOneInputChar( unsigned int key, unsigned int output )
{
	if( GInputManager::IsKeyDown( key ) && ! GInputManager::WasKeyDown( key ) )
	{
		Scores[ SubmittedPosition ].Name[ TextEntryColumn ] = output;
		++TextEntryColumn;
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------------------------------------
bool					GScoreboard::RenderScores()
{
	const static float half_width = (PT_WORLD_WIDTH + PT_GUI_WIDTH) / 2.0f;

	const float y_step = 38.0f;
	float pos_y = PT_WORLD_HEIGHT - 175.0f;

	for( unsigned int i = 0; i < PT_GUI_SCORE_NUM_ENTRIES; ++i )
	{
		//Draw score
		unsigned int temp_score = Scores[ i ].Score;
		
		const float x_step = 25.0f;
		float pos_x = ( (x_step * PT_GUI_SCORE_COLUMNS) - (x_step / 2.0f) ) + 15.0f;

		for( unsigned int j = 0; j < PT_GUI_SCORE_COLUMNS; ++j )
		{
			//Extract the next digit
			unsigned int digit = temp_score % 10;
			temp_score /= 10;

			//Draw the mesh
			GTextManager::RenderText( TextMeshes[ 26 + digit ], pos_x, pos_y, 30.0f, 0.0f );

			//Update the char positions
			pos_x -= x_step;
		}

		//Draw name
		pos_x = (PT_WORLD_WIDTH / 2.0f) + 150.0f;
		for( unsigned int j = 0; j < PT_GUI_SCORE_MAX_NAME; ++j )
		{
			//Extract the next char
			char letter = toupper( Scores[ i ].Name[ j ] );
			if( letter >= 'A' && letter <= 'Z' )
			{
				letter -= 'A';
			}
			else
			{
				if( ScoreboardState == SCOREBOARD_STATE_ENTER && i == SubmittedPosition )
				{
					if( j == TextEntryColumn )
					{
						letter = 37;
					}
					else
					{
						letter = -1;
					}
				}
				else
				{
					letter = 36;
				}
			}

			//Draw the mesh
			if( letter != -1 )
			{
				GTextManager::RenderText( TextMeshes[ letter ], pos_x, pos_y, 30.0f, 0.0f );
			}

			//Update the char positions
			pos_x += x_step;
		}

		//Update step
		pos_y -= y_step;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GScoreboard::LoadScores()
{
	std::ifstream fin( SCORES_FILE, std::ios::binary );
	if( ! fin )
	{
		//This is fine - the file just hasn't been created yet
		return false;
	}

	fin.read( (char*)Scores, sizeof( Scores ) );

	fin.close();

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GScoreboard::SaveScores()
{
	std::ofstream fout( SCORES_FILE, std::ios::binary );
	if( ! fout )
	{
		OutputDebugStringA( "PT_ERROR: Could not open high scores file for write\n" );
		return false;
	}

	fout.write( (char*)Scores, sizeof( Scores ) );

	fout.close();

	return true;
}
