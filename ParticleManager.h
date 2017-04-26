//------------------------------------------------------------------------------
// File: ParticleManager.h
// Desc: Controls the particle systems
//
// Created: 22 September 2005 18:46:21
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_PARTICLEMANAGER_H
#define INCLUSIONGUARD_PARTICLEMANAGER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <list>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
enum PARTICLE_EFFECT
{
	PARTICLE_EFFECT_EXPLOSION,
	PARTICLE_EFFECT_STARFIELD,
};

class ParticleEffect;

class GParticleManager
{
public:
	static bool				Initialise();
	static bool				Shutdown();

	static bool				Process();
	static bool				Render( float frame_delta );

	static ParticleEffect*	CreateEffect( PARTICLE_EFFECT effect, float pos_x, float pos_y, float size );
	static bool				DestroyEffect( ParticleEffect* effect );

	static void				SetStencilEnabled( bool enabled );

private:
	typedef			std::list< ParticleEffect* >		EffectList;

	static EffectList		Effects;
	static EffectList		DeadEffects;

	static bool				Initialised;
};

#endif //INCLUSIONGUARD_PARTICLEMANAGER_H
