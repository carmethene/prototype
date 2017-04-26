//------------------------------------------------------------------------------
// File: TextManager.h
// Desc: 
//
// Created: 22 September 2005 15:50:49
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_TEXTMANAGER_H
#define INCLUSIONGUARD_TEXTMANAGER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <d3dx9.h>
#include <map>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class GTextManager
{
public:
	static bool				Initialise();
	static bool				Shutdown();

	static bool				CreateText( const char *text, unsigned int &text_id );
	static bool				DestroyText( unsigned int text_id );

	static bool				RenderText( unsigned int text_id, float pos_x, float pos_y, float scale, float spin );

private:
	typedef			std::map< unsigned int, LPD3DXMESH >		MeshMap;
	
	static MeshMap			TextMeshes;

	static unsigned int		NextTextID;

	static bool				Initialised;
};


#endif //INCLUSIONGUARD_TEXTMANAGER_H
