//------------------------------------------------------------------------------
// File: ParticleEffect.h
// Desc: Base class for a particle effect
//
// Created: 22 September 2005 18:53:42
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_PARTICLEEFFECT_H
#define INCLUSIONGUARD_PARTICLEEFFECT_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>


//------------------------------------------------------------------------------
// Constants:
//------------------------------------------------------------------------------
const unsigned int MAX_PARTICLES = 64;


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class GParticleManager;

struct Particle
{
	float x, y;
	float old_x, old_y;
	float vx, vy;
	float size;
	unsigned int colour;
	unsigned int life;
};

class ParticleEffect
{
	friend class GParticleManager;

public:
	bool					IsDead()				{ return Dead; }

protected:
	ParticleEffect();
	virtual ~ParticleEffect();

    virtual bool			Initialise( float pos_x, float pos_y, float size );
	virtual bool			Shutdown() = 0;

	virtual bool			Process() = 0;
	virtual bool			Render( float frame_delta );

	static bool				Initialise3D();
	static bool				Shutdown3D();

	unsigned int			ScaleColour( unsigned int colour, unsigned int max_life, unsigned int life );

	struct PARTICLE_VERTEX
	{
		unsigned int diffuse;
		float x,y,z;
		float size;
	};

	static LPDIRECT3DVERTEXBUFFER9			ParticleVB;
	static LPDIRECT3DVERTEXDECLARATION9		ParticleDecl;

	static bool								StencilEnabled;

	Particle				Particles[ MAX_PARTICLES ];

	float					PosX;
	float					PosY;
	float					Size;

	bool					Dead;
};


#endif //INCLUSIONGUARD_PARTICLEEFFECT_H
