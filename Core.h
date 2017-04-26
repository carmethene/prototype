//------------------------------------------------------------------------------
// File: Core.h
// Desc: State manager for game, menu, etc
//
// Created: 24 September 2005 16:54:05
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_CORE_H
#define INCLUSIONGUARD_CORE_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class Menu;
class Game;

enum CORE_STATE
{
	CORE_STATE_UNINITIALISED,
	CORE_STATE_MENU,
	CORE_STATE_SCORE,
	CORE_STATE_GAME,
	CORE_STATE_QUIT,
};

class GCore
{
public:
	static bool					Initialise();
	static bool					Shutdown();

	static bool					RestoreDevice();

	static bool					Process();
	static bool					Render();

	static void					SwitchState( CORE_STATE state )			{ NextState = state; }

private:
	static bool					ShutdownState( CORE_STATE state );
	static bool					InitialiseState( CORE_STATE state );

	static bool					ProcessState( CORE_STATE state );
	static bool					RenderState( CORE_STATE state );

	static bool					ReallySwitchState();

	static Menu					*MenuObject;
	static Game					*GameObject;

	static CORE_STATE			CurrentState;
	static CORE_STATE			NextState;

};


#endif //INCLUSIONGUARD_CORE_H
