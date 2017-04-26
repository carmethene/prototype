//------------------------------------------------------------------------------
// File: Mesh.cpp
// Desc: Container for meshes and materials
//
// Created: 15 September 2005 21:45:48
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "Mesh.h"
#include <cassert>
#include <sstream>
#include <string>

//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
Mesh::Mesh()
{
	MeshObject		= NULL;
	Materials		= NULL;
	Textures		= NULL;
	NumMaterials	= 0;

	Radius = 0.0f;

	MeshLoaded = false;
}

//------------------------------------------------------------------------------------------------------------
Mesh::~Mesh()
{
	if( MeshLoaded )
	{
		Destroy();
	}
}

//------------------------------------------------------------------------------------------------------------
bool				Mesh::LoadMesh( const char * const filename )
{
	if( MeshLoaded )
	{
		assert( false );
		OutputDebugStringA( "Mesh already has loaded data\n" );
		return false;
	}

	//load the mesh
	LPD3DXBUFFER materials_buffer = NULL;

	if( FAILED( D3DXLoadMeshFromXA( filename, D3DXMESH_VB_MANAGED | D3DXMESH_IB_MANAGED, DXUTGetD3DDevice(), NULL, &materials_buffer, NULL, &NumMaterials, &MeshObject ) ) )
	{
		std::stringstream error_str;
		error_str << "PT_ERROR: Could not load mesh " << filename << "\n";
		OutputDebugStringA( error_str.str().c_str() );
		return false;
	}

	//extract the materials
	try
	{
		Materials = new D3DXMATERIAL[ NumMaterials ];
	}
	catch( std::bad_alloc& )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory loading mesh\n" );
		SAFE_RELEASE( materials_buffer );
		SAFE_RELEASE( MeshObject );
		return false;
	}
	D3DXMATERIAL* materials_buffer_ptr = reinterpret_cast< D3DXMATERIAL* >( materials_buffer->GetBufferPointer() );
	memcpy( Materials, materials_buffer_ptr, NumMaterials * sizeof( D3DXMATERIAL ) );

	//load the textures
	try
	{
		Textures = new LPDIRECT3DTEXTURE9[ NumMaterials ];
	}
	catch( std::bad_alloc& )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory loading mesh\n" );
		SAFE_DELETE_ARRAY( Materials );
		SAFE_RELEASE( materials_buffer );
		SAFE_RELEASE( MeshObject );
		return false;
	}
	memset( Textures, 0, NumMaterials * sizeof( LPDIRECT3DTEXTURE9 ) );

	for( DWORD i = 0; i < NumMaterials; ++ i )
	{
		if( materials_buffer_ptr[ i ].pTextureFilename != NULL )
		{
			//add the mesh's path to the texture filename
			std::string file = std::string( "assets/textures/" );
			file += materials_buffer_ptr[ i ].pTextureFilename;

			if( FAILED( D3DXCreateTextureFromFileA( DXUTGetD3DDevice(), file.c_str(), &Textures[ i ] ) ) )
			{
				OutputDebugStringA( "PT_ERROR: Failed to load mesh texture\n" );
				Textures[ i ] = NULL;
			}
		}

		//set up ambient light on all materials since the dx samples don't bother with it
		D3DCOLORVALUE &ambient = Materials[i].MatD3D.Ambient;
		ambient.r = ambient.g = ambient.b = 0.3f;
		ambient.a = 1.0f;

		//fix diffuse colours
		D3DCOLORVALUE &diffuse = Materials[i].MatD3D.Diffuse;
		diffuse.r = diffuse.b = 0.0f;
		diffuse.g = 0.3f;
		diffuse.a = 0.8f;
	}

	SAFE_RELEASE( materials_buffer );

	//Calculate the mesh bounding cylinder
	Radius = 0.0f;

	struct MeshVertex { D3DXVECTOR3 p, n; };
	MeshVertex* vertices = NULL;
	if( SUCCEEDED( MeshObject->LockVertexBuffer( 0, reinterpret_cast< void** >( &vertices ) ) ) )
	{
		for( unsigned int vertex = 0; vertex < MeshObject->GetNumVertices(); ++vertex )
		{
			const D3DXVECTOR3& currentVertex = vertices[ vertex ].p;
			Radius = max( Radius, currentVertex.x );
			Radius = max( Radius, -currentVertex.x );
			Radius = max( Radius, currentVertex.z );
			Radius = max( Radius, -currentVertex.z );
		}

		MeshObject->UnlockVertexBuffer();
	}

	MeshLoaded = true;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				Mesh::Destroy()
{
	if( ! MeshLoaded )
	{
		assert( false );
        OutputDebugStringA( "PT_ERROR: Mesh does not have data\n" );
		return false;
	}

	if( Textures )
	{
		for( DWORD i = 0; i < NumMaterials; ++i )
		{
			SAFE_RELEASE( Textures[ i ] );
		}
		SAFE_DELETE_ARRAY( Textures );
	}

	SAFE_DELETE_ARRAY( Materials );
	SAFE_RELEASE( MeshObject );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				Mesh::Draw()
{
	assert( DXUTGetD3DDevice() );

	if( ! MeshLoaded )
	{
		assert( false );
		OutputDebugStringA( "PT_ERROR: Mesh does not have data\n" );
		return false;
	}

	assert( MeshObject );

	for( DWORD material = 0; material < NumMaterials; ++material )
	{
		if( Textures && Textures[ material ] )
		{
			DXUTGetD3DDevice()->SetTexture( 0, Textures[ material ] );
		}
		DXUTGetD3DDevice()->SetMaterial( &(Materials[ material ].MatD3D) );

		MeshObject->DrawSubset( material );

		DXUTGetD3DDevice()->SetTexture( 0, NULL );
	}

	return true;
}
