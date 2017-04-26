//------------------------------------------------------------------------------
// File: SoundManager.h
// Desc: Controls loading and playing of sound files
//
// Created: 23 September 2005 15:28:05
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_SOUNDMANAGER_H
#define INCLUSIONGUARD_SOUNDMANAGER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "fmod.hpp"
#include "fmod_errors.h"
#include <map>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class GSoundManager
{
public:
	static bool					Initialise();
	static bool					Shutdown();

	static bool					Play( unsigned int sound, unsigned int loop = 0 );
	static bool					Stop( unsigned int sound );

	static bool					IsMuted();
	static void					SetMuted( bool muted );

private:
	static bool					LoadSounds();
	static bool					UnloadSounds();
	static bool					LoadOneSound( unsigned int number, const char *filename );

	typedef			std::map< unsigned int, FMOD::Sound* >		SoundMap;

	static FMOD::System			*SoundSystem;
	static SoundMap				Sounds;

	static bool					Muted;

	static bool					Initialised;
};

#endif //INCLUSIONGUARD_SOUNDMANAGER_H
