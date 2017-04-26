//------------------------------------------------------------------------------
// File: ParticleManager.cpp
// Desc: Controls the particle systems
//
// Created: 22 September 2005 18:45:39
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "ParticleManager.h"
#include "ParticleEffect.h"
#include "ParticleEffectExplosion.h"
#include "ParticleEffectStarfield.h"
#include <cassert>
#include <new>
#include <algorithm>


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
GParticleManager::EffectList	GParticleManager::Effects;
GParticleManager::EffectList	GParticleManager::DeadEffects;
bool							GParticleManager::Initialised = false;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool				GParticleManager::Initialise()
{
	assert( ! Initialised );

	if( ! ParticleEffect::Initialise3D() )
	{
		return false;
	}

	ParticleEffect::StencilEnabled = false;

	Initialised = true;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GParticleManager::Shutdown()
{
	assert( Initialised );

	for( EffectList::iterator iter = Effects.begin(); iter != Effects.end(); ++iter )
	{
		ParticleEffect *effect = *iter;
		delete effect;
	}
	Effects.clear();

	for( EffectList::iterator iter2 = DeadEffects.begin(); iter2 != DeadEffects.end(); ++iter2 )
	{
		ParticleEffect *effect = *iter2;
		delete effect;
	}
	DeadEffects.clear();

	ParticleEffect::Shutdown3D();

	Initialised = false;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GParticleManager::Process()
{
	assert( Initialised );

	//empty the deadlist
	for( EffectList::iterator dead_iter = DeadEffects.begin(); dead_iter != DeadEffects.end(); ++dead_iter )
	{
        ParticleEffect *effect = *dead_iter;
		effect->Shutdown();
		delete effect;
	}
	DeadEffects.clear();

	//process the live list
	EffectList::iterator iter = Effects.begin();
	while( iter != Effects.end() )
	{
		ParticleEffect *effect = *iter;
		if( ! effect->Process() )
		{
			effect->Dead = true;
            DeadEffects.push_back( effect );
			iter = Effects.erase( iter );
			continue;
		}

		++iter;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GParticleManager::Render( float frame_delta )
{
	assert( Initialised );

	for( EffectList::iterator iter = Effects.begin(); iter != Effects.end(); ++iter )
	{
		ParticleEffect *effect = *iter;
		effect->Render( frame_delta );
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
ParticleEffect*		GParticleManager::CreateEffect( PARTICLE_EFFECT effect, float pos_x, float pos_y, float size )
{
	assert( Initialised );

	ParticleEffect *pe = NULL;

	switch( effect )
	{
	case PARTICLE_EFFECT_EXPLOSION:
		pe = new(std::nothrow) ParticleEffectExplosion;
		break;

	case PARTICLE_EFFECT_STARFIELD:
		pe = new(std::nothrow) ParticleEffectStarfield;
		break;

	default:
		OutputDebugStringA( "PT_ERROR: Unknown particle effect type\n" );
		return NULL;
	};

	if( ! pe )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory creating particle effect\n" );
		return NULL;
	}

	if( ! pe->Initialise( pos_x, pos_y, size ) )
	{
		delete pe;
		return NULL;
	}

	Effects.push_back( pe );

	return pe;
}

//------------------------------------------------------------------------------------------------------------
bool				GParticleManager::DestroyEffect( ParticleEffect *effect )
{
	assert( Initialised );
	assert( effect );
	assert( ! effect->Dead );

	EffectList::iterator iter = std::find( Effects.begin(), Effects.end(), effect );
	if( iter == Effects.end() )
	{
		OutputDebugStringA( "PT_ERROR: Could not find particle effect\n" );
		return false;
	}

	effect->Dead = true;
	Effects.erase( iter );
	DeadEffects.push_back( effect );

	return true;
}

//------------------------------------------------------------------------------------------------------------
void				GParticleManager::SetStencilEnabled( bool enabled )
{
	ParticleEffect::StencilEnabled = enabled;
}
