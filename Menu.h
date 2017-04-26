//------------------------------------------------------------------------------
// File: Menu.h
// Desc: Front end menu system
//
// Created: 24 September 2005 16:53:08
//
// (c)2005 Neil Wakefield, Lionhead Studios Ltd
//------------------------------------------------------------------------------


#ifndef INCLUSIONGUARD_MENU_H
#define INCLUSIONGUARD_MENU_H


//------------------------------------------------------------------------------
// Included files:
//------------------------------------------------------------------------------
#include <d3dx9.h>


//------------------------------------------------------------------------------
// Prototypes and declarations:
//------------------------------------------------------------------------------
class Menu
{
public:
	Menu();
	~Menu();

	bool				Initialise();
	bool				Shutdown();

	bool				Process();
	bool				Render();

private:
	bool				ProcessInput();

	enum MENU_SELECTION
	{
		MENU_SELECTION_GAME,
		MENU_SELECTION_SCORE,
		MENU_SELECTION_QUIT,
	};

	unsigned int		LastUpdateTime;

	MENU_SELECTION		MenuSelection;

	unsigned int		LogoText;
	unsigned int		GameText;
	unsigned int		ScoreText;
	unsigned int		QuitText;
	unsigned int		CopyrightText;

};


#endif //INCLUSIONGUARD_MENU_H
