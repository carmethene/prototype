//------------------------------------------------------------------------------
// File: ParticleEffectStarfield.cpp
// Desc: A starfield effect
//
// Created: 26 September 2005 10:54:20
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ParticleEffectStarfield.h"
#include "Constants.h"


//------------------------------------------------------------------------------
// Constants:
//------------------------------------------------------------------------------
const static float			MIN_SPEED	= 20.0f;
const static float			MAX_SPEED	= 30.0f;
const static float			MAX_SIZE	= 4.0f;
const static unsigned int	MAX_LIFE	= 25;
const static unsigned int	COLOUR		= 0xFFAAFFAA;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
ParticleEffectStarfield::ParticleEffectStarfield()
{

}

//------------------------------------------------------------------------------------------------------------
ParticleEffectStarfield::~ParticleEffectStarfield()
{

}

//------------------------------------------------------------------------------------------------------------
bool				ParticleEffectStarfield::Initialise( float pos_x, float pos_y, float size )
{
	//Scatter the particle lifetimes so they don't all start at once
	for( unsigned int i = 0; i < MAX_PARTICLES; ++i )
	{
		Particle &particle = Particles[ i ];

		particle.x = 10000.0f;
		particle.y = 10000.0f;
		particle.old_x = 10000.0f;
		particle.old_y = 10000.0f;
		particle.vx = 0.0f;
		particle.vy = 0.0f;
		particle.life = rand() % 30;
		particle.colour = 0x00000000;
	}

	return ParticleEffect::Initialise( pos_x, pos_y, size );
}

//------------------------------------------------------------------------------------------------------------
bool				ParticleEffectStarfield::Shutdown()
{
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				ParticleEffectStarfield::Process()
{
	const static float max_norm = sqrtf( (PT_WORLD_HEIGHT*PT_WORLD_HEIGHT)+(PT_WORLD_WIDTH*PT_WORLD_WIDTH) ) / 2.0f;

	for( unsigned int i = 0; i < MAX_PARTICLES; ++i )
	{
		Particle &particle = Particles[ i ];
		if( particle.life == 0 )
		{
			//Dead particle, recreate
			particle.x = PosX;
			particle.y = PosY;
			particle.old_x = PosX;
			particle.old_y = PosY;

			float dx = float( ( rand() % 200 ) - 100 ) / 100.0f;
			float dy = float( ( rand() % 200 ) - 100 ) / 100.0f;
			const float norm = sqrtf( (dx*dx) + (dy*dy) );
			dx /= norm;
			dy /= norm;

			float speed = ( float( rand() % 100 ) / 100.0f );
			speed *= MAX_SPEED - MIN_SPEED;
			speed += MIN_SPEED;

			particle.vx = speed * dx;
			particle.vy = speed * dy;

			particle.life = MAX_LIFE;
			particle.size = 0.0f;
			particle.colour = 0x00000000;
		}
		else
		{
			--particle.life;

			particle.old_x = particle.x;
			particle.old_y = particle.y;

			particle.x += particle.vx;
			particle.y += particle.vy;

			//Scale size and colour by life
			const float percentage = float(MAX_LIFE - particle.life) / float(MAX_LIFE);
			particle.size = percentage * MAX_SIZE;
			particle.colour = ScaleColour( COLOUR, MAX_LIFE, min( MAX_LIFE, (MAX_LIFE - particle.life) + 7 ) );
		}
	}

	return true;
}
