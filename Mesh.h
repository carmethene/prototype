//------------------------------------------------------------------------------
// File: Mesh.h
// Desc: Container for meshes and materials
//
// Created: 15 September 2005 21:45:46
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_MESH_H
#define INCLUSIONGUARD_MESH_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <d3dx9.h>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class Mesh
{
public:
	Mesh();
	~Mesh();

	bool				LoadMesh( const char * const filename );
	bool				Destroy();

	bool				Draw();

	float				GetRadius()						{ return Radius; }

private:
	LPD3DXMESH			MeshObject;
	LPD3DXMATERIAL		Materials;
	LPDIRECT3DTEXTURE9*	Textures;
	DWORD				NumMaterials;

	float				Radius;

	bool				MeshLoaded;

};


#endif //INCLUSIONGUARD_MESH_H
