//------------------------------------------------------------------------------
// File: Game.h
// Desc: Game core
//
// Created: 14 September 2005 13:08:36
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_GAME_H
#define INCLUSIONGUARD_GAME_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <d3d9.h>
#include <d3dx9.h>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class Object;
class Player;
class HUD;


class Game
{
public:
	Game();
	~Game();

	bool			Initialise();
	bool			Shutdown();

	bool			Process();
	bool			Render();

private:
	bool			SetGameOver();

	unsigned int	LastUpdateTime;

	//Players
	unsigned int	HumanPlayer;
	unsigned int	ComputerPlayer;

	HUD				*GameHud;

	unsigned int	GameOverText;
	bool			GameOver;
};


#endif //INCLUSIONGUARD_GAME_H
