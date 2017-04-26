//------------------------------------------------------------------------------
// File: PostProcessor.h
// Desc: System to apply screen-space post processing effects
//
// Created: 21 September 2005 12:07:37
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_POSTPROCESSOR_H
#define INCLUSIONGUARD_POSTPROCESSOR_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
enum PP_EFFECT
{
	PP_EFFECT_GLOW,
};

class GPostProcessor
{
public:
	static bool							Initialise();
	static bool							Shutdown();

	static bool							InitialiseDefaultPool();
	static bool							ShutdownDefaultPool();

	static bool							ApplyEffect( PP_EFFECT effect );

private:
	static bool							LoadShader( const char *filename, LPDIRECT3DVERTEXSHADER9 *output );
	static bool							LoadShader( const char *filename, LPDIRECT3DPIXELSHADER9 *output );

	static bool							ApplyGlow();

	//Screen-aligned quad
	struct SCREEN_QUAD_VERTEX
	{
		float x, y, z;
		float u, v;
	};

	static LPDIRECT3DVERTEXBUFFER9		ScreenQuad;
	static LPDIRECT3DVERTEXDECLARATION9	ScreenQuadDecl;

	//Textures
	static LPDIRECT3DTEXTURE9			RenderTarget1;
	static LPDIRECT3DTEXTURE9			RenderTarget2;

	//Shaders
	static LPDIRECT3DVERTEXSHADER9		VSTransform;
	static LPDIRECT3DPIXELSHADER9		PSGaussian;
	static LPDIRECT3DPIXELSHADER9		PSModAlpha;

};


#endif //INCLUSIONGUARD_POSTPROCESSOR_H
