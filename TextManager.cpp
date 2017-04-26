//------------------------------------------------------------------------------
// File: TextManager.cpp
// Desc: Manages text rendering
//
// Created: 22 September 2005 15:50:50
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "TextManager.h"
#include "constants.h"
#include <cassert>


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
GTextManager::MeshMap		GTextManager::TextMeshes;
unsigned int				GTextManager::NextTextID = 0;
bool						GTextManager::Initialised = false;


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool				GTextManager::Initialise()
{
	assert( ! Initialised );

	Initialised = true;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GTextManager::Shutdown()
{
	assert( Initialised );

	for( MeshMap::iterator iter = TextMeshes.begin(); iter != TextMeshes.end(); ++iter )
	{
		LPD3DXMESH mesh = iter->second;
		SAFE_RELEASE( mesh );
	}
	TextMeshes.clear();

	NextTextID = 0;

	Initialised = false;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GTextManager::CreateText( const char *text, unsigned int &text_id )
{
	assert( Initialised );
	assert( TextMeshes.find( NextTextID ) == TextMeshes.end() );

	//Register our font with Windows
	if( ! AddFontResourceA( PT_FONT_FILE ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not load font file\n" );
		return false;
	}

	//Create the windows font object
	HDC hdc = CreateCompatibleDC( NULL );
	if( hdc == NULL )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory creating font\n" );
		RemoveFontResourceA( PT_FONT_FILE );
		return false;
	}

	int font_height = -MulDiv( PT_FONT_SIZE, GetDeviceCaps(hdc, LOGPIXELSY), 72 );

	HFONT font_handle = CreateFontA( font_height, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, PT_FONT_NAME );
	if( font_handle == NULL )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory creating font\n" );
		DeleteDC( hdc );
		RemoveFontResourceA( PT_FONT_FILE );
		return false;
	}

	//Create the meshes from the font object
	HFONT old_font_handle = (HFONT)SelectObject(hdc, font_handle);

	LPD3DXMESH text_mesh = NULL;

	if( FAILED( D3DXCreateTextA( DXUTGetD3DDevice(), hdc, text, 0.001f, 0.2f, &text_mesh, NULL, NULL ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory creating font\n" );
		SelectObject( hdc, old_font_handle );
		DeleteObject( font_handle );
		DeleteDC( hdc );
		RemoveFontResourceA( PT_FONT_FILE );
		return false;
	}

	//Tidy up font data
	SelectObject( hdc, old_font_handle );
	DeleteObject( font_handle );
	DeleteDC( hdc );
	RemoveFontResourceA( PT_FONT_FILE );

	//Store the text mesh
	text_id = NextTextID;
	TextMeshes[ NextTextID ] = text_mesh;
	++NextTextID;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GTextManager::DestroyText( unsigned int text_id )
{
	assert( Initialised );

	MeshMap::iterator iter = TextMeshes.find( text_id );
	if( iter == TextMeshes.end() )
	{
		OutputDebugStringA( "PT_ERROR: Could not find text id\n" );
		return false;
	}

	LPD3DXMESH mesh = iter->second;
	SAFE_RELEASE( mesh );
	TextMeshes.erase( iter );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GTextManager::RenderText( unsigned int text_id, float pos_x, float pos_y, float scale, float spin )
{
	assert( Initialised );
	assert( DXUTGetD3DDevice() );

	//Find the mesh
	MeshMap::iterator iter = TextMeshes.find( text_id );
	if( iter == TextMeshes.end() )
	{
		OutputDebugStringA( "PT_ERROR: Could not find text id\n" );
		return false;
	}

	LPD3DXMESH mesh = iter->second;

	//Set up lighting
	D3DLIGHT9 light;
	memset( &light, 0, sizeof( D3DLIGHT9 ) );
	light.Type			= D3DLIGHT_DIRECTIONAL;
	light.Position		= D3DXVECTOR3( (PT_WORLD_WIDTH+PT_GUI_WIDTH) / 2.0f, (PT_WORLD_HEIGHT-PT_GUI_WIDTH) / 2.0f, -PT_CAMERA_DEPTH );
	light.Direction		= D3DXVECTOR3( 0.0f, 0.0f, 1.0f );

	light.Diffuse.r		= 1.0f;
	light.Diffuse.g		= 1.0f;
	light.Diffuse.b		= 1.0f;
	light.Range			= 1000.0f;
	light.Attenuation0	= 0.0f;
	DXUTGetD3DDevice()->SetLight( 0, &light );
	DXUTGetD3DDevice()->LightEnable( 0, TRUE );
	DXUTGetD3DDevice()->SetRenderState( D3DRS_LIGHTING, TRUE );
	DXUTGetD3DDevice()->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0xff, 0xff, 0xff, 0xff ) );

	//Set up materials
	D3DMATERIAL9 font_material;
	memset( &font_material, 0, sizeof( D3DMATERIAL9 ) );
	memcpy( &(font_material.Ambient), PT_FONT_COLOUR_AMBIENT, 4 * sizeof( float ) );
	memcpy( &(font_material.Diffuse), PT_FONT_COLOUR_DIFFUSE, 4 * sizeof( float ) );
	DXUTGetD3DDevice()->SetMaterial( &font_material );

	//Set up transforms
	D3DXMATRIX mat_translation_y;
	static float offset = -0.3333f;
	D3DXMatrixTranslation( &mat_translation_y, 0.0f, offset, 0.0f );

	D3DXMATRIX mat_scale;
	D3DXMatrixScaling( &mat_scale, scale, scale, scale );
		
	D3DXMATRIX mat_rotation_x;
	D3DXMatrixRotationX( &mat_rotation_x, spin );

	D3DXMATRIX mat_translation;
	D3DXMatrixTranslation( &mat_translation, pos_x, pos_y, PT_CAMERA_DEPTH / -2.0f );

	D3DXMATRIX world_transform;
	D3DXMatrixMultiply( &world_transform, &mat_rotation_x, &mat_translation );
	D3DXMatrixMultiply( &world_transform, &mat_scale, &world_transform );
	D3DXMatrixMultiply( &world_transform, &mat_translation_y, &world_transform );

	DXUTGetD3DDevice()->SetTransform( D3DTS_WORLD, &world_transform );

	//Draw the mesh
	mesh->DrawSubset( 0 );

	//Tidy up lighting
	DXUTGetD3DDevice()->LightEnable( 0, FALSE );
	DXUTGetD3DDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );

	return true;
}
