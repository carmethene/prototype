//------------------------------------------------------------------------------
// File: HUD.cpp
// Desc: The in-game UI
//
// Created: 16 September 2005 11:01:51
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "HUD.h"
#include "AssetManager.h"
#include "Mesh.h"
#include "Constants.h"
#include <cassert>


//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
struct LINE_VERTEX
{
	DWORD diffuse;
	float x,y,z;
};

//------------------------------------------------------------------------------------------------------------
HUD::HUD()
{
	memset( &NumberMeshes, 0, sizeof( NumberMeshes ) );
	CylinderMesh	= NULL;
	LineVB			= NULL;
	LineDecl		= NULL;

	Lives	= 0;
	Score	= 0;
	Health	= 0.0f;

	Initialised = false;
}

//------------------------------------------------------------------------------------------------------------
HUD::~HUD()
{

}

//------------------------------------------------------------------------------------------------------------
bool					HUD::Initialise()
{
	assert( ! Initialised );

	//Create a cylinder mesh for the health bar
	if( FAILED( D3DXCreateCylinder( DXUTGetD3DDevice(), 0.5f, 0.5f, 1.0f, 20, 1, &CylinderMesh, NULL ) ) )
	{
		return false;
	}

	//Create a line to mark off the playing field
	if( FAILED( DXUTGetD3DDevice()->CreateVertexBuffer( 3 * sizeof( LINE_VERTEX ), D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &LineVB, NULL ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory creating line mesh\n" );
		SAFE_RELEASE( CylinderMesh );
		return false;
	}

	void *buffer = NULL;
	if( FAILED( LineVB->Lock( 0, 3 * sizeof(LINE_VERTEX), &buffer, 0 ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not lock line vb\n" );
		SAFE_RELEASE( CylinderMesh );
		SAFE_RELEASE( LineVB );
		return false;
	}

	LINE_VERTEX vertices[] = 
	{
		{ 0xffffffff, PT_WORLD_WIDTH,	PT_WORLD_HEIGHT,	0.0f },
		{ 0xffffffff, PT_WORLD_WIDTH,	0.0f,				0.0f },
		{ 0xffffffff, 0.0f,				0.0f,				0.0f },
	};

	memcpy( buffer, vertices, sizeof(vertices) );
	LineVB->Unlock();

	//Create a vertex declaration for the line
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, 4, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		D3DDECL_END(),
	};

	if( FAILED( DXUTGetD3DDevice()->CreateVertexDeclaration( decl, &LineDecl ) ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not create line decl\n" );
		SAFE_RELEASE( CylinderMesh );
		SAFE_RELEASE( LineVB );
		return false;
	}

	//Create the score font meshes...

	//Register our font with Windows
	if( ! AddFontResourceA( PT_FONT_FILE ) )
	{
		OutputDebugStringA( "PT_ERROR: Could not load font file\n" );
		RemoveFontResourceA( PT_FONT_FILE );
		return false;
	}

	//Create the windows font object
	HDC hdc = CreateCompatibleDC( NULL );
	if( hdc == NULL )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory creating font\n" );
		SAFE_RELEASE( CylinderMesh );
		SAFE_RELEASE( LineVB );
		SAFE_RELEASE( LineDecl );
		RemoveFontResourceA( PT_FONT_FILE );
		return false;
	}

	int font_height = -MulDiv( PT_FONT_SIZE, GetDeviceCaps(hdc, LOGPIXELSY), 72 );

	HFONT font_handle = CreateFontA( font_height, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, PT_FONT_NAME );
	if( font_handle == NULL )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory creating font\n" );
		SAFE_RELEASE( CylinderMesh );
		SAFE_RELEASE( LineVB );
		SAFE_RELEASE( LineDecl );
		DeleteDC( hdc );
		RemoveFontResourceA( PT_FONT_FILE );
		return false;
	}

	//Create the meshes from the font object
	HFONT old_font_handle = (HFONT)SelectObject(hdc, font_handle);
	char text[ 2 ] = { 0 };

	for( unsigned int i = 0; i < 10; ++i )
	{
		text[ 0 ] = '0' + i;
		if( FAILED( D3DXCreateTextA( DXUTGetD3DDevice(), hdc, text, 0.001f, 0.2f, &(NumberMeshes[ i ]), NULL, NULL ) ) )
		{
			NumberMeshes[ i ] = NULL;
		}
	}

	//Tidy up font data
	SelectObject( hdc, old_font_handle );
	DeleteObject( font_handle );
	DeleteDC( hdc );
	RemoveFontResourceA( PT_FONT_FILE );

	Initialised = true;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					HUD::Shutdown()
{
	assert( Initialised );

	for( unsigned int i = 0; i < 10; ++i )
	{
		SAFE_RELEASE( NumberMeshes[ i ] );
	}

	SAFE_RELEASE( CylinderMesh );
	SAFE_RELEASE( LineVB );
	SAFE_RELEASE( LineDecl );

	Initialised = false;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					HUD::Render()
{
	assert( Initialised );

	StartLights();

	if( ! RenderLines() )
	{
		return false;
	}

	if( ! RenderScore() )
	{
		return false;
	}

	if( ! RenderLives() )
	{
		return false;
	}

	if( ! RenderHealth() )
	{
		return false;
	}

	StopLights();

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					HUD::StartLights()
{
	assert( DXUTGetD3DDevice() );

	//Set up fixed lighting
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

	memset( &light, 0, sizeof( D3DLIGHT9 ) );
	light.Type			= D3DLIGHT_POINT;
	light.Position		= D3DXVECTOR3( -(PT_GUI_WIDTH / 2.0f), PT_WORLD_WIDTH / 2.0f, -PT_CAMERA_DEPTH );

	light.Diffuse.r		= 10.0f;
	light.Diffuse.g		= 10.0f;
	light.Diffuse.b		= 10.0f;
	light.Range			= 1000.0f;
	light.Attenuation0	= 1.0f;
	DXUTGetD3DDevice()->SetLight( 1, &light );
	DXUTGetD3DDevice()->LightEnable( 1, TRUE );

	DXUTGetD3DDevice()->SetRenderState( D3DRS_LIGHTING, TRUE );
	DXUTGetD3DDevice()->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_ARGB( 0xff, 0xff, 0xff, 0xff ) );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					HUD::StopLights()
{
	assert( DXUTGetD3DDevice() );

	DXUTGetD3DDevice()->LightEnable( 0, FALSE );
	DXUTGetD3DDevice()->LightEnable( 1, FALSE );
	DXUTGetD3DDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					HUD::RenderLines()
{
	D3DMATERIAL9 font_material;
	memset( &font_material, 0, sizeof( D3DMATERIAL9 ) );
	memcpy( &(font_material.Ambient), PT_GUI_COLOUR_LINE_AMBIENT, 4 * sizeof( float ) );
	DXUTGetD3DDevice()->SetMaterial( &font_material );

	D3DXMATRIX transform;
	D3DXMatrixIdentity( &transform );
	DXUTGetD3DDevice()->SetTransform( D3DTS_WORLD, &transform );
	DXUTGetD3DDevice()->SetStreamSource( 0, LineVB, 0, sizeof( LINE_VERTEX ) );
	DXUTGetD3DDevice()->SetVertexDeclaration( LineDecl );
	DXUTGetD3DDevice()->DrawPrimitive( D3DPT_LINESTRIP, 0, 2 );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					HUD::RenderScore()
{
	assert( DXUTGetD3DDevice() );

	const float font_size = (PT_GUI_WIDTH / 2.0f);
	float pos_x = (font_size * PT_GUI_SCORE_COLUMNS) - (font_size / 2.0f);
	float pos_y = -PT_GUI_WIDTH * 0.68f;

	//Create the font material
	D3DMATERIAL9 font_material;
	memset( &font_material, 0, sizeof( D3DMATERIAL9 ) );
	memcpy( &(font_material.Diffuse), PT_FONT_COLOUR_DIFFUSE, 4 * sizeof( float ) );
	memcpy( &(font_material.Ambient), PT_FONT_COLOUR_AMBIENT, 4 * sizeof( float ) );
    DXUTGetD3DDevice()->SetMaterial( &font_material );

	//Need a temporary copy of the score since we're going to mangle it
	unsigned int temp_score = Score;

	for( unsigned int i = 0; i < PT_GUI_SCORE_COLUMNS; ++i )
	{
		//Extract the next digit
        unsigned int digit = temp_score % 10;
		temp_score /= 10;

        //Transform the mesh to the correct position on screen
		const static float font_scale = PT_GUI_WIDTH * 0.6f;
		D3DXMATRIX mat_scale;
		D3DXMatrixScaling( &mat_scale, font_scale, font_scale, font_scale );

		D3DXMATRIX mat_translation;
		D3DXMatrixTranslation( &mat_translation, pos_x, pos_y, 0.0f );

		D3DXMATRIX world_transform;
		D3DXMatrixMultiply( &world_transform, &mat_scale, &mat_translation );
		DXUTGetD3DDevice()->SetTransform( D3DTS_WORLD, &world_transform );

		//Draw the mesh
        if( NumberMeshes[digit] )
		{
			if( FAILED( NumberMeshes[digit]->DrawSubset( 0 ) ) )
			{
				OutputDebugStringA( "PT_ERROR: Font mesh failed to draw\n" );
				return false;
			}
		}
		
		//Update the char positions
		pos_x -= font_size;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					HUD::RenderLives()
{
	//Get the ship mesh
	Mesh *mesh = GAssetManager::GetMesh( 0 );
	if( ! mesh )
	{
		return false;
	}

	//Transform
	const float scale = 2.0f;
	const float x_step = PT_GUI_WIDTH;
	float pos_x = PT_WORLD_WIDTH + (PT_GUI_WIDTH / 2.0f);
	float pos_y = -(PT_GUI_WIDTH * 0.5f);

	for( unsigned int i = 0; i < Lives; ++i )
	{
		D3DXMATRIX mat_scale;
		D3DXMatrixScaling( &mat_scale, scale, scale, scale );

		D3DXMATRIX mat_rotation_x;
		D3DXMatrixRotationX( &mat_rotation_x, D3DX_PI / 2 );

		D3DXMATRIX mat_translation;
		D3DXMatrixTranslation( &mat_translation, pos_x, pos_y, 0.0f );

		D3DXMATRIX world_transform;
		D3DXMatrixMultiply( &world_transform, &mat_rotation_x, &mat_translation );
		D3DXMatrixMultiply( &world_transform, &mat_scale, &world_transform );
		DXUTGetD3DDevice()->SetTransform( D3DTS_WORLD, &world_transform );

		//Render
		mesh->Draw();
		
		pos_x -= x_step;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool					HUD::RenderHealth()
{
	assert( CylinderMesh );

	//Some constant values
	const float gui_width_by_2 = PT_GUI_WIDTH / 2.0f;
	const float margin = gui_width_by_2 / 2.0f;
	const float world_height_with_margin = PT_WORLD_HEIGHT - gui_width_by_2;

	//Render the background cylinder...
	//Transform
	D3DXMATRIX mat_rotate_x;
	D3DXMatrixRotationX( &mat_rotate_x, D3DX_PI / 2.0f );

	D3DXMATRIX mat_scale;
	D3DXMatrixScaling( &mat_scale, gui_width_by_2, world_height_with_margin, 1.0f );

	D3DXMATRIX mat_translation;
	D3DXMatrixTranslation( &mat_translation, PT_WORLD_WIDTH + gui_width_by_2, PT_WORLD_HEIGHT / 2.0f, 0.0f );

	D3DXMATRIX world_transform;
	D3DXMatrixMultiply( &world_transform, &mat_scale, &mat_translation );
	D3DXMatrixMultiply( &world_transform, &mat_rotate_x, &world_transform );
	DXUTGetD3DDevice()->SetTransform( D3DTS_WORLD, &world_transform );

	//Material
	D3DMATERIAL9 health_material;
	memset( &health_material, 0, sizeof( D3DMATERIAL9 ) );
	memcpy( &(health_material.Diffuse), PT_GUI_COLOUR_HEALTH_BG, 4 * sizeof( float ) );
	memcpy( &(health_material.Ambient), PT_GUI_COLOUR_HEALTH_AMBIENT, 4 * sizeof( float ) );
	DXUTGetD3DDevice()->SetMaterial( &health_material );

	//Render
	CylinderMesh->DrawSubset( 0 );

	//Render the foreground cylinder
	//Transform
	const float scale = world_height_with_margin * Health;
	D3DXMatrixScaling( &mat_scale, gui_width_by_2, scale, 1.0f );

	D3DXMatrixTranslation( &mat_translation, PT_WORLD_WIDTH + gui_width_by_2, (scale / 2.0f) + margin, -1.0f );

	D3DXMatrixMultiply( &world_transform, &mat_scale, &mat_translation );
	D3DXMatrixMultiply( &world_transform, &mat_rotate_x, &world_transform );
	DXUTGetD3DDevice()->SetTransform( D3DTS_WORLD, &world_transform );

	//Material
	memset( &health_material, 0, sizeof( D3DMATERIAL9 ) );
	memcpy( &(health_material.Diffuse), PT_GUI_COLOUR_HEALTH_FG, 4 * sizeof( float ) );
	memcpy( &(health_material.Ambient), PT_GUI_COLOUR_HEALTH_AMBIENT, 4 * sizeof( float ) );
	DXUTGetD3DDevice()->SetMaterial( &health_material );

	CylinderMesh->DrawSubset( 0 );

	return true;
}
