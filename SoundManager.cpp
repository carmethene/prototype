//------------------------------------------------------------------------------
// File: SoundManager.cpp
// Desc: Controls loading and playing of sound files
//
// Created: 23 September 2005 15:28:07
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "SoundManager.h"
#include <cassert>
#include <fstream>
#include <string>
#include <sstream>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
FMOD::System*				GSoundManager::SoundSystem	= NULL;;
GSoundManager::SoundMap		GSoundManager::Sounds;
bool						GSoundManager::Muted		= false;
bool						GSoundManager::Initialised	= false;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool					GSoundManager::Initialise()
{
	assert( ! Initialised );

	if( FMOD_OK != FMOD::System_Create( &SoundSystem ) )
	{
		OutputDebugStringA( "SM_ERROR: Could not initialise FMOD\n" );
		return false;
	}

	if( FMOD_OK != SoundSystem->init( 32, FMOD_INIT_NORMAL, 0 ) )
	{
		OutputDebugStringA( "SM_ERROR: Could not initialise FMOD\n" );
		return false;      
	}

	if( ! LoadSounds() )
	{
		return false;
	}

	Initialised = true;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GSoundManager::Shutdown()
{
	assert( Initialised );

	UnloadSounds();

	SoundSystem->release();
	SoundSystem = NULL;

	Initialised = false;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GSoundManager::Play( unsigned int sound, unsigned int loop )
{
	assert( Initialised );

	if( Muted )
	{
		return true;
	}

	SoundMap::iterator iter = Sounds.find( sound );
	if( iter == Sounds.end() )
	{
		OutputDebugStringA( "SM_ERROR: Could not find sound number\n" );
		return false;
	}

	FMOD::Sound *sound_obj = iter->second;
	
	sound_obj->setLoopCount( loop );

	if( FMOD_OK != SoundSystem->playSound( (FMOD_CHANNELINDEX)sound, sound_obj, false, NULL ) )
	{
		OutputDebugStringA( "SM_ERROR: Could not play sound\n" );
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GSoundManager::Stop( unsigned int sound )
{
	assert( Initialised );

	SoundMap::iterator iter = Sounds.find( sound );
	if( iter == Sounds.end() )
	{
		OutputDebugStringA( "SM_ERROR: Could not find sound number\n" );
		return false;
	}

	FMOD::Channel *channel = NULL;
	if( FMOD_OK != SoundSystem->getChannel( sound, &channel ) )
	{
		OutputDebugStringA( "SM_ERROR: Could not find sound channel\n" );
		return false;
	}
	
	if( FMOD_OK != channel->stop() )
	{
		OutputDebugStringA( "SM_ERROR: Could not stop channel\n" );
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GSoundManager::IsMuted()
{
	return Muted;
}

//------------------------------------------------------------------------------------------------------------
void					GSoundManager::SetMuted( bool muted )
{
	if( muted )
	{
		for( SoundMap::iterator iter = Sounds.begin(); iter != Sounds.end(); ++iter )
		{
            Stop( iter->first );
		}
	}

	Muted = muted;
}

//------------------------------------------------------------------------------------------------------------
bool					GSoundManager::LoadSounds()
{
	std::ifstream fin( "assets/sounds.txt" );
	if( ! fin )
	{
		OutputDebugStringA( "SM_ERROR: Could not open sound manifest file\n" );
		return false;
	}

	const size_t MAX_LINE = 1024;
	char line_buffer[ MAX_LINE ];
	unsigned int line_count = 1;

	while( fin )
	{
		//get the line
		fin.getline( line_buffer, MAX_LINE );
		std::stringstream line_stream;
		line_stream.str( line_buffer );
		line_stream >> std::ws;
		if( line_stream.str().size() == 0 )
		{
			continue;
		}

		if( line_stream.str()[0] == '#' )
		{
			//this is a comment
			continue;
		}

		unsigned int asset_number = 0;
		std::string asset_file;

		line_stream >> asset_number;
		line_stream >> asset_file;

		std::string filename = std::string( "assets/" ) + asset_file;
		LoadOneSound( asset_number, filename.c_str() );
	}

	fin.close();

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GSoundManager::UnloadSounds()
{
	for( SoundMap::iterator iter = Sounds.begin(); iter != Sounds.end(); ++iter )
	{
		FMOD::Sound *sound = iter->second;
		sound->release();
	}
	Sounds.clear();

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GSoundManager::LoadOneSound( unsigned int number, const char *filename )
{ 
	assert( SoundSystem );

	//Check the sound number hasn't already been used
	if( Sounds.find( number ) != Sounds.end() )
	{
		OutputDebugStringA( "SM_ERROR: Sound number already in use\n" );
		return false;
	}

	FMOD::Sound *sound = NULL;
	if( FMOD_OK != SoundSystem->createSound( filename, FMOD_HARDWARE, 0, &sound ) )
	{
		OutputDebugStringA( "SM_ERROR: Could not load sound\n" );
		return false;
	}
	
	sound->setLoopCount( 0 );

	Sounds[ number ] = sound;

	return true;
}
