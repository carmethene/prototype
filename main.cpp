//------------------------------------------------------------------------------
// File: main.cpp
// Desc: Entry point
//
// Created: 14 September 2005 13:07:32
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "constants.h"
#include "Core.h"
#include "AssetManager.h"
#include "SoundManager.h"
#include "TextManager.h"
#include "PostProcessor.h"

#ifdef _DEBUG
#include <ctime>
#endif


//------------------------------------------------------------------------------
// Globals:
//------------------------------------------------------------------------------
bool	EnablePostProcessing = true;


//--------------------------------------------------------------------------------------
// Rejects any devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsDeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, 
								 D3DFORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	// Typically want to skip backbuffer formats that don't support alpha blending
	IDirect3D9* pD3D = DXUTGetD3DObject(); 
	if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
		AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, 
		D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
		return false;

	//We only want 32bit with alpha
	if( BackBufferFormat != D3DFMT_A8R8G8B8 )
	{
		return false;
	}

	//If post-processing is enabled, we require PS2.0
	if( EnablePostProcessing && pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
	{
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, const D3DCAPS9* pCaps, void* pUserContext )
{
	return true;
}

//--------------------------------------------------------------------------------------
// Create any D3DPOOL_MANAGED resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	if( ! GAssetManager::Initialise() )
	{
		return E_FAIL;
	}

	if( EnablePostProcessing )
	{
		if( ! GPostProcessor::Initialise() )
		{
			return E_FAIL;
		}
	}

	if( ! GTextManager::Initialise() )
	{
		return E_FAIL;
	}

	if( ! GCore::Initialise() )
	{
		return E_FAIL;
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3DPOOL_DEFAULT resources here 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9* pd3dDevice, 
							   const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	if( EnablePostProcessing )
	{
		if( ! GPostProcessor::InitialiseDefaultPool() )
		{
			return E_FAIL;
		}
	}

	if( ! GCore::RestoreDevice() )
	{
		return E_FAIL;
	}

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	//Update FPS in debug builds
#ifdef _DEBUG
	static unsigned int next_time = 0;
	const unsigned int time = clock();
	if( time >= next_time ) 
	{
		char buffer[ 100 ];
		sprintf( buffer, "%S - %f FPS", PT_WINDOW_TITLE, DXUTGetFPS() );
		SetWindowTextA( DXUTGetHWND(), buffer );
		
		next_time = time + 1000;
	}
#endif

	if( ! GCore::Process() )
	{
		PostQuitMessage( 0 );
	}
}


//--------------------------------------------------------------------------------------
// Render the scene 
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	HRESULT hr;

	// Clear the render target and the zbuffer 
	V( pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0) );

	// Render the scene
	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		GCore::Render();

		V( pd3dDevice->EndScene() );
	}

	//Apply the post processing
	if( EnablePostProcessing )
	{
		GPostProcessor::ApplyEffect( PP_EFFECT_GLOW );
	}
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
						 bool* pbNoFurtherProcessing, void* pUserContext )
{
	return 0;
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnResetDevice callback here 
//--------------------------------------------------------------------------------------
void CALLBACK OnLostDevice( void* pUserContext )
{
	if( EnablePostProcessing )
	{
		GPostProcessor::ShutdownDefaultPool();
	}
}


//--------------------------------------------------------------------------------------
// Release resources created in the OnCreateDevice callback here
//--------------------------------------------------------------------------------------
void CALLBACK OnDestroyDevice( void* pUserContext )
{
	GCore::Shutdown();

	GAssetManager::Shutdown();

	if( EnablePostProcessing )
	{
		GPostProcessor::Shutdown();
	}

	GTextManager::Shutdown();
}

//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int )
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

	// Set the callback functions
	DXUTSetCallbackDeviceCreated( OnCreateDevice );
	DXUTSetCallbackDeviceReset( OnResetDevice );
	DXUTSetCallbackDeviceLost( OnLostDevice );
	DXUTSetCallbackDeviceDestroyed( OnDestroyDevice );
	DXUTSetCallbackMsgProc( MsgProc );
	DXUTSetCallbackFrameRender( OnFrameRender );
	DXUTSetCallbackFrameMove( OnFrameMove );

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	// We require a stencil buffer
	CGrowableArray<D3DFORMAT>* pDSFormats = DXUTGetEnumeration()->GetPossibleDepthStencilFormatList();
	pDSFormats->RemoveAll();
	pDSFormats->Add( D3DFMT_D24S8 );
	pDSFormats->Add( D3DFMT_D24X4S4 );
	pDSFormats->Add( D3DFMT_D15S1 );
	pDSFormats->Add( D3DFMT_D24FS8 );

	// Initialize DXUT and create the desired Win32 window and Direct3D device for the application
	DXUTInit( false, false, false );
	DXUTSetMultimonSettings( false );
	DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
	DXUTCreateWindow( PT_WINDOW_TITLE );

	unsigned int screen_width = unsigned int( (PT_SCREEN_HEIGHT * PT_WORLD_WIDTH) / PT_WORLD_HEIGHT );

	//Try to create a device with post processing
	EnablePostProcessing = true;
	if( S_OK != DXUTCreateDevice( D3DADAPTER_DEFAULT, true, screen_width, PT_SCREEN_HEIGHT, IsDeviceAcceptable, ModifyDeviceSettings ) )
	{
		MessageBox( DXUTGetHWND(), L"Your video card does not support Pixel Shader 2.0\nVisual quality will be severely reduced", PT_WINDOW_TITLE, MB_OK | MB_ICONEXCLAMATION );
		EnablePostProcessing = false;
		DXUTCreateDevice( D3DADAPTER_DEFAULT, true, screen_width, PT_SCREEN_HEIGHT, IsDeviceAcceptable, ModifyDeviceSettings );
	}

	// Initialise the non-d3d systems
	GSoundManager::Initialise();

	// Start the game loop
	DXUTMainLoop();

	// Shut down the non-d3d systems
	GSoundManager::Shutdown();

	CoUninitialize();

	return DXUTGetExitCode();
}
