//------------------------------------------------------------------------------
// File: PostProcessor.cpp
// Desc: System to apply screen-space post processing effects
//
// Created: 21 September 2005 12:07:35
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "PostProcessor.h"
#include "Constants.h"


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
LPDIRECT3DVERTEXBUFFER9			GPostProcessor::ScreenQuad		= NULL;
LPDIRECT3DVERTEXDECLARATION9	GPostProcessor::ScreenQuadDecl	= NULL;

LPDIRECT3DTEXTURE9				GPostProcessor::RenderTarget1	= NULL;
LPDIRECT3DTEXTURE9				GPostProcessor::RenderTarget2	= NULL;

LPDIRECT3DVERTEXSHADER9			GPostProcessor::VSTransform		= NULL;
LPDIRECT3DPIXELSHADER9			GPostProcessor::PSGaussian		= NULL;
LPDIRECT3DPIXELSHADER9			GPostProcessor::PSModAlpha		= NULL;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool				GPostProcessor::Initialise()
{
	assert( DXUTGetD3DDevice() );

	//Create a vertex buffer for the screen-aligned quad
	const int vb_size = 4 * sizeof( SCREEN_QUAD_VERTEX );

	if( FAILED( DXUTGetD3DDevice()->CreateVertexBuffer( vb_size, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &ScreenQuad, NULL ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not create post-processing screen quad vb\n" );
		return false;
	}

	SCREEN_QUAD_VERTEX vertices[] =
	{
		{ -1.0f, 1.0f, 0.5f, 0.0f, 0.0f },
		{ 1.0f, 1.0f, 0.5f, 1.0f, 0.0f },
		{ 1.0f, -1.0f, 0.5f, 1.0f, 1.0f },
		{ -1.0f, -1.0f, 0.5f, 0.0f, 1.0f },
	};

	SCREEN_QUAD_VERTEX *buffer = NULL;
	if( FAILED( ScreenQuad->Lock( 0, sizeof( vertices ), (void**)&buffer, 0 ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not lock post-processing screen quad vb\n" );
		SAFE_RELEASE( ScreenQuad );
		return false;
	}

	memcpy( buffer, vertices, sizeof( vertices ) );
	ScreenQuad->Unlock();

	//Create a vertex declaration for the quad
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END(),
	};
	if( FAILED( DXUTGetD3DDevice()->CreateVertexDeclaration( decl, &ScreenQuadDecl ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not create screen quad decl\n" );
		SAFE_RELEASE( ScreenQuad );
		return false;
	}

	//Load shaders
	if( ! LoadShader( "assets/shaders/transform.vsh", &VSTransform ) ||
		! LoadShader( "assets/shaders/gaussian.psh", &PSGaussian ) ||
		! LoadShader( "assets/shaders/modalpha.psh", &PSModAlpha ) )
	{
		SAFE_RELEASE( ScreenQuad );
		SAFE_RELEASE( ScreenQuadDecl );

		SAFE_RELEASE( VSTransform );
		SAFE_RELEASE( PSGaussian );
		SAFE_RELEASE( PSModAlpha );

		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GPostProcessor::Shutdown()
{
	SAFE_RELEASE( ScreenQuad );
	SAFE_RELEASE( ScreenQuadDecl );

	SAFE_RELEASE( VSTransform );
	SAFE_RELEASE( PSGaussian );
	SAFE_RELEASE( PSModAlpha );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GPostProcessor::InitialiseDefaultPool()
{
	//Create rendertargets for post processing
	const D3DSURFACE_DESC *surface_desc	= DXUTGetBackBufferSurfaceDesc();
	const unsigned int width_small		= surface_desc->Width / 2;
	const unsigned int height_small		= surface_desc->Height / 2;

	if( FAILED( DXUTGetD3DDevice()->CreateTexture( width_small, height_small, 1, D3DUSAGE_RENDERTARGET,
		surface_desc->Format, D3DPOOL_DEFAULT, &RenderTarget1, NULL ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not create rendertarget 1\n" );

		return false;
	}

	if( FAILED( DXUTGetD3DDevice()->CreateTexture( width_small, height_small, 1, D3DUSAGE_RENDERTARGET,
		surface_desc->Format, D3DPOOL_DEFAULT, &RenderTarget2, NULL ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not create rendertarget 2\n" );
		SAFE_RELEASE( RenderTarget1 );

		return false;
	}

	//Set samplers
	DXUTGetD3DDevice()->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	DXUTGetD3DDevice()->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	DXUTGetD3DDevice()->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GPostProcessor::ShutdownDefaultPool()
{
	SAFE_RELEASE( RenderTarget1 );
	SAFE_RELEASE( RenderTarget2 );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GPostProcessor::ApplyEffect( PP_EFFECT effect )
{
	switch( effect )
	{
	case PP_EFFECT_GLOW:
		return ApplyGlow();

	default:
		OutputDebugStringA( "PT_ERROR: Unknown effect type\n" );
		return false;
	};

	//Should never get here
	assert( false );
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GPostProcessor::LoadShader( const char *filename, LPDIRECT3DVERTEXSHADER9 *output )
{
	LPD3DXBUFFER code	= NULL;
	LPD3DXBUFFER errors	= NULL;

	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	if( FAILED( D3DXAssembleShaderFromFileA( filename, NULL, NULL, flags, &code, &errors ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Failed to assemble vertex shader, errors:\n" );
		OutputDebugStringA( (char*)errors->GetBufferPointer() );
		OutputDebugStringA( "\n" );

		SAFE_RELEASE( code );
		SAFE_RELEASE( errors );

		return false;
	}

	if( FAILED( DXUTGetD3DDevice()->CreateVertexShader( (DWORD*)code->GetBufferPointer(), output ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Failed to create vertex shader\n" );

		SAFE_RELEASE( code );
		SAFE_RELEASE( errors );

		return false;
	}

	SAFE_RELEASE( code );
	SAFE_RELEASE( errors );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GPostProcessor::LoadShader( const char *filename, LPDIRECT3DPIXELSHADER9 *output )
{
	LPD3DXBUFFER code	= NULL;
	LPD3DXBUFFER errors	= NULL;

	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	if( FAILED( D3DXAssembleShaderFromFileA( filename, NULL, NULL, flags, &code, &errors ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Failed to assemble vertex shader, errors:\n" );
		OutputDebugStringA( (char*)errors->GetBufferPointer() );
		OutputDebugStringA( "\n" );

		SAFE_RELEASE( code );
		SAFE_RELEASE( errors );

		return false;
	}

	if( FAILED( DXUTGetD3DDevice()->CreatePixelShader( (DWORD*)code->GetBufferPointer(), output ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Failed to create vertex shader\n" );

		SAFE_RELEASE( code );
		SAFE_RELEASE( errors );

		return false;
	}

	SAFE_RELEASE( code );
	SAFE_RELEASE( errors );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GPostProcessor::ApplyGlow()
{
	const LPDIRECT3DDEVICE9 d3d_device	= DXUTGetD3DDevice();
	const D3DSURFACE_DESC *surface_desc	= DXUTGetBackBufferSurfaceDesc();

	LPDIRECT3DSURFACE9 back_buffer		= NULL;
	LPDIRECT3DSURFACE9 texture_surface	= NULL;

	d3d_device->SetRenderState( D3DRS_ZENABLE, FALSE );
	d3d_device->SetStreamSource( 0, ScreenQuad, 0, sizeof( SCREEN_QUAD_VERTEX ) );
	d3d_device->SetVertexDeclaration( ScreenQuadDecl );

	//1) Copy the backbuffer to a texture
	d3d_device->GetRenderTarget( 0, &back_buffer );
	RenderTarget1->GetSurfaceLevel( 0, &texture_surface );
	d3d_device->StretchRect( back_buffer, NULL, texture_surface, NULL, D3DTEXF_NONE );
	SAFE_RELEASE( texture_surface );

	//2) Modulate the colour by the alpha
	RenderTarget2->GetSurfaceLevel( 0, &texture_surface );
	d3d_device->SetRenderTarget( 0, texture_surface );
	SAFE_RELEASE( texture_surface );
	d3d_device->SetTexture( 0, RenderTarget1 );

	d3d_device->SetVertexShader( VSTransform );
	d3d_device->SetPixelShader( PSModAlpha );

	if( SUCCEEDED( d3d_device->BeginScene() ) )
	{
		d3d_device->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
		d3d_device->EndScene();
	}
	
	//3) Blur the texture in the x direction
	RenderTarget1->GetSurfaceLevel( 0, &texture_surface );
	d3d_device->SetRenderTarget( 0, texture_surface );
	SAFE_RELEASE( texture_surface );
	d3d_device->SetTexture( 0, RenderTarget2 );

	d3d_device->SetPixelShader( PSGaussian );
	const float x_step = PT_PP_GAUSSIAN_AMOUNT / surface_desc->Height;
	const D3DXVECTOR4 step_vec_x( x_step, 0.0f, 1.0f, 1.0f );
	d3d_device->SetPixelShaderConstantF( 0, (float*)&step_vec_x, 1 );

	if( SUCCEEDED( d3d_device->BeginScene() ) )
	{
		d3d_device->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
		d3d_device->EndScene();
	}

	//4) Blur the texture in the y direction
	RenderTarget2->GetSurfaceLevel( 0, &texture_surface );
	d3d_device->SetRenderTarget( 0, texture_surface );
	SAFE_RELEASE( texture_surface );
	d3d_device->SetTexture( 0, RenderTarget1 );

	const float y_step = PT_PP_GAUSSIAN_AMOUNT / surface_desc->Width;
	const D3DXVECTOR4 step_vec_y( 0.0f, y_step, 1.0f, 1.0f );
	d3d_device->SetPixelShaderConstantF( 0, (float*)&step_vec_y, 1 );

	if( SUCCEEDED( d3d_device->BeginScene() ) )
	{
		d3d_device->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
		d3d_device->EndScene();
	}

	//5) Additively blend the texture with the back buffer
	d3d_device->SetRenderTarget( 0, back_buffer );
	SAFE_RELEASE( back_buffer );
	d3d_device->SetTexture( 0, RenderTarget1 );
	d3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	d3d_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	d3d_device->SetPixelShader( NULL );

	if( SUCCEEDED( d3d_device->BeginScene() ) )
	{		
		d3d_device->DrawPrimitive( D3DPT_TRIANGLEFAN, 0, 2 );
		d3d_device->EndScene();
	}

	//Tidy up render states
	d3d_device->SetRenderState( D3DRS_ZENABLE, TRUE );
	d3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	d3d_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ZERO );
	d3d_device->SetStreamSource( 0, NULL, 0, 0 );
	d3d_device->SetVertexShader( NULL );
	d3d_device->SetPixelShader( NULL );
	d3d_device->SetTexture( 0, NULL );

   	return true;
}
