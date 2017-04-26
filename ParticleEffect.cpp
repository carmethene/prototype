//------------------------------------------------------------------------------
// File: ParticleEffect.cpp
// Desc: Base class for a particle effect
//
// Created: 22 September 2005 18:53:44
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "ParticleEffect.h"
#include "constants.h"
#include <memory.h>


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
LPDIRECT3DVERTEXBUFFER9			ParticleEffect::ParticleVB		= NULL;
LPDIRECT3DVERTEXDECLARATION9	ParticleEffect::ParticleDecl	= NULL;
bool							ParticleEffect::StencilEnabled	= false;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
ParticleEffect::ParticleEffect()
{
	memset( &Particles, 0, MAX_PARTICLES * sizeof( Particle ) );

	PosX = 0.0f;
	PosY = 0.0f;
	Size = 0.0f;

	Dead = false;
}

//------------------------------------------------------------------------------------------------------------
ParticleEffect::~ParticleEffect()
{

}

//------------------------------------------------------------------------------------------------------------
bool				ParticleEffect::Initialise( float pos_x, float pos_y, float size )
{
	PosX = pos_x;
	PosY = pos_y;
	Size = size;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				ParticleEffect::Render( float frame_delta )
{
	LPDIRECT3DDEVICE9 d3d_device = DXUTGetD3DDevice();
	assert( d3d_device );

	//Lock the VB to copy the particles into
	PARTICLE_VERTEX *buffer = NULL;
	if( FAILED( ParticleVB->Lock( 0, MAX_PARTICLES * sizeof(PARTICLE_VERTEX), (void**)&buffer, 0 ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not lock particle system vb\n" );
		return false;
	}

	//Copy the particles to the vertex buffer
	unsigned int particle_count = 0;
	for( unsigned int i = 0; i < MAX_PARTICLES; ++i )
	{
        Particle &particle = Particles[ i ];
		if( particle.life == 0 )
		{
			continue;
		}

		PARTICLE_VERTEX &vertex = buffer[ particle_count ];

		vertex.x = ((particle.x - particle.old_x) * frame_delta) + particle.old_x;
		vertex.y = ((particle.y - particle.old_y) * frame_delta) + particle.old_y;
		vertex.z = PT_CAMERA_DEPTH / -2.0f;

		vertex.diffuse	= particle.colour;
		vertex.size		= particle.size;

		++particle_count;
	}

	ParticleVB->Unlock();

	//Draw the particles
	if( particle_count > 0 )
	{
        D3DXMATRIX transform;
		D3DXMatrixIdentity( &transform );
		d3d_device->SetTransform( D3DTS_WORLD, &transform );

		if( StencilEnabled )
		{
			d3d_device->SetRenderState( D3DRS_STENCILENABLE, TRUE );
			d3d_device->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_EQUAL );
			d3d_device->SetRenderState( D3DRS_STENCILREF, 1 );
		}

		d3d_device->SetStreamSource( 0, ParticleVB, 0, sizeof( PARTICLE_VERTEX ) );
		d3d_device->SetVertexDeclaration( ParticleDecl );
		d3d_device->DrawPrimitive( D3DPT_POINTLIST, 0, particle_count );
		d3d_device->SetStreamSource( 0, NULL, 0, 0 );

		if( StencilEnabled )
		{
			d3d_device->SetRenderState( D3DRS_STENCILENABLE, FALSE );
			d3d_device->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_ALWAYS );
			d3d_device->SetRenderState( D3DRS_STENCILREF, 0 );
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				ParticleEffect::Initialise3D()
{
	//Create a vb to store the particles
	if( FAILED( DXUTGetD3DDevice()->CreateVertexBuffer( MAX_PARTICLES * sizeof(PARTICLE_VERTEX), 0, 0, D3DPOOL_MANAGED, &ParticleVB, NULL ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not create particle system vb\n" );
		return false;
	}

	//Create a vertex declaration for the particles
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, 4, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_PSIZE, 0 },
		D3DDECL_END(),
	};

	if( FAILED( DXUTGetD3DDevice()->CreateVertexDeclaration( decl, &ParticleDecl ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not create particle system decl\n" );
		SAFE_RELEASE( ParticleVB );
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				ParticleEffect::Shutdown3D()
{
	SAFE_RELEASE( ParticleVB );
	SAFE_RELEASE( ParticleDecl );

	return true;
}

//------------------------------------------------------------------------------------------------------------
unsigned int		ParticleEffect::ScaleColour( unsigned int colour, unsigned int max_life, unsigned int life )
{
	unsigned int retval = 0;

	unsigned char *in_colours = (unsigned char*)&colour;
	unsigned char *out_colours = (unsigned char*)&retval;

	const float scale = float( life ) / float( max_life );

	out_colours[ 0 ] = unsigned char( float( in_colours[ 0 ] ) * scale );
	out_colours[ 1 ] = unsigned char( float( in_colours[ 1 ] ) * scale );
	out_colours[ 2 ] = unsigned char( float( in_colours[ 2 ] ) * scale );
	out_colours[ 3 ] = unsigned char( float( in_colours[ 3 ] ) * scale );

	return retval;
}