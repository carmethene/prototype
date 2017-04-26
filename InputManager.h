//------------------------------------------------------------------------------
// File: InputManager.h
// Desc: Wrapper for directinput
//
// Created: 15 September 2005 14:58:32
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_INPUTMANAGER_H
#define INCLUSIONGUARD_INPUTMANAGER_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class GInputManager
{
public:
	static bool					Initialise();
	static bool					Shutdown();

	static bool					Process();
	static bool					IsKeyDown( unsigned char key );
	static bool					WasKeyDown( unsigned char key );

private:
	static LPDIRECTINPUT8		DirectInput;
	static LPDIRECTINPUTDEVICE8	KeyboardDevice;

	static char					KeyState[256];
	static char					PreviousKeyState[256];

};

#endif //INCLUSIONGUARD_INPUTMANAGER_H
