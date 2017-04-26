//------------------------------------------------------------------------------
// File: AssetManager.h
// Desc: Stores direct3d assets for use in the game
//
// Created: 14 September 2005 14:21:23
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_ASSETMANAGER_H
#define INCLUSIONGUARD_ASSETMANAGER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <map>
#include <d3dx9.h>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class Mesh;


class GAssetManager
{
public:
	static bool						Initialise();
	static bool						Shutdown();

	static Mesh*					GetMesh( unsigned int number );
	static LPDIRECT3DTEXTURE9		GetTexture( unsigned int number );

private:
	static bool						LoadManifest();
	static bool						LoadMesh( unsigned int number, std::string &file );
	static bool						LoadTexture( unsigned int number, std::string &file );

	typedef		std::map< unsigned int, Mesh* >					MeshMap;
	typedef		std::map< unsigned int, LPDIRECT3DTEXTURE9 >	TextureMap;
	
	static MeshMap					Meshes;
	static TextureMap				Textures;

	static bool						Initialised;
};

#endif //INCLUSIONGUARD_ASSETMANAGER_H
