//------------------------------------------------------------------------------
// File: ParticleEffectStarfield.h
// Desc: A starfield effect
//
// Created: 26 September 2005 10:54:12
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_PARTICLEEFFECTSTARFIELD_H
#define INCLUSIONGUARD_PARTICLEEFFECTSTARFIELD_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ParticleEffect.h"


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class ParticleEffectStarfield : public ParticleEffect
{
public:
	ParticleEffectStarfield();
	~ParticleEffectStarfield();

	bool				Initialise( float pos_x, float pos_y, float size );
	bool				Shutdown();

	bool				Process();

};


#endif //INCLUSIONGUARD_PARTICLEEFFECTSTARFIELD_H
