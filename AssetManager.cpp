//------------------------------------------------------------------------------
// File: AssetManager.cpp
// Desc: Stores direct3d assets for use in the game
//
// Created: 14 September 2005 14:21:03
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "AssetManager.h"
#include "Mesh.h"
#include <fstream>
#include <string>
#include <sstream>
#include <cassert>

//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
GAssetManager::MeshMap			GAssetManager::Meshes;
GAssetManager::TextureMap		GAssetManager::Textures;
bool							GAssetManager::Initialised = false;

//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool				GAssetManager::Initialise()
{
	assert( ! Initialised );

	if( ! LoadManifest() )
	{
		return false;
	}

	Initialised = true;
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GAssetManager::Shutdown()
{
	assert( Initialised );

	for( MeshMap::iterator mesh_iter = Meshes.begin(); mesh_iter != Meshes.end(); ++mesh_iter )
	{
		Mesh *mesh = mesh_iter->second;
		mesh->Destroy();
		delete mesh;
	}
	Meshes.clear();

	for( TextureMap::iterator texture_iter = Textures.begin(); texture_iter != Textures.end(); ++texture_iter )
	{
		LPDIRECT3DTEXTURE9 texture = texture_iter->second;
		texture->Release();
	}
	Textures.clear();

	Initialised = false;
	return true;
}

//------------------------------------------------------------------------------------------------------------
Mesh*				GAssetManager::GetMesh( unsigned int number )
{
	assert( Initialised );

	MeshMap::const_iterator iter = Meshes.find( number );
	if( iter == Meshes.end() )
	{
		assert( false );

		std::stringstream error_str;
		error_str << "PT_ERROR: Mesh #" << number << " does not exist!\n";
		OutputDebugStringA( error_str.str().c_str() );

		return NULL;
	}

	return iter->second;
}

//------------------------------------------------------------------------------------------------------------
LPDIRECT3DTEXTURE9	GAssetManager::GetTexture( unsigned int number )
{
	assert( Initialised );

	TextureMap::const_iterator iter = Textures.find( number );
	if( iter == Textures.end() )
	{
		assert( false );

		std::stringstream error_str;
		error_str << "PT_ERROR: Texture #" << number << " does not exist!\n";
		OutputDebugStringA( error_str.str().c_str() );

		return NULL;
	}

	return iter->second;
}

//------------------------------------------------------------------------------------------------------------
bool				GAssetManager::LoadManifest()
{
	assert( ! Initialised );

	std::ifstream fin( "assets/assets.txt" );
	if( ! fin )
	{
		OutputDebugStringA( "PT_ERROR: Could not open asset manifest file\n" );
		return false;
	}

	const size_t MAX_LINE = 1024;
	char line_buffer[ MAX_LINE ];
	unsigned int line_count = 1;

	while( fin )
	{
		//get the line
		fin.getline( line_buffer, MAX_LINE );
		std::stringstream line_stream;
		line_stream.str( line_buffer );
		line_stream >> std::ws;
		if( line_stream.str().size() == 0 )
		{
			continue;
		}

		if( line_stream.str()[0] == '#' )
		{
			//this is a comment
			continue;
		}

		//Get details of the asset
		std::string asset_type, asset_file;
		unsigned int asset_number = 0;

		line_stream >> asset_type;
		line_stream >> asset_number;
		line_stream >> asset_file;

		//load the asset
		if( asset_type == "MESH" )
		{
			LoadMesh( asset_number, asset_file );
		}
		else if( asset_type == "TEXTURE" )
		{
			LoadTexture( asset_number, asset_file );
		}
		else
		{
			std::stringstream error_str;
			error_str << "PT_ERROR: Assets.txt#";
			error_str << line_count;
			error_str << " - Unknown asset type: ";
			error_str << asset_type;
			error_str << "\n";
			OutputDebugStringA( error_str.str().c_str() );
		}

		++line_count;
	}
	fin.close();

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GAssetManager::LoadMesh( unsigned int number, std::string &file )
{
	assert( ! Initialised );
	
	if( Meshes.find( number ) != Meshes.end() )
	{
		std::stringstream error_str;
		error_str << "PT_ERROR: Mesh #" << number << " has already been assigned a file\n";
		OutputDebugStringA( error_str.str().c_str() );
		return false;
	}

	Mesh *mesh = NULL;
	try
	{
		mesh = new Mesh;
	}
	catch( std::bad_alloc& )
	{
		OutputDebugStringA( "PT_ERROR: Out of memory loading mesh\n" );
		return false;
	}

	assert( mesh );

	std::string real_file = std::string( "assets/" ) + file;
	if( ! mesh->LoadMesh( real_file.c_str() ) )
	{
		return false;
	}

	Meshes[ number ] = mesh;

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool				GAssetManager::LoadTexture( unsigned int number, std::string &file )
{
	assert( ! Initialised );

	if( Textures.find( number ) != Textures.end() )
	{
		std::stringstream error_str;
		error_str << "PT_ERROR: Texture #" << number << " has already been assigned a file\n";
		OutputDebugStringA( error_str.str().c_str() );
		return false;
	}

	LPDIRECT3DTEXTURE9 texture;
	std::string real_file = std::string( "assets/" ) + file;
	
	if( FAILED( D3DXCreateTextureFromFileExA( DXUTGetD3DDevice(), real_file.c_str(), D3DX_DEFAULT, D3DX_DEFAULT,
					1, 0, D3DFMT_FROM_FILE, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &texture ) ) )
	{
		std::stringstream error_str;
		error_str << "PT_ERROR: Could not load texture " << file << "\n";
		OutputDebugStringA( error_str.str().c_str() );
		return false;
	}

	Textures[ number ] = texture;
	return true;
}
