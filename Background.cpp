//------------------------------------------------------------------------------
// File: Background.cpp
// Desc: Global system to maintain background rendering
//
// Created: 21 September 2005 17:54:42
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "Background.h"
#include "AssetManager.h"
#include "constants.h"


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9				GBackground::Texture	= NULL;
LPDIRECT3DVERTEXBUFFER9			GBackground::QuadVB		= NULL;
LPDIRECT3DVERTEXDECLARATION9	GBackground::QuadDecl	= NULL;

unsigned int					GBackground::TextureWidth	= 0;
unsigned int					GBackground::TextureHeight	= 0;

unsigned int					GBackground::StarsX[ GBackground::NUM_STARS ];
unsigned int					GBackground::StarsY[ GBackground::NUM_STARS ];
unsigned int					GBackground::StarsOldY[ GBackground::NUM_STARS ];
unsigned int					GBackground::StarsSpeed[ GBackground::NUM_STARS ];

bool							GBackground::Initialised = false;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
struct QuadVertex
{
	float x, y, z;
	float u, v;
};

//------------------------------------------------------------------------------------------------------------
bool					GBackground::Initialise()
{
	assert( ! Initialised );

	assert( DXUTGetD3DDevice() );

	if( FAILED( D3DXCreateTexture( DXUTGetD3DDevice(), UINT(PT_WORLD_WIDTH / 3.0f), UINT(PT_WORLD_HEIGHT / 2.0), 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &Texture ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not create background texture\n" );
		return false;
	}

	D3DSURFACE_DESC desc;
	if( FAILED( Texture->GetLevelDesc( 0, &desc ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not get texture description\n" );
		SAFE_RELEASE( Texture );
		return false;
	}

	TextureWidth = desc.Width;
	TextureHeight = desc.Height;

	if( FAILED( DXUTGetD3DDevice()->CreateVertexBuffer( 4 * sizeof(QuadVertex), 0, 0, D3DPOOL_MANAGED, &QuadVB, NULL ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not create background quad vb\n" );
		SAFE_RELEASE( Texture );
		return false;
	}

	void *buffer = NULL;
	if( FAILED( QuadVB->Lock( 0, 4 * sizeof(QuadVertex), &buffer, 0 ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not lock background quad vb\n" );
		SAFE_RELEASE( Texture );
		SAFE_RELEASE( QuadVB );
		return false;
	}

	QuadVertex vertices[] = 
	{
		{ 0.0f,				0.0f,				10.0f, 0.0f, 0.0f },
		{ 0.0f,				PT_WORLD_HEIGHT,	10.0f, 0.0f, 1.0f },
		{ PT_WORLD_WIDTH,	PT_WORLD_HEIGHT,	10.0f, 1.0f, 1.0f },
		{ PT_WORLD_WIDTH,	0.0f,				10.0f, 1.0f, 0.0f },
	};
	memcpy( buffer, vertices, sizeof(vertices) );
	QuadVB->Unlock();

	//Create a vertex declaration for the quad
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END(),
	};

	if( FAILED( DXUTGetD3DDevice()->CreateVertexDeclaration( decl, &QuadDecl ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not create quad decl\n" );
		SAFE_RELEASE( Texture );
		SAFE_RELEASE( QuadVB );
		return false;
	}

	//Initialise the starfield
	for( unsigned int i = 0; i < NUM_STARS; ++i )
	{
		StarsX[ i ]	= rand() % TextureWidth;
		StarsY[ i ]	= rand() % TextureHeight;
		StarsOldY[ i ] = StarsY[ i ];

		StarsSpeed[ i ]	= ( rand() % (PT_WORLD_STARFIELD_MAX_SPEED - PT_WORLD_STARFIELD_MIN_SPEED) ) + PT_WORLD_STARFIELD_MIN_SPEED;
	}

	Initialised = true;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GBackground::Shutdown()
{
	assert( Initialised );

	SAFE_RELEASE( Texture );
	SAFE_RELEASE( QuadVB );
	SAFE_RELEASE( QuadDecl );

	Initialised = false;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GBackground::Process()
{
	assert( Initialised );

	for( unsigned int i = 0; i < NUM_STARS; ++i )
	{
		StarsOldY[ i ] = StarsY[ i ];

		if( StarsY[ i ] < StarsSpeed[ i ] )
		{
			StarsY[ i ] += TextureHeight;
		}

		StarsY[ i ] -= StarsSpeed[ i ];
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					GBackground::Render( float frame_delta )
{
	assert( Initialised );

	LPDIRECT3DDEVICE9 d3d_device = DXUTGetD3DDevice();
	assert( d3d_device );

	//Clear the texture
	D3DLOCKED_RECT rect;
	if( FAILED( Texture->LockRect( 0, &rect, NULL, 0 ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not lock background texture" );
		return false;
	}
	memset( rect.pBits, 0, TextureHeight * rect.Pitch );

	//Draw the stars
	for( unsigned int i = 0; i < NUM_STARS; ++i )
	{
		int interp_y = 0;
		if( StarsY[ i ] < StarsOldY[ i ] )
		{
			interp_y = StarsY[ i ] - StarsOldY[ i ];
			interp_y = int( float( interp_y ) * frame_delta );
			interp_y += StarsOldY[ i ];
		}
		else
		{
			interp_y = (StarsY[ i ]-TextureHeight) - StarsOldY[ i ];
			interp_y = int( float( interp_y ) * frame_delta );
			interp_y += StarsOldY[ i ];
		}

		if( interp_y < 0 )
		{
			interp_y += TextureHeight;
		}

		int offset = (interp_y * rect.Pitch) + (StarsX[ i ] * 4);
		int *ptr = (int*)((char*)rect.pBits + offset);

		*ptr = PT_WORLD_STARFIELD_COLOUR;

		//make faster stars appear larger
		if( StarsSpeed[ i ] >= PT_WORLD_STARFIELD_LARGE_SPEED )
		{
            if( StarsX[ i ] > 0 )
			{
				*(ptr-1) = PT_WORLD_STARFIELD_COLOUR;
			}
			if( StarsX[ i ] < (TextureWidth-1) )
			{
				*(ptr+1) = PT_WORLD_STARFIELD_COLOUR;
			}
			if( interp_y > 0 )
			{
				*(ptr-(rect.Pitch/4)) = PT_WORLD_STARFIELD_COLOUR;
			}
			if( (unsigned int)interp_y < (TextureHeight-1) )
			{
				*(ptr+(rect.Pitch/4)) = PT_WORLD_STARFIELD_COLOUR;
			}
		}
	}

	Texture->UnlockRect( 0 );

	//Render the background texture, writing into the stencil at the same time
	D3DXMATRIX transform;
	D3DXMatrixIdentity( &transform );
	d3d_device->SetTransform( D3DTS_WORLD, &transform );

	d3d_device->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0xff, 0xff, 0xff, 0xff ) );

	d3d_device->SetStreamSource( 0, QuadVB, 0, sizeof( QuadVertex ) );
	d3d_device->SetVertexDeclaration( QuadDecl );

	d3d_device->SetTexture( 0, Texture );

	d3d_device->SetRenderState( D3DRS_STENCILENABLE, TRUE );
	d3d_device->SetRenderState( D3DRS_STENCILREF, 1 );
	d3d_device->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_REPLACE );
	d3d_device->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE );

	d3d_device->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );

	d3d_device->SetRenderState( D3DRS_STENCILENABLE, FALSE );
	d3d_device->SetRenderState( D3DRS_STENCILREF, 0 );
	d3d_device->SetRenderState( D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP );
	d3d_device->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );

	d3d_device->SetTexture( 0, NULL );

	return true;
}
