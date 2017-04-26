//------------------------------------------------------------------------------
// File: Background.h
// Desc: Global system to maintain background rendering
//
// Created: 21 September 2005 17:54:40
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_BACKGROUND_H
#define INCLUSIONGUARD_BACKGROUND_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <d3d9.h>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class GBackground
{
public:
	static bool							Initialise();
	static bool							Shutdown();

	static bool							Process();
	static bool							Render( float frame_delta );

private:
	static LPDIRECT3DTEXTURE9			Texture;
	static LPDIRECT3DVERTEXBUFFER9		QuadVB;
	static LPDIRECT3DVERTEXDECLARATION9	QuadDecl;

	static unsigned int					TextureWidth;
	static unsigned int					TextureHeight;

	//Starfield
	const static unsigned int			NUM_STARS = 100;
	static unsigned int					StarsX[ NUM_STARS ];
	static unsigned int					StarsY[ NUM_STARS ];
	static unsigned int					StarsOldY[ NUM_STARS ];
	static unsigned int					StarsSpeed[ NUM_STARS ];
	
	static bool							Initialised;
};

#endif //INCLUSIONGUARD_BACKGROUND_H
