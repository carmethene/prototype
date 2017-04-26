//------------------------------------------------------------------------------
// File: InputManager.cpp
// Desc: Wrapper for directinput
//
// Created: 15 September 2005 14:52:26
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------
#pragma warning( disable : 4995 )


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include "dxstdafx.h"
#include "InputManager.h"


//------------------------------------------------------------------------------
// Statics:
//------------------------------------------------------------------------------
LPDIRECTINPUT8			GInputManager::DirectInput				= NULL;
LPDIRECTINPUTDEVICE8	GInputManager::KeyboardDevice			= NULL;
char					GInputManager::KeyState[256]			= {0};
char					GInputManager::PreviousKeyState[256]	= {0};

//------------------------------------------------------------------------------
// Definitions:
//------------------------------------------------------------------------------
bool			GInputManager::Initialise()
{
	if( FAILED( DirectInput8Create( DXUTGetHINSTANCE(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL ) ) )
	{
		return false;
	}

	if( FAILED( DirectInput->CreateDevice( GUID_SysKeyboard, &KeyboardDevice, NULL ) ) )
	{
		SAFE_RELEASE( DirectInput );
		return false;
	}

	if( FAILED( KeyboardDevice->SetDataFormat( &c_dfDIKeyboard ) ) ||
		FAILED( KeyboardDevice->SetCooperativeLevel( DXUTGetHWND(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE ) ) )
	{
		SAFE_RELEASE( KeyboardDevice );
		SAFE_RELEASE( DirectInput );
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool			GInputManager::Shutdown()
{
	SAFE_RELEASE( KeyboardDevice );
	SAFE_RELEASE( DirectInput );

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool			GInputManager::Process()
{
	assert( KeyboardDevice );

	memcpy( PreviousKeyState, KeyState, sizeof( PreviousKeyState ) );
	memset( KeyState, 0, sizeof( KeyState ) );

	if( FAILED( KeyboardDevice->GetDeviceState( sizeof( KeyState ), KeyState ) ) )
	{
		KeyboardDevice->Acquire();
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool			GInputManager::IsKeyDown( unsigned char key )
{
	return( (KeyState[ key ] & 0x80) != 0 );
}

//------------------------------------------------------------------------------------------------------------
bool			GInputManager::WasKeyDown( unsigned char key )
{
	return( (PreviousKeyState[ key ] & 0x80) != 0 );
}
