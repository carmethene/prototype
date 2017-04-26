//------------------------------------------------------------------------------
// File: ParticleEffectExplosion.h
// Desc: An explosion effect
//
// Created: 22 September 2005 19:04:26
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_PARTICLEEFFECTEXPLOSION_H
#define INCLUSIONGUARD_PARTICLEEFFECTEXPLOSION_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ParticleEffect.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class ParticleEffectExplosion : public ParticleEffect
{
public:
	ParticleEffectExplosion();
	~ParticleEffectExplosion();

	bool				Initialise( float pos_x, float pos_y, float size );
	bool				Shutdown();

	bool				Process();

};


#endif //INCLUSIONGUARD_PARTICLEEFFECTEXPLOSION_H
