//------------------------------------------------------------------------------
// File: ParticleEffectExplosion.cpp
// Desc: An explosion effect
//
// Created: 22 September 2005 19:04:28
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ParticleEffectExplosion.h"


//------------------------------------------------------------------------------
// Constants:
//------------------------------------------------------------------------------
const static float			MAX_SPEED	= 30.0f;
const static float			MAX_SIZE	= 3.0f;
const static unsigned int	MAX_LIFE	= 10;
const static unsigned int	COLOUR		= 0xFF88FF88;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
ParticleEffectExplosion::ParticleEffectExplosion()
{

}

//------------------------------------------------------------------------------------------------------------
ParticleEffectExplosion::~ParticleEffectExplosion()
{

}

//------------------------------------------------------------------------------------------------------------
bool				ParticleEffectExplosion::Initialise( float pos_x, float pos_y, float size )
{
	for( unsigned int i = 0; i < MAX_PARTICLES; ++i )
	{
		Particle &particle = Particles[ i ];
		particle.x = pos_x;
		particle.y = pos_y;
		particle.old_x = pos_x;
		particle.old_y = pos_y;
		particle.life = (rand() % MAX_LIFE) * unsigned int(size);
		particle.size = ( float( rand() % 100 ) / 100.0f ) * MAX_SIZE * size;
		particle.colour = ScaleColour( COLOUR, MAX_LIFE * unsigned int(size), particle.life );

		const unsigned int MAX_SPEED_2 = (unsigned int)(MAX_SPEED * 2.0f);

		particle.vx = float( rand() % MAX_SPEED_2 ) - MAX_SPEED;
		particle.vy = float( rand() % MAX_SPEED_2 ) - MAX_SPEED;
	}

	return ParticleEffect::Initialise( pos_x, pos_y, size );
}

//------------------------------------------------------------------------------------------------------------
bool				ParticleEffectExplosion::Shutdown()
{
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				ParticleEffectExplosion::Process()
{
	bool found_live_particle = false;

	for( unsigned int i = 0; i < MAX_PARTICLES; ++i )
	{
		Particle &particle = Particles[ i ];
		if( particle.life == 0 )
		{
			continue;
		}

		found_live_particle = true;

		--particle.life;

		particle.old_x = particle.x;
		particle.old_y = particle.y;

		particle.x += particle.vx;
		particle.y += particle.vy;

		particle.colour = ScaleColour( COLOUR, MAX_LIFE * unsigned int(Size), particle.life );
	}

	if( ! found_live_particle )
	{
		//All particles are dead
		return false;
	}

	return true;
}
