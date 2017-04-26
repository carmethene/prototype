//------------------------------------------------------------------------------
// File: HUD.h
// Desc: The in-game UI
//
// Created: 16 September 2005 11:01:50
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_HUD_H
#define INCLUSIONGUARD_HUD_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <d3dx9.h>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class HUD
{
public:
	HUD();
	~HUD();

	bool							Initialise();
	bool							Shutdown();

	bool							Render();

	void							SetScore( unsigned int score )		{ Score = score; }
	void							SetLives( unsigned int lives )		{ Lives = min( lives, 2 ); }
	void							SetHealth( float health )			{ Health = max( 0.0f, health ); }

private:
	bool							StartLights();
	bool							StopLights();

	bool							RenderLines();
	bool							RenderScore();
	bool							RenderLives();
	bool							RenderHealth();

	//Direct3D data
	LPD3DXMESH						NumberMeshes[ 10 ];
	LPD3DXMESH						CylinderMesh;
	LPDIRECT3DVERTEXBUFFER9			LineVB;
	LPDIRECT3DVERTEXDECLARATION9	LineDecl;

	//State information
	unsigned int					Lives;
	unsigned int					Score;
	float							Health;		//Fraction 0.0 .. 1.0

	bool							Initialised;

};

#endif //INCLUSIONGUARD_HUD_H
